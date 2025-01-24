import os
import hashlib

Import("env")

paths = {
    "src": os.path.join("web", "dist"),
    "dest": os.path.join("include", "web")
}

def dump_to_file(src, dest, name):
    with open(src, 'rb') as src_file, open(dest, 'w') as header_file:
        # Read in binary data from file
        file_data = src_file.read()

        # Calculate data stream length
        data_length = len(file_data)

        # Caclulate file sha356sum
        file_hash = hashlib.sha256(file_data).hexdigest()

        # Perform file hexdump into new header file
        header_file.write("#pragma once\n\n")
        header_file.write("#include <Arduino.h>\n\n")
        
        header_file.write(f"#define {name}_len {data_length}\n")
        header_file.write(f"#define {name}_sha \"{file_hash}\"\n\n")
        
        header_file.write(f"const uint8_t {name}[] PROGMEM = {{\n")
        
        for i in range(0, data_length, 16):
            chunk = file_data[i:i + 16]
            hex_values = ', '.join(f'0x{byte:02x}' for byte in chunk)
            header_file.write(f"    {hex_values},\n")
        
        header_file.write("};\n")

        print(f"Dumped {src} to {dest}. Size: {data_length} bytes.")

def run_command(command):
    result = os.system(command)
    if result != 0:
        raise RuntimeError(f"Command failed with exit code {result}: {command}")

print("Running prebuild script...")

run_command("cd web && npm install")
run_command("cd web && npm run build")

# Clean destination path
os.makedirs(paths["dest"], exist_ok=True)
for file in os.listdir(paths["dest"]):
    os.remove(os.path.join(paths["dest"], file))

# Perform file hexdump for each file in ./web/dist
# Output dumped header files to ./include/web
for file in os.listdir(paths["src"]):
    src_path = os.path.join(paths["src"], file)
    if not os.path.isfile(src_path):
        continue  # Skip directories or invalid files

    # Generate the destination file and array name
    base_name, extension = os.path.splitext(file)
    base_name_split = base_name.split(".")[0]  # Extract the split result outside the f-string
    dest_file = f"{base_name_split}_{extension[1:]}.h"
    dest_path = os.path.join(paths["dest"], dest_file)
    array_name = f"{base_name_split}_{extension[1:]}"

    dump_to_file(src_path, dest_path, array_name)

print("Prebuild script finished.")
