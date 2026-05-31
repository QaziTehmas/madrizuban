import { useMemo, useState, useRef } from 'react'
import './App.css'

const API_URL = import.meta.env.VITE_API_URL || 'http://localhost:3000'

const DEFAULT_SOURCE = `// MadriZuban ka aik pyara sa program
int x = 10;
string msg = "Salam Duniya! Ye Roman Urdu compiler hai.";
arzkro msg;

agar (x > 5) {
  arzkro "x panch se bara hai";
} warna {
  arzkro "x chota hai";
}

arzkro "Ab loop chalega:";
jabtak (x > 7) {
  arzkro x;
  x = x - 1;
}
`

const TABS = [
  { id: 'output', label: 'Nateejah (Output)' },
  { id: 'tokens', label: 'Haroof (Tokens)' },
  { id: 'ast', label: 'AST (Tree)' },
  { id: 'tac', label: 'TAC (Likhai)' },
  { id: 'errors', label: 'Ghaltiyan (Errors)' },
  { id: 'log', label: 'Poora Log (Full Log)' },
]

function downloadFile(filename, content) {
  const blob = new Blob([content], { type: 'text/plain;charset=utf-8' })
  const url = URL.createObjectURL(blob)
  const link = document.createElement('a')
  link.href = url
  link.download = filename
  link.click()
  URL.revokeObjectURL(url)
}

function cleanBlock(block) {
  return block.replace(/^\s+|\s+$/g, '')
}

function extractExecutionOutput(text) {
  const startIdx = text.search(/Program Output:/i);
  if (startIdx === -1) return '';
  const afterStart = text.slice(startIdx + "Program Output:".length);
  const lines = afterStart.split(/\r?\n/);

  let topDividerLineIndex = -1;
  for (let i = 0; i < lines.length; i++) {
    if (lines[i].trim().length > 0) {
      topDividerLineIndex = i;
      break;
    }
  }
  if (topDividerLineIndex === -1) return '';

  let bottomDividerLineIndex = -1;
  for (let i = topDividerLineIndex + 1; i < lines.length; i++) {
    if (lines[i].includes('Execution complete')) {
      bottomDividerLineIndex = i - 1;
      break;
    }
  }

  if (bottomDividerLineIndex === -1) {
    for (let i = topDividerLineIndex + 1; i < lines.length; i++) {
      if (lines[i].trim().match(/^[─\-=\s]+$/)) {
        bottomDividerLineIndex = i;
        break;
      }
    }
  }

  if (bottomDividerLineIndex === -1 || bottomDividerLineIndex <= topDividerLineIndex) {
    return cleanBlock(lines.slice(topDividerLineIndex + 1).join('\n'));
  }

  return cleanBlock(lines.slice(topDividerLineIndex + 1, bottomDividerLineIndex).join('\n'));
}

function extractTokens(text) {
  const startIdx = text.search(/Token Type/i);
  if (startIdx === -1) return '';

  const afterStart = text.slice(startIdx);
  const lines = afterStart.split(/\r?\n/);

  let dataStartIdx = 2;
  if (lines[1] && lines[1].trim().startsWith('-')) {
    dataStartIdx = 2;
  } else {
    dataStartIdx = 1;
  }

  const resultLines = [];
  for (let i = dataStartIdx; i < lines.length; i++) {
    const line = lines[i];
    if (line.includes('tokens erkened') || line.includes('Phase 2') || line.trim().startsWith('──') || line.trim().startsWith('==')) {
      break;
    }
    resultLines.push(line);
  }
  return cleanBlock(resultLines.join('\n'));
}

function extractAST(text) {
  const startIdx = text.search(/Abstract\s+Syntax\s+Tree:/i);
  if (startIdx === -1) return '';

  const afterStart = text.slice(startIdx + "Abstract Syntax Tree:".length);
  const lines = afterStart.split(/\r?\n/);

  const resultLines = [];
  for (let i = 0; i < lines.length; i++) {
    const line = lines[i];
    if (line.includes('Phase 3') || line.includes('Semantic Analysis') || line.trim().startsWith('──') || line.trim().startsWith('==')) {
      break;
    }
    resultLines.push(line);
  }
  return cleanBlock(resultLines.join('\n'));
}

