        .global _start

        .text
_start:

        mov     $1, %rax                # set write syscall
        mov     $1, %rdi                # set output to stdout
        mov     $message, %rsi          # move message to start index
        mov     $23, %rdx               # move message length
        syscall                         # call kernel

        mov     $60, %rax               # set exit syscall
        xor     %rdi, %rdi              # zero rdi for exit 0
        syscall                         # call kernel
        
message:
        .ascii  "This is a test message\n"
        