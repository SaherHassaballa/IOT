    #data.
.data
msg:    .asciiz "enter integer: "
    # space: .asciiz "
space:  .asciiz " "

.text
main:
    # li $a0, 4
    # la $v0, msg
    # syscall
    la      $a0,    msg
    li      $v0,    4
    syscall

    # li $v0, 4
    li      $v0,    5
    syscall
    move    $a1,    $v0

    li      $t0,    0
    li      $a2,    100

    # move:
loop:
    mul     $a0,    $a1,    $t0
    bge     $a0,    $a2,    endloop
    li      $v0,    1
    syscall
    li      $v0,    4
    la      $a0,    space
    syscall
    addi    $t0,    $t0,    1
    j       loop

endloop:
    li      $v0,    10

exit:
    li      $v0,    10
    syscall