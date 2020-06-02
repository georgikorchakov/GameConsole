EOF = 0x00
PUSH_IMMEDIATE = 0x01
PUSH_ABSOLUTE  = 0x02
PUSH_RELATIVE  = 0x03

POP_ABSOLUTE   = 0x04
POP_RELATIVE   = 0x05

ADD = 0x06
SUB = 0x07
MUL = 0x08
DIV = 0x09

EQU = 0x0A
NEQ = 0x0B
GRT = 0x0C
LES = 0x0D
GEQ = 0x0E
LEQ = 0x0F

JMP = 0x10
CJP = 0x11

POP = 0x12
DECL = 0x13

JST = 0x14  # Jump from stack

RET = 0x15  # Push rv register on the top of the stack
STR = 0x16  # Store last element in rv register

# Predefined void functions
FILL_RECT = 0x50
DISPLAY = 0x51
CLEAR = 0x52
DELAY = 0x53

# Predefined uint functions
READ_LEFT_BUTTON = 0x60
READ_RIGHT_BUTTON = 0x61