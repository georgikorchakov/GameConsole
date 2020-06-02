/*
Pins:
 * GND = GND
 * VCC = 5V
 * SCL = A5
 * SDA = A4
*/

#define STACK_LENGTH   256
#define PROGRAM_SIZE   611

// Macros
#define _FETCH_16BIT_POINTER(instruction) (*((uint16_t*)(instruction + 1)))
#define _FETCH_16BIT_RELATIVE_POINTER(instruction) (*((uint16_t*)(instruction + 1)) + *(instruction + 3))

// Opcodes
#define EOF 0x00

#define PUSH_IMMEDIATE 0x01
#define PUSH_ABSOLUTE  0x02
#define PUSH_RELATIVE  0x03

#define POP_ABSOLUTE   0x04
#define POP_RELATIVE   0x05

#define ADD 0x06
#define SUB 0x07
#define MUL 0x08
#define DIV 0x09

#define EQU 0x0A
#define NEQ 0x0B
#define GRT 0x0C
#define LES 0x0D
#define GEQ 0x0E
#define LEQ 0x0F

#define JMP 0x10
#define CJP 0x11

#define POP 0x12
#define DECL 0x13

#define JST 0x14
#define RET 0x15
#define STR 0x16

#define FILL_RECT 0x50
#define DISPLAY 0x51
#define CLEAR 0x52
#define DELAY 0x53

#define READ_LEFT_BUTTON 0x60
#define READ_RIGHT_BUTTON 0x61

//#define DEBUG
#define MEASURE_TIME

typedef struct stack_t {
  uint8_t stack[STACK_LENGTH];
  uint8_t top;
} stack_t;

uint8_t rv_register = 0; // Return Value Register

stack_t value_stack;
uint16_t pc = 0;
uint16_t address = 0;
uint8_t popped_value;

uint8_t x = 0;
uint8_t y = 0;
uint8_t w = 0;
uint8_t h = 0;
uint8_t t = 0;


uint8_t program[PROGRAM_SIZE] = {
  19,  1,   128, 1,   30,  7,   1,   2,   9,   4,
  1,   0,   19,  1,   4,   4,   1,   0,   19,  1,
  10,  4,   1,   0,   19,  1,   10,  4,   1,   0,
  19,  1,   2,   4,   1,   0,   19,  1,   2,   4,
  1,   0,   19,  1,   2,   4,   1,   0,   19,  1,
  1,   4,   1,   0,   19,  1,   1,   4,   1,   0, 
  16,  100, 0,   2,   3,   0,   2,   2,   0,   7,
  2,   3,   0,   2,   3,   0,   7,   2,   3,   0, 
  1,   2,   8,   1,   1,   6,   2,   4,   0,   1, 
  2,   8,   1,   1,   6,   80,  18,  18,  18,  20, 
  1,   1,   17,  108, 0,   16,  98,  2,   82,  96, 
  17,  116, 0,   16,  149, 0,   2,   9,   0,   1,
  3,   15,  17,  128, 0,   16,  136, 0,   1,   0, 
  4,   9,   0,   16,  146, 0,   2,   9,   0,   2, 
  9,   0,   7,   4,   9,   0,   16,  149, 0,   97,
  17,  156, 0,   16,  169, 0,   2,   9,   0,   2,
  9,   0,   6,   4,   9,   0,   16,  169, 0,   2,
  9,   0,   1,   30,  6,   1,   127, 12,  17,  184,
  0,   16,  192, 0,   1,   97,  4,   9,   0,   16,
  192, 0,   2,   2,   0,   17,  201, 0,   16,  214,
  0,   2,   7,   0,   2,   5,   0,   6,   4,   7,
  0,   16,  246, 0,   2,   7,   0,   2,   6,   0,
  15,  17,  227, 0,   16,  236, 0,   2,   5,   0,
  4,   7,   0,   16,  246, 0,   2,   7,   0,   2,
  5,   0,   7,   4,   7,   0,   2,   1,   0,   17,
  255, 0,   16,  12,  1,   2,   6,   0,   2,   4,
  0,   6,   4,   6,   0,   16,  44,  1,   2,   6,
  0,   2,   6,   0,   15,  17,  25,  1,   16,  34,
  1,   2,   5,   0,   4,   6,   0,   16,  44,  1,
  2,   6,   0,   2,   4,   0,   7,   4,   6,   0,
  2,   7,   0,   2,   6,   0,   15,  17,  57,  1,
  16,  85,  1,   2,   2,   0,   1,   1,   10,  17,
  69,  1,   16,  77,  1,   1,   0,   4,   2,   0, 
  16,  82,  1,   1,   1,   4,   2,   0,   16,  85, 
  1,   2,   7,   0,   2,   6,   0,   6,   1,   128,
  14,  17,  101, 1,   16,  129, 1,   2,   2,   0,
  1,   1,   10,  17,  113, 1,   16,  121, 1,   1,
  0,   4,   2,   0,   16,  126, 1,   1,   1,   4,
  2,   0,   16,  129, 1,   2,   6,   0,   2,   6,
  0,   15,  17,  142, 1,   16,  170, 1,   2,   1,
  0,   1,   1,   10,  17,  154, 1,   16,  162, 1,
  1,   0,   4,   1,   0,   16,  167, 1,   1,   1,
  4,   1,   0,   16,  170, 1,   2,   7,   0,   2,
  10,  0,   1,   30,  6,   15,  17,  186, 1,   16,
  12,  2,   2,   7,   0,   2,   10,  0,   14,  17,
  199, 1,   16,  9,   2,   2,   6,   0,   2,   6,
  0,   6,   1,   50,  14,  17,  215, 1,   16,  6,
  2,   2,   6,   0,   2,   6,   0,   6,   1,   52,
  15,  17,  231, 1,   16,  3,   2,   2,   1,   0,
  1,   1,   10,  17,  243, 1,   16,  251, 1,   1,
  0,   4,   1,   0,   16,  0,   2,   1,   1,   4,
  1,   0,   16,  3,   2,   16,  6,   2,   16,  9,
  2,   16,  12,  2,   2,   6,   0,   2,   6,   0,
  6,   1,   63,  14,  17,  28,  2,   16,  68,  2,
  1,   128, 1,   30,  7,   1,   2,   9,   4,   9,
  0,   1,   128, 1,   2,   9,   1,   40,  7,   4,
  7,   0,   1,   10,  4,   6,   0,   1,   1,   4,
  2,   0,   1,   1,   4,   1,   0,   16,  68,  2,
  1,   84,  1,   2,   2,   9,   0,   2,   9,   0,
  2,   9,   0,   16,  63,  0,   2,   9,   0,   1, 
  50,  1,   30,  1,   5,   80,  81,  16,  100, 0,
  0
};

