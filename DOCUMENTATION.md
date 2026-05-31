# MardiZuban Developer Documentation

Welcome to the official developer and usage documentation for **MardiZuban** (Roman Urdu programming language compiler and web IDE platform). This document details language syntax specifications, compiler execution commands, Express API endpoints, frontend web architecture, and troubleshooting procedures.

---

## Table of Contents
1. [MardiZuban Language Specification](#1-mardizuban-language-specification)
   - [Data Types](#data-types)
   - [Variables & Assignments](#variables--assignments)
   - [Printing Output (`arzkro`)](#printing-output-arzkro)
   - [Branching (`agar` / `warna`)](#branching-agar--warna)
   - [Loops (`jabtak`)](#loops-jabtak)
2. [Compiler CLI Usage](#2-compiler-cli-usage)
3. [REST API Documentation](#3-rest-api-documentation)
   - [`GET /health`](#get-health)
   - [`POST /api/compile`](#post-apicompile)
4. [Frontend Web IDE Architecture](#4-frontend-web-ide-architecture)
5. [Troubleshooting Guide](#5-troubleshooting-guide)

---

## 1. MardiZuban Language Specification

MardiZuban is a strongly-typed imperative language featuring C-like block structures, using Roman Urdu keywords.

### Data Types

| Type Keyword | Description | Range / Literal Form |
| :--- | :--- | :--- |
| `int` | 32-bit signed integer | `-2,147,483,648` to `2,147,483,647` |
| `string` | UTF-8 String literal | Block enclosed in double quotes (e.g. `"Salam"`) |
| `bool` | Boolean state | `sach` (true) or `jhoot` (false) |

### Variables & Assignments

Variables must be declared with their type before being assigned. Re-declaration in the same scope is prohibited.

```javascript
// Variable Declarations
int a = 10;
string msg = "MardiZuban";
bool flag = sach;

// Value updates (assignments)
a = 25;
flag = jhoot;
```

### Printing Output (`arzkro`)

The `arzkro` keyword prints variable values or literal values to standard output, followed by a newline:

```javascript
arzkro "Salam!";    // Prints: Salam!
arzkro a;           // Prints the value of variable 'a'
arzkro flag;        // Prints: jhoot (if false)
```

### Branching (`agar` / `warna`)

Allows conditional executions of code blocks. The conditional expression inside the parenthesis must evaluate to a boolean or integer condition.

```javascript
int score = 85;

agar (score >= 90) {
    arzkro "A Grade!";
} warna {
    agar (score >= 80) {
        arzkro "B Grade!";
    } warna {
        arzkro "Fail!";
    }
}
```

### Loops (`jabtak`)

Repeats execution of a block of code `jabtak` (while) the conditional expression evaluates to `sach` (true or non-zero).

```javascript
int n = 5;
int factorial = 1;

jabtak (n > 0) {
    factorial = factorial * n;
    n = n - 1;
}

arzkro factorial; // Prints: 120
```

---

## 2. Compiler CLI Usage

The hand-written C compiler executable `madrizuban.exe` (or `madrizuban` on Unix) serves as the core parsing and execution engine.

### Run Source File
```bash
./madrizuban.exe <filepath.mz>
```
*Example:*
```bash
./madrizuban.exe tests/test1_arithmetic.mz
```

### Interactive Compiler Output Stages
Upon execution, the compiler outputs diagnostic logs for the five execution phases:
1. **`── Phase 1: Lexical Analysis  (Lexer) ──`**: Prints token table containing line numbers, token structures (e.g. `INT_LIT`, `IDENT`), and literal lexemes.
2. **`── Phase 2: Syntax Analysis   (Parser) ──`**: Validates CFG syntax rules and prints the Abstract Syntax Tree (AST).
3. **`── Phase 3: Semantic Analysis ──`**: Performs type validation and prints the variable registration Symbol Table.
4. **`── Phase 4: Intermediate Code Generation  (TAC) ──`**: Emits the generated Three-Address Code instructions.
5. **`── Phase 5: Execution  (Interpreter) ──`**: Runs the program and displays stdout text inside the output delimiters.

---

## 3. REST API Documentation

The Express service (`backend/api`) hosts the REST API to run the compiled binary on demand.

### `GET /health`
Returns service health and checks compiler binary availability.

**Example Request:**
```bash
curl http://localhost:3000/health
```

**Response (200 OK):**
```json
{
  "ok": true,
  "service": "madrizuban-api",
  "compilerAvailable": true
}
```

### `POST /api/compile`
Accepts MardiZuban source code, executes compilation, and returns stdout logs.

**Headers:**
- `Content-Type: application/json`

**Request Body JSON:**
```json
{
  "source": "int a = 5; arzkro a * 10;"
}
```

**Response (200 OK):**
```json
{
  "ok": true,
  "exitCode": 0,
  "signal": null,
  "stdout": "<compiler diagnostic logs>",
  "stderr": "",
  "compilerBinary": "d:\\Tehmas\\8\\Compiler Construction Lab\\Project\\madrizuban\\backend\\compiler\\madrizuban.exe"
}
```

**Response (500 Internal Error / Compile Fail):**
```json
{
  "ok": false,
  "exitCode": 1,
  "signal": null,
  "stdout": "<partial logs built before failure>",
  "stderr": "[MadriZuban Ghalti] bhaiyaaaa Line 1: ';' chahiye...",
  "compilerBinary": "d:\\Tehmas\\8\\Compiler..."
}
```

---

## 4. Frontend Web IDE Architecture

The React IDE (`frontend/`) coordinates compilation requests and parses stdout strings into specific UI tabs:

1. **Height & Viewport Constraint**: The outer `.app-shell` uses `height: 100vh; overflow: hidden`. This locks browser viewport scrollbars, enforcing internal scrolls.
2. **Scrolling synchronization**: Scroll positions of the code `textarea` and the `.line-numbers` list are synchronized programmatically via the `onScroll` handler:
   ```javascript
   const handleScroll = (event) => {
     lineNumbersRef.current.scrollTop = event.target.scrollTop;
   };
   ```
3. **Gutter Width margins**: Large-screen side margins are removed using broad layout rules in `App.css`.
4. **Segment Parsing**: Extracted outputs are parsed using robust line-splitting functions in `App.jsx` (`extractExecutionOutput`, `extractTokens`, `extractAST`, `extractTAC`, `extractErrors`) to make them resilient against ANSI coloring codes and character set encodings.

---

## 5. Troubleshooting Guide

### 1. Error: `listen EADDRINUSE: address already in use :::3000`
- **Cause**: The API server is already running in another process or command-line session (e.g. background tasks or past terminals) using port 3000.
- **Resolution**:
  - **Option A (Find & Kill process)**:
    - *Windows Command:*
      ```powershell
      Get-Process -Id (Get-NetTCPConnection -LocalPort 3000).OwningProcess | Stop-Process -Force
      ```
    - *Linux/macOS Command:*
      ```bash
      kill -9 $(lsof -t -i:3000)
      ```
  - **Option B (Configure Port)**: Change the backend listening port by starting it with the `PORT` env parameter:
    ```bash
    $env:PORT=3001; npm start
    ```

### 2. Unicode / Character Set corruption (Dashes appear as ``)
- **Cause**: Windows Command Prompt or PowerShell uses CP1252 or OEM code page by default, rendering UTF-8 box characters incorrectly.
- **Resolution**: This is a console rendering issue only. The backend API captures standard bytes and correctly parses them as UTF-8 in the web browser. To fix local terminal encoding, run:
  ```powershell
  chcp 65001
  ```

### 3. File Read returns Garbage data on Windows compiles
- **Cause**: Text-mode file reading converts CRLF (`\r\n`) to LF (`\n`), reducing size while `ftell` returns the raw disk size. This causes uninitialized memory reads at the end of the file.
- **Resolution**: Standard C source compiles now use the actual count from `fread` to write the null-terminator:
  ```c
  size_t actual = fread(buf, 1, sz, f);
  buf[actual] = '\0';
  ```
