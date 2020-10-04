let objModelIndex = 2;

let leftMenuData = [
    ["Input", ["Switch", "Button", "Number", "Clock"]],
    ["Output", ["Led", "Display", "Color Led", "Point Display"]],
    ["Logic", ["And", "Or", "XOr", "Not", "Decoder", "Door"]],
    ["Maths", ["Half adder", "Full adder"]],
    ["Memory", ["1 Bit", "D flip-flop", "Gated RS latch", "RS latch"]]
];

let objNames = new Map();
let objTypes = [];

let complexComps = new Map();
complexComps.set("And", { rotate: 0, insLen: 2, insLenMin: 2, insLenMax: 16 });
complexComps.set("Or", { rotate: 0, insLen: 2, insLenMin: 2, insLenMax: 16 });
complexComps.set("XOr", { rotate: 0, insLen: 2, insLenMin: 2, insLenMax: 16 });
complexComps.set("Number", { rotate: 3, insLen: 2, insLenMin: 2, insLenMax: 16 });
complexComps.set("Door", { rotate: 0, insLen: 2, insLenMin: 2, insLenMax: 64 });
complexComps.set("Decoder", { rotate: 0, insLen: 2, insLenMin: 2, insLenMax: 6 });

function getObjByName(name) {
    let obj = objNames.get(name);
    if (obj != undefined) return obj;
    let xcomp = complexComps.get(name);
    if (xcomp != undefined) {
        return objNames.get(name + xcomp.insLen);
    }
}

function increesObjModelIndex() {
    let xcomp = complexComps.get(paletItems[paletIndex]);
    if (xcomp != undefined)
    xcomp.insLen = Math.min(xcomp.insLen+1, xcomp.insLenMax);
}

function decreesObjModelIndex() {
    let xcomp = complexComps.get(paletItems[paletIndex]);
    if (xcomp != undefined)
        xcomp.insLen = Math.max(xcomp.insLen-1, xcomp.insLenMin);
}

function getRotateOfObj(name) {
    let obj = objNames.get(name);
    if (obj == undefined) {
        let xcomp = complexComps.get(name);
        return xcomp.rotate;
    }
    return objTypes[obj].rotate;
}

function setRotateOfObj(name, r) {
    let xcomp = complexComps.get(name);
    if (xcomp == undefined) {
        let obj = objNames.get(name);
        objTypes[obj].rotate = r;
    } else {
        xcomp.rotate = r;
    }
}


objNames.set("Button", objTypes.length); // 0
objTypes.push({
    name: "Button", w: 0, h: 0,
    wires: [{ dir: "right", pos: 0, hide: true }, { dir: "left", pos: 0, hide: true }, { dir: "down", pos: 0, hide: true }, { dir: "up", pos: 0, hide: true }],
    leds: [{ txt: true, on: "─", off: "O", undef: "O", x: 0, y: 0 }]
});

objNames.set("1 Bit", objTypes.length);// 1
objTypes.push({
    name: "1 Bit", w: 1, h: 3,
    wires: [{ dir: "right", pos: 0, txt: "Q" }, { dir: "left", pos: 0, txt: "A" }, { dir: "left", pos: 1, txt: "D" }, { dir: "left", pos: 2, txt: "S" }, { dir: "left", pos: 3, txt: "R" }],
    text: [{ txt: "1b", x: 0.5, y: 1.5 }]
});

objNames.set("XOrDeprecated", objTypes.length); // 2
objTypes.push({
    name: "XOrDeprecated", w: 1, h: 1,
    wires: [{ dir: "left", pos: 0 }, { dir: "left", pos: 1 }, { dir: "right", pos: 0 }],
    text: [{ txt: "=1", x: 0.5, y: 0.5 }]
});

objNames.set("Not", objTypes.length); // 3
objTypes.push({
    name: "Not", w: 1, h: 0,
    wires: [{ dir: "left", pos: 0 }, { dir: "right", pos: 0, not: true }],
    text: [{ txt: "1", x: 1, y: 0 }]
});