unsigned long long int start_time = 0;
unsigned long long int end_time = 0;

/*
 * Graphics Library Dclarations
 */
typedef struct disp_object_t {
  uint8_t r0;
  uint8_t r1;
  uint8_t r2;
  uint8_t r3;
  uint8_t r4;
  uint8_t r5;
  uint8_t type;
} disp_object_t;

typedef struct display_objects_t {
  disp_object_t objects[16];
  uint8_t number;
} display_opbjects_t;

void init_display_objects(struct display_objects_t * objects);
void clear_display_objects(struct display_objects_t * objects);
void fill_rectangle(struct display_objects_t * objects, uint8_t x, uint8_t y, uint8_t w, uint8_t h);
uint8_t check_dot_in_rectangle(struct disp_object_t rect, uint8_t x, uint8_t y);
void print_display_objects(struct display_objects_t * objects);
uint8_t check_dot(struct display_objects_t * objects, uint8_t x, uint8_t y);
void display_display_objects(struct display_objects_t * objects);

void ssd1306_command(uint8_t command);
void init_display();
void clear();
void dot(uint8_t x, uint8_t y);
void ssd1306_commandList(const uint8_t *c, uint8_t n);
void full_screen(const uint8_t *pixels, uint8_t n);
void print_boot_screen();

display_objects_t display_objects;

void setup() {
  Serial.begin(9600);
  init_stack(&value_stack);
  init_display_objects(&display_objects); 
  init_display();

  DDRB &= ~(1 << PB1);
  DDRB &= ~(1 << PB2);

  print_boot_screen();
  delay(10000);
  
  #ifdef MEASURE_TIME
    start_time = micros();
  #endif
}

