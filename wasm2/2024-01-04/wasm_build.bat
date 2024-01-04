set /a stack_size=1*1024*1024 REM 1MB Stack size
set /a mem_size=256*1024*1024 REM 256MB of memory avilable in total

REM Maximum speed
clang -D __WASM__ -Ofast -nostdlib -mbulk-memory --target=wasm32 -Wl,--no-entry -Wl,--shared-memory -Wl,--no-check-features -Wl,--import-memory -Wl,--max-memory=%mem_size% -Wl,--initial-memory=%mem_size% -Wl,-z,stack-size=%stack_size% -Wl,-allow-undefined-file=platform_wasm/wasm.syms -Wl,--export-all -Wl,--export=__stack_pointer -Wl,-O3 -pthread -o platform_wasm/web.wasm platform_wasm/web.c

REM Debugging
REM clang -D __WASM__ -g -O0 -nostdlib -mbulk-memory --target=wasm32 -Wl,--no-entry -Wl,--shared-memory -Wl,--no-check-features -Wl,--import-memory -Wl,--max-memory=%mem_size% -Wl,--initial-memory=%mem_size% -Wl,-z,stack-size=%stack_size% -Wl,-allow-undefined-file=platform_wasm/wasm.syms -Wl,--export-all -Wl,--export=__stack_pointer -Wl,-O3 -o platform_wasm/web.wasm platform_wasm/web.c