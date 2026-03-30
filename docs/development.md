# Development Workflow

## Build and Run

Build GUI app:

```bash
./scripts/build.sh
```

Run GUI app:

```bash
./scripts/start.sh
```

## Tests

Run tests:

```bash
./scripts/test.sh
```

The first script call will ensure `tests/lib` exists by cloning:
- `https://github.com/google/googletest.git`

## Common Commands

Use a custom build directory:

```bash
./scripts/build.sh build-debug
./scripts/start.sh build-debug
./scripts/test.sh build-debug
```

## Troubleshooting

- If Qt is not discovered automatically, install `qt` or `qt@6` with Homebrew.
- If test dependencies are broken, remove `tests/lib` and run scripts again.
