const fs = require('fs');
const os = require('os');
const path = require('path');
const { execFile } = require('child_process');

function getProjectRoot() {
  return path.resolve(__dirname, '..', '..', '..');
}

function getCompilerDirectory() {
  return path.join(getProjectRoot(), 'backend', 'compiler');
}

function getCompilerBinary() {
  const compilerDir = getCompilerDirectory();
  const candidates = [
    process.env.COMPILER_PATH,
    path.join(compilerDir, process.platform === 'win32' ? 'madrizuban.exe' : 'madrizuban'),
    path.join(compilerDir, 'madrizuban.exe'),
    path.join(compilerDir, 'madrizuban')
  ].filter(Boolean);

  for (const candidate of candidates) {
    if (fs.existsSync(candidate)) {
      return candidate;
    }
  }

  return null;
}

function writeTempSource(source) {
  const tempDir = fs.mkdtempSync(path.join(os.tmpdir(), 'madrizuban-'));
  const sourcePath = path.join(tempDir, 'input.mz');
  fs.writeFileSync(sourcePath, source, 'utf8');
  return { tempDir, sourcePath };
}

function removeTempDir(tempDir) {
  fs.rmSync(tempDir, { recursive: true, force: true });
}

function runCompiler(source, options = {}) {
  const compilerBinary = getCompilerBinary();
  if (!compilerBinary) {
    throw new Error('Compiler binary not found. Build backend/compiler first or set COMPILER_PATH.');
  }

  const { tempDir, sourcePath } = writeTempSource(source);
  const timeout = Number.isFinite(options.timeoutMs) ? options.timeoutMs : 15000;
  const cwd = getCompilerDirectory();

  return new Promise((resolve) => {
    execFile(compilerBinary, [sourcePath], { cwd, timeout, maxBuffer: 10 * 1024 * 1024 }, (error, stdout, stderr) => {
      removeTempDir(tempDir);

      if (error) {
        resolve({
          ok: false,
          exitCode: typeof error.code === 'number' ? error.code : 1,
          signal: error.signal || null,
          stdout: stdout || '',
          stderr: stderr || error.message,
          compilerBinary
        });
        return;
      }

      resolve({
        ok: true,
        exitCode: 0,
        signal: null,
        stdout: stdout || '',
        stderr: stderr || '',
        compilerBinary
      });
    });
  });
}

module.exports = {
  getCompilerBinary,
  runCompiler
};
