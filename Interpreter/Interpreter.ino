#define STACK_LENGTH   512
#define PROGRAM_SIZE   0x4f

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

//#define DEBUG
#define MEASURE_TIME

typedef struct stack_t {
  uint8_t stack[STACK_LENGTH];
  uint8_t top;
} stack_t;

stack_t value_stack;
uint16_t pc = 0;
uint8_t popped_value;

uint8_t program[PROGRAM_SIZE] = {
  19, 1, 6, 4, 1, 0, 19, 1, 1, 4, 1, 0, 19, 1, 0, 4, 1, 0, 19, 1, 0, 4, 1, 0, 19, 1, 1, 4, 1, 0, 2, 1, 0, 2, 6, 0, 13, 17, 43, 0, 16, 77, 0, 2, 2, 0, 4, 3, 0, 2, 4, 0, 4, 2, 0, 2, 3, 0, 2, 3, 0, 6, 4, 4, 0, 2, 1, 0, 1, 1, 6, 4, 1, 0, 16, 30, 0, 18, 0
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
      pc += 3;
      break;
    case POP_RELATIVE:
      popped_value = pop(&value_stack);
      value_stack.stack[value_stack.top - _FETCH_16BIT_RELATIVE_POINTER(instruction)] = popped_value;
      pc += 4;
      break;
    case ADD:
      #ifdef DEBUG
        Serial.print("Add (at ");
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
      sub(&value_stack);
      pc += 1;
    case MUL:
      #ifdef DEBUG
        Serial.print("Multiply (at ");
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
      divide(&value_stack);
      pc += 1;
      break;
    case EQU:
      #ifdef DEBUG
        Serial.print("Compare (at ");
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
      not_equal(&value_stack);
      pc += 1;
      break;
    case GRT:
      greater(&value_stack);
      pc += 1;
      break;
    case LES:
      #ifdef DEBUG
        Serial.print("Compare (at ");
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
      greater_equal(&value_stack);
      pc += 1;
      break;
    case LEQ:
      less_equal(&value_stack);
      pc += 1;
      break;
    case JMP:
      pc = _FETCH_16BIT_POINTER(instruction);
      break;
    case POP:
      value_stack.top--;
      pc += 1;
      break;
    case CJP:
      if(pop(&value_stack)){
        #ifdef DEBUG
          Serial.print("CJP to ");
          Serial.println(_FETCH_16BIT_POINTER(instruction), HEX);
        #endif
        pc = _FETCH_16BIT_POINTER(instruction);
      } else {
        pc += 3;
      }
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
    case EOF:
      #ifdef MEASURE_TIME
        end_time = micros();
        Serial.print("Program execution time = ");
        Serial.print((int)(end_time - start_time));
        Serial.println("us");
      #endif
      print_stack();
      while(true);
  }
}

// Helpers
void push(stack_t * stack, uint8_t number) {
  stack->stack[stack->top] = number;
  stack->top++;
}

uint8_t pop(stack_t * stack){
  stack->top--;
  return stack->stack[stack->top];
}

void init_stack(stack_t * stack) {
  stack->top = 0;
}

static inline void add(stack_t * stack) {
  stack->top--;
  stack->stack[stack->top-1] += stack->stack[stack->top];
}

static inline void mul(stack_t * stack) {
  stack->top--;
  stack->stack[stack->top-1] *= stack->stack[stack->top];
}

static inline void sub(stack_t * stack) {
  stack->top--;
  stack->stack[stack->top-1] -= stack->stack[stack->top];
}

static inline void divide(stack_t * stack) {
  stack->top--;
  stack->stack[stack->top-1] /= stack->stack[stack->top];
}

static inline void equal(stack_t * stack){
  stack->top--;
  stack->stack[stack->top-1] = (stack->stack[stack->top-1] == stack->stack[stack->top]);
}

static inline void not_equal(stack_t * stack){
  stack->top--;
  stack->stack[stack->top-1] = (stack->stack[stack->top-1] != stack->stack[stack->top]);
}

static inline void greater(stack_t * stack){
  stack->top--;
  stack->stack[stack->top-1] = (stack->stack[stack->top-1] > stack->stack[stack->top]);
}

static inline void less(stack_t * stack){
  stack->top--;
  stack->stack[stack->top-1] = (stack->stack[stack->top-1] < stack->stack[stack->top]);
}

static inline void greater_equal(stack_t * stack){
  stack->top--;
  stack->stack[stack->top-1] = (stack->stack[stack->top-1] >= stack->stack[stack->top]);
}

static inline void less_equal(stack_t * stack){
  stack->top--;
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
