
let select = false;
let paletItems = ["Select", "Eraser", "Wire", "Led", "Switch", "And", "Not", "XOr", "Or"];
let paletIndex = 0;

let itemButtonSelected = undefined;

function RefreshGUI() {
    leftMenu = document.getElementById("leftBar");

    for (const folderName of leftMenuData) {
        let folder = document.createElement("button");
        folder.className = "folder";
        folder.onclick = folderOnClick;
        folder.id = folderName[0];
        let title = document.createElement("div");
        title.className = "folderText unselectText";
        title.innerHTML = folderName[0];
        folder.appendChild(title);

        let file = document.createElement("div");
        file.className = "file";
        for (const itemName of folderName[1]) {
            let item = document.createElement("button");
            item.className = "item itemColor unselectText";
            item.id = "__button - " + itemName;
            item.onclick = function () {
                if (paletIndex < 3) {
                    selectPalet(3);
                }
                paletItems[paletIndex] = itemName;
                selectItemButton(this);
            };
            item.innerHTML = itemName;
            item.style.fontSize = "12px";
            file.appendChild(item);
        }
        leftMenu.appendChild(folder);
        leftMenu.appendChild(file);
        if (file.offsetHeight > 250) {
            file.style.overflowY = "auto";
            file.theRealH = "130px";
            file.style.maxHeight = file.theRealH;
        } else {
            file.theRealH = file.offsetHeight + "px";
            file.style.maxHeight = file.theRealH;
        }
    }
}

function folderOnClick() {
    if (this.nextSibling.style.maxHeight == "0px") {
        this.nextSibling.style.maxHeight = this.nextSibling.theRealH;
        this.nextSibling.style.borderBottom = "1px solid #333";
    } else {
        this.nextSibling.style.maxHeight = "0px";
        this.nextSibling.style.borderBottom = "0px solid #333";
    }
}

function unselectItemButton() {
    if (itemButtonSelected != undefined) {
        itemButtonSelected.style.outlineWidth = "0px";
        itemButtonSelected = undefined;
    }
}

function selectItemButton(button) {
    if (paletIndex == 0 || paletIndex == 1 || paletIndex == 2) {
        document.getElementById("pA" + paletIndex).style.outlineWidth = "0px";
    }
    unselectItemButton();
    itemButtonSelected = button;
    itemButtonSelected.style.outlineWidth = "1px";
}

function selectPalet(n, autoUnSelect) {

    if (paletIndex == 0) {
        if (autoUnSelect != false) board.unSelect();
    }

    if (paletIndex == 0 || paletIndex == 1 || paletIndex == 2) {
        document.getElementById("pA" + paletIndex).style.outlineWidth = "0px";
    }

    paletIndex = n;

    if (n == 0 || n == 1 || n == 2) {
        document.getElementById("pA" + n).style.outlineWidth = "1px";
        unselectItemButton();
    } else {
        let e = document.getElementById("__button - " + paletItems[paletIndex]);
        if (e == undefined) console.error("ERROR");
        else selectItemButton(e);
    }

    requestDrawAll = true;
}

//let wiresMapMemoryPtr = undefined;
//let meoryArray = undefined;

let keyRepeat = false;
let keyRepeatCount = 0;

addEventListener("init-env", () => {
    document.addEventListener('keydown', keyPressEv);
    document.addEventListener('keyup', () => { keyRepeat = false; keyRepeatCount = 0; });
    //document.addEventListener('keypress', (e) => { console.log(e.repeat); });
});

