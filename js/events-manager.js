addEventListener("init-env", () => {
    canvas.c.addEventListener("mousedown", function (e) {
        if (e.buttons == 1) {
            canvas.wireTempStatic = true;
            canvas.wireTemp = [getMousePos(), getMousePos()];
            canvas.wireDir = "h";
            refreshCanvasTemp();
        }
    });

    document.addEventListener("mouseup", function (e) {
        canvas.wireTemp = undefined;
        refreshCanvasTemp();
    });

    canvas.c.addEventListener("mouseup", function (e) {
        if (e.buttons == 0 && canvas.wireTemp != undefined) {
            if (paletItems[paletIndex] == "Wire") {
                if (canvas.wireTemp[0].x == canvas.wireTemp[1].x && canvas.wireTemp[0].y == canvas.wireTemp[1].y) {
                    if (board.state == "edit") {
                        let x = canvas.mouse.x - canvas.wireTemp[1].x * board.scale + board.x;
                        let y = canvas.mouse.y - canvas.wireTemp[1].y * board.scale + board.y;
                        let state = (x > y ? 0 : 1) + (-x < y ? 0 : 2);
                        let p = { x: canvas.wireTemp[1].x, y: canvas.wireTemp[1].y };
                        let d = 0;
                        if (state == 1) d = 1;
                        else if (state == 2) { p.y--; d = 1; }
                        else if (state == 3) p.x--;
                        if (!board.clickEvent(canvas.wireTemp[1])) {
                            if (!board.swapObjNot(p, d)) {
                                board.swapNode(canvas.wireTemp[1]);
                                refreshCanvasWires();
                            }
                        }
                        refreshCanvasObj();
                    } else {
                        board.clickEvent(canvas.wireTemp[1]);
                    }
                } else {
                    if (canvas.wireDir == "h") {
                        board.addWire(canvas.wireTemp[0], {
                            x: canvas.wireTemp[1].x,
                            y: canvas.wireTemp[0].y
                        }, canvas.wireTemp[1]);
                    } else {
                        board.addWire(canvas.wireTemp[0], {
                            x: canvas.wireTemp[0].x,
                            y: canvas.wireTemp[1].y
                        }, canvas.wireTemp[1]);
                    }
                }
                refreshCanvasWires();
            } else if (paletItems[paletIndex] == "Eraser") {
                if (canvas.wireTemp[0].x == canvas.wireTemp[1].x && canvas.wireTemp[0].y == canvas.wireTemp[1].y) {
                    board.removeObj(canvas.wireTemp[1]);
                    refreshCanvasObj();
                } else {
                    if (canvas.wireDir == "h") {
                        board.removeWire(canvas.wireTemp[0], {
                            x: canvas.wireTemp[1].x,
                            y: canvas.wireTemp[0].y
                        }, canvas.wireTemp[1]);
                    } else {
                        board.removeWire(canvas.wireTemp[0], {
                            x: canvas.wireTemp[0].x,
                            y: canvas.wireTemp[1].y
                        }, canvas.wireTemp[1]);
                    }
                    refreshCanvasWires();
                }
            } else if (paletItems[paletIndex] == "Select") {
                let x0 = canvas.wireTemp[0].x;
                let y0 = canvas.wireTemp[0].y;
                let x1 = canvas.wireTemp[1].x;
                let y1 = canvas.wireTemp[1].y;

                if (!select) {
                    if (x0 > x1) {
                        let t = x0;
                        x0 = x1;
                        x1 = t;
                    } if (y0 > y1) {
                        let t = y0;
                        y0 = y1;
                        y1 = t;
                    }

                    select = board.selectRect(x0, y0, x1 - x0, y1 - y0);
                    canvas.wireTemp = undefined;
                    refreshAllCanvas();
                } else if (canvas.wireTempStatic) {
                    board.unSelect();
                }
            } else {
                let id = getObjByName(paletItems[paletIndex]);
                if (id != undefined) {
                    board.addObj(canvas.wireTemp[1], id);
                    refreshCanvasObj();
                }
            }
            canvas.wireTemp = undefined;
            refreshCanvasTemp();
        }
    });

    document.addEventListener("mousemove", function (e) {
        if (e.x < canvas.obj.offsetLeft || e.y < canvas.obj.offsetTop)
            refreshCanvasTemp();
    });

    canvas.c.addEventListener("mousemove", function (e) {
        let mX = e.x - canvas.obj.offsetLeft;
        let mY = e.y - canvas.obj.offsetTop;
        if ((e.buttons & 2) > 0) { // RIGHT
            board.x += canvas.mouse.x - mX;
            board.y += canvas.mouse.y - mY;
            refreshAllCanvas();
        } else if ((e.buttons & 1) > 0 && canvas.wireTemp != undefined) {
            let mp = getMousePos();
            if (canvas.wireTemp[0].x == canvas.wireTemp[1].x && canvas.wireTemp[0].y == canvas.wireTemp[1].y && !(canvas.wireTemp[0].x == mp.x && canvas.wireTemp[0].y == mp.y)) {
                if (Math.abs(canvas.wireTemp[0].x - mp.x) > Math.abs(canvas.wireTemp[0].y - mp.y))
                    canvas.wireDir = "h";
                else canvas.wireDir = "v";
            }
            canvas.wireTemp[2] = canvas.wireTemp[1];
            canvas.wireTemp[1] = mp;
            if (canvas.wireTemp[0].x != mp.x || canvas.wireTemp[0].y != mp.y) {
                canvas.wireTempStatic = false;
            }
            refreshCanvasTemp();
        }
        canvas.mouse.x = mX;
        canvas.mouse.y = mY;
    }, true);
});

