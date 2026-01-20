const sample = `import io from "runtime/io"

io.write(io.stdout, "hello\\n")
`;

const outputEl = document.getElementById("output");
const sourceEl = document.getElementById("source");
const runBtn = document.getElementById("run");
const statusEl = document.getElementById("status");

sourceEl.value = sample;
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
    const code = sourceEl.value;
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
