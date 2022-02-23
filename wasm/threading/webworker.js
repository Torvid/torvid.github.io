executing = false
Initialized = false;
memory = 0;
global_exports = 0;
global_exports_loaded = 0;
thread_index = -1

function CStringToJsString(n) {
    // n is a pointer to the string
    // Strings are null-terminated, so read characters untill we hit a 0.
    // Stop at 200 if the string is too long.
    // This should probably be changed to also take a length value for memory-safety reasons?

    result = ""
    for (i = 0; i < 200; i++) {
        if (memory[n + i] == 0)
            break
        result += String.fromCharCode(memory[n + i]);
    }
    return result
}

function PlatformPrint(n) {
    console.log(CStringToJsString(n))
}

function LoadFileIntoCanvas(Destination, Filename) { }
function PlatformTime() { }
function LoadPersistentData(Destination, SizeInBytes) { }
function SavePersistentData(Source, SizeInBytes) { }
function PlatformThreadCall(funcptr, index) { }
function print_num(n) {
    console.log(n)
}

async function init(memoryThing) {
    let imports = {};
    imports['LoadFileIntoCanvas'] = LoadFileIntoCanvas
    imports['PlatformPrint'] = PlatformPrint
    imports['PlatformTime'] = PlatformTime
    imports['LoadPersistentData'] = LoadPersistentData
    imports['SavePersistentData'] = SavePersistentData
    imports['PlatformThreadCall'] = PlatformThreadCall
    imports['print_num'] = print_num
    imports['sqrtf'] = function (a) { return Math.sqrtf(a) }
    imports['floorf'] = function (a) { return Math.floor(a) }
    imports['ceilf'] = function (a) { return Math.ceil(a) }
    imports['sinf'] = function (a) { return Math.sin(a) }
    imports['cosf'] = function (a) { return Math.cos(a) }
    imports['atan2'] = function (a, b) { return Math.atan2(a, b) }
    imports['log10'] = function (a) { return Math.log10(a) }
    imports['fabsf'] = function (a) { return Math.abs(a) }
    imports['pow'] = function (a, b) { return Math.pow(a, b) }
    imports['fminf'] = function (a, b) { return Math.min(a, b) }
    imports['fmaxf'] = function (a, b) { return Math.max(a, b) }
    imports['roundf'] = function (a) { return Math.round(a) }
    imports['exp2f'] = function (a) { return Math.exp(a) }
    imports['acosf'] = function (a) { return Math.acos(a) }
    imports['memory'] = memoryThing

    const { instance } = await WebAssembly.instantiateStreaming(fetch("web.wasm"), { "env": imports });

    // Alternate way of loading wasm that works even if the webserver doesn't support wasm files.
    //const response = await fetch("web.wasm");
    //const buffer = await response.arrayBuffer();
    //const obj = await WebAssembly.instantiate(buffer, { "env": imports });
    //instance = obj.instance;

    global_exports = instance.exports;
    global_exports_loaded = true;
}

recievedCall = false;

function sendit() {
    if (global_exports_loaded && Initialized) {

        global_exports.WebThreadCall(thread_index);
    }
    setTimeout(sendit, 1);
}
setTimeout(sendit, 1);

function ReceiveData(a) {

    Initialize = a.data[0];
    thread_index = a.data[1];
    memoryThing = a.data[2];
    function_pointer = a.data[3];
    memory = new Uint8ClampedArray(memoryThing.buffer, 0, memoryThing.buffer.byteLength)
    init(memoryThing);
    Initialized = true;
}

onmessage = ReceiveData