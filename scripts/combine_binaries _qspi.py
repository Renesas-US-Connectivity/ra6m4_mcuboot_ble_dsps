from intelhex import IntelHex
import sys
from intelhex import IntelHex
import sys

def load_binary_to_hex(file_path, start_address, max_length=None):
    with open(file_path, 'rb') as f:
        data = f.read()
        if max_length is not None:
            data = data[:max_length]
        ih = IntelHex()
        ih.frombytes(data, offset=start_address)
        return ih

def combine_bins_to_hex(bin1_path, bin2_path, hex_path):
    # First binary: limit to 0x18000 bytes, start at 0x00000000
    bin1_hex = load_binary_to_hex(bin1_path, start_address=0x00000000, max_length=0x18000)

    # Second binary: full file, start at 0x60000000
    bin2_hex = load_binary_to_hex(bin2_path, start_address=0x60000000)

    # Merge
    bin1_hex.merge(bin2_hex, overlap='error')

    # Write out to Intel HEX
    bin1_hex.write_hex_file(hex_path)

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python bin_to_hex.py <binary1> <binary2> <output.hex>")
        sys.exit(1)

    bin1_path = sys.argv[1]
    bin2_path = sys.argv[2]
    output_hex = sys.argv[3]

    combine_bins_to_hex(bin1_path, bin2_path, output_hex)
    print(f"Intel HEX file written to: {output_hex}")