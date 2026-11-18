#!/usr/bin/env python3
import os
import subprocess
import sys
from pathlib import Path

# ANSI colors
GREEN = "\033[92m"
RED = "\033[91m"
YELLOW = "\033[93m"
BLUE = "\033[94m"
RESET = "\033[0m"
BOLD = "\033[1m"

def run_command(cmd):
    result = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    return result.returncode, result.stdout, result.stderr

def main():
    root_dir = Path(__file__).parent.parent
    citu_bin = root_dir / "bin" / "citu"

    if not citu_bin.exists():
        print(f"{RED}Error: Compiler binary not found at {citu_bin}. Run 'make' first.{RESET}")
        sys.exit(1)

    print(f"\n{BOLD}{BLUE}================================================================={RESET}")
    print(f"{BOLD}{BLUE}                CITU COMPILER TEST SUITE RUNNER                  {RESET}")
    print(f"{BOLD}{BLUE}================================================================={RESET}\n")

    passed_count = 0
    failed_count = 0
    total_tests = 0

    # 1. Canonical Examples (Must succeed)
    print(f"{BOLD}1. Canonical Example Suite (examples/):{RESET}")
    example_files = sorted((root_dir / "examples").glob("*.citu"))
    for f in example_files:
        total_tests += 1
        rc, out, err = run_command([str(citu_bin), str(f)])
        if rc == 0:
            print(f"  [{GREEN}PASS{RESET}] {f.name} (Valid syntax)")
            passed_count += 1
        else:
            print(f"  [{RED}FAIL{RESET}] {f.name} (Expected success, returned {rc})")
            print(f"         {err.strip()}")
            failed_count += 1

    # 2. Valid Feature Coverage Tests (Must succeed)
    print(f"\n{BOLD}2. Additional Feature Coverage Suite (tests/valid/):{RESET}")
    valid_files = sorted((root_dir / "tests" / "valid").glob("*.citu"))
    for f in valid_files:
        total_tests += 1
        rc, out, err = run_command([str(citu_bin), str(f)])
        if rc == 0:
            print(f"  [{GREEN}PASS{RESET}] {f.name} (Valid syntax)")
            passed_count += 1
        else:
            print(f"  [{RED}FAIL{RESET}] {f.name} (Expected success, returned {rc})")
            print(f"         {err.strip()}")
            failed_count += 1

    # 3. Negative Lexical Tests (Must fail)
    print(f"\n{BOLD}3. Negative Lexical Error Suite (tests/invalid_lex/):{RESET}")
    lex_err_files = sorted((root_dir / "tests" / "invalid_lex").glob("*.citu"))
    for f in lex_err_files:
        total_tests += 1
        rc, out, err = run_command([str(citu_bin), "--tokens", str(f)])
        if rc != 0:
            print(f"  [{GREEN}PASS{RESET}] {f.name} (Correctly rejected lexical error)")
            passed_count += 1
        else:
            print(f"  [{RED}FAIL{RESET}] {f.name} (Expected lexical failure, but succeeded)")
            failed_count += 1

    # 4. Negative Syntax Tests (Must fail)
    print(f"\n{BOLD}4. Negative Syntax Error Suite (tests/invalid_syntax/):{RESET}")
    syntax_err_files = sorted((root_dir / "tests" / "invalid_syntax").glob("*.citu"))
    for f in syntax_err_files:
        total_tests += 1
        rc, out, err = run_command([str(citu_bin), str(f)])
        if rc != 0:
            print(f"  [{GREEN}PASS{RESET}] {f.name} (Correctly rejected syntax error)")
            passed_count += 1
        else:
            print(f"  [{RED}FAIL{RESET}] {f.name} (Expected syntax failure, but succeeded)")
            failed_count += 1

    # Summary
    print(f"\n{BOLD}{BLUE}================================================================={RESET}")
    print(f"{BOLD}Test Results Summary:{RESET}")
    print(f"  Total Tests : {total_tests}")
    print(f"  Passed      : {GREEN}{passed_count}{RESET}")
    print(f"  Failed      : {RED if failed_count > 0 else GREEN}{failed_count}{RESET}")
    print(f"{BOLD}{BLUE}================================================================={RESET}\n")

    if failed_count > 0:
        sys.exit(1)
    else:
        print(f"{BOLD}{GREEN}ALL {total_tests} TESTS PASSED SUCCESSFULLY!{RESET}\n")
        sys.exit(0)

if __name__ == "__main__":
    main()
