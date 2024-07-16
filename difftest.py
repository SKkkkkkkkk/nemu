#!/usr/bin/env python3
import re
import argparse

def parse_qemu_log(file_path):
    qemu_instructions = []
    with open(file_path, 'r') as file:
        for line in file:
            match = re.match(r"^Trace.* \[([0-9a-fA-F]+)/([0-9a-fA-F]+)/.*\]", line)
            if match:
                addr = match.group(2)
                qemu_instructions.append(addr)
    return qemu_instructions

def parse_my_emulator_log(file_path):
    emulator_instructions = []
    with open(file_path, 'r') as file:
        for line in file:
            match = re.match(r"(^0x)([0-9a-fA-F]+):.*", line)
            if match:
                addr = match.group(2)
                emulator_instructions.append(addr)
    return emulator_instructions

def write_parsed_log(file_path, instructions):
    with open(file_path, 'w') as file:
        for instr in instructions:
            file.write(f"{instr}\n")

def compare_logs(qemu_log, emulator_log):
    qemu_instructions = parse_qemu_log(qemu_log)
    emulator_instructions = parse_my_emulator_log(emulator_log)
    
    # write parsed logs to files
    write_parsed_log(f"{qemu_log}.parsed.log", qemu_instructions)
    write_parsed_log(f"{emulator_log}.parsed.log", emulator_instructions)
    
    min_length = min(len(qemu_instructions), len(emulator_instructions))
    differences = []

    for i in range(min_length):
        if qemu_instructions[i] != emulator_instructions[i]:
            differences.append((i, qemu_instructions[i], emulator_instructions[i]))

    if len(qemu_instructions) != len(emulator_instructions):
        differences.append(("length", len(qemu_instructions), len(emulator_instructions)))

    return differences

def main():
    parser = argparse.ArgumentParser(description="Compare instruction execution sequences between QEMU and custom emulator logs.")
    parser.add_argument('qemu_log', type=str, help='Path to the QEMU log file.')
    parser.add_argument('emulator_log', type=str, help='Path to the custom emulator log file.')
    
    args = parser.parse_args()
    
    differences = compare_logs(args.qemu_log, args.emulator_log)
    
    if not differences:
        print("The logs are identical.")
    else:
        print("Differences found:")
        for diff in differences:
            if diff[0] == "length":
                print(f"Different lengths: QEMU log has {diff[1]} instructions, Emulator log has {diff[2]} instructions.")
            else:
                print(f"Instruction {diff[0]}: QEMU = {diff[1]}, Emulator = {diff[2]}")

if __name__ == "__main__":
    main()