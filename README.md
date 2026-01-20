# Protolex Web (Emscripten)

This builds a browser version of the interpreter and a small playground.

## Build

Make sure EMSDK is active:

```sh
source /Users/avialle/dev/emsdk/emsdk_env.sh
```

Then build:

```sh
make web
```

Output files:
- `web/protolex.js`
- `web/protolex.wasm`
- `web/protolex.data`

## Run locally

Serve the `web/` folder with a static server:

```sh
python3 -m http.server 8000 -d web
```

Open `http://localhost:8000` and run code in the textarea.

Notes:
- The editor uses CodeMirror 5 from a CDN for syntax highlighting.