function keyPressEv(e) {

    if (e.ctrlKey) e.preventDefault();
    if (board == undefined) return;

    if (keyRepeatCount % 8 == 0 || !keyRepeat) {
        if (e.key == "z" && e.ctrlKey) board.undoAction();
        else if (e.key == "y" && e.ctrlKey) board.reverseUndo();
        else if (e.key == "e" || e.key == "E") {
            if (board.state == "play") pauseButtonOnClick();
            else stepButtonOnClick();
        }
    }
    keyRepeatCount++;

    if (e.key == "r" || e.key == "R") increesObjModelIndex(); 
    else if (e.key == "f" || e.key == "F") decreesObjModelIndex();

    if (!keyRepeat) {
        if (e.key == "1") selectPalet(0);
        else if (e.key == "2") selectPalet(1);
        else if (e.key == "3") selectPalet(2);
        else if (e.key == "4") selectPalet(3);
        else if (e.key == "5") selectPalet(4);
        else if (e.key == "6") selectPalet(5);
        else if (e.key == "7") selectPalet(6);
        else if (e.key == "8") selectPalet(7);
        else if (e.key == "9") selectPalet(8);

        else if (e.key == "s" && e.ctrlKey) {
            let wiresMapMemoryPtr = Module.__saveBoard();
            let memory;
            if (Module.HEAP32.buffer.constructor.name == "SharedArrayBuffer") memory = sliceToNonShared(Module.HEAP8, wiresMapMemoryPtr, wiresMapMemoryPtr+Module.HEAP32[wiresMapMemoryPtr / (32 / 8)]);
            else memory = new Uint8Array(Module.HEAP8.buffer, wiresMapMemoryPtr, Module.HEAP32[wiresMapMemoryPtr / (32 / 8)])
            saveDataToFile("board.cir", memory);
            console.log("Saved: ", (Module.HEAP32[wiresMapMemoryPtr / (32 / 8)] / 1024).toPrecision(2) + "KB");
            requestDrawAll = true;
        }

        else if (e.key == "Delete" || e.key == "Backspace") board.deleteSelected();

        else if (e.key == "q" || e.key == "Q") {
            if (board.state == "play") stopButtonOnClick();
            else playButtonOnClick();
        }

        else if (paletIndex < 3) {
            if (e.key == "v" || e.key == "V") board.pasteSelected();
            else if (paletItems[paletIndex] == "Select") {
                if (e.ctrlKey) {
                    if (e.key == "d" || e.key == "D") board.duplicateSelected();
                    else if (e.key == "c" || e.key == "C") board.copySelected();
                } else {
                    if (e.key == "a" || e.key == "A") board.rotateSelection(1);
                    else if (e.key == "d" || e.key == "D") board.rotateSelection(3);
                }
            }
        }
        else if (e.key == "w" || e.key == "W") rotateObjType(1);
        else if (e.key == "a" || e.key == "A") rotateObjType(2);
        else if (e.key == "s" || e.key == "S") rotateObjType(3);
        else if (e.key == "d" || e.key == "D") rotateObjType(0);
    }
    requestDrawAll = true;
    if (e.key != "Control") keyRepeat = true;
}

function sliceToNonShared(bufferArray, start, end) {
    if (end == undefined) end = bufferArray.length;
    var result = new ArrayBuffer(end - start);
    var resultArray = new Uint8Array(result);
    for (var i = 0; i < resultArray.length; i++)
       resultArray[i] = bufferArray[i + start];
    return resultArray;
}

function rotateObjType(r) {
    let obj = objTypes[getObjByName(paletItems[paletIndex])];
    if (obj != undefined)
        setRotateOfObj(obj.name, r);
}

paletIndex = 2;
document.getElementById("pA2").style.outlineWidth = "1px";
RefreshGUI();





let w = 10;
let h = 30;

function stepButtonOnClick() {
    if (board.state == "edit") {
        board.initRun();
        board.state = "pause";
        updateButtonsPlayStop();
        selectPalet(2);
    } else {
        board.runStep();
        requestDrawObjWire = true;
    }
}

function stopButtonOnClick() {
    board.stopRun();
}

function pauseButtonOnClick() {
    board.set_async_run(false);
    board.state = "pause";
    updateButtonsPlayStop();
    board.runStep();
    requestDrawAll = true;
}

function updateButtonsPlayStop() {
    document.getElementById("stop").hidden = !(board.state == "play" || board.state == "pause");
    document.getElementById("pause").hidden = !(board.state == "play");
    document.getElementById("step").hidden = !(board.state == "edit" || board.state == "pause");
    document.getElementById("play").hidden = !(board.state == "edit" || board.state == "pause");
}

