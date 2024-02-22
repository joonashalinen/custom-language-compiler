    .extern print_int
    .extern print_bool
    .extern read_int
    .global main
    
    .section .text
    
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $8, %rsp
    movl $1, -8(%rbp)
    movq   %rbp, %rsp
    popq   %rbp
    ret