objNames.set("Switch", objTypes.length); // 4
objTypes.push({
    name: "Switch", w: 0, h: 0,
    wires: [{ dir: "right", pos: 0, hide: true }, { dir: "left", pos: 0, hide: true }, { dir: "down", pos: 0, hide: true }, { dir: "up", pos: 0, hide: true }],
    leds: [{ txt: true, on: "1", off: "0", undef: "0", x: 0, y: 0 }]
});

objNames.set("Led", objTypes.length);
objTypes.push({
    name: "Led", w: 0, h: 0,
    wires: [{ dir: "right", pos: 0, hide: true }, { dir: "left", pos: 0, hide: true }, { dir: "down", pos: 0, hide: true }, { dir: "up", pos: 0, hide: true }],
    leds: [{ x: 0, y: 0, w: 1, h: 1 }]
});


objNames.set("Display", objTypes.length);
objTypes.push({
    name: "Display", w: 5, h: 5, rotate: 3,
    wires: [{ dir: "right", pos: 0 }, { dir: "right", pos: 1 }, { dir: "right", pos: 2 }, { dir: "right", pos: 3 }, { dir: "right", pos: 4 }, { dir: "right", pos: 5 }],
    leds: [
        { x: 3.7, y: 0, w: 1.6, h: 1 }, { x: 3.7, y: 2.5, w: 1.6, h: 1 }, { x: 3.7, y: 5, w: 1.6, h: 1 }, { x: 3, y: 0.5, w: 1, h: 2.65 }, { x: 3, y: 2.85, w: 1, h: 2.65 }, { x: 5, y: 0.5, w: 1, h: 2.65 }, { x: 5, y: 2.85, w: 1, h: 2.65 },
        { x: 0.7, y: 0, w: 1.6, h: 1 }, { x: 0.7, y: 2.5, w: 1.6, h: 1 }, { x: 0.7, y: 5, w: 1.6, h: 1 }, { x: 0, y: 0.5, w: 1, h: 2.65 }, { x: 0, y: 2.85, w: 1, h: 2.65 }, { x: 2, y: 0.5, w: 1, h: 2.65 }, { x: 2, y: 2.85, w: 1, h: 2.65 }]
});

objNames.set("Color Led", objTypes.length); // 7
objTypes.push({
    name: "Color Led", w: 2, h: 2, rotate: 3,
    wires: [{ dir: "right", pos: 0 }, { dir: "right", pos: 1 }, { dir: "right", pos: 2 }],
    leds: [{ x: 0, y: 0, w: 3, h: 3 }]
});

objNames.set("Clock", objTypes.length); // 8
objTypes.push({
    name: "Clock", w: 2, h: 2,
    wires: [{ dir: "right", pos: 1 }],
    text: [{ txt: "Clock", x: 1, y: 1 }]
});

objNames.set("Half adder", objTypes.length); // 9
objTypes.push({
    name: "Half adder", w: 1, h: 1,
    wires: [{ dir: "left", pos: 0 }, { dir: "left", pos: 1 }, { dir: "right", pos: 0 }, { dir: "right", pos: 1, txt: "C" }],
    text: [{ txt: "HA", x: 0.5, y: 0.5 }]
});

objNames.set("Full adder", objTypes.length); // 10
objTypes.push({
    name: "Full adder", w: 1, h: 2,
    wires: [{ dir: "left", pos: 0 }, { dir: "left", pos: 1 }, { dir: "left", pos: 2 }, { dir: "right", pos: 0 }, { dir: "right", pos: 1, txt: "C" }],
    text: [{ txt: "FA", x: 0.5, y: 1 }]
});

objNames.set("D flip-flop", objTypes.length); // 11
objTypes.push({
    name: "D flip-flop", w: 1, h: 1,
    wires: [{ dir: "left", pos: 0, txt: "D" }, { dir: "left", pos: 1, clk: true }, { dir: "right", pos: 0, txt: "Q" }],
    text: [{ txt: "FF", x: 0.5, y: 0.5 }]
});