function normalizeWheelSpeed(event) {
    var normalized;
    if (event.wheelDelta) {
        normalized = (event.wheelDelta % 120 - 0) == -0 ? event.wheelDelta / 120 : event.wheelDelta / 12;
    } else {
        var rawAmmount = event.deltaY ? event.deltaY : event.detail;
        normalized = -(rawAmmount % 3 ? rawAmmount * 10 : rawAmmount / 3);
    }
    return normalized * 100;
}
addEventListener("init-env", () => {
    canvas.c.addEventListener('wheel', function (e) { // zoom

        let pastScale = board.scale;
        board.scale += board.scale * (20 / normalizeWheelSpeed(e));
        board.scale = parseInt(Math.min(501, Math.max(6, board.scale)));
        let badNumbers = [66, 41, 35, 32, 31, 26, 21, 20, 18, 16, 15, 13, 12];
        while (badNumbers.filter(n => n == board.scale).length != 0) board.scale += 1;
        let scaleRatio = board.scale / pastScale;
        board.x = parseInt(board.x + scaleRatio * (board.x + canvas.mouse.x) - (board.x + canvas.mouse.x));
        board.y = parseInt(board.y + scaleRatio * (board.y + canvas.mouse.y) - (board.y + canvas.mouse.y));
        refreshAllCanvas();
        e.preventDefault();
    });
});

document.body.ondragover = function (e) {
    e.preventDefault();
}

let fileType;

addEventListener("init-env", () => {
    document.body.addEventListener('drop', function (e) {
        e.preventDefault();

        if (e.dataTransfer.items) {
            for (var i = 0; i < e.dataTransfer.items.length; i++) {
                if (e.dataTransfer.items[i].kind === 'file') {
                    let file = e.dataTransfer.items[i].getAsFile();
                    fileType = file.type;
                    reader.readAsArrayBuffer(file);
                }
            }
        } else {
            for (var i = 0; i < e.dataTransfer.files.length; i++) {
                //let file = e.dataTransfer.files[i].getAsFile();
                //reader.readAsArrayBuffer(file);
            }
        }
    });
});

const reader = new FileReader();
addEventListener("init-env", () => {
    reader.addEventListener('loadend', () => {
        if (fileType == "image/png") {
            let pixels = new PNG(new Uint8Array(reader.result)).decode();
            let memorySize = 0
            let pixelIndex = 0;
            for (let i = 0; i < 32; i += 2) {
                memorySize += get_bit(pixels[pixelIndex + 0], 0) << i;
                memorySize += get_bit(pixels[pixelIndex + 1], 0) << i + 1;
                pixelIndex += 4;
            }

            if (memorySize >= 14) {
                let arr = createSharedArray(memorySize, "u8");

                pixelIndex = 0;
                for (let b = 0; b < memorySize; b++) {
                    let n = 0;
                    for (let i = 0; i < 8; i += 2) {
                        n += get_bit(pixels[pixelIndex], 0) << i;
                        n += get_bit(pixels[pixelIndex + 1], 0) << i + 1;
                        pixelIndex += 4;
                    }
                    arr[b] = n;
                }
                board.stopRun();
                Module.__loadWires(arr.byteOffset);
                console.log("Loaded: ", (memorySize / 1024).toPrecision(2) + "KB");
                deleteSharedArray(arr);
            }
        } else {
            let arr = createSharedArray(reader.result.byteLength, "u8");
            const view = new Uint8Array(reader.result);
            for (let i = 0; i < arr.byteLength; i++) {
                arr[i] = view[i];
            }
            board.stopRun();
            Module.__loadWires(arr.byteOffset);
            console.log("Loaded: ", (reader.result.byteLength / 1024).toPrecision(2) + "KB");
            deleteSharedArray(arr);
        }

        refreshAllCanvas();
    });
});

document.oncontextmenu = function () {
    return false;
}

let lastRunInstance = 0;

let notDrawCount = 0;
let drawCount = 0;
let runSpeed = 2;
let sliderRunSpeed = document.getElementById("runSpeed");
var sliderRunSpeedText = document.getElementById("runSpeedText");
sliderRunSpeed.oninput = function () {
    runSpeed = Math.round(this.value * this.value * this.value);
    if (runSpeed === 0) sliderRunSpeedText.innerHTML = "speed snail";
    else if (runSpeed === 1) sliderRunSpeedText.innerHTML = "speed turtle";
    else if (runSpeed < 60) sliderRunSpeedText.innerHTML = "speed slow";
    else if (runSpeed < 999) sliderRunSpeedText.innerHTML = "speed fast";
    else sliderRunSpeedText.innerHTML = "speed max";
}

Module.onRuntimeInitialized = function () {

    new Board("default");
    dispatchEvent(new Event("init-env"));

    resizeCanvas();
    updateButtonsPlayStop();

    setInterval(function () {
        if (board.state == "play") {
            let now = performance.now();
            if (now - lastRunInstance < 50) {
                for (let i = 0; i < runSpeed; i++) {
                    board.runStep(false);
                    notDrawCount++;
                }
            } else {
                if (++drawCount % 100 == 0) console.log("run ticks: ", notDrawCount + drawCount);
                board.runStep(true);
                lastRunInstance = now;
            }
        }
    }, 0);

};