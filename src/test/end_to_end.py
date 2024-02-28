import os, subprocess

def run():
    folder = "end-to-end-cases"
    testFileNames = os.listdir(folder)
    # For each test case.
    for fileName in testFileNames:
        if fileName[0] != "_":
            print("")
            with open(folder + "/" + fileName, encoding='utf-8') as file:
                # Read the test case file.
                contents = file.read()
                programCode, expectedPrints = contents.split("\n!expect!\n")
                # Write the program code into its own file so the compiler can read it.
                with open(folder + "/_program.mylang", "w", encoding='utf-8') as programFile:
                    programFile.write(programCode)
                # Compile the corresponding program from the program code.
                compilerResult = subprocess.run(["cd .. && ./main.out test/end-to-end-cases/_program.mylang test/end-to-end-cases/_program._program.s"], shell=True, capture_output=True, text=True)
                if compilerResult.stderr != "":
                    print("Error during compilation of test: " + fileName + ".\nError message:\n" + compilerResult.stderr)
                else:
                    # Run the compiled program.
                    programResult = subprocess.run(["cd .. && ./program.out"], shell=True, capture_output=True, text=True)
                    # Check if the prints match with what we expect.
                    if (programResult.stdout != expectedPrints + "\n") and (programResult.stdout != expectedPrints):
                        errorMessage = "Failed test: " + fileName + "\nExpected:\n" + expectedPrints + "\n\n----\nReceived:\n" + programResult.stdout + "\n----\n"
                        print(errorMessage)
                    else:
                        print("Passed test: " + fileName)

run()