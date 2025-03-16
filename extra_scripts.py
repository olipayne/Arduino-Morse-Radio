import os
import sys

# Add scripts directory to path
# Use getcwd() instead of __file__ since __file__ is not defined in PlatformIO environment
scripts_dir = os.path.join(os.getcwd(), "scripts")
sys.path.append(scripts_dir)

Import("env")

# Import our version generation module
import version

def before_build(source, target, env):
    # Generate version header
    git_version = version.generate_version_header()
    print(f"Building firmware version: {git_version}")

env.AddPreAction("buildprog", before_build) 