from subprocess import Popen, PIPE
import time
import os

rand_generator_path = "/home/apavel/malloc/build/test/rand-test/rand-generator.py"
instructions_output_file = "/home/apavel/malloc/build/test/rand-test/"
rand_test_executable_path = "/home/apavel/malloc/build/test/rand-test/rand_test"

cwd = os.path.dirname(os.path.realpath(__file__))

for i in range(0, 1000):

    print(f"Test index: {i}")

    process = Popen(["python3", rand_generator_path, f"{instructions_output_file}/tmp_rand_commands", "10000", "0", "0", "60000", "2000"], stdout=PIPE, cwd=cwd)
    (output, err) = process.communicate()
    exit_code = process.wait()

    process = Popen([rand_test_executable_path, f"{instructions_output_file}/tmp_rand_commands"], stdout=PIPE, cwd=cwd)
    (output, err) = process.communicate()
    exit_code = process.wait()

    if exit_code < 0:
        print("TEST FAILED")
        exit(1)

print("Test completed OK")