# Build the Windows .vst3 without installing a compiler

1. Create a new GitHub repository.
2. Upload the **contents of this DVAES_EQ folder**, including the hidden `.github` directory (not the parent directory). Easiest option: run `git init`, `git add .`, `git commit -m "DVAES EQ"` and push to the new repository. Or upload via GitHub web interface, preserving `.github/workflows/build-windows.yml`.
3. Go to the repository **Actions** tab and choose **Build DVAES EQ VST3 (Windows)** → **Run workflow** (or push to `main` to start it automatically).
4. When the build finishes successfully, open the run and download artifact **DVAES_EQ_Windows_VST3**. Unzip the artifact, then unzip `DVAES_EQ_Windows_VST3.zip`.
5. Copy the resulting `DVAES EQ.vst3` folder/bundle to `C:\Program Files\Common Files\VST3` and rescan in FL Studio.

Notes: This has not been executed here. Requires GitHub Actions enabled and online build service to download JUCE. If compilation fails, open the step's logs to see the exact C++ error.
