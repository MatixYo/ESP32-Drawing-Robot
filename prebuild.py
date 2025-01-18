import os
import hashlib

Import("env")

print("Running prebuild script...")

def hash(path, algorithm="sha256"):
    hash_func = hashlib.new(algorithm)

    for root, dirs, files in os.walk(path):
        if root.endswith(os.path.join(path, "dist")):
            continue

        for file in files:
            with open(os.path.join(root, file), "rb") as f:
                for chunk in iter(lambda: f.read(4096), b""):
                    hash_func.update(chunk)
    
    return hash_func.hexdigest()

# Create sha256sum of web project data
prj_hash = hash("./web")
hash_file = "hash.txt"

# Check if web project hashes match
if os.path.exists(hash_file):
    with open(hash_file, "r") as f:
        last_hash = f.readline().strip()
    build = last_hash != prj_hash
else:
    build = True
        
# Perform new build otherwise
if build:
    print("Building project...")
    if not os.path.exists("web/node_modules"):
        env.Execute("cd web && npm install")

    env.Execute("cd web && npm run build")
    env.Execute("python hexdump.py")

    with open(hash_file, "w") as f:
        f.write(prj_hash)
else:
    print("No changes detected. Skipping build.")

print("Prebuild script finished.")
