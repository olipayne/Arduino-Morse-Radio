#!/usr/bin/env python3
"""
Release script for Arduino Morse Radio project.

This script automates the release process:
1. Reads current version from VERSION file
2. Bumps version based on user choice (patch/minor/major)
3. Updates VERSION file
4. Generates include/Version.h file with the new version
5. Commits the changes (VERSION + Version.h)
6. Creates and pushes git tag
7. Triggers GitHub Actions to build and publish binaries

Usage:
    python scripts/release.py patch   # 1.10.3 -> 1.10.4
    python scripts/release.py minor   # 1.10.3 -> 1.11.0
    python scripts/release.py major   # 1.10.3 -> 2.0.0
"""

import sys
import os
import subprocess
import re
from pathlib import Path

def run_command(cmd, check=True):
    """Run a shell command and return the result."""
    print(f"Running: {cmd}")
    result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    if check and result.returncode != 0:
        print(f"Error running command: {cmd}")
        print(f"stdout: {result.stdout}")
        print(f"stderr: {result.stderr}")
        sys.exit(1)
    return result

def get_current_version():
    """Read the current version from VERSION file."""
    version_file = Path("VERSION")
    if not version_file.exists():
        print("ERROR: VERSION file not found!")
        sys.exit(1)
    
    version = version_file.read_text().strip()
    print(f"Current version: {version}")
    return version

def parse_version(version):
    """Parse version string into major, minor, patch components."""
    match = re.match(r'^(\d+)\.(\d+)\.(\d+)$', version)
    if not match:
        print(f"ERROR: Invalid version format: {version}")
        print("Expected format: major.minor.patch (e.g., 1.10.3)")
        sys.exit(1)
    
    return int(match.group(1)), int(match.group(2)), int(match.group(3))

def bump_version(version, bump_type):
    """Bump version according to semantic versioning rules."""
    major, minor, patch = parse_version(version)
    
    if bump_type == "patch":
        patch += 1
    elif bump_type == "minor":
        minor += 1
        patch = 0
    elif bump_type == "major":
        major += 1
        minor = 0
        patch = 0
    else:
        print(f"ERROR: Invalid bump type: {bump_type}")
        print("Valid options: patch, minor, major")
        sys.exit(1)
    
    new_version = f"{major}.{minor}.{patch}"
    print(f"New version: {new_version}")
    return new_version

def check_git_status():
    """Ensure we're in a clean git state."""
    result = run_command("git status --porcelain")
    if result.stdout.strip():
        print("ERROR: Working directory is not clean!")
        print("Please commit or stash your changes before releasing.")
        print("\nUnstaged changes:")
        print(result.stdout)
        sys.exit(1)
    
    # Check if we're on main branch
    result = run_command("git branch --show-current")
    current_branch = result.stdout.strip()
    if current_branch != "main":
        print(f"WARNING: You're on branch '{current_branch}', not 'main'")
        response = input("Continue anyway? (y/N): ").lower()
        if response != 'y':
            print("Aborted.")
            sys.exit(1)

def check_for_existing_tag(version):
    """Check if tag already exists."""
    tag = f"v{version}"
    result = run_command(f"git tag -l {tag}", check=False)
    if result.stdout.strip():
        print(f"ERROR: Tag {tag} already exists!")
        print("Please choose a different version or delete the existing tag.")
        sys.exit(1)

def update_version_file(new_version):
    """Update the VERSION file with the new version."""
    version_file = Path("VERSION")
    version_file.write_text(new_version + "\n")
    print(f"Updated VERSION file to {new_version}")

def generate_version_header():
    """Generate the Version.h file from the current VERSION file."""
    try:
        # Import the version module
        sys.path.append(str(Path(__file__).parent))
        import version
        
        # Generate the header file
        git_version = version.generate_version_header()
        print(f"Generated Version.h with version: {git_version}")
        return git_version
    except Exception as e:
        print(f"ERROR: Failed to generate Version.h: {e}")
        sys.exit(1)

def main():
    if len(sys.argv) != 2:
        print("Usage: python scripts/release.py <patch|minor|major>")
        print("\nExamples:")
        print("  python scripts/release.py patch   # 1.10.3 -> 1.10.4")
        print("  python scripts/release.py minor   # 1.10.3 -> 1.11.0") 
        print("  python scripts/release.py major   # 1.10.3 -> 2.0.0")
        sys.exit(1)
    
    bump_type = sys.argv[1].lower()
    
    # Change to project root directory
    script_dir = Path(__file__).parent
    project_root = script_dir.parent
    os.chdir(project_root)
    
    print("🚀 Starting release process...")
    print(f"📁 Working directory: {os.getcwd()}")
    
    # Pre-flight checks
    print("\n1️⃣ Checking git status...")
    check_git_status()
    
    # Get current version and calculate new version
    print("\n2️⃣ Reading current version...")
    current_version = get_current_version()
    new_version = bump_version(current_version, bump_type)
    
    # Check for existing tag
    print("\n3️⃣ Checking for existing tags...")
    check_for_existing_tag(new_version)
    
    # Show summary and confirm
    print(f"\n📋 Release Summary:")
    print(f"   Current version: {current_version}")
    print(f"   New version:     {new_version}")
    print(f"   Bump type:       {bump_type}")
    print(f"   Git tag:         v{new_version}")
    
    response = input("\nProceed with release? (y/N): ").lower()
    if response != 'y':
        print("❌ Release cancelled.")
        sys.exit(0)
    
    try:
        # Update VERSION file
        print("\n4️⃣ Updating VERSION file...")
        update_version_file(new_version)
        
        # Generate Version.h file
        print("\n5️⃣ Generating Version.h file...")
        generate_version_header()
        
        # Commit the version bump and Version.h
        print("\n6️⃣ Committing version bump...")
        run_command("git add VERSION include/Version.h")
        run_command(f'git commit -m "chore: bump version to v{new_version}"')
        
        # Create and push tag
        print("\n7️⃣ Creating git tag...")
        run_command(f"git tag v{new_version}")
        
        print("\n8️⃣ Pushing to GitHub...")
        run_command("git push origin main")
        run_command(f"git push origin v{new_version}")
        
        print(f"\n✅ Release v{new_version} completed successfully!")
        print(f"🔗 GitHub Actions will now build and publish the binaries.")
        print(f"📦 Check the release at: https://github.com/olipayne/Arduino-Morse-Radio/releases/tag/v{new_version}")
        
    except KeyboardInterrupt:
        print("\n❌ Release cancelled by user.")
        sys.exit(1)
    except Exception as e:
        print(f"\n❌ Release failed: {e}")
        print("\n🔄 You may need to clean up manually:")
        print("   - Check git status: git status")
        print(f"   - Remove tag if created: git tag -d v{new_version}")
        print("   - Reset VERSION file if needed")
        print("   - Reset include/Version.h if needed")
        sys.exit(1)

if __name__ == "__main__":
    main()
