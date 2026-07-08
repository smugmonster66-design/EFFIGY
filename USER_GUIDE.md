# EFFIGY — User Guide

**Guitar DI in. Finished record out.**

EFFIGY is a complete guitar channel in one plugin: every stage between a raw DI and a mix-ready, produced guitar tone, voiced as a single system. It is not an amp sim and models no named hardware — the circuits, mic voicings and cabinets are original designs. Format: VST3 / AU, mono in → stereo out.

---

## 1. Installation

### Windows (VST3)
1. Copy the `EFFIGY.vst3` folder from `dist/VST3/` into your VST3 folder:
   `C:\Program Files\Common Files\VST3\`
2. Rescan plugins in your DAW.

### macOS (VST3 + AU)
1. Unzip `EFFIGY-macOS-<version>.zip` from `dist/macOS/`.
2. Run `EFFIGY-<version>.pkg`. It installs both formats to the standard locations:
   - VST3 → `/Library/Audio/Plug-Ins/VST3/`
   - AU → `/Library/Audio/Plug-Ins/Components/`
3. **First launch:** the package is not Apple-notarized. If macOS blocks it, either right-click the installer and choose *Open*, or run:
   ```
   xattr -dr com.apple.quarantine "/Library/Audio/Plug-Ins/VST3/EFFIGY.vst3"
   xattr -dr com.apple.quarantine "/Library/Audio/Plug-Ins/Components/EFFIGY.component"
   ```
4. Rescan plugins / validate the AU in your DAW.

### Channel configuration
EFFIGY is **mono in → stereo out**. Insert it on a mono guitar DI track routed to a stereo output, or on a stereo track (the input is summed to mono internally). Stereo is created inside the plugin at the cab and produce stages.

---

## 2. Quick Start

1. Insert EFFIGY on your DI track.
2. Pick a **preset** from the menu (top right) — presets are complete finished tones, organised by circuit.
3. Set **In Gain** so the meter sits healthy (peaks around -12 to -6 dB).
4. Adjust **Drive**, **Tone**, and **Feel** to taste.
5. Tweak the **Cab** (IR choice, blend, pans) and **Produce** blocks to finish.

Every control has a **hover tooltip** describing what it does — hover a moment to read it.

---

## 3. The Signal Chain

The chain is a fixed serial order, laid out left → right in the UI:

```
DI IN (mono)
  → Input / DI Conditioning
  → Gate + Tightener
  → Drive (one of five circuits) + Feel Engine
  → Tone Stack
  → Cab (dual IR slots)
  → Produce (Gloss → Doubler → Width → Room → Tilt)
  → Output
