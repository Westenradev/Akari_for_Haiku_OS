# Akari

Akari (明かり, "light") is a small, free, open-source, **native app for Haiku OS** — a night light that warms the screen color in the evening so the display is easier on your eyes at night.

Keywords: haiku haiku-os beos haiku-app native cpp night-light screen-warmth blue-light-filter utility open-source community

> ⚠️ **Disclaimer**
>
> Akari is a first-time project by its author and a learning project — expect rough edges.
> It is experimental and meant as a test: it is provided **as-is, with no warranty of any kind**.
> It was developed **with AI assistance**.
> **Use it at your own risk.** The screen tint changes what you see; if that could cause a problem for you, don't run it.

## What it does

- A small always-on-top floating window and a Deskbar replicant show the current state.
- A slider sets the warmth (color temperature, about 6500 K during the day down to about 3000 K at night) and a checkbox enables/disables the tint.
- An optional schedule turns the tint on at a start time and off at an end time (default 20:00–07:00), with a manual override.
- The tint is applied through the best method available on your system: hardware gamma ramps when the driver supports them, otherwise a translucent full-screen overlay window. The chosen method is shown in the window's status line.
- Settings (window position, warmth, schedule, enabled state, last method) persist in `~/config/settings/Akari/settings`.
- The Deskbar replicant shows a lit lantern while the tint is active and an unlit one when it is off.

## Building

On Haiku (R1/beta4 or later, x86_64):

```sh
pkgman install -y cmd:git cmd:cmake cmd:ninja cmd:g++ haiku_devel
git clone https://github.com/Westenradev/Akari_for_Haiku_OS.git
cd Akari_for_Haiku_OS
cmake -B build -G Ninja
ninja -C build
./build/Akari
```

Host-side unit tests for the platform-independent core (no Haiku needed, any C++17 compiler):

```sh
cmake -B build-test -DAKARI_BUILD_TESTS=ON -S .
cmake --build build-test
ctest --test-dir build-test --output-on-failure
```

Regenerating the icons (pure Python, self-verifying):

```sh
python3 tools/make-icon.py
```

## Contributing

Akari is a gift to the Haiku community. Anyone is welcome to contribute — issues and pull requests are very welcome, and experienced Haiku developers are warmly invited to mentor and correct beginner mistakes. Please read `AGENTS.md` for the project rules (coding style, architecture, testing).

## Future ideas

- Per-workspace or per-monitor tint strength.
- Smooth transitions (fade in/out over several seconds instead of instant switches).
- Localization of UI strings via the Haiku catalog system.
- A settings window separate from the floating panel.
- Gamma-ramp support probing for more drivers as Haiku's screen API grows.
- CLI flags (e.g. `Akari --off`, `Akari --warmth 4000`) for scripting.

## License

MIT License — see [LICENSE](LICENSE). Copyright (c) 2026 Westenra Softworks.
