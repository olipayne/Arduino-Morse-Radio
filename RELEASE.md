# Release Process

This project uses a simple manual release process to avoid version conflicts.

## Making a Release

1. **Update VERSION file**
   ```bash
   echo "1.11.0" > VERSION
   ```

2. **Commit the version update**
   ```bash
   git add VERSION
   git commit -m "chore: bump version to v1.11.0"
   ```

3. **Create and push tag**
   ```bash
   git tag v1.11.0
   git push origin main
   git push origin v1.11.0
   ```

4. **Build release binaries** (optional - can be done locally)
   ```bash
   pio run -e release
   pio run -e debug
   ```

## Version Display

- **Development builds**: `v1.11.0-dev-main-abc12345-dirty`
  - Shows base version + branch + commit hash + dirty flag
  - Always unique and traceable

- **Release builds**: `v1.11.0`  
  - Clean version number when on matching git tag
  - Shows in web interface and serial output

## Benefits

- ✅ No chicken-and-egg versioning problems
- ✅ Manual control over releases
- ✅ Development builds are uniquely identifiable
- ✅ Simple and predictable process
- ✅ No complex CI/CD dependencies
