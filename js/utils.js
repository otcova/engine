function mod(x, m) {
    return (x % m + m) % m;
}

function getMousePos() {
    return {
        x: Math.round((canvas.mouse.x + board.x) / board.scale),
        y: Math.round((canvas.mouse.y + board.y) / board.scale)
    };
}

let blob = undefined;

function saveData(data) {

    blob = new Blob([data], { type: "octet/stream" });
    downloadBlob(blob, "board");
}


function readTextFile(file) {
    let txt = "null";
    var rawFile = new XMLHttpRequest();
    rawFile.open("GET", file, false);
    rawFile.onreadystatechange = function () {
        if (rawFile.readyState === 4) {
            if (rawFile.status === 200 || rawFile.status == 0) {
                txt = rawFile.responseText;
            }
        }
    }
    rawFile.send(null);
    return txt;
}

function downloadBlob(blob, filename) {
    if (window.navigator.msSaveOrOpenBlob) // IE10+
        window.navigator.msSaveOrOpenBlob(blob, filename);
    else { // Others
        var a = document.createElement("a"),
            url = URL.createObjectURL(blob);
        a.href = url;
        a.download = filename;
        document.body.appendChild(a);
        a.click();
        setTimeout(function () {
            document.body.removeChild(a);
            window.URL.revokeObjectURL(url);
        }, 0);
    }
}

/*
function getImg(url) {
    let img = new Image();
    img.src = url;
    let canvas = document.createElement('canvas');
    canvas.style["float"] = "left";

    console.log(img.width);
    console.log(img.height);
    let context = canvas.getContext('2d');
    canvas.width = img.width;
    canvas.height = img.height;
    context.fillStyle = "#0F0";
    context.fillRect(0, 0, 1000, 1000);
    context.drawImage(img, 0, 0, img.width, img.height);

    for (let x = 0; x < 2; x++) {
        for (let y = 0; y < img.height; y++) {
            console.log(context.getImageData(x, y, 1, 1).data)
        }
    }
    document.body.appendChild(canvas);
    //return context.getImageData(x, y, 1, 1).data;
}
*/
function get_bit(v, index) {
    return (v & (1 << index))
}
/*

function loadPngBoard(url) {
    PNG.load(url, function (pngImg) {
        console.log(pngImg.decode);
        let pixels = pngImg.decode();
        let memorySize = 0
        let pixelIndex = 0;
        for (let i = 0; i < 32; i += 2) {
            memorySize += get_bit(pixels[pixelIndex + 0], 0) << i;
            memorySize += get_bit(pixels[pixelIndex + 1], 0) << i + 1;
            pixelIndex += 4;
        }

        let imgArr = createSharedArray(memorySize, "u8");

        pixelIndex = 0;
        for (let b = 0; b < memorySize; b++) {
            let n = 0;
            for (let i = 0; i < 8; i += 2) {
                n += get_bit(pixels[pixelIndex], 0) << i;
                n += get_bit(pixels[pixelIndex + 1], 0) << i + 1;
                pixelIndex += 4;
            }
            imgArr[b] = n;
        }
        board.stopRun();
        Module.__loadWires(imgArr.byteOffset);
        refreshAllCanvas();
    });
}*/