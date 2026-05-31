const express = require('express');
const { runCompiler, getCompilerBinary } = require('./compilerRunner');

const app = express();
const port = process.env.PORT || 3000;

app.use(express.json({ limit: '1mb' }));

app.get('/health', (req, res) => {
  res.json({
    ok: true,
    service: 'madrizuban-api',
    compilerAvailable: Boolean(getCompilerBinary())
  });
});

app.post('/api/compile', async (req, res) => {
  try {
    const source = typeof req.body.source === 'string' ? req.body.source : '';
    const timeoutMs = Number(req.body.timeoutMs);

    if (!source.trim()) {
      return res.status(400).json({
        ok: false,
        error: 'Request body must include a non-empty source string.'
      });
    }

    const result = await runCompiler(source, {
      timeoutMs: Number.isFinite(timeoutMs) ? timeoutMs : undefined
    });

    return res.status(result.ok ? 200 : 500).json(result);
  } catch (error) {
    return res.status(500).json({
      ok: false,
      error: error.message || 'Unexpected server error.'
    });
  }
});

app.use((error, req, res, next) => {
  res.status(500).json({
    ok: false,
    error: error.message || 'Unhandled error.'
  });
});

app.listen(port, () => {
  console.log(`MadriZuban API listening on port ${port}`);
});
