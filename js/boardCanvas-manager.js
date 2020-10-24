let canvas = {
    mouse: {
        x: 0,
        y: 0
    }
};

canvas.background = document.getElementById('boardCanvasBackground');
canvas.wire = document.getElementById('boardCanvasWires');
canvas.obj = document.getElementById('boardCanvasObj');
canvas.temp = document.getElementById('boardCanvasTemp');
canvas.ctxw = canvas.wire.getContext('2d');
canvas.ctxo = canvas.obj.getContext('2d');
canvas.ctxt = canvas.temp.getContext('2d');
canvas.ctxb = canvas.background.getContext('2d', {
    alpha: true
});

function resizeCanvas() {
    canvas.backgroundScale = -1;
    canvas.background.width = canvas.c.offsetWidth;
    canvas.background.height = canvas.c.offsetHeight;
    canvas.backgroundImageData = canvas.ctxb.getImageData(0, 0, canvas.background.width, canvas.background.height);

    canvas.wire.width = canvas.background.width;
    canvas.wire.height = canvas.background.height;
    canvas.obj.width = canvas.background.width;
    canvas.obj.height = canvas.background.height;
    canvas.temp.width = canvas.background.width;
    canvas.temp.height = canvas.background.height;

    requestDrawAll = true;
}

function refreshCanvasBackground() {

    if (canvas.backgroundScale != board.scale) {

        let buf = new ArrayBuffer(canvas.backgroundImageData.data.length);
        let buf8 = new Uint8ClampedArray(buf);
        let data = new Uint32Array(buf);

        let scale = parseInt(board.scale);
        canvas.backgroundScale = board.scale;

        for (var y = 0; y < canvas.background.height; y += scale) {
            for (var x = 0; x < canvas.background.width; x += scale) {
                data[y * canvas.background.width + x] = 4281593395;
            }
        }

        canvas.backgroundImageData.data.set(buf8);
    }

    canvas.ctxb.fillStyle = "#646464";
    canvas.ctxb.fillRect(0, 0, canvas.background.width, canvas.background.height);
    canvas.ctxb.putImageData(canvas.backgroundImageData, mod(-board.x, parseInt(board.scale)), mod(-board.y, parseInt(board.scale)));
}

function refreshCanvasWires() {
    canvas.ctxw.clearRect(0, 0, canvas.wire.width, canvas.wire.height);
    canvas.ctxw.strokeStyle = '#3E3';
    board.drawWires();
}

function refreshCanvasObj() {
    canvas.ctxo.clearRect(0, 0, canvas.obj.width, canvas.obj.height);
    board.drawObjs();
}

