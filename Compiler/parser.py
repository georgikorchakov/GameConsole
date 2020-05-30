from error_reporting import error_token
from tokenizer import Token


class Statement:
    def __init__(self):
        pass

    def accept(self, visitor):
        raise NotImplementedError()


class ExpressionStatement(Statement):
    def __init__(self, expression):
        self.expression = expression

    def accept(self, visitor):
        visitor.visit_expression_statement(self)
    
    def __repr__(self):
        return f"({self.expression})"


class DeclarationStatement(Statement):
    def __init__(self, data_type, identifier):
        self.data_type = data_type
        self.identifier = identifier

    def accept(self, visitor):
        visitor.visit_declaration_statement(self)

    def __repr__(self):
        return f"({self.data_type.data_type.lexeme}[{self.data_type.length}] {self.identifier.lexeme})"


class BlockStatement(Statement):
    def __init__(self, statements, create_scope=True):
        self.statements = statements
        self.create_scope = create_scope
    
    def accept(self, visitor):
        visitor.visit_block_statement(self)

    def __repr__(self):
        return f"[{self.statements}]"


class IfStatement(Statement):
    def __init__(self, condition, block_if_true, elifs, block_if_false):
        self.condition = condition
        self.block_if_true = block_if_true
        self.elifs = elifs
        self.block_if_false = block_if_false
    
    def accept(self, visitor):
        visitor.visit_if_statement(self)

    def __repr__(self):
        return f"(if {self.condition} then {self.block_if_true} {self.elifs} else {self.block_if_false})"


class ElifStatement(Statement):
    def __init__(self, condition, block_if_true):
        self.condition = condition
        self.block_if_true = block_if_true
    
    def accept(self, visitor):
        visitor.visit_elif_statement(self)

    def __repr__(self):
        return f"(elif {self.condition} then {self.block_if_true}"


class WhileStatement(Statement):
    def __init__(self, condition, block):
        self.condition = condition
        self.block = block
    
    def accept(self, visitor):
        visitor.visit_while_statement(self)

    def __repr__(self):
        return f"(while {self.condition}  {self.block}"

class FunctionStatement(Statement):
    def __init__(self, data_type, name, parameters, body):
        self.data_type = data_type
        self.name = name
        self.parameters = parameters
        self.body = body

    def accept(self, visitor):
        visitor.visit_function_statement(self)
    
    def __repr__(self):
        return f"(function {self.name.lexeme} ({self.parameters}) {self.body})"


class Expression:
    def __init__(self):
        pass

    def accept(self, visitor):
        raise NotImplementedError()


class CalleeExpression(Expression):
    def __init__(self, callee, paren, arguments):
        self.callee = callee
        self.paren = paren
        self.arguments = arguments

    def accept(self, visitor):
        visitor.visit_callee_expression(self)

    def __repr__(self):
        return f"(callee {self.callee} {self.arguments})"


class AssignmentExpression(Expression):
    def __init__(self, variable, expression):
        self.variable = variable
        self.expression = expression

    def accept(self, visitor):
        visitor.visit_assignment_expression(self)
    
    def __repr__(self):
        return f"(assignement {self.variable} {self.expression})"


class BinaryExpression(Expression):
    def __init__(self, left, operator, right):
        self.left = left
        self.operator = operator
        self.right = right

    def accept(self, visitor):
        visitor.visit_binary_expression(self)
    
    def __repr__(self):
        return f"({self.operator.lexeme} {self.left} {self.right})"

class TypeExpression(Expression):
    def __init__(self, data_type, length=1):
        self.data_type = data_type
        self.length = length
    
    def accept(self, visitor):
        visitor.visit_type_expression(self)

    def __repr__(self):
        return f"({self.data_type} {self.length})"

class GroupingExpression(Expression):
    def __init__(self, expression):
        self.expression = expression

    def accept(self, visitor):
        visitor.visit_grouping_expression(self)
    
    def __repr__(self):
        return f"(group {self.expression})"


class LogicalExpression(Expression):
    def __init__(self, left, operator, right):
        self.left = left
        self.operator = operator
        self.right = right

    def accept(self, visitor):
        visitor.visit_logical_expression(self)
    
    def __repr__(self):
        return f"({self.operator.lexeme} {self.left} {self.right})"


class LiteralExpression(Expression):
    def __init__(self, literal):
        self.literal = literal

    def accept(self, visitor):
        visitor.visit_literal_expression(self)
    
    def __repr__(self):
        return repr(self.literal)

  
class VariableExpression(Expression):
    def __init__(self, variable):
        self.variable = variable

    def accept(self, visitor):
        visitor.visit_variable_expression(self)
    
    def __repr__(self):
        return self.variable.lexeme


