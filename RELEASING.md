# Release Procedure

`main` is protected against direct commits, and publishing to the PlatformIO Registry is automated via GitHub Actions whenever a **GitHub Release** is published.

## 1. Prepare the Release Branch

1. **Sync your local `main` branch**
   ```bash
   git checkout main
   git pull origin main
   ```

2. **Create a release branch** (replace `0.3.0` with your target version)
   ```bash
   git checkout -b release/v0.3.0
   ```

3. **Bump the version in `library.json`**
   - Update `"version"` using Semantic Versioning (e.g., `"0.3.0"`).
   - Ensure `README.md` and `examples/` reflect any new features or changes.

4. **Commit and push the release branch**
   ```bash
   git add library.json
   git commit -m "Bump version to 0.3.0"
   git push -u origin release/v0.3.0
   ```

## 2. Merge via Pull Request

1. Open a Pull Request on GitHub (`https://github.com`) from `release/v0.3.0` into `main`.
2. Wait for any status checks to pass, then merge the PR into `main`.
3. *(Optional)* Delete the `release/v0.3.0` branch on GitHub and locally:
   ```bash
   git checkout main
   git pull origin main
   git branch -d release/v0.3.0
   ```

## 3. Create a GitHub Release

1. Go to the **Releases** page (`https://github.com`) and click **Draft a new release**.
2. Click **Choose a tag**, type the version prefixed with `v` (e.g., `v0.3.0` — must match `"version"` in `library.json`), and click **Create new tag: v0.3.0 on publish**.
3. Ensure **Target** is set to `main`.
4. Set **Release title** to `v0.3.0`.
5. Click **Generate release notes** (or write a summary of changes).
6. Click **Publish release**.

## 4. Verify Automated Publishing

1. Go to the **Actions** tab (`https://github.com`) and confirm the **Publish to PlatformIO Registry** workflow completes without errors.
2. Check [PlatformIO Registry](https://platformio.org) to verify the new version is live.
