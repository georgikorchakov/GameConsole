from opcodes import *
from error_reporting import error
from parser import VariableExpression, LiteralExpression

class Variable:
    def __init__(self, data_type, name, address):
        self.data_type = data_type
        self.name = name
        self.address = address
    
    def get_length(self):
        if self.data_type in ["int", "uint", "char", "bool"]:
            return 1

    def __repr__(self):
        return f"({self.name}: addr {self.address})"


class Function:
    def __init__(self, data_type, name, address, number_of_declarations, number_of_scopes):
        self.data_type = data_type
        self.name = name
        self.address = address
        self.number_of_declarations = number_of_declarations
        self.number_of_scopes = number_of_scopes

    def __repr__(self):
        return f"{self.data_type} {self.name}(Declarations:{self.number_of_declarations}) Addr:{self.address}"


class Compiler:
    def __init__(self, statements):
        self.statements = statements
        self.bytecode = []

        self.current_address = 0x00
        self.variables = [{}]
        self.stack_top = 0

        self.functions = {}
        self.last_function = []

        self.predefined_void_functions = {
            "fill_rect": FILL_RECT,
            "display": DISPLAY,
            "clear": CLEAR,
            "delay": DELAY
        }

        self.predefined_uint_functions = {
            "read_left_button": READ_LEFT_BUTTON,
            "read_right_button": READ_RIGHT_BUTTON,
        }

    def compile(self):
        for statement in self.statements:
            statement.accept(self)
        
        self.bytecode.append(EOF)
        return self.bytecode

    def add_variable(self, data_type, name):
        self.variables[-1][name] = Variable(data_type, name, self.stack_top)
        self.stack_top += 1
        #self.stack_top += self.variables[name].get_length()

    def get_var_address(self, name):
        for scope_num in range(len(self.variables)):
            scope = self.variables[-(scope_num + 1)]

            if name in scope:
                return self.stack_top - scope[name].address

        raise Exception(f"Variable '{name}' not found!")

    def visit_expression_statement(self, statement):
        statement.expression.accept(self)

    def visit_declaration_statement(self, statement):
        self.bytecode.append(DECL)
        self.add_variable(statement.data_type.data_type.lexeme, statement.identifier.lexeme)

        if len(self.last_function) > 0:
            if self.last_function[-1].number_of_scopes == len(self.variables):
                self.last_function[-1].number_of_declarations += 1

    def visit_block_statement(self, statement):
        if statement.create_scope == True:
            self.variables.append({})
        
        for stmt in statement.statements:
            stmt.accept(self)
        
        if statement.create_scope == True:
            for _ in self.variables.pop():
                self.bytecode.append(POP)
                self.stack_top -= 1
            
    def visit_if_statement(self, statement):
        empty_jump_possitions = []

        # Put first condition in bytecode
        statement.condition.accept(self)

        # Append operation number in jump_address (position 0)
        empty_jump_possitions.append(len(self.bytecode))

        # Append first conditional jump
        self.append_empty_conditional_jump()

        # Go trought all elif statements and put conditions in bytecode
        for elif_statement in statement.elifs:
            # Append condition in bytecode
            elif_statement.condition.accept(self)

            # Append elif condition jump possition
            empty_jump_possitions.append(len(self.bytecode))

            # Append elif condition jump
            self.append_empty_conditional_jump()
        
        # Append else JMP
        empty_jump_possitions.append(len(self.bytecode))
        self.append_empty_jump()

        self.add_current_position_to_empty_jump(empty_jump_possitions[0])
        del empty_jump_possitions[0]
        statement.block_if_true.accept(self)
        empty_jump_possitions.append(len(self.bytecode))
        self.append_empty_jump()

        for elif_statement in statement.elifs:
            self.add_current_position_to_empty_jump(empty_jump_possitions[0])
            del empty_jump_possitions[0]
            elif_statement.block_if_true.accept(self)
            empty_jump_possitions.append(len(self.bytecode))

            self.append_empty_jump()

        self.add_current_position_to_empty_jump(empty_jump_possitions[0])

        del empty_jump_possitions[0]
        statement.block_if_false.accept(self)

        for address in empty_jump_possitions:
            self.add_current_position_to_empty_jump(address)
        
        empty_jump_possitions = []
    
    def add_current_position_to_empty_jump(self, address):
        self.bytecode[address + 1] = len(self.bytecode) & 0x00FF
        self.bytecode[address + 2] = (len(self.bytecode) & 0xFF00) >> 8

    def append_empty_conditional_jump(self):
        self.bytecode.append(CJP)
        self.bytecode.append(0x00)
        self.bytecode.append(0x00)
        self.stack_top -= 1
    
    def append_empty_jump(self):
        self.bytecode.append(JMP)
        self.bytecode.append(0x00)
        self.bytecode.append(0x00)

    def visit_while_statement(self, statement):
        condition_start_address = len(self.bytecode)
        statement.condition.accept(self)

        self.bytecode.append(CJP)
        self.bytecode.append((len(self.bytecode) + 5) & 0x00FF)
        self.bytecode.append(((len(self.bytecode) + 5) & 0xFF00) >> 8)

        self.stack_top -= 1

        end_jmp = len(self.bytecode)
        self.append_empty_jump()

        statement.block.accept(self)
        self.bytecode.append(JMP)
        self.bytecode.append(condition_start_address & 0x00FF)
        self.bytecode.append((condition_start_address & 0xFF00) >> 8)

        self.add_current_position_to_empty_jump(end_jmp)


    def visit_function_statement(self, statement):
        jump_before_function_start = len(self.bytecode)
        self.append_empty_jump()

        self.variables.append({})
        self.functions[statement.name.lexeme] = Function(statement.data_type.data_type.lexeme, statement.name.lexeme, len(self.bytecode), 0, len(self.variables))

        for parameter in statement.parameters:
            self.add_variable(parameter.data_type.data_type.lexeme, parameter.identifier.lexeme)
            self.functions[statement.name.lexeme].number_of_declarations += 1

        self.last_function.append(self.functions[statement.name.lexeme])   
        statement.body.accept(self)
        self.functions[statement.name.lexeme] = self.last_function.pop()

        if self.functions[statement.name.lexeme].data_type == "void":
            for _ in range(self.functions[statement.name.lexeme].number_of_declarations):
                self.bytecode.append(POP)
                self.stack_top -= 1
    
            self.bytecode.append(JST)
        else:
            self.stack_top -= self.functions[statement.name.lexeme].number_of_declarations

        self.add_current_position_to_empty_jump(jump_before_function_start)
        self.variables.pop()

    def visit_return_statement(self, statement):
        statement.expression.accept(self)

        self.bytecode.append(STR)
        self.stack_top -= 1

        for _ in range(self.last_function[-1].number_of_declarations):
            self.bytecode.append(POP)
    
        self.bytecode.append(JST)    
        
    def visit_callee_expression(self, expression):
        if expression.callee.variable.lexeme not in self.predefined_void_functions and expression.callee.variable.lexeme not in self.predefined_uint_functions:
            empty_16bit_jump = len(self.bytecode)
            self.push_to_stack_16bit(0)

            self.stack_top += 2
            for argument in expression.arguments:
                argument.accept(self)
            self.stack_top -= 2

            self.bytecode.append(JMP)
            self.bytecode.append(self.functions[expression.callee.variable.lexeme].address & 0x00FF)
            self.bytecode.append((self.functions[expression.callee.variable.lexeme].address & 0xFF00) >> 8)

            self.stack_top -= len(expression.arguments)

            self.bytecode[empty_16bit_jump + 1] = len(self.bytecode) & 0x00FF
            self.bytecode[empty_16bit_jump + 3] = (len(self.bytecode) & 0xFF00) >> 8

            if self.functions[expression.callee.variable.lexeme].data_type != "void":
                self.bytecode.append(RET)
                self.stack_top += 1
        else:
            for argument in expression.arguments:
                argument.accept(self)

            self.stack_top -= len(expression.arguments)

            if expression.callee.variable.lexeme in self.predefined_uint_functions:
                self.stack_top += 1
                self.bytecode.append(self.predefined_uint_functions[expression.callee.variable.lexeme])
            else:
                self.bytecode.append(self.predefined_void_functions[expression.callee.variable.lexeme])

    def push_to_stack_16bit(self, var):
        self.bytecode.append(PUSH_IMMEDIATE)
        self.bytecode.append(var & 0x00FF)
        self.bytecode.append(PUSH_IMMEDIATE)
        self.bytecode.append((var & 0xFF00) >> 8)

    def visit_assignment_expression(self, expression):
        expression.expression.accept(self)
        self.bytecode.append(POP_ABSOLUTE)
        self.stack_top -= 1

        address = self.get_var_address(expression.variable.variable.lexeme)
        self.bytecode.append(address & 0x00FF)
        self.bytecode.append((address & 0xFF00) >> 8)

    def visit_binary_expression(self, expression):
        expression.left.accept(self)
        expression.right.accept(self)

        if expression.operator.type == "plus":
            self.bytecode.append(ADD)
        elif expression.operator.type == "minus":
            self.bytecode.append(SUB)
        elif expression.operator.type == "slash":
            self.bytecode.append(DIV)
        elif expression.operator.type == "star":
            self.bytecode.append(MUL)
        elif expression.operator.type == "equal_equal":
            self.bytecode.append(EQU)
        elif expression.operator.type == "bang_equal":
            self.bytecode.append(NEQ)
        elif expression.operator.type == "greater":
            self.bytecode.append(GRT)
        elif expression.operator.type == "less":
            self.bytecode.append(LES)
        elif expression.operator.type == "greater_equal":
            self.bytecode.append(GEQ)
        elif expression.operator.type == "less_equal":
            self.bytecode.append(LEQ)
        
        self.stack_top -= 1
        
    def visit_type_expression(self, expression):
        pass

    def visit_grouping_expression(self, expression):
        expression.expression.accept(self)

    def visit_logical_expression(self, expression):
        pass

    def visit_literal_expression(self, expression):
        self.bytecode.append(PUSH_IMMEDIATE)
        self.stack_top += 1

        if expression.literal > 255:
            error("UNKNOWN", "Literal exceedes maximum value!")
            raise ValueError()

        self.bytecode.append(expression.literal)

    def visit_variable_expression(self, expression):
        self.bytecode.append(PUSH_ABSOLUTE)
        address = self.get_var_address(expression.variable.lexeme)
        self.stack_top += 1
        self.bytecode.append(address & 0x00FF)
        self.bytecode.append((address & 0xFF00) >> 8)

    def visit_list_expression(self, expression):
        pass

    def visit_unary_expression(self, expression):
        pass

    def __repr__(self):
        def to_eight_bit_hex(number):
            hex_number = hex(number)
            list_hex = list(hex_number)
            if len(list_hex) == 3:
                return "" + list_hex[0] + list_hex[1] + "0" + list_hex[2]
            
            return hex_number

        def get_command_number(i):
            return to_eight_bit_hex(i & 0x00FF) + " " + to_eight_bit_hex((i & 0xFF00) >> 8) + ":  "

        string = "------\n"

        i = 0
        while i != len(self.bytecode):
            opcode = self.bytecode[i]

            if opcode == PUSH_IMMEDIATE:
                string += get_command_number(i) + "PUSH_IMMEDIATE " + "const(" + to_eight_bit_hex(self.bytecode[i + 1]) + ")\n"
                i = i + 1
            elif opcode == PUSH_ABSOLUTE:
                string += get_command_number(i) + "PUSH_ABSOLUTE " + "address(" + to_eight_bit_hex(self.bytecode[i + 1]) + " " + to_eight_bit_hex(self.bytecode[i + 2]) + ")\n"
                i = i + 2
            elif opcode == POP_ABSOLUTE:
                string += get_command_number(i) + "POP_ABSOLUTE " + "address(" + to_eight_bit_hex(self.bytecode[i + 1]) + " " + to_eight_bit_hex(self.bytecode[i + 2]) + ")\n"
                i = i + 2
            elif opcode == JMP:
                string += get_command_number(i) + "JMP " + "PC(" + to_eight_bit_hex(self.bytecode[i + 1]) + " " + to_eight_bit_hex(self.bytecode[i + 2]) + ")\n"
                i = i + 2
            elif opcode == CJP:
                string += get_command_number(i) + "CJP " + "PC(" + to_eight_bit_hex(self.bytecode[i + 1]) + " " + to_eight_bit_hex(self.bytecode[i + 2]) + ")\n"
                i = i + 2
            elif opcode == DECL:
                string += get_command_number(i) + "DECL\n"
            elif opcode == STR:
                string += get_command_number(i) + "STR\n"
            elif opcode == JST:
                string += get_command_number(i) + "JST\n"
            elif opcode == RET:
                string += get_command_number(i) + "RET\n"
            elif opcode == POP:
                string += get_command_number(i) + "POP\n"
            elif opcode == ADD:
                string += get_command_number(i) + "ADD\n"
            elif opcode == SUB:
                string += get_command_number(i) + "SUB\n"
            elif opcode == MUL:
                string += get_command_number(i) + "MUL\n"
            elif opcode == DIV:
                string += get_command_number(i) + "DIV\n"
            elif opcode == EQU:
                string += get_command_number(i) + "EQU\n"
            elif opcode == NEQ:
                string += get_command_number(i) + "NEQ\n"
            elif opcode == GRT:
                string += get_command_number(i) + "GRT\n"
            elif opcode == LES:
                string += get_command_number(i) + "LES\n"
            elif opcode == GEQ:
                string += get_command_number(i) + "GEQ\n"
            elif opcode == LEQ:
                string += get_command_number(i) + "LEQ\n"
            elif opcode == FILL_RECT:
                string += get_command_number(i) + "FILL_RECT\n"
            elif opcode == DISPLAY:
                string += get_command_number(i) + "DISPLAY\n"
            elif opcode == CLEAR:
                string += get_command_number(i) + "CLEAR\n"
            elif opcode == DELAY:
                string += get_command_number(i) + "DELAY\n"
            elif opcode == READ_LEFT_BUTTON:
                string += get_command_number(i) + "READ_LEFT_BUTTON\n"
            elif opcode == READ_RIGHT_BUTTON:
                string += get_command_number(i) + "READ_RIGHT_BUTTON\n"
            elif opcode == EOF:
                string += get_command_number(i) + "EOF\n"
            else:
                string += get_command_number(i) + str(opcode)
                
            i = i + 1

        return string