# DVAES EQ — VST3 equalizer

Native C++/JUCE five-band parametric equalizer for Windows (FL Studio, Ableton, Reaper, etc.).

## Features
- Low shelf, 3 bell filters, high shelf
- Frequency: 20–20,000 Hz; gain: ±18 dB; adjustable Q: 0.2–10
- On/off per band, global bypass, output volume ±18 dB
- Live frequency response curve
- FL Studio/DAW parameter automation and project-state persistence
- Mono and stereo processing

## Build on Windows
1. Install **Visual Studio 2022** with **Desktop development with C++** and **CMake**.
2. Install Git and CMake (version 3.22 or newer), if not already included.
3. In PowerShell, navigate to this folder and run `./build-windows.ps1` (or use the manual commands below).
4. The compiled `.vst3` bundle is under `build/DVAES_EQ_artefacts/Release/VST3/` (exact folder depends on generator).
5. Copy `DVAES EQ.vst3` to `C:\Program Files\Common Files\VST3\` and rescan plugins in FL Studio.

Manual commands:
```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --target DVAES_EQ_VST3
```

JUCE is fetched automatically by CMake on the first configure (internet required).

## Notes
This archive contains **source code**, not a precompiled Windows binary. It has not been tested by compiling in this environment. This is a first working implementation, not a commercial mastering EQ: there is no FFT spectrum analyzer, oversampling, mid/side mode or dynamic EQ. Biquad coefficients are updated per audio block; fast automation may produce zipper/click artifacts.
