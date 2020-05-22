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
        "VAR", "SET", "IF", "ELSEIF", "ELSE", "END"
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

        self.add_token("NUMBER", number_literal)

    def identifier(self):
        while self.peek().isalnum():
            self.current += 1

        text = self.source[self.start:self.current]

        if text in self.keywords:
            self.add_token(text)
        else:
            self.add_token("IDENTIFIER")

    def next_token(self):
        c = self.source[self.current]
        self.current += 1

        if c == "(":
            self.add_token("LEFT_PAREN")
        elif c == ")":
            self.add_token("RIGHT_PAREN")
        elif c == "+":
            self.add_token("PLUS")
        elif c == "-":
            self.add_token("MINUS")
        elif c == "*":
            self.add_token("STAR")
        elif c == "/":
            self.add_token("SLASH")
        elif c == "=":
            if self.match("="): # ==
                self.add_token("EQUAL_EQUAL")
            else:
                error(self.line, "expected '==', got '='")
        elif c == "|":
            if self.match("|"): # ||
                self.add_token("OR")
            else:
                error(self.line, "expected '||', got '|'")
        elif c == "&":
            if self.match("&"): # &&
                self.add_token("AND")
            else:
                error(self.line, "expected '||', got '|'")
        elif c == "!":
            if self.match("="): # !=
                self.add_token("BANG_EQUAL")
            else:
                self.add_token("BANG")
        elif c == ">":
            if self.match("="): # >=
                self.add_token("GREATER_EQUAL")
            else:
                self.add_token("GREATER")
        elif c == "<":
            if self.match("="): # <=
                self.add_token("LESS_EQUAL")
            else:
                self.add_token("LESS")
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
    tokenizer = Tokenizer("#coment\nVAR a 10*(x - 23)\nVAR b 4\nIF a > 3\nVAR pi 3\nVAR s 10\n END", 0)
    tokens = tokenizer.scan()

    from pprint import pprint
    pprint(tokens)