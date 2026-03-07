# Release Process

This project uses `release-please` as the only release mechanism.

## How Releases Work

1. Merge conventional commits into `main` (`feat:`, `fix:`, and `feat!`/`BREAKING CHANGE`).
2. The `Release Please` workflow opens or updates a release PR that bumps:
   - `CHANGELOG.md`
   - `VERSION`
   - `.release-please-manifest.json`
3. Merge the release PR.
4. `release-please` creates a Git tag (`vX.Y.Z`) and publishes the GitHub Release.
5. In the same `Release Please` workflow run, firmware binaries are built from that tag and uploaded to the release.

## Required Repository Settings

- Enable `Settings -> Actions -> General -> Allow GitHub Actions to create and approve pull requests`.

## Notes

- Do not manually bump `VERSION` for normal releases.
- Do not manually create release tags.
- `include/Version.h` is generated during builds from `VERSION` and git state.
