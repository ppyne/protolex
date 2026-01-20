const sample = `import io from "runtime/io"

io.write(io.stdout, "hello\\n")
`;

const outputEl = document.getElementById("output");
const sourceEl = document.getElementById("source");
const runBtn = document.getElementById("run");
const statusEl = document.getElementById("status");

let editor = null;

if (window.CodeMirror && window.CodeMirror.defineSimpleMode) {
  CodeMirror.defineSimpleMode("protolex", {
    start: [
      { regex: /#.*$/, token: "comment" },
      { regex: /"(?:[^"\\]|\\.)*"/, token: "string" },
      {
        regex:
          /\b(?:0x[0-9a-fA-F]+|0b[01]+|0o[0-7]+|\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?\b/,
        token: "number",
      },
      {
        regex:
          /\b(?:import|from|fn|if|else|mutate|undefine|try|catch|finally|throw)\b/,
        token: "keyword",
      },
      { regex: /\b(?:true|false|null)\b/, token: "atom" },
      {
        regex:
          /\b(?:has|clone|freeze|isAbsent|isInt|isFloat|isString|isBool|isTable|isFunction)\b/,
        token: "keyword",
      },
      {
        regex:
          /\b(?:List|Array|Map|Set|Stack|Queue|Tree|Graph)\b/,
        token: "atom",
      },
      { regex: /\bproto\b/, token: "atom" },
      { regex: /==|!=|<=|>=|&&|\|\||[+\-*/%<>=.]/, token: "operator" },
    ],
  });
  editor = CodeMirror.fromTextArea(sourceEl, {
    mode: "protolex",
    theme: "protolex",
    lineNumbers: true,
    lineWrapping: true,
  });
  editor.setValue(sample);
} else {
  sourceEl.value = sample;
}

runBtn.disabled = true;

const appendOut = (text) => {
  outputEl.textContent += text + "\n";
};

const clearOut = () => {
  outputEl.textContent = "";
};

let runtime = null;

const moduleConfig = {
  print: (text) => appendOut(text),
  printErr: (text) => appendOut(`[error] ${text}`),
};

window.protolexWrite = (text) => {
  appendOut(text);
};

window.protolexWriteErr = (text) => {
  appendOut(`[error] ${text}`);
};

window.startProtolex = () => {
  if (typeof Protolex !== "function") {
    appendOut("[error] protolex.js did not load");
    return;
  }
  Protolex(moduleConfig).then((instance) => {
    runtime = instance;
    statusEl.textContent = "Runtime ready";
    runBtn.disabled = false;
  });
};

const runProgram = () => {
  if (!runtime || !runtime.FS || !runtime.callMain) {
    appendOut("[error] runtime not ready");
    return;
  }
  clearOut();
  statusEl.textContent = "Running…";
  runBtn.disabled = true;
  try {
    const code = editor ? editor.getValue() : sourceEl.value;
    runtime.FS.writeFile("/program.plx", code);
    runtime.callMain(["/program.plx"]);
  } catch (err) {
    appendOut(`[exception] ${err}`);
  } finally {
    runBtn.disabled = false;
    statusEl.textContent = "Runtime ready";
  }
};

runBtn.addEventListener("click", runProgram);
