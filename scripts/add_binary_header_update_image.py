import struct
import sys
import os

def insert_header(input_path, output_path):
    with open(input_path, 'rb') as f:
        data = f.read()

    original_length = len(data)
    header = struct.pack('>II', 0xA5A5A5A5, original_length)  # Big-endian format

    with open(output_path, 'wb') as f:
        f.write(header)
        f.write(data)

    print(f"Output written to {output_path} ({original_length} original bytes + 8-byte header)")

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Usage: python insert_header.py <input_file> <output_file>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    if not os.path.isfile(input_file):
        print(f"Error: input file '{input_file}' does not exist.")
        sys.exit(1)

    insert_header(input_file, output_file)