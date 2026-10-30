# Akari architecture

Written for a reviewer who has only this repository.

## Module map

```
src/
├── main.cpp               # entry point: creates App, runs it
├── App.{h,cpp}            # BApplication (signature application/x-vnd.WestenraSoftworks-Akari);
│                          # owns Settings + TintController, creates the window and the
│                          # Deskbar replicant, routes messages between them
├── AkariWindow.{h,cpp}    # floating always-on-top panel: enable checkbox, warmth slider,
│                          # schedule fields, override checkbox, status line, App menu
├── ReplicantView.{h,cpp}  # BArchivable BView for the Deskbar tray / Desktop; click toggles
│                          # the tint via BMessenger to the app; lit/unlit icon
├── TintController.{h,cpp} # owns all tint state transitions: computes active state from
│                          # settings + schedule + override, picks the method (gamma probe vs
│                          # overlay fallback), applies it
├── OverlayWindow.{h,cpp}  # borderless B_NO_BORDER_WINDOW_LOOK full-screen window with one
│                          # B_OP_ALPHA view; the portable tint fallback
├── Settings.{h,cpp}       # loads/saves a BMessage at ~/config/settings/Akari/settings
└── core/                  # PLATFORM-INDEPENDENT pure C++17, no Be headers
    ├── ColorMath.{h,cpp}  # kelvin clamping, slider<->kelvin, kelvin->RGB (Tanner Helland
    │                      # approximation), warmth->overlay alpha
    └── Schedule.{h,cpp}   # DayTime validation/conversion, IsWithinSchedule (handles
                           # overnight ranges like 20:00-07:00 and on==off)
```

## Data flow

1. `App` loads `Settings` (BMessage from `find_directory(B_USER_SETTINGS_PATH)/Akari/settings`),
   pushes them into `TintController`, and creates `AkariWindow` + `ReplicantView`.
2. UI events (slider, checkboxes, text fields) arrive as BMessages in `AkariWindow::MessageReceived`
   and are forwarded to `TintController` setters. The replicant sends `kMsgReplicantToggle` to the
   app via `BMessenger`, which flips `enabled`.
3. `TintController::Apply()` computes `active = enabled && (override ? overrideActive :
   schedule.enabled ? IsWithinSchedule(schedule, now) : true)` and, if active, shows/updates the
   overlay (fallback) with color+alpha derived from the warmth slider via `core/ColorMath`.
   A 30 s `BMessageRunner` tick in `AkariWindow` calls `TintController::Tick()` so schedule
   boundaries are picked up.
4. `AkariWindow::QuitRequested()` saves the window frame and settings back through
   `TintController::SaveSettings()`.

## Why core/ is separate

The agent sandbox and CI have no Haiku, so `src/core/` must compile with any host C++17
compiler. That is what `tests/test_core.cpp` (ctest) covers: temperature->RGB endpoints and
monotonicity, clamping, slider<->K mapping, overlay alpha mapping, and all schedule edge cases
(on==off, overnight wrap, exact boundaries, disabled schedule, invalid times).

## Tint methods

- **Gamma ramps** (preferred when available): probed at runtime; on current Haiku the public
  `BScreen` API exposes brightness but no gamma-ramp setter, so the probe reports unsupported
  and the overlay is used. `TintController::MethodName()` reports what was chosen; the status
  line in the window shows it so users can report issues.
- **Overlay window** (fallback, guaranteed): a full-screen borderless window above everything
  with a single view drawn in `B_OP_ALPHA` with an amber fill whose alpha encodes warmth
  (`WarmthToAlpha`). It does not take focus (`B_AVOID_FOCUS`) and is not interactive.

## Extending

- New warmth curve: change `core/ColorMath.cpp` only, add tests in `tests/test_core.cpp`.
- New schedule semantics: change `core/Schedule.cpp` only, add tests.
- New UI control: add it in `AkariWindow::BuildLayout`, forward its message to
  `TintController`; never put logic in the view.
- New tint method: implement in `TintController` behind a `TintMethod` enum value, update
  `MethodName()` and the status line.
- New persisted setting: add a field to `AppSettings`, and read/write it in
  `Settings::Load`/`Settings::Save`.

## Icons

`tools/make-icon.py` (pure stdlib Python) generates `resources/icon_lit.hvif` and
`resources/icon_unlit.hvif` — original chōchin lantern geometry in HVIF format — and
self-verifies each file by re-parsing it with the same `write_coord`/`read_coord` encoding
Haiku's `FlatIconImporter` uses. The lit variant is embedded in `resources/app.rdef` as
`vector_icon`; CMake compiles the rdef with `rc` and attaches it with `xres` on Haiku.