void loop() {
  uint8_t * instruction = program + pc;
  
  #ifdef DEBUG
    Serial.print("                                    PC: ");
    Serial.print(pc);
    Serial.print(" Instruction: ");
    Serial.println(*instruction, HEX);
  #endif
  
  switch(*instruction) {
    case PUSH_IMMEDIATE:
      #ifdef DEBUG
        Serial.print("Pushing imediate (at ");
        Serial.print(value_stack.top, HEX);
        Serial.print(") ");
        Serial.println(*(instruction + 1), HEX);
      #endif
      push(&value_stack, *(instruction + 1));
      pc += 2;
      break;
    case PUSH_ABSOLUTE:
      #ifdef DEBUG
        Serial.print("Pushing absolute (at ");
        Serial.print(value_stack.top, HEX);
        Serial.print(") ");
        Serial.println(_FETCH_16BIT_POINTER(instruction), HEX);
      #endif
      push(&value_stack, value_stack.stack[value_stack.top - _FETCH_16BIT_POINTER(instruction)]);
      pc += 3;
      break;
    case PUSH_RELATIVE:
      push(&value_stack, value_stack.stack[value_stack.top - _FETCH_16BIT_RELATIVE_POINTER(instruction)]);
      pc += 4;
      break;
    case POP_ABSOLUTE:
      #ifdef DEBUG
        Serial.print("Popping absolute (at ");
        Serial.print(value_stack.top, HEX);
        Serial.print(") ");
        Serial.println(_FETCH_16BIT_POINTER(instruction), HEX);
      #endif
      popped_value = pop(&value_stack);
      value_stack.stack[value_stack.top - _FETCH_16BIT_POINTER(instruction)] = popped_value;
      //value_stack.stack[value_stack.top - _FETCH_16BIT_POINTER(instruction)] = pop(&value_stack);
      pc += 3;
      break;
    case POP_RELATIVE:
      popped_value = pop(&value_stack);
      value_stack.stack[value_stack.top - _FETCH_16BIT_RELATIVE_POINTER(instruction)] = popped_value;
      pc += 4;
      break;
    case ADD:
      #ifdef DEBUG
        Serial.print("ADD (at ");
        Serial.print(value_stack.top, HEX);
        Serial.print(") ");
        Serial.print(value_stack.stack[value_stack.top - 1], HEX);
        Serial.print(" + ");
        Serial.println(value_stack.stack[value_stack.top - 2], HEX);
      #endif
      add(&value_stack);
      pc += 1;
      break;
    case SUB:
      #ifdef DEBUG
        Serial.print("SUB (at ");
        Serial.print(value_stack.top, HEX);
        Serial.print(") ");
        Serial.print(value_stack.stack[value_stack.top - 1], HEX);
        Serial.print(" - ");
        Serial.println(value_stack.stack[value_stack.top - 2], HEX);
      #endif
      sub(&value_stack);
      pc += 1;
      break;
    case MUL:
      #ifdef DEBUG
        Serial.print("MUL (at ");
        Serial.print(value_stack.top, HEX);
        Serial.print(") ");
        Serial.print(value_stack.stack[value_stack.top - 1], HEX);
        Serial.print(" * ");
        Serial.println(value_stack.stack[value_stack.top - 2], HEX);
      #endif
      mul(&value_stack);
      pc += 1;
      break;
    case DIV:
      #ifdef DEBUG
        Serial.print("DIV (at ");
        Serial.print(value_stack.top, HEX);
        Serial.print(") ");
        Serial.print(value_stack.stack[value_stack.top - 1], HEX);
        Serial.print(" / ");
        Serial.println(value_stack.stack[value_stack.top - 2], HEX);
      #endif
      divide(&value_stack);
      pc += 1;
      break;
    case EQU:
      #ifdef DEBUG
        Serial.print("EQU (at ");
        Serial.print(value_stack.top, HEX);
        Serial.print(") ");
        Serial.print(value_stack.stack[value_stack.top - 1], HEX);
        Serial.print(" == ");
        Serial.println(value_stack.stack[value_stack.top - 2], HEX);
      #endif
      equal(&value_stack);
      pc += 1;
      break;
    case NEQ:
      #ifdef DEBUG
        Serial.print("NEQ (at ");
        Serial.print(value_stack.top, HEX);
        Serial.print(") ");
        Serial.print(value_stack.stack[value_stack.top - 1], HEX);
        Serial.print(" != ");
        Serial.println(value_stack.stack[value_stack.top - 2], HEX);
      #endif
      not_equal(&value_stack);
      pc += 1;
      break;
    case GRT:
      #ifdef DEBUG
        Serial.print("GRT (at ");
        Serial.print(value_stack.top, HEX);
        Serial.print(") ");
        Serial.print(value_stack.stack[value_stack.top - 1], HEX);
        Serial.print(" > ");
        Serial.println(value_stack.stack[value_stack.top - 2], HEX);
      #endif
      greater(&value_stack);
      pc += 1;
      break;
    case LES:
      #ifdef DEBUG
        Serial.print("LES (at ");
        Serial.print(value_stack.top, HEX);
        Serial.print(") ");
        Serial.print(value_stack.stack[value_stack.top - 1], HEX);
        Serial.print(" < ");
        Serial.println(value_stack.stack[value_stack.top - 2], HEX);
      #endif
      less(&value_stack);
      pc += 1;
      break;
    case GEQ:
      #ifdef DEBUG
        Serial.print("GEQ (at ");
        Serial.print(value_stack.top, HEX);
        Serial.print(") ");
        Serial.print(value_stack.stack[value_stack.top - 1], HEX);
        Serial.print(" >= ");
        Serial.println(value_stack.stack[value_stack.top - 2], HEX);
      #endif
      greater_equal(&value_stack);
      pc += 1;
      break;
    case LEQ:
      #ifdef DEBUG
        Serial.print("LEQ (at ");
        Serial.print(value_stack.top, HEX);
        Serial.print(") ");
        Serial.print(value_stack.stack[value_stack.top - 1], HEX);
        Serial.print(" <= ");
        Serial.println(value_stack.stack[value_stack.top - 2], HEX);
      #endif
      less_equal(&value_stack);
      pc += 1;
      break;
    case JMP:
      #ifdef DEBUG
        Serial.print("JMP (at ");
        Serial.print(value_stack.top, HEX);
        Serial.print(") to ");
        Serial.println(_FETCH_16BIT_POINTER(instruction), HEX);
      #endif
      pc = _FETCH_16BIT_POINTER(instruction);
      break;
    case POP:
      #ifdef DEBUG
        Serial.print("POP (at ");
        Serial.print(value_stack.top, HEX);
        Serial.println(")");
      #endif
      value_stack.top--;
      pc += 1;
      break;
    case DECL:
      #ifdef DEBUG
        Serial.print("Declare (at ");
        Serial.print(value_stack.top, HEX);
        Serial.println(") ");
      #endif
      value_stack.top++;
      pc += 1;
      break;
    case RET:
      #ifdef DEBUG
        Serial.print("RET: ");
        Serial.println(rv_register, HEX);
      #endif
      push(&value_stack, rv_register);
      pc += 1;
      break;
    case STR:
      rv_register = pop(&value_stack);
      #ifdef DEBUG
        Serial.print("STR: ");
        Serial.println(rv_register, HEX);
      #endif
      pc += 1;
      break;
    case JST:
      address = pop_16bit(&value_stack);
      #ifdef DEBUG
        Serial.print("JST to ");
        Serial.println(address, HEX);
      #endif
      pc = address;
      break;
    case CJP:
      #ifdef DEBUG
        Serial.print("CJP to ");
        Serial.println(_FETCH_16BIT_POINTER(instruction), HEX);
      #endif
      if(pop(&value_stack)){
        pc = _FETCH_16BIT_POINTER(instruction);
      } else {
        pc += 3;
      }
      break;
    case FILL_RECT:
      h = pop(&value_stack);
      w = pop(&value_stack);
      y = pop(&value_stack);
      x = pop(&value_stack);
      fill_rectangle(&display_objects, x, y, w, h);
      pc += 1;
      break;
    case DISPLAY:
      display_display_objects(&display_objects);
      pc += 1;
      break;
    case CLEAR:
      clear_display_objects(&display_objects);
      pc += 1;
      break;
    case DELAY:
      t = pop(&value_stack);
      delay(t);
      pc += 1;
      break;
    case READ_LEFT_BUTTON:
      push(&value_stack, !((PINB & (1 << PB1)) == 0));
      pc += 1;
      break;
    case READ_RIGHT_BUTTON:
      push(&value_stack, !((PINB & (1 << PB2)) == 0));
      pc += 1;
      break;
    case EOF:
      #ifdef MEASURE_TIME
        end_time = micros();
        Serial.print("Program execution time = ");
        Serial.print((long)(end_time - start_time));
        Serial.println("us");
      #endif
      print_stack();
      while(true);
  }
}

