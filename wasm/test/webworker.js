//console.log("Hello test aaa");
executing = false
memory = 0;

function CStringToJsString(n) { }
function PlatformPrint(n) { }
function LoadFileIntoCanvas(Destination, Filename) { }
function PlatformTime() { }
function LoadPersistentData(Destination, SizeInBytes) { }
function SavePersistentData(Source, SizeInBytes) { }
function PlatformThreadCall(funcptr, index) { }
function print_num(n) { }

async function init(function_pointer, memory) {
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


    const { instance } = await WebAssembly.instantiateStreaming(fetch("web.wasm"), { "env": memory });

    // Alternate way of loading wasm that works even if the webserver doesn't support wasm files.
    //const response = await fetch("web.wasm");
    //const buffer = await response.arrayBuffer();
    //const obj = await WebAssembly.instantiate(buffer, { "env": imports });
    //instance = obj.instance;

    console.log("function_pointer: " + function_pointer);

    global_exports = instance.exports;
    global_exports.CallFunc(function_pointer)
    //global_exports_loaded = true;
    //reportWindowSize();
}

function ReceiveData(a) {
    if (executing)
        return;
    
    executing = true;
    //console.log("Startup!");


    function_pointer = a.data[0];
    memory = a.data[1];
    thread_index = a.data[2];
    init(function_pointer, memory);
}

onmessage = ReceiveData