function extractTAC(text) {
  const startIdx = text.search(/Three-Address\s+Code/i);
  if (startIdx === -1) return '';

  const afterStart = text.slice(startIdx);
  const lines = afterStart.split(/\r?\n/);

  const resultLines = [];
  for (let i = 1; i < lines.length; i++) {
    const line = lines[i];
    if (line.includes('TAC generation complete') || line.includes('Phase 5') || line.includes('Execution') || line.trim().startsWith('──') || line.trim().startsWith('==')) {
      break;
    }
    resultLines.push(line);
  }
  return cleanBlock(resultLines.join('\n'));
}

function extractErrors(stdout, stderr) {
  const cleanStdout = stdout.replace(/\x1b\[[0-9;]*m/g, '').replace(/\u001b\[[0-9;]*m/g, '');
  const cleanStderr = stderr.replace(/\x1b\[[0-9;]*m/g, '').replace(/\u001b\[[0-9;]*m/g, '');

  const errorLines = [];
  const lines = cleanStdout.split('\n');
  for (const line of lines) {
    if (line.includes('Ghalti') || line.includes('ghalti') || line.includes('Khabardar') || line.includes('ERROR:')) {
      errorLines.push(line.trim());
    }
  }

  if (cleanStderr.trim()) {
    errorLines.push(cleanStderr.trim());
  }
  return errorLines.join('\n');
}

function parseCompilerOutput(stdout, stderr = '') {
  const cleanStdout = stdout.replace(/\x1b\[[0-9;]*m/g, '').replace(/\u001b\[[0-9;]*m/g, '');
  const cleanStderr = stderr.replace(/\x1b\[[0-9;]*m/g, '').replace(/\u001b\[[0-9;]*m/g, '');

  const output = extractExecutionOutput(cleanStdout);
  const tokens = extractTokens(cleanStdout);
  const ast = extractAST(cleanStdout);
  const tac = extractTAC(cleanStdout);
  const errors = extractErrors(stdout, stderr);

  return {
    output,
    tokens,
    ast,
    tac,
    errors,
    log: cleanStdout.trim(),
  }
}

function prettifySection(section) {
  if (!section) {
    return 'Abhi tak kuch nahi hai dikhane ko.'
  }

  return section
    .split('\n')
    .map((line) => line.replace(/\s+$/g, ''))
    .join('\n')
    .trimEnd()
}

function App() {
  const [source, setSource] = useState(DEFAULT_SOURCE)
  const [fileName, setFileName] = useState('main.mz')
  const [activeTab, setActiveTab] = useState('output')
  const [isRunning, setIsRunning] = useState(false)
  const [status, setStatus] = useState('Chalanay kay liye tayyar')
  const [result, setResult] = useState({
    output: '',
    tokens: '',
    ast: '',
    tac: '',
    errors: '',
    log: '',
  })

  const textareaRef = useRef(null)
  const lineNumbersRef = useRef(null)

  const handleScroll = (event) => {
    if (lineNumbersRef.current) {
      lineNumbersRef.current.scrollTop = event.target.scrollTop
    }
  }

  const tabCounts = useMemo(() => ({
    tokens: result.tokens ? result.tokens.split('\n').length : 0,
    ast: result.ast ? result.ast.split('\n').length : 0,
    tac: result.tac ? result.tac.split('\n').length : 0,
  }), [result])

  const handleRun = async () => {
    const trimmed = source.trim()
    if (!trimmed) {
      setStatus('Pehle MadriZuban code likhein')
      setActiveTab('errors')
      setResult((current) => ({
        ...current,
        errors: 'Source code khaali hai.',
      }))
      return
    }

    setIsRunning(true)
    setStatus('Compiler chal raha hai...')

    try {
      const response = await fetch(`${API_URL}/api/compile`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({ source }),
      })

      const payload = await response.json()

      const errorText = payload.stderr || payload.error || '';
      const parsed = parseCompilerOutput(payload.stdout || '', errorText);

      if (!response.ok || payload.ok === false) {
        setResult({
          ...parsed,
          errors: errorText || parsed.errors || 'Compilation nakaam rahi.',
        })
        setActiveTab('errors')
        setStatus('Ghalti mili!')
        return
      }

      setResult(parsed)
      setActiveTab(parsed.errors ? 'errors' : 'output')
      setStatus('Kaam ho gaya!')
    } catch (error) {
      const message = error.message || 'Server tak pohnchne mein ghalti.'
      setResult((current) => ({
        ...current,
        errors: message,
      }))
      setActiveTab('errors')
      setStatus('Server nahi mila')
    } finally {
      setIsRunning(false)
    }
  }

  const handleSave = () => {
    downloadFile(fileName || 'main.mz', source)
    setStatus(`${fileName} mehfooz ho gayi!`)
  }

  const visibleSection = (() => {
    if (activeTab === 'output') {
      return prettifySection(result.output)
    }

    if (activeTab === 'tokens') {
      return prettifySection(result.tokens)
    }

    if (activeTab === 'ast') {
      return prettifySection(result.ast)
    }

    if (activeTab === 'tac') {
      return prettifySection(result.tac)
    }

    if (activeTab === 'errors') {
      return prettifySection(result.errors)
    }

    return prettifySection(result.log)
  })()

  const sourceLineCount = source.split('\n').length
  const sourceLines = source.split('\n')

  return (
    <div className="app-shell">
      <header className="topbar">
        <div className="brand-block">
          <div>
            <p className="eyebrow">MardiZuban: Apni Zuban mein Coding</p>
            <h1>Likhein, Chalayein, Dekhein</h1>
          </div>
        </div>

        <div className="topbar-actions">
          <div className="status-pill" aria-live="polite">
            {status}
          </div>
          <button type="button" className="ghost-button" onClick={handleSave}>
            Mehfooz Karein
          </button>
          <button
            type="button"
            className="primary-button"
            onClick={handleRun}
            disabled={isRunning}
          >
            <span className="run-icon" aria-hidden="true">
              ▶
            </span>
            {isRunning ? 'Chal raha hai...' : 'Chalao'}
          </button>
        </div>
      </header>

      <main className="workspace">
        <section className="panel editor-panel">
          <div className="panel-header">
            <div>
              <p className="panel-label">Likhai ki Jagah (Editor)</p>
              <h2>{fileName}</h2>
            </div>
            <label className="file-input-wrap">
              <span>File ka Naam</span>
              <input
                type="text"
                value={fileName}
                onChange={(event) => setFileName(event.target.value)}
                spellCheck="false"
              />
            </label>
          </div>

          <div className="editor-frame">
            <div className="line-numbers" ref={lineNumbersRef} aria-hidden="true">
              {sourceLines.map((_, index) => (
                <span key={index}>{index + 1}</span>
              ))}
              {sourceLineCount === 0 ? <span>1</span> : null}
            </div>
            <textarea
              ref={textareaRef}
              value={source}
              onChange={(event) => setSource(event.target.value)}
              onScroll={handleScroll}
              spellCheck="false"
              aria-label="MadriZuban source editor"
            />
          </div>

          <div className="panel-footer">
            <span>{sourceLineCount} lines</span>
            <span>{source.length} haroof</span>
          </div>
        </section>

        <section className="panel output-panel">
          <div className="panel-header output-header">
            <div>
              <p className="panel-label">Compiler ka Nazara</p>
              <h2>Nateejah aur Marahil</h2>
            </div>
            <div className="tab-counts">
              <span>{tabCounts.tokens} tokens</span>
              <span>{tabCounts.ast} AST lines</span>
              <span>{tabCounts.tac} TAC lines</span>
            </div>
          </div>

          <div className="tabs" role="tablist" aria-label="Compiler output tabs">
            {TABS.map((tab) => (
              <button
                key={tab.id}
                type="button"
                role="tab"
                aria-selected={activeTab === tab.id}
                className={activeTab === tab.id ? 'tab active' : 'tab'}
                onClick={() => setActiveTab(tab.id)}
              >
                {tab.label}
              </button>
            ))}
          </div>

          <pre className="output-shell" aria-live="polite">
            {visibleSection || 'Code chala kar nateejah yahan dekhein.'}
          </pre>
        </section>
      </main>
    </div>
  )
}

export default App
