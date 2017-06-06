/*
v1 = $0 + $0
if v1 4
v1 = $0 - $7
ret
*/

pushq %rbp
movq %rsp, %rbp
subq $16, %rsp

movl $0, %edx
addl $0, %edx
movl %edx, -4(%rbp)

movl -4(%rbp), %edx
cmpl $0, %edx
jne return

movl $0, %edx
subl $7, %edx
movl %edx, -4(%rbp)

movl -4(%rbp), %eax
leave
ret
