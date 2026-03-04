# orbital
A gravity defying delay

## Build

```bash
# Standalone
cmake --build build --config Release --target Orbital_Standalone -j$(sysctl -n hw.logicalcpu)
open build/Orbital_artefacts/Release/Standalone/Orbital.app

# VST3 (auto-installs to ~/Library/Audio/Plug-Ins/VST3/)
cmake --build build --config Release --target Orbital_VST3 -j$(sysctl -n hw.logicalcpu)
```

The VST3 build automatically copies the plugin to `~/Library/Audio/Plug-Ins/VST3/Orbital.vst3`. Rescan or reload in Ableton to pick up changes.

## Project Structure

- `src/PluginProcessor.h/.cpp` — Audio engine (oscillators, filters, distortion, envelope, delay)
- `src/PluginEditor.h/.cpp` — GUI (synthwave-themed with custom LookAndFeel)
- `CMakeLists.txt` — JUCE CMake build config

