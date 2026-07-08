# EFFIGY

**Guitar DI in. Finished record out.**

A guitar channel in a box — every stage between a raw DI and a mix-ready, produced guitar tone, inside one JUCE 8 plugin (VST3 / AU). Mono in → stereo out.

## Signal chain
```
DI IN (mono) -> Input/DI -> Gate+Tightener -> Drive (5 circuits) + Feel -> Tone Stack -> Cab (dual IR) -> Produce -> Output -> STEREO OUT
```

## Building

```
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

JUCE 8.0.4 is fetched automatically via CMake FetchContent. Factory IRs are embedded via BinaryData.

## Distribution
- **Windows:** VST3 in `dist/` (built locally).
- **macOS:** GitHub Actions builds a universal VST3+AU, wraps them in `EFFIGY-macOS-<ver>.pkg`, zips it, and publishes as a workflow artifact (and to releases on `v*` tags).
