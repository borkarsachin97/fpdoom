.text
.arm
1: b 1b
2: b 2b
   ldr pc, 3f + 12
4: b 4b
5: b 5b
6: b 6b
   ldr pc, 3f
7: b 7b

3: .long 0x11111111
   .long 0x22222222
   .long 0x33333333
   .long 0x44444444
