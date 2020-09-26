let leftMenuData = [
    ["Input", ["Switch", "Clock"]],
    ["Output", ["Led", "Display", "Color Led"]],
    ["Logic", ["And", "Or", "XOr", "Not"]],
    ["Maths", ["Half adder", "Full adder"]],
    ["Memory", ["D flip-flop", "Gated RS latch", "RS latch"]]
];

let objNames = new Map();
let objTypes = [];

objNames.set("And", objTypes.length); // 0
objTypes.push({
    name: "And", w: 1, h: 1,
    wires: [{ dir: "left", pos: 0 }, { dir: "left", pos: 1 }, { dir: "right", pos: 0 }],
    text: [{ txt: "&", x: 0.5, y: 0.5 }]
});

objNames.set("Or", objTypes.length); // 1
objTypes.push({
    name: "Or", w: 1, h: 1,
    wires: [{ dir: "left", pos: 0 }, { dir: "left", pos: 1 }, { dir: "right", pos: 0 }],
    text: [{ txt: "≥1", x: 0.5, y: 0.5 }]
});

objNames.set("XOr", objTypes.length);
objTypes.push({
    name: "XOr", w: 1, h: 1,
    wires: [{ dir: "left", pos: 0 }, { dir: "left", pos: 1 }, { dir: "right", pos: 0 }],
    text: [{ txt: "=1", x: 0.5, y: 0.5 }]
});

objNames.set("Not", objTypes.length);
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
    wires: [{ dir: "left", pos: 0 }, { dir: "left", pos: 1 }, { dir: "right", pos: 0 }, { dir: "right", pos: 1 }],
    text: [{ txt: "HA", x: 0.5, y: 0.5 }]
});

objNames.set("Full adder", objTypes.length); // 10
objTypes.push({
    name: "Full adder", w: 1, h: 2,
    wires: [{ dir: "left", pos: 0 }, { dir: "left", pos: 1 }, { dir: "left", pos: 2 }, { dir: "right", pos: 0 }, { dir: "right", pos: 1 }],
    text: [{ txt: "FA", x: 0.5, y: 1 }]
});

objNames.set("D flip-flop", objTypes.length); // 11
objTypes.push({
    name: "D flip-flop", w: 1, h: 1,
    wires: [{ dir: "left", pos: 0, txt: "D" }, { dir: "left", pos: 1, clk: true }, { dir: "right", pos: 0, txt: "Q" }],
    text: [{ txt: "FF", x: 0.5, y: 0.5 }]
});

objNames.set("Gated RS latch", objTypes.length);
objTypes.push({
    name: "Gated RS latch", w: 1, h: 2,
    wires: [{ dir: "left", pos: 0, txt: "S" }, { dir: "left", pos: 1, clk: true }, { dir: "left", pos: 2, txt: "R" }, { dir: "right", pos: 0, txt: "Q" }],
    text: [{ txt: "RS", x: 0.5, y: 1 }]
});

objNames.set("RS latch", objTypes.length);
objTypes.push({
    name: "RS latch", w: 1, h: 1,
    wires: [{ dir: "right", pos: 0, txt: "Q" }, { dir: "left", pos: 0, txt: "S" }, { dir: "left", pos: 1, txt: "R" }],
    text: [{ txt: "RS", x: 0.5, y: 0.5 }]
});