STEREO OUT
```

---

## 4. Stage Reference

### 4.1 INPUT
| Control | Range | What it does |
|---|---|---|
| **In Gain** | -24…+24 dB | Level into the plugin. Calibrate here so presets respond consistently. |
| **Pickup Tilt** | -3…+3 dB | Tilt EQ around 800 Hz to normalise pickups before the drive. Tilt **negative** for bright single-coils, **positive** for dark humbuckers. Default 0. |

A fixed 25 Hz subsonic high-pass is always active to keep the low end clean.

### 4.2 GATE / TIGHT
Both share one envelope detector on the conditioned signal.

| Control | Range | What it does |
|---|---|---|
| **Tight** | 40–350 Hz | Pre-drive resonant high-pass. Decides how much low-string energy reaches the distortion — raise it to tighten palm mutes and cut intermodulation mud. At minimum it is effectively off (clean-safe). |
| **Reso** | 0.3–2.5 | Resonance/Q of the Tight filter. Higher = a focused bump at the cutoff. |
| **Gate** | on/off | Noise gate. Off by default on clean circuits, on for high-gain. |
| **Thresh** | -80…0 dB | Level below which the signal is muted. |
| **Release** | 20–500 ms | Base release; the actual release is program-dependent — faster under dense palm-mutes, slower under sustained chords. |

### 4.3 DRIVE — five original circuits
Select with **Circuit**. Ordered clean → extreme:

| Circuit | Territory |
|---|---|
| **WAX** | Pristine clean → warm preamp. Soft asymmetric saturation, big headroom. |
| **STRAW** | Edge of breakup. Very touch-sensitive; cleans up at low input, breaks with pick attack. |
| **WICKER** | Crunch → classic rock. Midrange-complex, wide sweet spot. |
| **BRONZE** | Tight modern high gain. Cascaded stages, firm low end. |
| **ASH** | Extreme / modern metal. Highest gain, fast attack, built-in fizz management. |

| Control | What it does |
|---|---|
| **Drive** | Gain into the nonlinearity. |
| **Character** | Circuit-specific voicing — bias asymmetry (WAX), attack looseness (STRAW), clip blend (WICKER), stage-cascade balance (BRONZE), edge/clarity (ASH). |
| **Level** | Drive-stage output, for volume matching. |
| **OS 4x** | 4× oversampling of the nonlinearity (less aliasing, adds reported latency). Leave on for tracking. |

### 4.4 FEEL
Envelope-driven modulation of the drive — the difference between a tone processor and something that plays like an amp. Tuned per circuit (deeper/slower on vintage voicings, shallow/fast on modern).

| Control | What it does |
|---|---|
| **Feel** | Master macro (0–100%) scaling both Sag and Bloom within each circuit's range. 0 = static waveshaping only. |
| **Sag Depth** | How much the drive momentarily dips on transients (power-supply sag). |
| **Sag Rec** | How fast sag recovers. Slower = more vintage bloom. |
| **Bloom Knee** | Input level below which the tone cleans up — roll back your guitar volume under this knee for a natural clean. |

### 4.5 TONE
Global 4-band stack, post-drive, with per-circuit voicing tables (same knobs, different centre frequencies and Qs per circuit). Neutral (all noon) is voiced to sound finished.

| Control | What it does |
|---|---|
| **Bass / Mid / Treble / Presence** | ±12 dB each. |
| **Mid Shift** | Sweeps the Mid band's centre (300 Hz – 1.5 kHz) — the most genre-defining EQ move on guitar. |

### 4.6 CAB
Stereo begins here. Two convolution slots blended and panned.

| Control | What it does |
|---|---|
| **IR A / IR B** | Choose a factory IR or *None*, per slot. |
| **Load IR A / B** | Load your own WAV impulse (any sample rate; resampled on load). Stored by file path in the session. |
| **Blend** | Equal-power blend between slot A and slot B. |
| **Lvl A / Lvl B** | Per-slot level trim. |
| **Pan A / Pan B** | Per-slot stereo position — pan the two slots apart for a wide cab. |
| **Low Cut** | Post-cab high-pass, 20–200 Hz. |
| **High Cut** | Post-cab low-pass, 3–20 kHz (tames fizz). |
| **Cab Byp** | Bypass the cab (for external IR chains). |

**Factory IR families:** VESSEL, RELIC, IDOL, PYRE, MONUMENT — each with Core / Edge / Soft (and Twin on some). Try one bright and one dark slot panned apart as a starting point.

### 4.7 PRODUCE
The "finished tone" chain. Serial: **Gloss → Doubler → Width → Room → Tilt.** Each block is individually switchable.

| Block | Controls | What it does |
|---|---|---|
| **Gloss** | On, Gloss Amt | Optical-style compressor with auto makeup. Mainly for clean/funk/pop; off by default on high-gain. |
| **Doubler** | Doubler, Dbl Amt, Dbl Spread | ADT-style aperiodic micro-pitch + short delay per side. A "double without tracking twice" tool — it won't beat two real passes, and that's OK. |
| **Width** | Width, Width % | Stereo width 0–150% on the doubled/panned image. Watch the meter for mono compatibility. |
| **Room** | Room, Room Mix, Room Size | Short post-cab ambience — the "recorded in a space" cue. |
| **Tilt** | Tilt, Tilt dB | Final ±3 dB tilt around 700 Hz — the last-inch mix decision. |

### 4.8 OUTPUT
| Control | What it does |
|---|---|
| **Output** | Master trim (-24…+24 dB). |
| **Bypass** | Global bypass with a click-free crossfade. |
| **Meter** (right edge) | Stereo peak. Red at the top = clipping; back off Output or Level. |

---

## 5. Presets

28 factory presets, organised by circuit and spanning genres to prove the full-spectrum range (jazz and funk through djent and doom). Each preset is a **complete finished tone** — circuit, drive, feel, tone, cab pairing, and produce settings — not just a drive setting. Use them as finished starting points and adjust In Gain to your pickups.

---

## 6. Tips

- **Set input first.** In Gain drives everything downstream; get the meter healthy before dialing tone.
- **Tight is your friend on high gain.** If chugs sound flubby, raise Tight before touching Bass.
- **Feel to taste.** For modern tight rhythm keep Feel low; for expressive blues/lead push it up.
- **Two cabs, panned.** The cheapest wide-cab trick is a bright IR panned one way and a darker IR the other — it's built in.
- **Latency.** OS 4x adds reported latency (the DAW compensates). Turn it off only to save CPU on non-critical passes.
- **Session recall.** User IRs are stored by file path. If you move or rename the WAV, reload it.

---

## 7. Troubleshooting

| Symptom | Fix |
|---|---|
| No sound | Check the track is mono-in → stereo-out; check global **Bypass** is off and **Cab** has an IR loaded (or is bypassed). |
| Very quiet / very loud | Adjust **In Gain** and drive **Level**; watch the output meter. |
| User IR didn't recall | The WAV moved or was renamed — click **Load IR** and pick it again. |
| macOS won't open the plugin | Clear the quarantine flag (see Installation), or right-click-open the installer. |
| Boxes/odd characters in menus | Fixed in v1.0.1 — update to the latest build. |

---

*EFFIGY is for personal use. Original circuits, mic voicings and cabinets — a constructed likeness of a guitar production chain, not a capture of any named gear.*
