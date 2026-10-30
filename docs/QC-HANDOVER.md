# QC handover — Akari

Living checklist for a QC pass. Update after every milestone.

## Status: M0 skeleton complete

### What is done (M0)

- Repo layout per plan: `CMakeLists.txt`, `tests/`, `tools/`, `resources/`, `src/`, `docs/`.
- `src/core/ColorMath.{h,cpp}` and `src/core/Schedule.{h,cpp}` — pure C++17, no Be headers.
- `tests/test_core.cpp` + `tests/CMakeLists.txt` — ctest suite covering ColorMath endpoints,
  monotonicity, clamping, slider<->K, alpha mapping; Schedule validation, conversions, disabled,
  overnight wrap (20:00-07:00), same-day, on==off, invalid times, boundary wrap.
- Be API skeleton: `main.cpp`, `App`, `AkariWindow` (floating window, slider, schedule fields,
  status line, menu with About/Quit), `ReplicantView` (archivable Deskbar view), `TintController`
  (method pick + overlay application + state), `OverlayWindow` (borderless full-screen overlay),
  `Settings` (BMessage persistence at `~/config/settings/Akari/settings`).
- `tools/make-icon.py` generates lit + unlit HVIF lanterns (`resources/icon_lit.hvif`,
  `resources/icon_unlit.hvif`), each self-verified by re-parsing; lit variant embedded in
  `resources/app.rdef` (`app_signature`, `app_version` with Westenra Softworks copyright,
  `app_flags`, `file_types`, `vector_icon`).
- LICENSE (MIT, Copyright (c) 2026 Westenra Softworks), README.md (Haiku OS first line, keyword
  line, ⚠️ disclaimer, Contributing, build instructions, Future ideas, license note), AGENTS.md,
  .gitignore.

### How it was verified (sandbox, no Haiku)

- `cmake -B build-test -DAKARI_BUILD_TESTS=ON -S . && cmake --build build-test && ctest --test-dir build-test --output-on-failure`
  → `100% tests passed, 0 tests failed out of 1` (272 checks, 0 failures) after fixing four
  wrong hand-computed test expectations (6500 K blue ≈ 250 not 255; 3000 K green ≈ 177 / blue ≈ 110;
  19:59 is outside a 20:00–07:00 window).
- `python3 tools/make-icon.py` → both HVIFs written (195 bytes each), self-verify OK.
- Be-API code is NOT compiled in the sandbox (no libbe); it was written against the Haiku Book
  and current haiku/haiku headers (Window.h, View.h, Screen.h, Slider.h, Menu.h, Archivable.h,
  Deskbar.h, MessageRunner.h, rc parser/lexer, FlatIconImporter/FlatIconFormat).

### What to test on Haiku (M0 acceptance)

```sh
pkgman install -y cmd:git cmd:cmake cmd:ninja cmd:g++ haiku_devel
git clone https://github.com/Westenradev/Akari_for_Haiku_OS.git && cd Akari_for_Haiku_OS
cmake -B build -G Ninja && ninja -C build && ./build/Akari
```

- `ninja -C build` succeeds (watch for rdef/`rc`/`xres` step errors — the rdef syntax was
  validated against Haiku's `rc` parser source, but only `rc` on Haiku can confirm).
- App launches; the floating window appears and floats above other windows.
- Menu → About Akari shows the about alert; Quit exits cleanly.
- `~/config/settings/Akari/settings` is created on quit.
- App icon (lit lantern) appears in the Deskbar team list and the replicant shows in the tray.

### Known issues / limitations (M0)

- Gamma-ramp path is probed but currently always reports unsupported on Haiku (public BScreen
  has no gamma setter); the overlay is the only working tint method for now. Status line says so.
- The overlay window is not click-through: Haiku's public BWindow API has no click-through flag,
  so while the tint is active the overlay sits above other windows and eats clicks. Workaround for
  now: toggle Akari off. This is the main known limitation; see README Future ideas.
- Replicant icon loading from resources is best-effort; if it fails the replicant draws a
  colored square fallback.
- Schedule tick granularity is 30 s (BMessageRunner interval), so boundary toggles can lag up to
  30 s.
- No localization yet; UI strings are English.
