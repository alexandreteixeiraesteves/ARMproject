#!/usr/bin/python

import sys


def send_esp(password):
    print("Send password: " + password + " to esp32")
    return True


def send_file_esp(file):
    if not send_esp(file):
        return ""
    #TODO USB CONNECTION
    crypt = "test"
    return crypt


def filehandler(inputfile, outputfile):
    f = open(inputfile, "r")
    result = send_file_esp(f.read())
    d = open(outputfile, "w")
    d.write(result)


def unlock_mode(password):
    print("Send password : " + password)


if __name__ == '__main__':
    state = 0
    print(len(sys.argv))
    if sys.argv[1] == "init" or sys.argv[1] == "unlock":
        if len(sys.argv) != 3:
            exit(1)
        if send_esp(sys.argv[2]):
            print("OK")

    elif sys.argv[1] == "encrypt":
        if len(sys.argv) != 4:
            exit(1)
        filehandler(sys.argv[2], sys.argv[3])
    elif sys.argv[1] == "decrypt":
        if len(sys.argv) != 4:
            exit(1)
        filehandler(sys.argv[3], sys.argv[2])
    else:
        print("Unknown mode exiting with value 2")
        exit(2)
    exit(0)
