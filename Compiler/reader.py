def read(file_name):
    file = open(file_name, "r")
    code = file.read()
    file.close()
    return code