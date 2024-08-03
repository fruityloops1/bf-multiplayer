# This is ai slop

# usage:
# comment the out execute_process(
#    COMMAND bash -c "find ${SYMS_PATH} -type f -name '*.sym' -exec cat {} + > ${CMAKE_BINARY_DIR}/syms.ld"
#)
# ^ thingy inside CMakeLists.txt after having already generated it once
# then empty the linker script and do `make 2> something.txt` and pass the (non empty) linker script and the just created something.txt to this script
# then use the output linker script instead and it should strip about 1600kb from the executable

import sys
import re

def process_linker_script(linker_script, reference_file):
    with open(reference_file, 'r') as f:
        reference_content = f.read()

    with open(linker_script, 'r') as input_file, open('output_linker_script.ld', 'w') as output_file:
        for line in input_file:
            if line.strip() == '' or line.strip().startswith('/*') or line.strip().startswith('*/'):
                continue
            
            match = re.match(r'([^\s=]+)\s*=', line)
            if match:
                symbol = match.group(1)
                
                if symbol in reference_content:
                    output_file.write(line)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python cleanUpLinkerScript.py <linker_script> <reference_file>")
        sys.exit(1)

    linker_script = sys.argv[1]
    reference_file = sys.argv[2]
    process_linker_script(linker_script, reference_file)