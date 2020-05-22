from tokenizer import Tokenizer
from parser import Parser

if __name__ == "__main__":
    tokenizer = Tokenizer('''
        # This is a test program
        var a = 5
        var b = (a + 5) * (2 + 2)

        if (a < b) {
            a = b
        } elif (a > 5) {
            a = 5
        } 

        if (a == b) {
            b = 0
        } else {
            a = 0
        }

    ''', 0)
    tokens = tokenizer.scan()

    from pprint import pprint
    pprint(tokens)

    parser = Parser(tokens)
    tree = parser.parse()

    pprint(tree)