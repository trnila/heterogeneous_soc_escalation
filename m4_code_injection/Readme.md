# ARM Cortex-M4 instruction injection from the Linux side

```
  for(;;) {
    if(allowed == 42) {
1000000e:       78fb            ldrb    r3, [r7, #3]
10000010:       2b2a            cmp     r3, #42 ; 0x2a
10000012:       d103            bne.n   1000001c <main+0x1c>
      printf("ALLOWED\r\n");
10000014:       4805            ldr     r0, [pc, #20]   ; (1000002c <main+0x2c>)
10000016:       f000 f8d7       bl      100001c8 <iprintf>
1000001a:       e7f8            b.n     1000000e <main+0xe>
    } else {
      printf("%x\r\n", i++);
1000001c:       687b            ldr     r3, [r7, #4]
1000001e:       1c5a            adds    r2, r3, #1
10000020:       607a            str     r2, [r7, #4]
10000022:       4619            mov     r1, r3
10000024:       4802            ldr     r0, [pc, #8]    ; (10000030 <main+0x30>)
10000026:       f000 f8cf       bl      100001c8 <iprintf>
1000002a:       e7f0            b.n     1000000e <main+0xe>
```

The userspace application changes the instruction `bne.n` to `beq.n`.
