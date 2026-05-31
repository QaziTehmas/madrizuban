# Backend API

This folder contains the Node.js service that powers the web-based compiler.

## What it does

- accepts source code from the React frontend
- saves it to a temporary `.mz` file
- runs the MadriZuban compiler/interpreter from `backend/compiler`
- returns stdout, stderr, exit code, and compiler availability info

## Local run

From the repository root:

```bash
cd backend/api
npm install
npm start
```

The service listens on `http://localhost:3000` by default.

## Endpoints

- `GET /health` - basic health check
- `POST /api/compile` - compile and run source code

Example request body:

```json
{
	"source": "int x = 1; arzkro x;"
}
```

## Docker

The repository root contains a `Dockerfile` that builds the compiler and starts this API service. This is the recommended Railway deployment path.
