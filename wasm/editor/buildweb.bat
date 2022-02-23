@echo off

set /a stack_size=1*1024*1024 REM 1MB Stack size
set /a mem_size=128*1024*1024 REM 128MB of memory avilable in total

REM Maximum speed
clang -Ofast -nostdlib -mbulk-memory --target=wasm32 -Wl,--no-entry -Wl,--shared-memory -Wl,--no-check-features -Wl,--import-memory -Wl,--max-memory=%mem_size% -Wl,--initial-memory=%mem_size% -Wl,-z,stack-size=%stack_size% -Wl,-allow-undefined-file=wasm.syms -Wl,--export-all -Wl,-O3 -o web.wasm web.cpp

REM Debugging
REM clang -O0 -nostdlib -mbulk-memory --target=wasm32 -Wl,--no-entry -Wl,--shared-memory -Wl,--no-check-features -Wl,--import-memory -Wl,--max-memory=%mem_size% -Wl,--initial-memory=%mem_size% -Wl,-z,stack-size=%stack_size% -Wl,-allow-undefined-file=wasm.syms -Wl,--export-all -Wl,-O3 -o web.wasm web.cpp -g