// Helpers
void push(stack_t * stack, uint8_t number) {
  stack->stack[stack->top] = number;
  ++stack->top;
}

uint8_t pop(stack_t * stack){
  --stack->top;
  return stack->stack[stack->top];
}

uint16_t pop_16bit(stack_t * stack){
  stack->top -= 2;
  #ifdef DEBUG
    Serial.print("pop_16bit: ");
    Serial.println((stack->stack[stack->top] + (stack->stack[stack->top + 1] << 8)), HEX);
  #endif
  return stack->stack[stack->top] + (stack->stack[stack->top + 1] << 8);
}

void init_stack(stack_t * stack) {
  stack->top = 0;
}

static inline void add(stack_t * stack) {
  --stack->top;
  stack->stack[stack->top-1] += stack->stack[stack->top];
}

static inline void mul(stack_t * stack) {
  --stack->top;
  stack->stack[stack->top-1] *= stack->stack[stack->top];
}

static inline void sub(stack_t * stack) {
  --stack->top;
  stack->stack[stack->top-1] -= stack->stack[stack->top];
}

static inline void divide(stack_t * stack) {
  --stack->top;
  stack->stack[stack->top-1] /= stack->stack[stack->top];
}

static inline void equal(stack_t * stack){
  --stack->top;
  stack->stack[stack->top-1] = (stack->stack[stack->top-1] == stack->stack[stack->top]);
}

static inline void not_equal(stack_t * stack){
  --stack->top;
  stack->stack[stack->top-1] = (stack->stack[stack->top-1] != stack->stack[stack->top]);
}

static inline void greater(stack_t * stack){
  --stack->top;
  stack->stack[stack->top-1] = (stack->stack[stack->top-1] > stack->stack[stack->top]);
}

static inline void less(stack_t * stack){
  --stack->top;
  stack->stack[stack->top-1] = (stack->stack[stack->top-1] < stack->stack[stack->top]);
}

static inline void greater_equal(stack_t * stack){
  --stack->top;
  stack->stack[stack->top-1] = (stack->stack[stack->top-1] >= stack->stack[stack->top]);
}

static inline void less_equal(stack_t * stack){
  --stack->top;
  stack->stack[stack->top-1] = (stack->stack[stack->top-1] <= stack->stack[stack->top]);
}

