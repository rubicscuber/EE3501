.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

.section .data
.align

array: .word 35,42,15,64,47 //decimal values; each element is a 32 bit word offset by 4 for each element
sum: .word 0

.section .text
.align
.global main

main:
LDR R0,=array //Load memory addr of the list to R0

LDR R1,[R0]   //R1 is the result

MOV R3,0x04 // initialize counter

//0, 4, 8, 12, 16 are the offsets to get values in list

adder:
LDR R2,[R0,R3] //R0  points to value, store that value into R2, offset by value of R3
ADD R1,R1,R2   //R1 = R1 + R2 (value at index)
ADD R3,R3,0x04 //R3 += 4

compare:
CMP R3,0x14 //compare R3(count) with 20
BEQ exit //branch equal to exit
BAL adder

exit:
LDR R0, =sum  //load R0 with the address of sum
STR R1, [R0]  //store R1(resultant) to sum (pointed to by R0)

endprogram:
B endprogram
