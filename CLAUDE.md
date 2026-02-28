# Orbital - JUCE Synth Plugin

## Build & Test

```bash
cmake --build build --config Release --target Orbital_Standalone -j$(sysctl -n hw.logicalcpu)
open build/Orbital_artefacts/Release/Standalone/Orbital.app
```

## VST3 Hot Reload in Ableton

After building, create a symlink from the build output to Ableton's Rosetta VST3 directory so the plugin hot-reloads:

```bash
ln -sf "$(pwd)/build/Orbital_artefacts/Release/VST3/Orbital.vst3" "$HOME/Library/Audio/Plug-Ins/VST3/Orbital.vst3"
```

This only needs to be done once. After that, rebuilding the VST3 target will automatically make the new version available in Ableton (rescan or reopen the project).