void print_stack(){
  Serial.println("Stack");
  Serial.print("Stack_top: ");
  Serial.println(value_stack.top);
  Serial.println("        00      01      02      03      04      05      06      07      08      09      0A      0B      0C      0D      0E      0F");
  for(int i = 0; i < 16; i++){
    Serial.print(0x0 + i, HEX);
    Serial.print("\t");
    for(int j = 0; j < 16; j++){
      Serial.print(value_stack.stack[(0xF * i) + (0x00 + j)], HEX);
      Serial.print("\t");
    }
    Serial.println();
  }
}

/*
 * Graphics Library
 */
#include <Wire.h>

#define WIRE_MAX 32
#define SSD1306_BLACK               0
#define SSD1306_WHITE               1
#define SSD1306_INVERSE             2

#define SSD1306_MEMORYMODE          0x20
#define SSD1306_COLUMNADDR          0x21
#define SSD1306_PAGEADDR            0x22
#define SSD1306_SETCONTRAST         0x81
#define SSD1306_CHARGEPUMP          0x8D
#define SSD1306_SEGREMAP            0xA0
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON        0xA5 
#define SSD1306_NORMALDISPLAY       0xA6
#define SSD1306_INVERTDISPLAY       0xA7
#define SSD1306_SETMULTIPLEX        0xA8
#define SSD1306_DISPLAYOFF          0xAE
#define SSD1306_DISPLAYON           0xAF
#define SSD1306_COMSCANINC          0xC0 
#define SSD1306_COMSCANDEC          0xC8
#define SSD1306_SETDISPLAYOFFSET    0xD3
#define SSD1306_SETDISPLAYCLOCKDIV  0xD5
#define SSD1306_SETPRECHARGE        0xD9
#define SSD1306_SETCOMPINS          0xDA
#define SSD1306_SETVCOMDETECT       0xDB

#define SSD1306_SETLOWCOLUMN        0x00 
#define SSD1306_SETHIGHCOLUMN       0x10 
#define SSD1306_SETSTARTLINE        0x40

#define SSD1306_EXTERNALVCC         0x01 // External display voltage source
#define SSD1306_SWITCHCAPVCC        0x02 // Gen. display voltage from 3.3V

#define SSD1306_RIGHT_HORIZONTAL_SCROLL              0x26 
#define SSD1306_LEFT_HORIZONTAL_SCROLL               0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL  0x2A
#define SSD1306_DEACTIVATE_SCROLL                    0x2E
#define SSD1306_ACTIVATE_SCROLL                      0x2F
#define SSD1306_SET_VERTICAL_SCROLL_AREA             0xA3

#define ADDRESS 0x3C
#define HEIGHT  64
#define WIDTH   128



void ssd1306_command(uint8_t command){
  Wire.beginTransmission(ADDRESS);
  Wire.write((uint8_t)0x00);         // Co = 0, D/C = 0
  Wire.write(command);
  Wire.endTransmission();
}

void ssd1306_commandList(const uint8_t *c, uint8_t n) {
  Wire.beginTransmission(ADDRESS);
  Wire.write((uint8_t)0x00);         // Co = 0, D/C = 0
  
  uint8_t bytesOut = 1;
  while(n--) {
    if(bytesOut >= WIRE_MAX) {
      Wire.endTransmission();
      Wire.beginTransmission(ADDRESS);
      Wire.write((uint8_t)0x00);     // Co = 0, D/C = 0
      bytesOut = 1;
    }
    Wire.write(pgm_read_word_near(c++));
    bytesOut++;
  }
  
  Wire.endTransmission();
}

void init_display(){
  Wire.begin();
  Wire.setClock(400000);

  uint8_t vccstate = SSD1306_SWITCHCAPVCC;

  static const uint8_t PROGMEM init1[] = {
    SSD1306_DISPLAYOFF,                   // 0xAE
    SSD1306_SETDISPLAYCLOCKDIV,           // 0xD5
    0x80,                                 // the suggested ratio 0x80
    SSD1306_SETMULTIPLEX };               // 0xA8
  ssd1306_commandList(init1, sizeof(init1));
  ssd1306_command(HEIGHT - 1);

  static const uint8_t PROGMEM init2[] = {
    SSD1306_SETDISPLAYOFFSET,             // 0xD3
    0x0,                                  // no offset
    SSD1306_SETSTARTLINE | 0x0,           // line #0
    SSD1306_CHARGEPUMP };                 // 0x8D
  ssd1306_commandList(init2, sizeof(init2));

  ssd1306_command((vccstate == SSD1306_EXTERNALVCC) ? 0x10 : 0x14);

  static const uint8_t PROGMEM init3[] = {
    SSD1306_MEMORYMODE,                   // 0x20
    0x00,                                 // 0x0 act like ks0108
    SSD1306_SEGREMAP | 0x1,
    SSD1306_COMSCANDEC };
  ssd1306_commandList(init3, sizeof(init3));

  uint8_t comPins = 0x02;
  uint8_t contrast = 0x8F;

  comPins = 0x12;
  contrast = (vccstate == SSD1306_EXTERNALVCC) ? 0x9F : 0xCF;

  ssd1306_command(SSD1306_SETCOMPINS);
  ssd1306_command(comPins);
  ssd1306_command(SSD1306_SETCONTRAST);
  ssd1306_command(contrast);

  ssd1306_command(SSD1306_SETPRECHARGE); // 0xd9
  ssd1306_command((vccstate == SSD1306_EXTERNALVCC) ? 0x22 : 0xF1);
  static const uint8_t PROGMEM init5[] = {
    SSD1306_SETVCOMDETECT,               // 0xDB
    0x40,
    SSD1306_DISPLAYALLON_RESUME,         // 0xA4
    SSD1306_NORMALDISPLAY,               // 0xA6
    SSD1306_DEACTIVATE_SCROLL,
    SSD1306_DISPLAYON };                 // Main screen turn on
  ssd1306_commandList(init5, sizeof(init5));
}

