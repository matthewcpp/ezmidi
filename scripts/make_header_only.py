import sys
import os
import re

def main():
    script_dir = os.path.dirname(os.path.realpath(__file__))
    header_path = os.path.join(script_dir, "..", "include", "ezmidi", "ezmidi.h")
    src_folder = os.path.join(script_dir, "..", "src")

    with open(sys.argv[1], 'w') as output_file:
        process_header(header_path, output_file)

        output_file.write("\n#ifdef EZMIDI_IMPLEMENTATION\n")
        add_source_file(os.path.join(src_folder, "midi.h"), None, output_file)
        add_source_file(os.path.join(src_folder, "message_processor.h"), None, output_file)
        add_source_file(os.path.join(src_folder, "common.cpp"), None, output_file)
        add_source_file(os.path.join(src_folder, "message_processor.cpp"), None, output_file)
        add_source_file(os.path.join(src_folder, "windows.cpp"), "_WIN32", output_file)
        add_source_file(os.path.join(src_folder, "coremidi.cpp"), "__APPLE__", output_file)
        add_source_file(os.path.join(src_folder, "alsa.cpp"), "__linux__", output_file)
        output_file.write("\n#endif\n")
    return 0

def process_header(header_path, output_file):
    ignoring = False
    with open(header_path, 'r') as header_file:
        for line in header_file.readlines():
            if not ignoring and line.startswith("//!ignoreheaderonly on"):
                ignoring = True
            elif ignoring and line.startswith("//!ignoreheaderonly off"):
                ignoring = False
            elif not ignoring:
                output_file.write(line)

def add_source_file(source_path, guard, output_file):
    if guard is not None:
        output_file.write("\n#ifdef {}\n".format(guard))

    separator = "\n// {}\n".format('*' * 10)
    output_file.write("{}// {}{}".format(separator, os.path.basename(source_path), separator))

    with open(source_path, 'r') as source_file:
        for line in source_file.readlines():
            if not re.match('\s*#\s*include\s*".*"\s*', line):
                output_file.write(line)

    if guard is not None:
        output_file.write("\n#endif\n")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python make_header_only.py /path/to/output.h")
        sys.exit(1)

    sys.exit(main())