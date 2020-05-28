#define STACK_LENGTH 128
#define MEMORY_SIZE  256
#define PROGRAM_SIZE 0x5F

typedef struct stack_t {
  uint8_t stack[STACK_LENGTH];
  uint8_t top;
} stack_t;

stack_t value_stack;

uint8_t memory[MEMORY_SIZE];
uint16_t pc = 0;

uint8_t program[PROGRAM_SIZE] = {
  1, 5, 4, 0, 0, 1, 7, 4, 1, 0, 1, 10, 2, 1, 0, 6, 2, 0, 0, 1, 4, 6, 8, 4, 2, 0, 2, 0, 0, 2, 1, 0, 13, 17, 48, 0, 2, 0, 0, 1, 5, 12, 17, 81, 0, 16, 89, 0, 2, 0, 0, 1, 5, 10, 17, 60, 0, 16, 78, 0, 2, 2, 0, 4, 0, 0, 2, 0, 0, 1, 1, 6, 4, 0, 0, 16, 78, 0, 16, 95, 0, 1, 5, 4, 0, 0, 16, 95, 0, 2, 2, 0, 4, 0, 0
};

void setup() {
  init_stack(&value_stack);
  Serial.begin(9600);
}

void loop() {
  uint8_t * instruction = program + pc;
  Serial.println(*instruction, HEX);
  switch(*instruction) {
    case 0x01:
      push(&value_stack, *(instruction + 1));
      pc += 2;
      break;
    case 0x02:
      push(&value_stack, memory[*((uint16_t*)(instruction + 1))]);
      pc += 3;
      break;
    case 0x03:
      push(&value_stack, memory[*((uint16_t*)(instruction + 1)) + *(instruction + 3)]);
      pc += 4;
      break;
      
    case 0x04:
      memory[*((uint16_t*)(instruction + 1))] = pop(&value_stack);
      pc += 3;
      break;
    case 0x05:
      memory[*((uint16_t*)(instruction + 1)) + *(instruction + 3)] = pop(&value_stack);
      pc += 4;
      break;  
      
    case 0x06:
      add(&value_stack);
      pc += 1;
      break;
    case 0x07:
      sub(&value_stack);
      pc += 1;
    case 0x08:
      mul(&value_stack);
      pc += 1;
      break;
    case 0x09:
      divide(&value_stack);
      pc += 1;
      break;

    case 0x0A:
      equal(&value_stack);
      pc += 1;
      break;
    case 0x0B:
      not_equal(&value_stack);
      pc += 1;
      break;
    case 0x0C:
      greater(&value_stack);
      pc += 1;
      break;
    case 0x0D:
      less(&value_stack);
      pc += 1;
      break;
    case 0x0E:
      greater_equal(&value_stack);
      pc += 1;
      break;
    case 0x0F:
      less_equal(&value_stack);
      pc += 1;
      break;

    case 0x10:
      pc = *((uint16_t*)(instruction + 1));
      break;
    case 0x11:
      if(pop(&value_stack)){
        pc = *((uint16_t*)(instruction + 1));
      } else {
        pc += 3;
      }
    
  }
  
  if (pc >= PROGRAM_SIZE) {
    Serial.println("Result: ");
    for(int i = 0; i < 8; i++) {
      Serial.print(memory[i], HEX);
      Serial.print("  ");
    }
    while(true);
  }
  
}

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
