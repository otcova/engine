function createSharedArray(length, type) {
    switch (type) {
    case "i8":
        return new Int8Array(Module.HEAP8.buffer, Module._malloc(length), length);
    case "u8":
        return new Uint8Array(Module.HEAP8.buffer, Module._malloc(length), length);
    case "i16":
        return new Int16Array(Module.HEAP8.buffer, Module._malloc(length * 2), length);
    case "u16":
        return new Uint16Array(Module.HEAP8.buffer, Module._malloc(length * 2), length);
    case "i32":
        return new Int32Array(Module.HEAP8.buffer, Module._malloc(length * 4), length);
    case "u32":
        return new Uint32Array(Module.HEAP8.buffer, Module._malloc(length * 4), length);
    case "i64":
        return new BigInt64Array(Module.HEAP8.buffer, Module._malloc(length * 8), length);
    case "u64":
        return new BigUint64Array(Module.HEAP8.buffer, Module._malloc(length * 8), length);
    case "f32":
        return new Float32Array(Module.HEAP8.buffer, Module._malloc(length * 4), length);
    case "f64":
        return new Float64Array(Module.HEAP8.buffer, Module._malloc(length * 8), length);
    }
    console.assert(false, "invalid type: " + type);
    return undefined;
}
function deleteSharedArray(arr) {
    Module._free(arr.byteOffset);
}