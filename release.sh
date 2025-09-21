#!/bin/bash
# Convenient wrapper for the Python release script

if [ $# -ne 1 ]; then
    echo "Usage: ./release.sh <patch|minor|major>"
    echo ""
    echo "Examples:"
    echo "  ./release.sh patch   # 1.10.3 -> 1.10.4"
    echo "  ./release.sh minor   # 1.10.3 -> 1.11.0"
    echo "  ./release.sh major   # 1.10.3 -> 2.0.0"
    exit 1
fi

# Use python3 explicitly for better compatibility
python3 scripts/release.py "$1"
