        .global _start

        .text
_start:

        mov     $1, %rax                # set write syscall
        mov     $1, %rdi                # set output to stdout
        mov     $message, %rsi          # move message to start index
        mov     $23, %rdx               # move message length
        syscall                         # call kernel

        mov     $999, %rax               # set exit syscall
        mov     $1,  %rdi               # move 1 for exit(1)
        syscall                         # call kernel
message:
        .ascii  "This is a test message\n"
        