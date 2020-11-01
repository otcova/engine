let boards = [];
let boardIndex = -1;
let board = undefined;

class Board {
    constructor(boardName, boardMemory) {

        this.x = 0;
        this.y = 0;
        this.scale = 33;
        this.state = "edit";

        if (boardMemory == undefined) {
            this.id = Module.__createBoard();
        } else {

        }

        this.name = boardName;
        boards.push(this);
        board = this;
        boardIndex = this.id;
    }

    initRun() {
        Module.__initRun();
    }

    stopRun() {
        if (this.state != "edit") {
            Module.__stopRun();
            this.state = "edit";
            requestDrawAll = true;
            updateButtonsPlayStop();
        }
    }
    
    set_async_run(async_is_on) {
        Module.__set_async(async_is_on);
    }

    runStep() {
        Module.__runStep();
    }

    addWire(pa, pb, pc) {
        if (this.state == "edit")
            Module.__setWire(true, pa.x, pa.y, pb.x, pb.y, pc.x, pc.y);
    }

    canAddWire(pa, pb, pc) {
        return Module.__canAddWire(pa.x, pa.y, pb.x, pb.y, pc.x, pc.y);
    }

    swapNode(p) {
        this.stopRun();
        Module.__swapNode(p.x, p.y);
    }

    swapObjNot(p, d) {
        return Module.__swapObjNot(p.x, p.y, d);
    }

    clickEvent(p) {
        return Module.__clickEvent(p.x, p.y);
    }

    removeWire(pa, pb, pc) {
        this.stopRun();
        Module.__setWire(false, pa.x, pa.y, pb.x, pb.y, pc.x, pc.y);
    }

    addObj(p, objType) {
        this.stopRun();
        Module.__addObj(p.x, p.y, objType, getRotateOfObj(objTypes[objType].name));
    }

    canAddObj(objType, p) {
        return Module.__canAddObj(p.x, p.y, objType, getRotateOfObj(objTypes[objType].name));
    }

    removeObj(p) {
        this.stopRun();
        Module.__removeObj(p.x, p.y);
    }

    drawWires() {
        return Module.__drawWires(this.scale, this.x / this.scale, this.y / this.scale,
            canvas.obj.width / this.scale, canvas.obj.height / this.scale);
    }

    drawObjs() {
        return Module.__drawObjs(this.scale, this.x / this.scale, this.y / this.scale,
            canvas.obj.width / this.scale, canvas.obj.height / this.scale);
    }

    // Ctrl + z
    undoAction() {
        select = false;
        this.stopRun();
        Module.__undoAction();
        requestDrawAll = true;
    }

    reverseUndo() {
        select = false;
        this.stopRun();
        Module.__reverseUndo();
        requestDrawAll = true;
    }

    //Select
    selectRect(rectX, rectY, rectW, rectH) {
        if (Module.__selectRect(rectX, rectY, rectW, rectH)) {
            this.stopRun();
            return true;
        }
        return false;
    }

    moveSelected(x, y) {
        Module.__moveSelected(x, y, 0, 0);
    }

    rotateSelection(r) {
        Module.__rotateSelected(r);
    }

    deleteSelected() {
        Module.__deleteSelected();
        select = false;
    }

    unSelect() {
        Module.__unSelect();
        select = false;
        requestDrawAll = true;
    }

    drawSelect() {
        return Module.__drawSelect(this.scale, this.x, this.y);
    }

    duplicateSelected() {
        if (select) {
            Module.__duplicateSelected();
            requestDrawAll = true;
        }
    }

    copySelected() {
        if (select) {
            Module.__copySelected();
        }
    }

    pasteSelected() {
        this.stopRun();
        if (Module.__pasteSelected()) {
            selectPalet(0, false);
            select = true;
            requestDrawAll = true;
        }
    }
}

function drawRect(ctx, x, y, w, h) {
    ctx.fillRect((parseInt(x * board.scale - board.x) + 0.5), (parseInt(y * board.scale - board.y) + 0.5), parseInt(w * board.scale), parseInt(h * board.scale));
    ctx.strokeRect((parseInt(x * board.scale - board.x) + 0.5), (parseInt(y * board.scale - board.y) + 0.5), parseInt(w * board.scale), parseInt(h * board.scale));
}

function drawText(ctx, txt, x, y) {
    ctx.fillText(txt, x * board.scale - board.x, y * board.scale - board.y);
}

function rotateCon(w, width, height, rotate) {
    let len = height;
    if (w.dir == "down" || w.dir == "up") len = width;
    for (let i = 0; i < rotate; i++) {
        if (w.dir == "right") w.dir = "up";
        else if (w.dir == "down") { w.dir = "right"; w.pos = len - w.pos; }
        else if (w.dir == "left") w.dir = "down";
        else if (w.dir == "up") { w.dir = "left"; w.pos = len - w.pos; }
    }
}

