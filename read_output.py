import os
import sys
import struct

ARGV_COUNT = 2

INTERACTION_TYPES = {
    0: "NO_HIT",
    1: "WALL_HIT",
    2: "CYLINDER_HIT",
    3: "CELL_HIT"
}

def main():
    if len(sys.argv) != ARGV_COUNT:
        print("Usage: {} <directory_path>".format(__file__))
        print("\tExample: {} /path/to/directory".format(__file__))
        return

    _, directory_path = sys.argv
    
    process_files_in_directory(directory_path)
    print('Done Parsing the bin files')


def process_files_in_directory(directory_path):
    for filename in os.listdir(directory_path):
        if filename.endswith(".bin"):
            file_path = os.path.join(directory_path, filename)
            csv_file_path = file_path[0:-4] + ".csv"
            file_data = read_file(file_path)
            save_values_to_csv(file_data, csv_file_path, filename)


def read_file(file_path: str) -> bytes:
    with open(file_path, "rb") as f:
        file_data = f.read()

    return file_data


def save_values_to_csv(file_data: bytes, file_path: str, filename: str) -> None:
    if "cellPosition.bin" in filename:
        format_string = 'IIfffII'
    elif "state.bin" in filename:
        format_string = 'III'
    elif "MolExpr.bin" in filename:
        format_string = 'IfII'
    else:
        format_string = 'fII'

    number_of_elements = len(file_data) // struct.calcsize(format_string)

    with open(file_path, 'w') as f:
        f.write("data\n")
        for i in range(number_of_elements):
            current_pos = i * struct.calcsize(format_string)
            data = struct.unpack(format_string, file_data[current_pos:current_pos + struct.calcsize(format_string)])

            f.write("{}\n".format(",".join(map(str, data))))
	


if __name__ == "__main__":
    main()