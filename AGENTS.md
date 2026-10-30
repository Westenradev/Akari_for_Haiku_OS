# Akari — project rules

Akari is a small native night-light app for Haiku OS written in C++ against the Be API (libbe) only. No Qt, no GTK, no external dependencies.

## Identity

- Name is **Akari** everywhere: files, classes, UI strings, app signature `application/x-vnd.WestenraSoftworks-Akari`, commit messages.
- Author and copyright holder is **Westenra Softworks**. Use exactly that string in the LICENSE copyright line, the README license note, the `.rdef` `app_version` `long_info`, and the MIME vendor prefix. Never use any other name or vendor prefix anywhere in the repo.

## Layout and architecture

- `src/core/` is platform-independent pure C++17 with **no Be headers**. All business logic (color math, schedule state machine) lives there so it can be unit-tested with any host compiler.
- Be-API code in `src/` stays thin: views contain no business logic, `TintController` owns state transitions, `Settings` owns persistence. One process; `BMessenger` between the replicant and the app.
- See `docs/ARCHITECTURE.md` for the module map and data flow, `docs/QC-HANDOVER.md` for the living QC checklist.

## Coding style

- Haiku coding style: tabs, Allman braces, two blank lines after the include guard, `fMember` prefix, `BClass` naming, `status_t` returns.
- Build with `-Wall -Wextra`; treat warnings as fix-worthy.
- Short functions (aim under 40 lines), one responsibility per file, no global state, no TODOs/FIXMEs/placeholders, no clever tricks. Document every public function's contract in its header (what it does, params, return, thread-safety). Comments only where the *why* is non-obvious.
- Prefer the Be API over POSIX in UI/app code; `src/core/` may use standard C++.

## Testing

- After every change to `src/core/`, run in a host sandbox (Linux is fine):
  `cmake -B build-test -DAKARI_BUILD_TESTS=ON -S . && cmake --build build-test && ctest --test-dir build-test --output-on-failure`
- Add a new test whenever a bug in `src/core/` is fixed.
- Before using any Be API you are unsure about, check the Haiku Book (https://www.haiku-os.org/docs/api/) and the Haiku source (github.com/haiku/haiku). Do not guess signatures.
- On Haiku: `cmake -B build -G Ninja && ninja -C build && ./build/Akari`.

## Content rules

- Never mention any other product or platform utility by name anywhere in the repo; describe features generically (e.g. "warms the screen color in the evening").
- Never commit build artifacts (`build/`, `*.o`, `*.rsrc`, `*.hpkg`, `.DS_Store` — see `.gitignore`).
- One commit per milestone (`M1: ...`), small `Fix: ...` commits when reported test results require them.
- After each milestone, append to `docs/QC-HANDOVER.md`: what changed, which tests ran and their output summary, what to test on Haiku, known issues.
