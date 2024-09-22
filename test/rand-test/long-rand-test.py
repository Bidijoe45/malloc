from subprocess import Popen, PIPE
import os

rand_generator_path = "/Users/apavel/20-29.estudios/21.cursus/21.09.malloc/test/rand-test/rand-generator.py"
instructions_output_file = "/Users/apavel/20-29.estudios/21.cursus/21.09.malloc/build/test/rand-test"
rand_test_executable_path = "/Users/apavel/20-29.estudios/21.cursus/21.09.malloc/build/test/rand-test/rand_test"

cwd = os.path.dirname(os.path.realpath(__file__))

for i in range(0, 1000):

    print(f"Test index: {i}")

    process = Popen(["python3", rand_generator_path, f"{instructions_output_file}/tmp_rand_commands", "2000", "1000", "1", "70000", "1000"], stdout=PIPE, cwd=cwd)
    (output, err) = process.communicate()
    exit_code = process.wait()

    process = Popen([rand_test_executable_path, f"{instructions_output_file}/tmp_rand_commands"], stdout=PIPE, cwd=cwd)
    (output, err) = process.communicate()
    exit_code = process.wait()

    if exit_code < 0:
        print("TEST FAILED")
        exit(1)

print("Test completed OK")