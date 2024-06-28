import os
import json

# Read CMAKE_INCLUDE_PATH from environment variables
cmake_include_path = os.getenv('CMAKE_INCLUDE_PATH')
if not cmake_include_path:
    raise ValueError("Environment variable CMAKE_INCLUDE_PATH is not set")

# Split CMAKE_INCLUDE_PATH by colon and sort it
include_paths = cmake_include_path.split(':')
unique_include_paths = sorted(set(include_paths))
unique_include_paths.insert(0, "${workspaceFolder}/")


# Load c_cpp_properties.json file
vscode_config_path = '.vscode/c_cpp_properties.json'
with open(vscode_config_path, 'r') as file:
    vscode_config = json.load(file)

# Update includePath with unique include paths
vscode_config['configurations'][0]['includePath'] = unique_include_paths

# Write updated c_cpp_properties.json file
with open(vscode_config_path, 'w') as file:
    json.dump(vscode_config, file, indent=4)

print(f"Updated {vscode_config_path} with unique include paths from CMAKE_INCLUDE_PATH.")
