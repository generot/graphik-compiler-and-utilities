global main
extern printf, exit
section .data
fmt db "%d %d", 10, 0
section .text
main:
push rbp
mov rbp, rsp
sub rsp, 256
mov rax, 600
mov [rbp - 8], rax
mov rax, 10
mov [rbp - 16], rax
mov rax, [rbp - 8]
mov rcx, 2
cqo
idiv rcx
mov [rbp - 24], rax
mov rax, [rbp - 24]
mov [rbp - 32], rax
mov rax, [rbp - 32]
neg rax
mov [rbp - 40], rax
wl0:
mov rax, [rbp - 24]
neg rax
mov [rbp - 48], rax
wl1:
mov rax, [rbp - 48]
mov [rbp - 56], rax
mov rax, [rbp - 40]
mov [rbp - 64], rax
mov rax, [rbp - 56]
mov [rbp - 72], rax
mov rax, [rbp - 64]
mov [rbp - 80], rax
mov rax, 0
mov [rbp - 88], rax
wl2:
mov rax, [rbp - 56]
mov rdx, rax
mov rax, [rbp - 56]
imul rdx
mov rdx, rax
mov rax, [rbp - 64]
neg rax
mov rcx, [rbp - 64]
imul rdx
add rax, rcx
mov [rbp - 96], rax
mov rax, [rbp - 56]
mov rdx, rax
mov rax, [rbp - 64]
imul rdx
mov rcx, 2
imul rcx
mov [rbp - 104], rax
mov rax, [rbp - 96]
mov rdx, rax
mov rax, [rbp - 72]
add rax, rdx
mov [rbp - 56], rax
mov rax, [rbp - 104]
mov rdx, rax
mov rax, [rbp - 80]
add rax, rdx
mov [rbp - 64], rax
mov rax, [rbp - 56]
mov rdx, rax
mov rax, [rbp - 56]
imul rdx
mov rdx, rax
mov rax, [rbp - 64]
mov rcx, [rbp - 64]
imul rdx
add rax, rcx
mov [rbp - 112], rax
mov rax, [rbp - 112]
mov rcx, 4
cmp rax, rcx
setg al
movzx rax, al
cmp rax, 1
jne li0
jmp lw2
li0:
mov rax, [rbp - 88]
mov rcx, 1
add rax, rcx
mov [rbp - 88], rax
mov rax, [rbp - 88]
mov rcx, 10
cmp rax, rcx
setl al
movzx rax, al
cmp rax, 1
je wl2
lw2:
mov rax, [rbp - 88]
mov rcx, 7
cmp rax, rcx
setge al
movzx rax, al
cmp rax, 1
jne li1
mov rax, [rbp - 48]
mov rdx, rax
mov rax, [rbp - 40]
mov r8, rax
lea rcx, [fmt]
call printf
li1:
mov rax, [rbp - 48]
mov rcx, 1
add rax, rcx
mov [rbp - 48], rax
mov rax, [rbp - 48]
mov rdx, rax
mov rax, [rbp - 24]
cmp rdx, rax
setl al
movzx rax, al
cmp rax, 1
je wl1
lw1:
mov rax, [rbp - 40]
mov rcx, 1
add rax, rcx
mov [rbp - 40], rax
mov rax, [rbp - 40]
mov rdx, rax
mov rax, [rbp - 32]
cmp rdx, rax
setl al
movzx rax, al
cmp rax, 1
je wl0
lw0:
mov rax, 0
add rsp, 256
pop rbp
push rax
call exit
