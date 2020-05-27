from opcodes import *
from error_reporting import error

class Variable:
    def __init__(self, data_type, name, address):
        self.data_type = data_type
        self.name = name
        self.address = address
    
    def get_length(self):
        if self.data_type in ["int", "uint", "char", "bool"]:
            return 1


class Compiler:
    def __init__(self, statements):
        self.statements = statements
        self.currnet_address = 0x00
        self.variables = {}
        self.bytecode = []

    def compile(self):
        for statement in self.statements:
            print(statement)
            statement.accept(self)
        
        return self.bytecode

    def add_variable(self, data_type, name):
        self.variables[name] = Variable(data_type, name, self.currnet_address)
        self.currnet_address += self.variables[name].get_length()

    def get_var_address(self, name):
        return self.variables[name].address

    def visit_expression_statement(self, statement):
        statement.expression.accept(self)

    def visit_declaration_statement(self, statement):
        self.add_variable(statement.data_type.data_type.lexeme, statement.identifier.lexeme)

    def visit_block_statement(self, statement):
        for stmt in statement.statements:
            stmt.accept(self)

    def visit_if_statement(self, statement):
        statement.condition.accept(self)
        self.bytecode.append(CJP)

    def visit_elif_statement(self, statement):
        pass

    def visit_while_statement(self, statement):
        pass

    def visit_function_statement(self, statement):
        pass

    def visit_callee_expression(self, expression):
        pass

    def visit_assignment_expression(self, expression):
        expression.expression.accept(self)
        self.bytecode.append(POP_ABSOLUTE)

        address = self.get_var_address(expression.variable.variable.lexeme)
        self.bytecode.append((address & 0xFF00) >> 8)
        self.bytecode.append(address & 0x00FF)

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
        elif expression.operator.type == "equal":
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
        
    def visit_type_expression(self, expression):
        pass

    def visit_grouping_expression(self, expression):
        expression.expression.accept(self)

    def visit_logical_expression(self, expression):
        pass

    def visit_literal_expression(self, expression):
        self.bytecode.append(PUSH_IMMEDIATE)

        if expression.literal > 255:
            error("UNKNOWN", "Literal exceedes maximum value!")
            raise ValueError()

        self.bytecode.append(expression.literal)

    def visit_variable_expression(self, expression):
        self.bytecode.append(PUSH_ABSOLUTE)
        address = self.get_var_address(expression.variable.lexeme)
        self.bytecode.append((address & 0xFF00) >> 8)
        self.bytecode.append(address & 0x00FF)

    def visit_list_expression(self, expression):
        pass

    def visit_unary_expression(self, expression):
        pass

    def __repr__(self):
        string = "------\nVariables\n"
        for var_name in self.variables:
            string += str(var_name) + " = " + str(hex((self.variables[var_name].address & 0xFF00) >> 8)) + " " + str(hex(self.variables[var_name].address & 0x00FF)) + "\n"
        string += "------\n"

        i = 0
        while i != len(self.bytecode):
            opcode = self.bytecode[i]

            if opcode == PUSH_IMMEDIATE:
                string += "PUSH_IMMEDIATE " + "const(" + hex(self.bytecode[i + 1]) + ")\n"
                i = i + 1
            elif opcode == PUSH_ABSOLUTE:
                string += "PUSH_ABSOLUTE " + "address(" + hex(self.bytecode[i + 1]) + " " + hex(self.bytecode[i + 2]) + ")\n"
                i = i + 2
            elif opcode == POP_ABSOLUTE:
                string += "POP_ABSOLUTE " + "address(" + hex(self.bytecode[i + 1]) + " " + hex(self.bytecode[i + 2]) + ")\n"
                i = i + 2
            elif opcode == ADD:
                string += "ADD\n"
            elif opcode == SUB:
                string += "SUB\n"
            elif opcode == MUL:
                string += "MUL\n"
            elif opcode == DIV:
                string += "DIV\n"
            elif opcode == EQU:
                string += "EQU\n"
            elif opcode == NEQ:
                string += "NEQ\n"
            elif opcode == GRT:
                string += "GRT\n"
            elif opcode == LES:
                string += "LES\n"
            elif opcode == GEQ:
                string += "GEQ\n"
            elif opcode == LEQ:
                string += "LEQ\n"
            # elif opcode == JMP:
            # elif opcode == CJP:
            i = i + 1

        return string