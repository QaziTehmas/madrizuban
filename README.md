# MardiZuban — Web Compiler Platform & IDE

MardiZuban (Urdu: مادری زبان — Mother Tongue) is a programming language featuring Roman Urdu keywords (like `agar`, `warna`, `jabtak`, and `arzkro`) that compiles and runs through all five classic phases: lexical analysis, parsing, semantic checks, Three-Address Code (TAC) generation, and tree-walk interpretation.

This repository is organized as a clean monorepo containing the native compiler, the Node.js runner API, and the React browser-based IDE.

---

## Project Structure

- **`backend/compiler/`**: The core compiler written in C (C99).
- **`backend/api/`**: Express.js REST API that accepts source code, compiles it inside a temporary folder, and returns execution metrics.
- **`frontend/`**: React + Vite web IDE featuring a VS Code-themed dark editor, layout scroll locks, and segmented phase tabs.

---

## Prerequisites

Ensure you have the following installed:
1. **Node.js** (v18 or v20+)
2. **GCC Compiler** / MinGW toolchain (for building the native C executable)
3. **GNU Make** / `mingw32-make` (optional, for running make commands)

---

## Setup & Local Development

Follow these steps to build and run the entire platform locally:

### Step 1: Build the C Compiler
First, compile the C compiler code into an executable:
```bash
cd backend/compiler

# Option A: If you have Make installed:
# On Windows:
mingw32-make
# On Linux/macOS:
make

# Option B: Direct GCC compilation:
gcc -Wall -Wextra -g -std=c99 -pedantic src/main.c src/lexer.c src/parser.c src/symtable.c src/semantic.c src/codegen.c src/interpreter.c -o madrizuban.exe
```
This produces the `madrizuban.exe` (or `madrizuban` on Linux) executable.

### Step 2: Start the Backend API
The Express API acts as the bridge executing the compiler on request:
```bash
cd ../api
npm install
npm start
```
The server will start listening on **`http://localhost:3000`**. You can verify it by hitting `http://localhost:3000/health` in your browser.

### Step 3: Run the Frontend IDE
Launch the browser IDE:
```bash
cd ../../frontend
npm install
npm run dev
```
Open **`http://localhost:5173`** (or the URL printed by Vite) to access the MardiZuban Web IDE.

---

## Running Compiler Tests
To execute the smoke and regression test suite directly on the C compiler:
```bash
cd backend/compiler
# On Windows:
mingw32-make test
# On Linux:
make test
```
This tests arithmetic operations, conditional branches (`agar`/`warna`), while loops (`jabtak`), boolean validations, nested conditions, and syntax error detections.

---

## Docker & Production Deployment

The project is ready for multi-tier cloud deployment:
- **Backend API**: The root `Dockerfile` packages the Node environment, installs standard build systems, runs `make` to compile the compiler, and boots Express on port 3000. This is optimized to run as a single service on Railway or Render.
- **Frontend UI**: Built statically via `npm run build` and optimized to deploy to Vercel or Netlify (communicates with backend API using `VITE_API_URL`).
