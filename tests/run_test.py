import sys
import subprocess
from glob import glob
from termcolor import colored

test_files = sorted(glob("*.cr"))
expected_files = sorted(glob("*.expected"))


assert len(test_files) == len(expected_files), "Ensure the *.expected file exists for each *.cr test file"
test_cnt = len(test_files)

def simple_test():
    len_longest = len(max(test_files, key=len))
    results = []
    for i, (test_file, expected_file) in enumerate(zip(test_files, expected_files)):
        with open(expected_file) as f:
            expected = f.read().strip()

        try:
            test = subprocess.check_output(f"../carrot.out {test_file}", shell=True)
            test = test.decode(sys.stdout.encoding).strip()

            if test == expected:
                status = "PASS"
            else:
                status = "FAIL"
        except:
            status = "ERR"

        result = {
            "filename": test_file,
            "status": status
        }
        results.append(result)

        if status == "PASS":
            status = colored(status, "green") 
        elif status == "FAIL":
            status = colored(status, "red")
        else:
            status = colored(status, "yellow") 

        len_test_file = len(test_file)
        n_spaces = len_longest - len_test_file + 10
        print(f"{test_file} {'.' * n_spaces} ({i+1}/{test_cnt}): {status}")
    return results

if __name__ == "__main__":
    title = f"Running {test_cnt} tests"
    print(title)
    print("=" * len(title))
    results = simple_test()
    pass_cnt = len(list(obj for obj in results if obj["status"] == "PASS"))
    fail_cnt = len(list(obj for obj in results if obj["status"] == "FAIL"))
    err_cnt = len(list(obj for obj in results if obj["status"] == "ERR"))
    print("\nSummary")
    print( "=======")
    print(f"PASS : {pass_cnt}")
    print(f"FAIL : {fail_cnt}")
    print(f"ERR  : {err_cnt}")
