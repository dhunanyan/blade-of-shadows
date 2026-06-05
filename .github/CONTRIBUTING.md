# Contributing to Blade of Shadows

Thanks for contributing.

## Development Flow

1. Fork the repository or create a feature branch.
2. Keep changes focused on a single concern.
3. Build the GUI target:

```bash
./scripts/build.sh
```

4. Run the tests:

```bash
./scripts/test.sh
```

5. Open a pull request with a clear summary, screenshots for visual changes, and testing notes.

## Project Guidelines

- Prefer small, reviewable pull requests.
- Keep gameplay logic inside core/gameplay classes rather than Qt UI classes.
- Reuse existing project structure under `include/`, `src/`, `assets/`, and `tests/`.
- Do not commit `build/` artifacts or `tests/lib/`.
- When changing visuals or controls, include a short note describing the player-facing behavior change.

## Code Style

- Use modern C++ with the current project standard.
- Keep headers focused on declarations and small inline getters/setters.
- Keep implementation details in `.cpp` files.
- Write names and comments in English.

## Reporting Issues

Use the GitHub issue templates for bug reports and feature ideas whenever possible.