objNames.set("Gated RS latch", objTypes.length);// 12
objTypes.push({
    name: "Gated RS latch", w: 1, h: 2,
    wires: [{ dir: "left", pos: 0, txt: "S" }, { dir: "left", pos: 1, clk: true }, { dir: "left", pos: 2, txt: "R" }, { dir: "right", pos: 0, txt: "Q" }],
    text: [{ txt: "RS", x: 0.5, y: 1 }]
});

objNames.set("RS latch", objTypes.length);// 13
objTypes.push({
    name: "RS latch", w: 1, h: 1,
    wires: [{ dir: "left", pos: 0, txt: "S" }, { dir: "left", pos: 1, txt: "R" }, { dir: "right", pos: 0, txt: "Q" }],
    text: [{ txt: "RS", x: 0.5, y: 0.5 }]
});

// extra

function logic_generator(name, sign, size) {
    objNames.set(name + size, objTypes.length);
    let comp = {
        name: name, w: 1, h: size - 1,
        wires: [],
        text: [{ txt: sign, x: 0.5, y: (size - 1) / 2 }]
    };
    for (let i = 0; i < size; i++)
        comp.wires.push({ dir: "left", pos: i });
    comp.wires.push({ dir: "right", pos: 0 });
    objTypes.push(comp);
}

for (let i = 2; i <= 16; i++)
    logic_generator("And", "&", i);
for (let i = 2; i <= 16; i++)
    logic_generator("Or", "≥1", i);
for (let i = 2; i <= 16; i++)
    logic_generator("XOr", "=1", i);

for (let size = 2; size <= 6; size++) { // Decoder
    let name = "Decoder";
    objNames.set(name + size, objTypes.length);
    let comp = {
        name, w: 2, h: Math.pow(2, size) - 1,
        wires: [],
        text: [{ txt: "DCODE", x: 1, y: 0 }]
    };
    comp.text[0].y = comp.h / 2;
    for (let i = 0; i < size; i++)
        comp.wires.push({ dir: "left", pos: i + Math.ceil((comp.h - size) / 2 ), txt: "" + Math.pow(2, i) });
    for (let i = 0; i <= comp.h; i++)
        comp.wires.push({ dir: "right", pos: i });
    objTypes.push(comp);
}

objNames.set("Point Display", objTypes.length);
let comp = {
    name: "Point Display", w: 9, h: 9, rotate: 3,
    wires: [{ dir: "right", pos: 1, txt: "C" }, { dir: "right", pos: 2, txt: "S" }],
    leds: []
};
for (let y = 0; y < 8; y++)
    for (let x = 0; x < 8; x++)
        comp.leds.push({ x: x + 0.8, y: y + 0.8, w: 1.4, h: 1.4 });
for (let i = 0; i < 6; i++)
    comp.wires.push({ dir: "right", pos: 3 + i, txt: 1 << i });
objTypes.push(comp);

for (let size = 2; size <= 64; size++) { // Door
    let name = "Door";
    objNames.set(name + size, objTypes.length);
    let comp = {
        name, w: 2, h: size - 1,
        wires: [{ dir: "down", pos: 1, txt: "&" }, { dir: "up", pos: 1, txt: "&" }],
        text: [{ txt: "DOOR", x: 1, y: 0 }]
    };
    comp.text[0].y = comp.h / 2;
    for (let i = 0; i < size; i++)
        comp.wires.push({ dir: "left", pos: i });
    for (let i = 0; i <= comp.h; i++)
        comp.wires.push({ dir: "right", pos: i, txt: "" + i });
    objTypes.push(comp);
}


for (let size = 2; size <= 16; size++) { // Number
    let name = "Number";
    objNames.set(name + size, objTypes.length);
    let comp = {
        name, w: 0, h: size-1, rotate: 3,
        wires: [{ dir: "down", pos: 0 }],
        text: [{ txt: "", x: 1, y: 0 }],
        leds: []
    };
    for (let i = 0; i < size; i++) {
        comp.wires.push({ dir: "left", pos: i });
        comp.wires.push({ dir: "right", pos: i });
        comp.leds.push({ txt: true, on: "1", off: "0", undef: "0", x: 0, y: size - i - 1 });
    }
    objTypes.push(comp);
}