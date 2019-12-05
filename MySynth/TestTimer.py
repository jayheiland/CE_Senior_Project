import threading

def printit():
  threading.Timer(5.0, printit).start()
  print("Hello, World!")

printit()

quit = False
while(quit != True):
    strn = input("Input something\n")
    if(strn != "go"):
        quit = True


print("Exited input loop")