function drawObj(x, y, rotate, objID, ctx, red, ledPowerPtr, cNotsPtr, selected) {
    let obj = objTypes[objID];
    if (obj == undefined) return;

    if (rotate == undefined) {
        rotate = getRotateOfObj(obj.name);
        if (rotate == undefined)
            rotate = 0;
    }
    let w = rotate % 2 == 0 ? obj.w : obj.h;
    let h = rotate % 2 == 0 ? obj.h : obj.w;

    if (red) ctx.strokeStyle = "#E55";
    else if (selected) ctx.strokeStyle = "#6AE";
    else ctx.strokeStyle = "#3E3";
    ctx.lineWidth = 1;
    ctx.beginPath();

    for (const wireC of obj.wires) {
        if (wireC.hide == true) continue;
        let wire = {};
        Object.assign(wire, wireC);
        rotateCon(wire, obj.w, obj.h, rotate);
        if (wire.dir == "left") {
            ctx.moveTo(x * board.scale - board.x, (y + wire.pos) * board.scale - board.y + 0.5);
            ctx.lineTo((x - 0.45) * board.scale - board.x, (y + wire.pos) * board.scale - board.y + 0.5);
        } else if (wire.dir == "right") {
            ctx.moveTo((x + w) * board.scale - board.x, (y + wire.pos) * board.scale - board.y + 0.5);
            ctx.lineTo((x + w + 0.45) * board.scale - board.x, (y + wire.pos) * board.scale - board.y + 0.5);
        } else if (wire.dir == "up") {
            ctx.moveTo((x + wire.pos) * board.scale - board.x + 0.5, y * board.scale - board.y);
            ctx.lineTo((x + wire.pos) * board.scale - board.x + 0.5, (y - 0.45) * board.scale - board.y);
        } else if (wire.dir == "down") {
            ctx.moveTo((x + wire.pos) * board.scale - board.x + 0.5, (y + h) * board.scale - board.y);
            ctx.lineTo((x + wire.pos) * board.scale - board.x + 0.5, (y + h + 0.45) * board.scale - board.y);
        }
    }
    ctx.stroke();

    ctx.strokeStyle = "#CCC";
    if (red) {
        ctx.fillStyle = "#B33";
    } else if (selected) {
        ctx.fillStyle = "#348";
    } else {
        ctx.fillStyle = "#333";
    }
    ctx.lineWidth = 1;
    let rectX = x - 0.35;
    let rectY = y - 0.35;
    let rectW = w + 0.7;
    let rectH = h + 0.7;
    drawRect(ctx, rectX, rectY, rectW, rectH);

    ctx.font = board.scale * 0.25 + "px Arial";
    ctx.textAlign = "center";
    ctx.textBaseline = "middle";

    let i = 0;
    for (const wireC of obj.wires) {
        let wire = {};
        Object.assign(wire, wireC);
        rotateCon(wire, obj.w, obj.h, rotate);


        let wireNot = wire.not;
        if (cNotsPtr != undefined) wireNot = Module.HEAP8[cNotsPtr + i];
        if (wireNot) {
            ctx.fillStyle = "#CCC";
            ctx.beginPath();
            if (wire.dir == "left") {
                ctx.arc(
                    (x - 0.4) * board.scale - board.x,
                    (y + wire.pos) * board.scale - board.y,
                    0.09 * board.scale, 0, 2 * Math.PI);
            } else if (wire.dir == "right") {
                ctx.arc(
                    (x + w + 0.4) * board.scale - board.x,
                    (y + wire.pos) * board.scale - board.y,
                    0.09 * board.scale, 0, 2 * Math.PI);
            } else if (wire.dir == "up") {
                ctx.arc(
                    (x + wire.pos) * board.scale - board.x,
                    (y - 0.4) * board.scale - board.y,
                    0.09 * board.scale, 0, 2 * Math.PI);
            } else if (wire.dir == "down") {
                ctx.arc(
                    (x + wire.pos) * board.scale - board.x,
                    (y + h + 0.4) * board.scale - board.y,
                    0.09 * board.scale, 0, 2 * Math.PI);
            }
            ctx.fill();
        } if (wire.txt != undefined) {
            ctx.fillStyle = "#FFF";
            ctx.beginPath();
            if (wire.dir == "left") {
                ctx.fillText(wire.txt,
                    (x - 0.17) * board.scale - board.x,
                    (y + wire.pos) * board.scale - board.y);
            } else if (wire.dir == "right") {
                ctx.fillText(wire.txt,
                    (x + w + 0.17) * board.scale - board.x,
                    (y + wire.pos) * board.scale - board.y);
            } else if (wire.dir == "up") {
                ctx.fillText(wire.txt,
                    (x + wire.pos) * board.scale - board.x,
                    (y - 0.17) * board.scale - board.y);
            } else if (wire.dir == "down") {
                ctx.fillText(wire.txt,
                    (x + wire.pos) * board.scale - board.x,
                    (y + h + 0.17) * board.scale - board.y);
            }
        } else if (wire.clk) {
            ctx.fillStyle = "#CCC";
            ctx.beginPath();
            if (wire.dir == "left") {
                ctx.moveTo((x - 0.35) * board.scale - board.x, (y + wire.pos - 0.12) * board.scale - board.y);
                ctx.lineTo((x - 0.1) * board.scale - board.x, (y + wire.pos) * board.scale - board.y);
                ctx.lineTo((x - 0.35) * board.scale - board.x, (y + wire.pos + 0.12) * board.scale - board.y);
            } else if (wire.dir == "right") {
                ctx.moveTo((x + w + 0.35) * board.scale - board.x, (y + wire.pos - 0.12) * board.scale - board.y);
                ctx.lineTo((x + w + 0.1) * board.scale - board.x, (y + wire.pos) * board.scale - board.y);
                ctx.lineTo((x + w + 0.35) * board.scale - board.x, (y + wire.pos + 0.12) * board.scale - board.y);
            } else if (wire.dir == "up") {
                ctx.moveTo((x + wire.pos - 0.12) * board.scale - board.x, (y - 0.35) * board.scale - board.y);
                ctx.lineTo((x + wire.pos) * board.scale - board.x, (y - 0.1) * board.scale - board.y);
                ctx.lineTo((x + wire.pos + 0.12) * board.scale - board.x, (y - 0.35) * board.scale - board.y);
            } else if (wire.dir == "down") {
                ctx.moveTo((x + wire.pos - 0.12) * board.scale - board.x, (y + h + 0.35) * board.scale - board.y);
                ctx.lineTo((x + wire.pos) * board.scale - board.x, (y + h + 0.1) * board.scale - board.y);
                ctx.lineTo((x + wire.pos + 0.12) * board.scale - board.x, (y + h + 0.35) * board.scale - board.y);
            }
            ctx.stroke();
        }

        ++i;
    }

    ctx.font = board.scale * 0.5 + "px Arial";
    if (obj.leds != undefined) {
        let i = 0;
        for (const led of obj.leds) {
            if (led.txt) {
                
                let tx = led.x;
                let ty = led.y;
                if (rotate == 1) {
                    tx = led.y;
                    ty = h - led.x;
                } else if (rotate == 2) {
                    tx = w - led.x;
                    ty = h - led.y;
                } else if (rotate == 3) {
                    tx = w - led.y;
                    ty = led.x;
                }
                ctx.fillStyle = "#FFF";

                let text = led.undef;
                if (ledPowerPtr != undefined) {
                    let ledColor = Module.HEAP8[ledPowerPtr + i];
                    if (ledColor == 0) text = led.off;
                    else text = led.on;
                }
                drawText(ctx, text, x + tx, y + ty + 0.05);
            } else {
                let ledColor = Module.HEAP8[ledPowerPtr + i];
                if (ledPowerPtr == undefined || ledColor == 0) ctx.fillStyle = "#444";
                else if (ledColor == 1) ctx.fillStyle = "#44F";
                else if (ledColor == 2) ctx.fillStyle = "#4F4";
                else if (ledColor == 3) ctx.fillStyle = "#4FF";
                else if (ledColor == 4) ctx.fillStyle = "#F44";
                else if (ledColor == 5) ctx.fillStyle = "#F4F";
                else if (ledColor == 6) ctx.fillStyle = "#FF4";
                else ctx.fillStyle = "#FFF";
                

                rectX = x + led.x - led.w/2;// - 0.5 + 0.22;
                rectY = y + led.y - led.h/2;// - 0.5 + 0.22;
                rectW = led.w;// - 0.44;
                rectH = led.h;// - 0.44;
                ctx.fillRect((rectX * board.scale - board.x), (rectY * board.scale - board.y),
                    (rectW * board.scale), (rectH * board.scale));

            }
            ++i;
        }
    }

    if (obj.text != undefined) {
        let tx, ty;
        ctx.fillStyle = "#FFF";
        for (const text of obj.text) {
            if (rotate == 0) {
                tx = text.x;
                ty = text.y;
            } else if (rotate == 1) {
                tx = text.y;
                ty = h - text.x;
            } else if (rotate == 2) {
                tx = w - text.x;
                ty = text.y;
            } else if (rotate == 3) {
                tx = text.y;
                ty = text.x;
            }
            drawText(ctx, text.txt, x + tx, y + ty + 0.05);
        }
    }
}