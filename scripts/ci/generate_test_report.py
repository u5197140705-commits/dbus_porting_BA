import sys
import re
from junit_xml import TestSuite, TestCase, TestError, TestFailure

def parse_zephyr_test_output(log_file_path):
    test_cases = []
    current_test_suite = None
    current_test_case_name = None
    current_test_case_output = []
    
    # Regex patterns for Zephyr test output
    # Example: "PASS - minimal_test.minimal_function_test"
    # Example: "FAIL - minimal_test.another_test"
    # Example: "RUN - minimal_test.test_case_name"
    run_pattern = re.compile(r"RUN - ([\w_]+\.[\w_]+)")
    pass_pattern = re.compile(r"PASS - ([\w_]+\.[\w_]+)")
    fail_pattern = re.compile(r"FAIL - ([\w_]+\.[\w_]+)")
    skip_pattern = re.compile(r"SKIP - ([\w_]+\.[\w_]+)")

    with open(log_file_path, 'r') as f:
        for line in f:
            line = line.strip()

            run_match = run_pattern.match(line)
            pass_match = pass_pattern.match(line)
            fail_match = fail_pattern.match(line)
            skip_match = skip_pattern.match(line)

            if run_match:
                # If a previous test case was being processed, finalize it
                if current_test_case_name:
                    # Assume previous test failed if not explicitly passed/skipped
                    if not any(p.match(current_test_case_name) for p in [pass_pattern, fail_pattern, skip_pattern]):
                        tc = TestCase(current_test_case_name, stdout="\n".join(current_test_case_output))
                        tc.add_failure("Test did not complete or explicitly pass/fail/skip.")
                        test_cases.append(tc)
                
                current_test_case_name = run_match.group(1)
                current_test_case_output = [line]
            elif pass_match:
                if current_test_case_name == pass_match.group(1):
                    tc = TestCase(current_test_case_name, stdout="\n".join(current_test_case_output + [line]))
                    test_cases.append(tc)
                else:
                    # Handle cases where a test passes without a preceding RUN (e.g., if parsing started mid-log)
                    tc = TestCase(pass_match.group(1), stdout=line)
                    test_cases.append(tc)
                current_test_case_name = None
                current_test_case_output = []
            elif fail_match:
                if current_test_case_name == fail_match.group(1):
                    tc = TestCase(current_test_case_name, stdout="\n".join(current_test_case_output + [line]))
                    tc.add_failure("Test failed.")
                    test_cases.append(tc)
                else:
                    tc = TestCase(fail_match.group(1), stdout=line)
                    tc.add_failure("Test failed.")
                    test_cases.append(tc)
                current_test_case_name = None
                current_test_case_output = []
            elif skip_match:
                if current_test_case_name == skip_match.group(1):
                    tc = TestCase(current_test_case_name, stdout="\n".join(current_test_case_output + [line]))
                    tc.add_skipped("Test skipped.")
                    test_cases.append(tc)
                else:
                    tc = TestCase(skip_match.group(1), stdout=line)
                    tc.add_skipped("Test skipped.")
                    test_cases.append(tc)
                current_test_case_name = None
                current_test_case_output = []
            else:
                if current_test_case_name:
                    current_test_case_output.append(line)

    # Handle the last test case if the log ends without an explicit pass/fail/skip
    if current_test_case_name:
        tc = TestCase(current_test_case_name, stdout="\n".join(current_test_case_output))
        tc.add_failure("Test did not complete or explicitly pass/fail/skip.")
        test_cases.append(tc)

    # Group test cases by suite (e.g., 'minimal_test' from 'minimal_test.minimal_function_test')
    test_suites_dict = {}
    for tc in test_cases:
        suite_name = tc.name.split('.')[0] if '.' in tc.name else "unknown_suite"
        if suite_name not in test_suites_dict:
            test_suites_dict[suite_name] = []
        test_suites_dict[suite_name].append(tc)

    test_suites = []
    for suite_name, cases in test_suites_dict.items():
        test_suites.append(TestSuite(suite_name, cases))

    return test_suites

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python generate_test_report.py <input_log_file> <output_junit_xml_file>")
        sys.exit(1)

    input_log_file = sys.argv[1]
    output_junit_xml_file = sys.argv[2]

    try:
        test_suites = parse_zephyr_test_output(input_log_file)
        with open(output_junit_xml_file, 'w') as f:
            TestSuite.to_file(f, test_suites, prettyprint=True)
        print(f"Successfully generated JUnit XML report: {output_junit_xml_file}")
    except FileNotFoundError:
        print(f"Error: Input log file not found at {input_log_file}")
        sys.exit(1)
    except Exception as e:
        print(f"An error occurred: {e}")
        sys.exit(1)