void clear(){
  static const uint8_t PROGMEM dlist1[] = {
    SSD1306_PAGEADDR,
    0,                         // Page start address
    0xFF,                      // Page end (not really, but works here)
    SSD1306_COLUMNADDR,
    0 };                       // Column start address
  ssd1306_commandList(dlist1, sizeof(dlist1));
  ssd1306_command(WIDTH - 1);  // Column end address

  uint16_t count = WIDTH * ((HEIGHT + 7) / 8);
 
  Wire.beginTransmission(ADDRESS);
  Wire.write((uint8_t)0x40);
  uint8_t bytesOut = 1;
  while(count--) {
    if(bytesOut >= WIRE_MAX) {
      Wire.endTransmission();
      Wire.beginTransmission(ADDRESS);
      Wire.write((uint8_t)0x40);
      bytesOut = 1;
    }
    Wire.write(0b00000000);
    bytesOut++;
  }
  Wire.endTransmission();
}

void write_independent_dot(uint8_t x, uint8_t y){
  uint8_t page = ((x) / 8);
  uint8_t column = y;
  uint8_t value = 1 << (x % 8);
  Serial.print("page: ");
  Serial.print(page);

  Serial.print(" column: ");
  Serial.print(column);

  Serial.print(" value: ");
  Serial.println(value, BIN);

  Wire.beginTransmission(ADDRESS);
  Wire.write((uint8_t)0x00);
  Wire.write(SSD1306_PAGEADDR);
  Wire.write(page);
  Wire.write(page);
  Wire.write(SSD1306_COLUMNADDR);
  Wire.write(column);
  Wire.write(column);
  Wire.endTransmission();
  
  Wire.beginTransmission(ADDRESS);
  Wire.write((uint8_t)0x40);
  Wire.write(value);
  //Wire.write(value);
  Wire.endTransmission();
}

/**
 * Print full screen from progmem
 */ 
void full_screen(const uint8_t *pixels, uint8_t n){
  uint8_t value = 0x00;
  uint8_t mask = 0x00;
  
  for (uint8_t page = 0; page < 8; page++){
    Wire.beginTransmission(ADDRESS);
    Wire.write((uint8_t)0x00);
    Wire.write(SSD1306_PAGEADDR);
    Wire.write((uint8_t)page);
    Wire.write((uint8_t)page);
    Wire.write(SSD1306_COLUMNADDR);
    Wire.write((uint8_t)0);
    Wire.write((uint8_t)127);
    Wire.endTransmission();
    
    for (int p = 0; p < 16; p++){
      Wire.beginTransmission(ADDRESS);
      Wire.write((uint8_t)0x40);
      for (int pos = 0; pos < 8; pos++){
        mask = (1 << (7 - pos));
        value = (((pgm_read_word_near((pixels + p) + 16 * 0) & mask) << pos) >> 7) | 
                (((pgm_read_word_near((pixels + p) + 16 * 1) & mask) << pos) >> 6) |
                (((pgm_read_word_near((pixels + p) + 16 * 2) & mask) << pos) >> 5) |
                (((pgm_read_word_near((pixels + p) + 16 * 3) & mask) << pos) >> 4) |
                (((pgm_read_word_near((pixels + p) + 16 * 4) & mask) << pos) >> 3) |
                (((pgm_read_word_near((pixels + p) + 16 * 5) & mask) << pos) >> 2) |
                (((pgm_read_word_near((pixels + p) + 16 * 6) & mask) << pos) >> 1) |
                (((pgm_read_word_near((pixels + p) + 16 * 7) & mask) << pos) >> 0);
        Wire.write(value);              
      }
      Wire.endTransmission();
    }
    
    pixels += 8*16;
     
  }
}

#define FILL_RECTANGLE_T 0x01
#define MAX_DISPLAY_OBJECTS 16

void init_display_objects(struct display_objects_t * objects)
{
  objects->number = 0;
}

void clear_display_objects(struct display_objects_t * objects)
{
  objects->number = 0;
}

void fill_rectangle(struct display_objects_t * objects, uint8_t x, uint8_t y, uint8_t w, uint8_t h){
  if (objects->number < 16){
    objects->objects[objects->number].type = FILL_RECTANGLE_T;
    objects->objects[objects->number].r0 = x;
    objects->objects[objects->number].r1 = y;
    objects->objects[objects->number].r2 = w;
    objects->objects[objects->number].r3 = h;
    ++objects->number;
  }
}

