#define STACK_LENGTH   128
#define MEMORY_SIZE    256
#define PROGRAM_SIZE   0x60

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



typedef struct stack_t {
  uint8_t stack[STACK_LENGTH];
  uint8_t top;
} stack_t;

stack_t value_stack;

uint8_t memory[MEMORY_SIZE];
uint16_t pc = 0;

uint8_t program[PROGRAM_SIZE] = {
  1, 5, 4, 0, 0, 1, 7, 4, 1, 0,
  1, 10, 2, 1, 0, 6, 2, 0, 0, 1,
  4, 6, 8, 4, 2, 0, 2, 0, 0, 2,
  1, 0, 13, 17, 48, 0, 2, 0, 0, 1,
  5, 12, 17, 81, 0, 16, 89, 0, 2, 0,
  0, 1, 5, 10, 17, 60, 0, 16, 78, 0,
  2, 2, 0, 4, 0, 0, 2, 0, 0, 1,
  1, 6, 4, 0, 0, 16, 78, 0, 16, 95,
  0, 1, 5, 4, 0, 0, 16, 95, 0, 2,
  2, 0, 4, 0, 0, 0
};

void setup() {
  init_stack(&value_stack);
  Serial.begin(9600);
}

void loop() {
  uint8_t * instruction = program + pc;
  Serial.println(*instruction, HEX);
  switch(*instruction) {
    case PUSH_IMMEDIATE:
      push(&value_stack, *(instruction + 1));
      pc += 2;
      break;
    case PUSH_ABSOLUTE:
      push(&value_stack, memory[_FETCH_16BIT_POINTER(instruction)]);
      pc += 3;
      break;
    case PUSH_RELATIVE:
      push(&value_stack, memory[_FETCH_16BIT_RELATIVE_POINTER(instruction)]);
      pc += 4;
      break;
    case POP_ABSOLUTE:
      memory[_FETCH_16BIT_POINTER(instruction)] = pop(&value_stack);
      pc += 3;
      break;
    case POP_RELATIVE:
      memory[_FETCH_16BIT_RELATIVE_POINTER(instruction)] = pop(&value_stack);
      pc += 4;
      break;
    case ADD:
      add(&value_stack);
      pc += 1;
      break;
    case SUB:
      sub(&value_stack);
      pc += 1;
    case MUL:
      mul(&value_stack);
      pc += 1;
      break;
    case DIV:
      divide(&value_stack);
      pc += 1;
      break;
    case EQU:
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
    case CJP:
      if(pop(&value_stack)){
        pc = _FETCH_16BIT_POINTER(instruction);
      } else {
        pc += 3;
      }
      break;
    case EOF:
      Serial.println("Result: ");
      for(int i = 0; i < 8; i++) {
        Serial.print(memory[i], HEX);
        Serial.print("  ");
      }
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
