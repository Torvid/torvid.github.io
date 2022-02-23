@echo off

set /a stack_size=1*1024*1024 REM 1MB Stack size
set /a mem_size=128*1024*1024 REM 128MB of memory avilable in total

clang -Ofast -nostdlib -mbulk-memory --target=wasm32 -Wl,--no-entry -Wl,--initial-memory=%mem_size% -Wl,-z,stack-size=%stack_size% -Wl,-allow-undefined-file=wasm.syms -Wl,--export-all -Wl,-O3 -o web.wasm web.cpp

REM clang -nostdlib -mbulk-memory --target=wasm32 -Wl,--no-entry -Wl,--initial-memory=%mem_size% -Wl,-z,stack-size=%stack_size% -Wl,-allow-undefined-file=wasm.syms -Wl,--export-all -o web.wasm web.cpp