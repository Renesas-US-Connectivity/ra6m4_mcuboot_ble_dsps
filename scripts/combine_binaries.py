import argparse

def truncate_after_offset(data: bytes, offset: int) -> bytes:
    """
    Keep only the data up to the given offset.
    """
    return data[:offset] if len(data) > offset else data

def combine_binaries(file1_path, file2_path, output_path, second_bin_offset=0x00010000, filler=0xFF):
    # Read and truncate the first binary after offset
    with open(file1_path, 'rb') as f1:
        raw_data1 = f1.read()
        data1 = truncate_after_offset(raw_data1, second_bin_offset)

    # Read and (optionally) strip trailing 0xFFs from second binary
    with open(file2_path, 'rb') as f2:
        data2 = f2.read()
        data2 = data2.rstrip(bytes([filler]))

    size1 = len(data1)
    if size1 > second_bin_offset:
        raise ValueError("First binary still exceeds offset 0x10000 after truncation!")

    # Fill the gap between binaries with 0xFF
    gap_size = second_bin_offset - size1
    filler_bytes = bytes([filler]) * gap_size

    # Combine all
    combined_data = data1 + filler_bytes + data2

    with open(output_path, 'wb') as out_file:
        out_file.write(combined_data)

    print(f"✅ Combined binary written to: {output_path}")
    print(f"📦 First binary truncated at: 0x{second_bin_offset:X} (size: {size1} bytes)")
    print(f"🧱 Gap filled with 0x{filler:02X}: {gap_size} bytes")
    print(f"📦 Second binary size after stripping: {len(data2)} bytes")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Combine two binaries, stripping first after 0x10000.")
    parser.add_argument("first_bin", help="Path to the first binary file (placed at 0x00000000)")
    parser.add_argument("second_bin", help="Path to the second binary file (placed at 0x00010000)")
    parser.add_argument("output_bin", help="Path to the output combined binary file")
    args = parser.parse_args()

    combine_binaries(args.first_bin, args.second_bin, args.output_bin)