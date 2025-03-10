let changeBrightness;
let mirrorImage;
let highlightText;
let zoomImage;
let brightness_val = 0;
let toggleMirror = false;
let toggleHighlightText = false;
let wasmReady = false; 
let videoStream = null;

Module.onRuntimeInitialized = function() {
    changeBrightness = Module.cwrap("change_brightness", null, ["number", "number", "number", "number"]);
    zoomImage = Module.cwrap("zoom_image", null, ["number", "number", "number","float"]);
    mirrorImage = Module.cwrap("mirror_image", null, ["number", "number", "number"]);
    highlightText = Module.cwrap("highlight_text", null, ["number", "number", "number"]);
    wasmReady = true;
    startVideo(); 
};

async function startVideo() {
    try {
        const stream = await navigator.mediaDevices.getUserMedia({ video: true });
        const video = document.getElementById("video");

        video.srcObject = stream;
        videoStream = stream;
        video.onloadedmetadata = () => {
            video.play();
            requestAnimationFrame(processFrame);
        };
    } catch (err) {
        console.error("Error accessing webcam:", err);
    }
}

function stopVideo() {
    if (videoStream) {
        videoStream.getTracks().forEach(track => track.stop()); 
        document.getElementById("video").srcObject = null;
        videoStream = null;
    }
}


let zoomLevel = 1.0;
const zoomStep = 0.1;
const minZoom = 1.0;
const maxZoom = 3.0;
const brightnessStep = 10;


function processFrame() {
    if (!wasmReady) return; 
    if (videoStream == null) return;

    const video = document.getElementById("video");
    const canvas = document.getElementById("canvas");
    const ctx = canvas.getContext("2d");

    const videoWidth = video.videoWidth;
    const videoHeight = video.videoHeight;
    canvas.width = videoWidth;
    canvas.height = videoHeight;

    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.drawImage(video, 0, 0, canvas.width, canvas.height);

    let imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);
    let dataPtr = Module._malloc(imageData.data.length);
    Module.HEAPU8.set(imageData.data, dataPtr);

    changeBrightness(dataPtr, canvas.width, canvas.height, brightness_val);
    zoomImage(dataPtr, canvas.width, canvas.height, zoomLevel);

    if (toggleMirror) {
        mirrorImage(dataPtr, canvas.width, canvas.height);
    }
    
    if (toggleHighlightText) {
        highlightText(dataPtr, canvas.width, canvas.height);
    }

    let outputData = Module.HEAPU8.subarray(dataPtr, dataPtr + imageData.data.length);
    imageData.data.set(outputData);
    ctx.putImageData(imageData, 0, 0);

    Module._free(dataPtr);

    requestAnimationFrame(processFrame);
}


function brightnessUp() {
    brightness_val = brightness_val + brightnessStep;
}
function brightnessDown() {
    brightness_val = brightness_val - brightnessStep;
}
function zoomIn() {
    if (zoomLevel < maxZoom) {
        zoomLevel += zoomStep;
    }
}
function zoomOut() {
    if (zoomLevel > minZoom) {
        zoomLevel -= zoomStep;
    }
}
function mirror() {
    toggleMirror = !toggleMirror;
}
function text() {
    toggleHighlightText = !toggleHighlightText;
}