class ListExpression(Expression):
    def __init__(self, expressions):
        self.expressions = expressions

    def accept(self, visitor):
        visitor.visit_list_expression(self)

    def __repr__(self):
        return repr(self.expressions)


class UnaryExpression(Expression):
    def __init__(self, operator, operand):
        self.operator = operator
        self.operand = operand

    def accept(self, visitor):
        visitor.visit_unary_expression(self)
    
    def __repr__(self):
        return f"({self.operator} {self.operand})"


class Parser:
    def __init__(self, tokens):
        self.tokens = tokens
        self.current = 0
        self.data_types = ["int", "uint", "long", "ulong", "char", "bool"]

    def parse(self):
        return self.parse_statements()

    def parse_statements(self):
        statements = []
        while not self.finished():
            statements.append(self.parse_statement())

        return statements
    
    def parse_statement(self):
        if self.match(*self.data_types):
            declaration = self.parse_declaration()

            if self.check("equal"):
                return self.var_declaration(declaration)

            if self.check("left_paren"):
                return self.function_declaration(declaration)

            self.consume("semicolon", "expected ';'")
            return declaration

        if self.match("if"):
            return self.if_declaration()

        if self.match("while"):
            return self.while_declaration()

        if self.match("for"):
            return self.for_declaration()

        if self.match("identifier"):
            expression =  self.expression_declaration()
            self.consume("semicolon", "expected ';'")
            return expression

    def parse_declaration(self):
        data_type = self.parse_data_type()
        identifier = self.consume("identifier", "expected identifier!")

        return DeclarationStatement(data_type, identifier)

    def var_declaration(self, declaration):
        self.consume("equal", "expected '='")
        value = self.parse_expression()
        self.consume("semicolon", "expected ';'")

        return BlockStatement([
            declaration,
            ExpressionStatement(
                AssignmentExpression(
                    VariableExpression(declaration.identifier),
                    value
                )
            )
            ], create_scope=False)

    def if_declaration(self):
        self.consume("left_paren", "expected '('")
        condition = self.parse_expression()
        self.consume("right_paren", "expected ')'")
        self.consume("left_brace", "expected '{'")
        block_if_true = self.parse_block()
        self.consume("right_brace", "expected '}'")

        elifs = []
        while self.match("elif"):
            self.consume("left_paren", "expected '('")
            elif_condition = self.parse_expression()
            self.consume("right_paren", "expected ')'")
            self.consume("left_brace", "expected '{'")
            block_elif = self.parse_block()
            self.consume("right_brace", "expected '}'")
            elifs.append(ElifStatement(elif_condition, block_elif))
        
        block_if_false = BlockStatement([])
        
        if self.match("else"):
            self.consume("left_brace", "expected '{'")
            block_if_false = self.parse_block()
            self.consume("right_brace", "expected '}'")

        return IfStatement(condition, block_if_true, elifs, block_if_false)

    def while_declaration(self):
        self.consume("left_paren", "expected '('")
        condition = self.parse_expression()
        self.consume("right_paren", "expected ')'")
        self.consume("left_brace", "expected '{'")
        block = self.parse_block()
        self.consume("right_brace", "expected '}'")

        return WhileStatement(condition, block)

    def expression_declaration(self):
        identifier = self.previous()
        variable_expression = VariableExpression(identifier)
        if self.match("equal"):
            expression = self.parse_expression()
            assignment_expression = AssignmentExpression(variable_expression, expression)
            return ExpressionStatement(assignment_expression)
        elif self.match("left_paren"):
            callee_expression = self.finish_call(variable_expression)
            return ExpressionStatement(callee_expression)

    def for_declaration(self):
        self.consume("left_paren", "expected '('")

        if self.match("semicolon"):
            initializer = None
        elif self.match(*self.data_types):
            declaration = self.parse_declaration()
            initializer = self.var_declaration(declaration)
        else:
            initializer = self.expression_declaration()

        condition = None
        if not self.match("semicolon"):
            condition = self.parse_expression()
        self.consume("semicolon", "expected ';'")

        increment = None
        if not self.check("right_paren"):
            self.advance()
            increment = self.expression_declaration()
        self.consume("right_paren", "expected ')'")

        self.consume("left_brace", "expected '{'")
        body = self.parse_block()
        self.consume("right_brace", "expected '}'")

        if increment != None:
            body.statements.append(increment)

        if condition == None:
            condition = LiteralExpression(1)
        body = WhileStatement(condition, body)

        if initializer != None:
            body = BlockStatement([initializer, body])

        return body

    def function_declaration(self, declaration):
        self.consume("left_paren", "expected '(' after function name!")

        parameters = []
        if not self.check("right_paren"):
            while True:
                if len(parameters) >= 8:
                    error_token(self.peek(), "function can't have more than 8 arguments!")

                if self.match(*self.data_types):
                    parameters.append(self.parse_declaration())

                elif not self.match("comma"):
                    break
                    
        self.consume("right_paren", "expected ')' after function parameters!")
        self.consume("left_brace", "expected '{'")
        body = self.parse_block()
        self.consume("right_brace", "expected '}'")

        return FunctionStatement(declaration.data_type, declaration.identifier, parameters, body)
    
    def parse_data_type(self):
        data_type = self.previous()

        if self.match("left_square"):
            length = self.consume("number", "expected number!")
            self.consume("right_square", "expected ']'!")
            return TypeExpression(data_type, length=length.literal)

        return TypeExpression(data_type)

    def parse_block(self):
        statements = []
        while not self.check("end") and not self.check("else") and not self.check("right_brace") and not self.finished():
            statements.append(self.parse_statement())

        return BlockStatement(statements)

    def parse_expression(self):
        return self.parse_or()

    def parse_or(self):
        expression = self.parse_and()

        while self.match("or"):
            operator = self.previous()
            right = self.parse_and()
            expression = LogicalExpression(expression, operator, right)

        return expression

    def parse_and(self):
        expression = self.parse_equality()

        while self.match("and"):
            operator = self.previous()
            right = self.parse_equality()
            expression = BinaryExpression(expression, operator, right)

        return expression

    def parse_equality(self):
        expression = self.parse_comparison()

        while self.match("bang_equal", "equal_equal"):
            operator = self.previous()
            right = self.parse_comparison()
            expression = BinaryExpression(expression, operator, right)

        return expression

    def parse_comparison(self):
        expression = self.parse_addition()

        while self.match("greater_equal", "greater", "less_equal", "less"):
            operator = self.previous()
            right = self.parse_addition()
            expression = BinaryExpression(expression, operator, right)

        return expression

    def parse_addition(self):
        expression = self.parse_multiplication()

        while self.match("plus", "minus"):
            operator = self.previous()
            right = self.parse_multiplication()
            expression = BinaryExpression(expression, operator, right)

        return expression
    
    def parse_multiplication(self):
        expression = self.parse_unary()

        while self.match("star", "slash"):
            operator = self.previous()
            right = self.parse_unary()
            expression = BinaryExpression(expression, operator, right)

        return expression

    def parse_unary(self):
        if self.match("bang", "minus"):
            operator = self.previous()
            right = self.parse_unary()
            return UnaryExpression(operator, right)

        return self.parse_call()

    def parse_call(self):
        expression = self.parse_primary()

        while self.match("left_paren"):
            expression = self.finish_call(expression)
        
        return expression

    def finish_call(self, callee):
        arguments = []
        if not self.check("right_paren"):
            while True:
                if len(arguments) >= 8:
                    error_token(self.peek(), "function can't have more than 8 arguments!")

                arguments.append(self.parse_expression())

                if not self.match("comma"):
                    break

        paren = self.consume("right_paren", "expected ')'")

        return CalleeExpression(callee, paren, arguments)

    def parse_primary(self):
        if self.match("number"):
            return LiteralExpression(self.previous().literal)

        if self.match("identifier"):
            return VariableExpression(self.previous())

        if self.match("True"):
            return LiteralExpression(1)
        
        if self.match("False"):
            return LiteralExpression(0)

        if self.match("left_paren"):
            expression = self.parse_expression()
            self.consume("right_paren", "expect ')' after expression")
            return GroupingExpression(expression)
        
        if self.match("left_square"):
            expressions = []
            if self.match("right_square"):
                return ListExpression(expressions)
            
            while True:
                expressions.append(
                    self.parse_expression()
                )

                if not self.match("comma"):
                    break

            self.consume("right_square", "expected ']'!")
            return ListExpression(expressions)


        error_token(self.previous(), "expected expression")
        raise Exception()
    
    # Check for token type and update current or raise exception
    def consume(self, token_type, message):
        if self.check(token_type):
            return self.advance()
        
        error_token(self.peek(), message)
        raise Exception()

    def previous(self):
        return self.tokens[self.current - 1]

    # Check for match in tokens and update current
    def match(self, *types):
        for token_type in types:
            if self.check(token_type):
                self.advance()
                return True

        return False

    # Check if current token type is equal to given token type
    def check(self, token_type):
        if self.finished():
            return False

        return self.peek().type == token_type

    def finished(self):
        return self.peek().type == "EOF"

    # Return current token
    def peek(self):
        return self.tokens[self.current]

    # Update current and return previous token
    def advance(self):
        if not self.finished():
            self.current += 1
        
        return self.previous()
