import os

def run():
    folder = "end-to-end-cases"
    testFileNames = os.listdir(folder)
    # For each test case.
    for fileName in testFileNames:
        with open(folder + "/" + fileName, encoding='utf-8') as file:
            # Read the test case file.
            contents = file.read()
            programCode, expectedPrints = contents.split("\n!expect!\n")
            # Write the program code into its own file so the compiler can read it.
            with open(folder + "/_program.mylang", "w", encoding='utf-8') as programFile:
                programFile.write(programCode)
            # Compile the corresponding program from the program code.
            os.popen("cd .. && ./main.out test/end-to-end-cases/_program.mylang test/end-to-end-cases/_program._program.s")
            # Run the compiled program.
            stream = os.popen("cd .. && ./program.out")
            # Read the prints outputted by the program.
            prints = stream.read()
            # Check if they match with what we expect.
            if prints != expectedPrints:
                errorMessage = "Failed test: " + fileName + "\nExpected:\n" + expectedPrints + "\n\nReceived:\n" + prints
                print(errorMessage)
            else:
                print("Passed test: " + fileName)

run()