$ErrorActionPreference = "Stop"
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
cmake --build build --config Release --target DVAES_EQ_VST3
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
Write-Host "Build complete. Look in build/DVAES_EQ_artefacts/Release/VST3"
