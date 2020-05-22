HAD_ERROR = False

def report(line, where, message):
    global HAD_ERROR
    HAD_ERROR = True
    print(f"[line {line}] Error {where}: {message}")

def error(line, message):
    report(line, "", message)

def error_token(token, message):
    if token.type == "EOF":
        report(token.line, "at end", message)
    else:
        report(token.line, f"at '{token.lexeme}'", message)