uint8_t check_dot_in_rectangle(struct disp_object_t rect, uint8_t x, uint8_t y){
  if ((x >= rect.r0) && (x <= rect.r0 + rect.r2))
  {
    if ((y >= rect.r1) && (y <= rect.r1 + rect.r3))
    {
      return 1;
    }
  }

  return 0;
}

uint8_t check_dot(struct display_objects_t * objects, uint8_t x, uint8_t y){
  for (int current_object = 0; current_object < objects->number; ++current_object){
    if (objects->objects[current_object].type == FILL_RECTANGLE_T){
      if(check_dot_in_rectangle(objects->objects[current_object], x, y))
      {
        return 1;
      }
    }
  }

  return 0;
}

void display_display_objects(struct display_objects_t * objects){
  static const uint8_t PROGMEM dlist1[] = {
    SSD1306_PAGEADDR,
    0,                         // Page start address
    0xFF,                      // Page end (not really, but works here)
    SSD1306_COLUMNADDR,
    0 };                       // Column start address
  ssd1306_commandList(dlist1, sizeof(dlist1));
  ssd1306_command(WIDTH - 1); // Column end address

  uint8_t column = 0;
  uint8_t page = 0;
  uint8_t value = 0;
 
  Wire.beginTransmission(ADDRESS);
  Wire.write((uint8_t)0x40);
  uint8_t bytesOut = 1;
  
  while(page < 64) {
    if(bytesOut >= WIRE_MAX) {
      Wire.endTransmission();
      Wire.beginTransmission(ADDRESS);
      Wire.write((uint8_t)0x40);
      bytesOut = 1;
    }
    
    value = check_dot(objects, column, page) |
           (check_dot(objects, column, page + 1) << 1) |
           (check_dot(objects, column, page + 2) << 2) |
           (check_dot(objects, column, page + 3) << 3) |
           (check_dot(objects, column, page + 4) << 4) |
           (check_dot(objects, column, page + 5) << 5) |
           (check_dot(objects, column, page + 6) << 6) |
           (check_dot(objects, column, page + 7) << 7);
    Wire.write(value);

    bytesOut++;
    column++;
    if (column == 128){
      column = 0;
      page += 8;
    }
  }
  Wire.endTransmission();
}

void print_display_objects(struct display_objects_t * objects){
  Serial.println("---- Display Objects ----");
  for (uint8_t current_object = 0; current_object < objects->number; ++current_object){
    Serial.print("Display object No: ");
    Serial.print(current_object);
    Serial.print(" r[");
    Serial.print(objects->objects[current_object].r0);
    Serial.print(", ");
    Serial.print(objects->objects[current_object].r1);
    Serial.print(", ");
    Serial.print(objects->objects[current_object].r2);
    Serial.print(", ");
    Serial.print(objects->objects[current_object].r3);
    Serial.print(", ");
    Serial.print(objects->objects[current_object].r4);
    Serial.print(", ");
    Serial.print(objects->objects[current_object].r5);
    Serial.print("] Type: ");
    if (objects->objects[current_object].type == FILL_RECTANGLE_T){
      Serial.println("FILL_RECTANGLE_T");
    }
  }
}

