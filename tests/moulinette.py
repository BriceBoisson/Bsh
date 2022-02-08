import argparse
from pathlib import Path
from difflib import unified_diff

import subprocess as sp
import yaml

from dataclasses import dataclass

@dataclass
class TestCase:
    def __init__(self, name, input = None, file = None):
        self.name = name
        self.input = input
        self.file = file

def diff(expected: str, actual: str) -> str:
    expected_lines = expected.splitlines(keepends=True)
    actual_lines = actual.splitlines(keepends=True)
    return ''.join(unified_diff(actual_lines, expected_lines, fromfile='actual', tofile='expected'))

def run_shell(shell: str, stdin: str) -> sp.CompletedProcess:
    return sp.run([shell], input=stdin, capture_output=True, text=True, env={})

def perform_check(expected: sp.CompletedProcess, actual: sp.CompletedProcess):
    assert expected.returncode == actual.returncode, f"EXIT_CODE_ERR: expected {expected.returncode}, got {actual.returncode}"
    assert expected.stdout == actual.stdout, f"STDOUT_ERR\n{diff(expected.stdout, actual.stdout)}"
    assert len(expected.stderr) == len(actual.stderr) or (len(expected.stderr) != 0 and len(actual.stderr) != 0), f"STDERR_EMPTY"

if __name__ == '__main__':
    parser = argparse.ArgumentParser("bshTestSuite")
    parser.add_argument('--binary', required=True, type=Path)
    parser.add_argument('--tests', required=True, nargs='+', default=[])
    args = parser.parse_args()

    binary_path = args.binary.absolute()

    tests=0
    errors=0

    for test in args.tests:
        with open(test, 'r') as file:
            testsuite = [TestCase(**testcase) for testcase in yaml.safe_load(file)]
        print(f"\033[6m## \033[0m\033[34m{test}\033[97m")
        for testcase in testsuite:
            tests += 1
            input = testcase.input
            if (testcase.file):
                with open(testcase.file, 'r') as file:
                    input = file.read()
            actual = run_shell(binary_path, input)
            expected = run_shell("dash", input)
            try:
                perform_check(expected, actual)
            except AssertionError as e:
                print(f"\033[1m[ \033[31mKO\033[97m ] \033[0m{testcase.name}\n{e}")
                errors += 1
            else:
                print(f"\033[1m[ \033[92mOK\033[97m ] \033[0m{testcase.name}")
        print("")
    
    if (errors > 1):
        s = 's'
    else:
        s = ''
    print(f"\n\033[33m✗ \033[34m{tests}\033[97m tests performed, \033[31m{errors}\033[97m error{s}\033[0m")
    exit(errors)
