**WebAssembly Simple Live Image Processing Application**

This project uses WebAssembly (WASM) with Emscripten to apply real-time image effects like brightness adjustment, zoom, mirroring, and text highlighting on a webcam feed or uploaded image.

**Features**

Adjust brightness (+/-), Zoom in and out, Mirror image, Highlight text

**Setup**

Ensure you have Emscripten installed.
Compile the C code using:
emcc features.c -o features.js -sEXPORTED_FUNCTIONS='["_change_brightness","_mirror_image","_zoom_image","_highlight_text","_malloc","_free"]' -sEXPORTED_RUNTIME_METHODS='["cwrap", "ccall"]' -sALLOW_MEMORY_GROWTH=1
Open index.html in a browser to test the application.

**Usage**

Click buttons to apply effects in real-time.
Upload an image or use a live webcam feed.

**Notes**

WebAssembly enables fast image processing in the browser.
Ensure the browser supports WebAssembly and getUserMedia for webcam access.
