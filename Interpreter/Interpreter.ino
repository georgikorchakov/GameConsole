#define STACK_LENGTH   512
#define PROGRAM_SIZE   0x46

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


uint8_t program[PROGRAM_SIZE] = {
 16, 55, 0, 2, 1, 0, 1, 1, 15, 17, 15, 0, 16, 23, 0, 1, 1, 22, 18, 20, 16, 23, 0, 1, 36, 1, 0, 2, 3, 0, 1, 1, 7, 16, 3, 0, 21, 1, 50, 1, 0, 2, 4, 0, 1, 2, 7, 16, 3, 0, 21, 6, 22, 18, 20, 19, 1, 65, 1, 0, 1, 12, 16, 3, 0, 21, 4, 1, 0, 0
};

unsigned long long int start_time = 0;
unsigned long long int end_time = 0;

void setup() {
  init_stack(&value_stack);
  Serial.begin(9600);
  
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
