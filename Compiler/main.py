from tokenizer import Tokenizer
from parser import Parser
from reader import read
import sys

if __name__ == "__main__":
    if len(sys.argv) == 1:
        print("Missing code file!\nTry: python3 main.py source.gcl")
        exit()

    code = read(sys.argv[1])

    tokenizer = Tokenizer(code, 0)
    tokens = tokenizer.scan()

    from pprint import pprint
    pprint(tokens)

    parser = Parser(tokens)
    tree = parser.parse()

    pprint(tree)