static const uint8_t PROGMEM initial_screen[1024] = {
  0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 
  0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b11111100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b11110000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b00000000, 0b00000011, 
  0b11000000, 0b11111100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b11110000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b00000000, 0b00000011, 
  0b11000011, 0b00000011, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00001100, 0b00001100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b00000000, 0b00000011, 
  0b11000011, 0b00000011, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00001100, 0b00001100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b00000000, 0b00000011, 
  0b11001100, 0b00000000, 0b00001111, 0b11000011, 0b11110011, 0b11000000, 0b11110000, 0b00110000, 0b00000000, 0b00111100, 0b00110011, 0b11000011, 0b11000011, 0b11000011, 0b00001111, 0b00000011, 
  0b11001100, 0b00000000, 0b00001111, 0b11000011, 0b11110011, 0b11000000, 0b11110000, 0b00110000, 0b00000000, 0b00111100, 0b00110011, 0b11000011, 0b11000011, 0b11000011, 0b00001111, 0b00000011, 
  0b11001100, 0b00001111, 0b00110000, 0b11000011, 0b00001100, 0b00110011, 0b11111100, 0b00110000, 0b00000000, 0b11000011, 0b00111100, 0b11001100, 0b00001100, 0b00110011, 0b00111111, 0b11000011, 
  0b11001100, 0b00001111, 0b00110000, 0b11000011, 0b00001100, 0b00110011, 0b11111100, 0b00110000, 0b00000000, 0b11000011, 0b00111100, 0b11001100, 0b00001100, 0b00110011, 0b00111111, 0b11000011, 
  0b11000011, 0b00000011, 0b00110000, 0b11000011, 0b00001100, 0b00110011, 0b00000000, 0b00001100, 0b00001100, 0b11000011, 0b00110000, 0b11000011, 0b11001100, 0b00110011, 0b00110000, 0b00000011, 
  0b11000011, 0b00000011, 0b00110000, 0b11000011, 0b00001100, 0b00110011, 0b00000000, 0b00001100, 0b00001100, 0b11000011, 0b00110000, 0b11000011, 0b11001100, 0b00110011, 0b00110000, 0b00000011, 
  0b11000000, 0b11111100, 0b00001111, 0b00110011, 0b00001100, 0b00110000, 0b11111100, 0b00000011, 0b11110000, 0b00111100, 0b00110000, 0b11001111, 0b11000011, 0b11000011, 0b00001111, 0b11000011, 
  0b11000000, 0b11111100, 0b00001111, 0b00110011, 0b00001100, 0b00110000, 0b11111100, 0b00000011, 0b11110000, 0b00111100, 0b00110000, 0b11001111, 0b11000011, 0b11000011, 0b00001111, 0b11000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b01111111, 0b11111111, 0b11111111, 0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b10100000, 0b00000000, 0b00000000, 0b01000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000001, 0b00010011, 0b11111111, 0b11111111, 0b00100000, 0b00000000, 0b00000000, 0b00000100, 0b01011100, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000010, 0b01101001, 0b00000000, 0b00000000, 0b10010000, 0b00000000, 0b00000000, 0b00000000, 0b00000001, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000001, 0b01100100, 0b10000000, 0b00000000, 0b01001000, 0b00000010, 0b00111000, 0b11111101, 0b11101100, 0b10000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b10000010, 0b01000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000010, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b01001001, 0b00100000, 0b00010101, 0b10111100, 0b10111001, 0b11001111, 0b00011011, 0b00111001, 0b01000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00100100, 0b10010000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01101101, 0b01000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00010010, 0b01001000, 0b00000110, 0b11100111, 0b00111010, 0b01110100, 0b11110110, 0b01110100, 0b01000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00001000, 0b00100100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01111101, 0b01000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000100, 0b10010010, 0b00000001, 0b11011011, 0b11010111, 0b01011001, 0b00011101, 0b00111000, 0b01000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000010, 0b01001001, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000010, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000001, 0b00100100, 0b10000000, 0b00000000, 0b01001000, 0b00101110, 0b01111011, 0b11111100, 0b10000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b10000010, 0b01000000, 0b00000000, 0b00100100, 0b00000000, 0b00000000, 0b00000001, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01001001, 0b00111111, 0b11111111, 0b11110010, 0b00000000, 0b00000010, 0b10011100, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00100100, 0b10000000, 0b00000000, 0b00000001, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00010010, 0b01000000, 0b00000000, 0b00000000, 0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00001000, 0b00100000, 0b00000000, 0b00000000, 0b01000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000100, 0b10010000, 0b00011000, 0b00000000, 0b00100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000010, 0b01001000, 0b00001100, 0b00000000, 0b00010000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00111100, 0b00000000, 0b00000000, 0b00000000, 0b00000001, 0b00100100, 0b00000000, 0b00000000, 0b00001000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00110100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b10000010, 0b01100110, 0b01100001, 0b10000100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00101100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01001001, 0b00110011, 0b00110000, 0b11000010, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00111100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00100100, 0b10000000, 0b00000000, 0b00011001, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00010010, 0b01000001, 0b10000000, 0b00001100, 0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00001000, 0b00100000, 0b11000000, 0b00000000, 0b01000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11001111, 0b00111100, 0b11110000, 0b00000000, 0b00000000, 0b00000000, 0b00000100, 0b10010000, 0b00000000, 0b00000000, 0b00100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11001101, 0b00110100, 0b11010000, 0b00000000, 0b00000000, 0b00000000, 0b00000010, 0b01001000, 0b00000000, 0b00000000, 0b00010000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11001011, 0b00101100, 0b10110000, 0b00000000, 0b00000000, 0b00000000, 0b00000001, 0b00100100, 0b00000000, 0b00000000, 0b00001000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b11110011, 
  0b11001111, 0b00111100, 0b11110000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b10000011, 0b11111111, 0b11111111, 0b11111100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b10110011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01000100, 0b00000000, 0b00000000, 0b00000100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b11010011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00101001, 0b10101001, 0b11001010, 0b11001000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b11110011, 
  0b11000000, 0b00111100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00010000, 0b00000000, 0b00000000, 0b00010000, 0b00000000, 0b00000000, 0b00000000, 0b00111100, 0b00000011, 
  0b11000000, 0b00110100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00001111, 0b11111111, 0b11111111, 0b11100000, 0b00000000, 0b00000000, 0b00000000, 0b00101100, 0b00000011, 
  0b11000000, 0b00101100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00110100, 0b00000011, 
  0b11000000, 0b00111100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00111100, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 
  0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 
  0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111  
};

void print_boot_screen(){
  clear();
  full_screen(initial_screen, sizeof(initial_screen));
}
