import subprocess
import os

def get_git_version():
    """
    Get version information from git:
    - If on a tag, use the tag name
    - Otherwise use the latest tag plus commits since tag
    - Include dirty flag if working directory has modifications
    """
    try:
        # Check if we're on a tag
        describe_cmd = ["git", "describe", "--tags", "--exact-match"]
        tag_version = subprocess.check_output(describe_cmd, stderr=subprocess.DEVNULL).decode().strip()
        return tag_version
    except subprocess.CalledProcessError:
        # Not on a tag, use describe to get tag plus commits
        try:
            describe_cmd = ["git", "describe", "--tags", "--dirty", "--always"]
            version = subprocess.check_output(describe_cmd).decode().strip()
            return version
        except subprocess.CalledProcessError:
            # Fallback if git commands fail
            return "unknown"

def generate_version_header():
    version = get_git_version()
    
    # Create header content
    header = f"""#ifndef VERSION_H
#define VERSION_H

// Automatically generated from Git - do not edit manually
#define FIRMWARE_VERSION "{version}"

#endif // VERSION_H
"""
    
    # Ensure directory exists
    os.makedirs("include", exist_ok=True)
    
    # Write header file
    with open("include/Version.h", "w") as f:
        f.write(header)
    
    print(f"Generated Version.h with version: {version}")
    return version

if __name__ == "__main__":
    generate_version_header() 