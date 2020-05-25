from error_reporting import error

class Token:
    def __init__(self, token_type, lexeme, literal, line):
        self.type = token_type
        self.lexeme = lexeme
        self.literal = literal
        self.line = line
    
    def __repr__(self):
        return f"<Token {self.type} {self.lexeme} {self.literal}>"

class Tokenizer:
    keywords = [
        "var",
        "if", "elif", "else",
        "while", "for",
        "True", "False"
    ]

    def __init__(self, source, line):
        self.source = source
        self.line = line

        self.start = 0
        self.current = 0
        self.tokens = []
    
    def scan(self):
        while not self.finished():
            self.start = self.current
            self.next_token()
        
        self.tokens.append(Token("EOF", "", None, self.line))
        return self.tokens

    def finished(self):
        return self.current >= len(self.source)
    
    def add_token(self, token_type, literal=None):
        self.tokens.append(Token(token_type, self.source[self.start:self.current], literal, self.line))

    def match(self, expected):
        if self.finished():
            return False

        if self.source[self.current] != expected:
            return False
        
        self.current += 1
        return True

    def peek(self):
        if self.finished():
            return "\0"

        return self.source[self.current]

    def number(self):
        while self.peek().isnumeric():
            self.current += 1
        
        number_literal = int(self.source[self.start:self.current])

        self.add_token("number", number_literal)

    def identifier(self):
        while self.peek().isalnum():
            self.current += 1

        text = self.source[self.start:self.current]

        if text in self.keywords:
            self.add_token(text)
        else:
            self.add_token("identifier")

    def next_token(self):
        c = self.source[self.current]
        self.current += 1

        if c == "(":
            self.add_token("left_paren")
        elif c == ")":
            self.add_token("right_paren")
        elif c == "{":
            self.add_token("left_brace")
        elif c == "}":
            self.add_token("right_brace")
        elif c == "+":
            self.add_token("plus")
        elif c == "-":
            self.add_token("minus")
        elif c == "*":
            self.add_token("star")
        elif c == "/":
            self.add_token("slash")
        elif c == ",":
            self.add_token("comma")
        elif c == ";":
            self.add_token("semicolon")
        elif c == "=":
            if self.match("="): # ==
                self.add_token("equal_equal")
            else:
                self.add_token("equal")
        elif c == "|":
            if self.match("|"): # ||
                self.add_token("or")
            else:
                error(self.line, "expected '||', got '|'")
        elif c == "&":
            if self.match("&"): # &&
                self.add_token("and")
            else:
                error(self.line, "expected '||', got '|'")
        elif c == "!":
            if self.match("="): # !=
                self.add_token("bang_equal")
            else:
                self.add_token("bang")
        elif c == ">":
            if self.match("="): # >=
                self.add_token("greater_equal")
            else:
                self.add_token("greater")
        elif c == "<":
            if self.match("="): # <=
                self.add_token("less_equal")
            else:
                self.add_token("less")
        elif c == "\n":
            self.line += 1
        elif c in [" ", "\t", "\r"]:
            pass
        elif c == "#":
            while self.peek() != "\n" and not self.finished():
                self.current += 1
        else:
            if c.isnumeric():
                self.number()
            elif c.isalpha():
                self.identifier()
            else:
                error(self.line, "unexpected character")

if __name__ == "__main__":
    tokenizer = Tokenizer("var a = (5+6)*10-5*(3+2)", 0)
    tokens = tokenizer.scan()

    from pprint import pprint
    pprint(tokens)