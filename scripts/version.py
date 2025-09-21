import subprocess
import os

def get_git_version():
    """
    Get version information from git:
    - If running in GitHub Actions with a release tag, use that tag
    - If on a tag, use the tag name
    - Otherwise use commit hash with branch info for development builds
    """
    # Check if we're running in GitHub Actions for a release
    if 'GITHUB_REF' in os.environ and os.environ['GITHUB_REF'].startswith('refs/tags/'):
        version = os.environ['GITHUB_REF'].replace('refs/tags/', '')
        print(f"Using GitHub release tag: {version}")
        return version
        
    try:
        # Check if we're on a tag
        describe_cmd = ["git", "describe", "--tags", "--exact-match"]
        tag_version = subprocess.check_output(describe_cmd, stderr=subprocess.DEVNULL).decode().strip()
        print(f"On release tag: {tag_version}")
        return tag_version
    except subprocess.CalledProcessError:
        # Not on a tag - use commit hash for development builds
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
            
            version = f"dev-{branch}-{commit_hash}{dirty}"
            print(f"Development build: {version}")
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