function refreshCanvasTemp() {
    canvas.ctxt.clearRect(0, 0, canvas.temp.width, canvas.temp.height);
    if (select) {
        if (canvas.wireTemp != undefined) {
            if (canvas.wireTemp.length > 2) {
                let dx = canvas.wireTemp[1].x - canvas.movSel.x;
                let dy = canvas.wireTemp[1].y - canvas.movSel.y;
                board.moveSelected(dx, dy);
                canvas.movSel.x += dx;
                canvas.movSel.y += dy;
            }
        }
        board.drawSelect();
    }
    else if (canvas.wireTemp != undefined) {
        if (paletItems[paletIndex] == "Wire" || paletItems[paletIndex] == "Eraser") {
            let ctx;
            if (paletItems[paletIndex] == "Wire") {
                if (board.state == "play" || board.state == "pause") return;
                ctx = canvas.ctxb;
                ctx.strokeStyle = '#3E3';
                if (canvas.wireDir == "h") {
                    if (!board.canAddWire(canvas.wireTemp[0], { x: canvas.wireTemp[1].x, y: canvas.wireTemp[0].y }, canvas.wireTemp[1])) {
                        ctx = canvas.ctxt;
                        ctx.strokeStyle = '#E55';
                    }
                } else {
                    if (!board.canAddWire(canvas.wireTemp[0], { x: canvas.wireTemp[0].x, y: canvas.wireTemp[1].y }, canvas.wireTemp[1])) {
                        ctx = canvas.ctxt;
                        ctx.strokeStyle = '#E55';
                    }
                }
                ctx.lineWidth = 1;
            } else {
                ctx = canvas.ctxt;
                ctx.strokeStyle = '#E55';
                ctx.lineWidth = 3;
            }

            ctx.beginPath();
            if (canvas.wireDir == "h") {
                ctx.moveTo(canvas.wireTemp[0].x * board.scale - board.x - (canvas.wireTemp[0].x < canvas.wireTemp[1].x ? 1 : -1), canvas.wireTemp[0].y * board.scale - board.y + 0.5);
                ctx.lineTo(canvas.wireTemp[1].x * board.scale - board.x + 0.5, canvas.wireTemp[0].y * board.scale - board.y + 0.5);
                ctx.lineTo(canvas.wireTemp[1].x * board.scale - board.x + 0.5, canvas.wireTemp[1].y * board.scale - board.y);
            } else {
                ctx.moveTo(canvas.wireTemp[0].x * board.scale - board.x + 0.5, canvas.wireTemp[0].y * board.scale - board.y - (canvas.wireTemp[0].y < canvas.wireTemp[1].y ? 1 : -1));
                ctx.lineTo(canvas.wireTemp[0].x * board.scale - board.x + 0.5, canvas.wireTemp[1].y * board.scale - board.y + 0.5);
                ctx.lineTo(canvas.wireTemp[1].x * board.scale - board.x, canvas.wireTemp[1].y * board.scale - board.y + 0.5);
            }
            ctx.stroke();
        } else if (paletItems[paletIndex] == "Select") {
            let x0 = canvas.wireTemp[0].x;
            let y0 = canvas.wireTemp[0].y;
            let x1 = canvas.wireTemp[1].x;
            let y1 = canvas.wireTemp[1].y;

            if (x0 > x1) {
                let t = x0;
                x0 = x1;
                x1 = t;
            } if (y0 > y1) {
                let t = y0;
                y0 = y1;
                y1 = t;
            }

            x0 = parseInt((x0 - 0.5) * board.scale - board.x);
            y0 = parseInt((y0 - 0.5) * board.scale - board.y);
            x1 = parseInt((x1 + 0.5) * board.scale - board.x);
            y1 = parseInt((y1 + 0.5) * board.scale - board.y);

            canvas.ctxt.lineWidth = 2;
            canvas.ctxt.strokeStyle = "#CCC";
            canvas.ctxt.fillStyle = "#FFF4";
            canvas.ctxt.fillRect(x0, y0, x1 - x0 + 1, y1 - y0 + 1);
            canvas.ctxt.strokeRect(x0, y0, x1 - x0 + 1, y1 - y0 + 1);
        } else {
            let objID = getObjByName(paletItems[paletIndex]);
            if (objID != undefined)
                drawObj(canvas.wireTemp[1].x, canvas.wireTemp[1].y, undefined, objID, canvas.ctxt, !board.canAddObj(objID, canvas.wireTemp[1]));
        }
    }
}

function refreshAllCanvas() {
    refreshCanvasBackground();
    refreshCanvasWires();
    refreshCanvasObj();
    refreshCanvasTemp();
}

let requestDrawAll = false;
let requestDrawObjWire = false;


function animationFrame() {
    requestAnimationFrame(animationFrame);
    
    if (requestDrawAll || board.state == "play") {
        refreshAllCanvas();
    } else if (requestDrawObjWire) {
        refreshCanvasObj();
        refreshCanvasWires();
    }
    
    requestDrawAll = false;
    requestDrawObjWire = false;
}

addEventListener("init-env", () => {
    canvas.c = document.getElementById('content');
    canvas.leftBar = document.getElementById('leftBar');
    window.addEventListener('resize', resizeCanvas);
    requestAnimationFrame(animationFrame);
});