# MadriZuban Monorepo

This repository is being reorganized for a web-based compiler setup.

## Current layout

- `backend/compiler/` - the existing C compiler/interpreter project
- `backend/api/` - future API server that will receive code and run the compiler
- `frontend/` - future React app with the editor and run/save UI

## Where the current compiler lives

The compiler sources, tests, and build files were moved to:

- `backend/compiler/src/`
- `backend/compiler/tests/`
- `backend/compiler/Makefile`
- `backend/compiler/README.txt`
- `backend/compiler/BUILD_AND_RUN.md`

## Build the compiler

```bash
cd backend/compiler
make
```

If `make` is not available on Windows, use the direct `gcc` command listed in `backend/compiler/BUILD_AND_RUN.md`.
