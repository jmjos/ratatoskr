# Development 

## Debugging

### map::at exception

SystemC catches access to std::vector elements, which do not exist. You can easily find the line with gdb (`gdb sim`)
- set pagination off
- catch throw
- commands
- backtrace
- continue
- end
- run

