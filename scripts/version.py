import subprocess
import os

def get_version(force_release=False):
    """
    Get version information:
    - Read from VERSION file for the base version
    - Add git info for development builds
    - If force_release=True, return clean version without dev suffix
    """
    # Read base version from VERSION file
    try:
        with open("VERSION", "r") as f:
            base_version = f.read().strip()
    except FileNotFoundError:
        base_version = "unknown"
    
    # If we're doing a release build, return clean version
    if force_release:
        print(f"Release build (forced): v{base_version}")
        return f"v{base_version}"
    
    try:
        # Check if we're on a tag that matches the VERSION file
        describe_cmd = ["git", "describe", "--tags", "--exact-match"]
        tag_version = subprocess.check_output(describe_cmd, stderr=subprocess.DEVNULL).decode().strip()
        
        # Remove 'v' prefix if present for comparison
        clean_tag = tag_version.lstrip('v')
        if clean_tag == base_version:
            print(f"Release build: v{base_version}")
            return f"v{base_version}"
    except subprocess.CalledProcessError:
        pass
    
    # Not on a matching tag - add development info
    try:
        # Get short commit hash
        commit_hash = subprocess.check_output(["git", "rev-parse", "--short=8", "HEAD"]).decode().strip()
        
        # Get branch name
        try:
            branch = subprocess.check_output(["git", "rev-parse", "--abbrev-ref", "HEAD"]).decode().strip()
            if branch == "HEAD":  # Detached HEAD state
                branch = "detached"
        except subprocess.CalledProcessError:
            branch = "unknown"
        
        # Check if working directory is dirty
        try:
            subprocess.check_output(["git", "diff-index", "--quiet", "HEAD"])
            dirty = ""
        except subprocess.CalledProcessError:
            dirty = "-dirty"
        
        version = f"v{base_version}-dev-{branch}-{commit_hash}{dirty}"
        print(f"Development build: {version}")
        return version
        
    except subprocess.CalledProcessError:
        # Fallback if git commands fail
        return f"v{base_version}-dev"

def generate_version_header(force_release=False):
    """
    Generate Version.h file with version information.
    
    Args:
        force_release: If True, generate clean release version without dev suffix
    """
    version = get_version(force_release=force_release)
    
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