function playButtonOnClick() {
    if (runDelay == -1) board.set_async_run(true);
    if (board.state == "edit") board.initRun();
    board.state = "play";
    updateButtonsPlayStop();
    selectPalet(2);
}
let ctx = document.getElementById("pause").getContext("2d");
addEventListener("init-env", () => {
    //Pause

    ctx.canvas.hidden = true;
    ctx.canvas.addEventListener("mouseup", pauseButtonOnClick);



    ctx.strokeStyle = "#333";
    ctx.fillStyle = "#3E3";

    ctx.fillRect(parseInt((ctx.canvas.width - w * 2.5) / 2), (ctx.canvas.height - h) / 2, w, h);
    ctx.strokeRect(parseInt((ctx.canvas.width - w * 2.5) / 2) + 0.5, (ctx.canvas.height - h) / 2 + 0.5, w, h);
    ctx.fillRect(parseInt((ctx.canvas.width - w * 2.5) / 2) + parseInt(w * 1.5) + 0.5, (ctx.canvas.height - h) / 2, w, h);
    ctx.strokeRect(parseInt((ctx.canvas.width - w * 2.5) / 2) + parseInt(w * 1.5) + 0.5, (ctx.canvas.height - h) / 2 + 0.5, w, h);

    //Stop
    ctx = document.getElementById("stop").getContext("2d");
    ctx.canvas.hidden = true;
    ctx.canvas.addEventListener("mouseup", stopButtonOnClick);



    ctx.strokeStyle = "#222";
    ctx.fillStyle = "#E44";

    ctx.fillRect(ctx.canvas.width / 2 - h / 2 + 0.5, ctx.canvas.height / 2 - h / 2 + 0.5, h, h);
    ctx.strokeRect(ctx.canvas.width / 2 - h / 2 + 0.5, ctx.canvas.height / 2 - h / 2 + 0.5, h, h);
    ctx.fillStyle = "#444";
    ctx.strokeStyle = "#333";

    //Step
    ctx = document.getElementById("step").getContext("2d");
    ctx.canvas.hidden = true;
    ctx.canvas.addEventListener("mouseup", stepButtonOnClick);


    ctx.strokeStyle = "#333";
    ctx.fillStyle = "#3E3";

    ctx.fillRect(parseInt((ctx.canvas.width - w * 3.5) / 2), (ctx.canvas.height - h) / 2, w, h);
    ctx.strokeRect(parseInt((ctx.canvas.width - w * 3.5) / 2) + 0.5, (ctx.canvas.height - h) / 2 + 0.5, w, h);

    ctx.strokeStyle = "#000";
    ctx.lineWidth = 0.6;
    w = Math.sqrt(h * h - (h / 2) * (h / 2));
    let dw = (ctx.canvas.width - w) / 2;
    let dh = (ctx.canvas.height - h) / 2;

    ctx.beginPath();
    ctx.moveTo(parseInt(dw + w * 0.4) + 0.5, dh + 0.5);
    ctx.lineTo(parseInt(dw + w * 0.4) + 0.5, dh + h + 0.5);
    ctx.lineTo(parseInt(dw + w * 0.4) + 0.5 + w, ctx.canvas.height / 2 + 0.5);
    ctx.lineTo(parseInt(dw + w * 0.4), dh);
    ctx.fill();
    ctx.stroke();

    //Play
    ctx = document.getElementById("play").getContext("2d");
    ctx.canvas.hidden = true;
    ctx.canvas.addEventListener("mouseup", playButtonOnClick);


    ctx.strokeStyle = "#000";
    ctx.lineWidth = 0.6;
    ctx.fillStyle = "#3E3";

    w = Math.sqrt(h * h - (h / 2) * (h / 2));
    dw = (ctx.canvas.width - w) / 2;
    dh = (ctx.canvas.height - h) / 2;

    ctx.beginPath();
    ctx.moveTo(dw + 0.5, dh + 0.5);
    ctx.lineTo(dw + 0.5, dh + h + 0.5);
    ctx.lineTo(dw + 0.5 + w, ctx.canvas.height / 2 + 0.5);
    ctx.lineTo(dw, dh);
    ctx.fill();
    ctx.stroke();
});

