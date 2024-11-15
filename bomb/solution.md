# bomb

## 实验使用命令

- objdump -t

​	用于打印程序所定义的符号表。

-  objdump -d

​	可以对二进制文件进行反汇编。

- gdb

​	程序的命令行调试工具。

```c
gdb name				//开始调试
b		fun_name		//设置断点
i 	b						//查看断点信息
r 							//运行程序
dias 						//查看当前函数的反汇编
stepi						//单步执行
next						//跳过函数
print						//可以打印内存、寄存器信息
x								//用于查看内存值
```

本实验共有6个爆点（有隐藏，暂时没尝试）。

基本流程：对要破解的阶段设置断点，以及`explode_bomb`设置断点。

## phase_1

通过观察`phase_1`的汇编。

```shell
0000000000400ee0 <phase_1>:
  400ee0: 48 83 ec 08                   subq    $0x8, %rsp
  400ee4: be 00 24 40 00                movl    $0x402400, %esi         # imm = 0x402400
  400ee9: e8 4a 04 00 00                callq   0x401338 <strings_not_equal>
  400eee: 85 c0                         testl   %eax, %eax
  400ef0: 74 05                         je      0x400ef7 <phase_1+0x17>
  400ef2: e8 43 05 00 00                callq   0x40143a <explode_bomb>
  400ef7: 48 83 c4 08                   addq    $0x8, %rsp
  400efb: c3                            retq
```

可以发现在调用`strings_not_equal`参数为内存`0x402400`中的内容。打印内存得到。

```
(gdb) x/s 0x402400
0x402400:       "Border relations with Canada have never been better."
```

## phase_2

```shell
0000000000400efc <phase_2>:
  400efc: 55                            pushq   %rbp
  400efd: 53                            pushq   %rbx
  400efe: 48 83 ec 28                   subq    $0x28, %rsp
  400f02: 48 89 e6                      movq    %rsp, %rsi
  400f05: e8 52 05 00 00                callq   0x40145c <read_six_numbers>
  400f0a: 83 3c 24 01                   cmpl    $0x1, (%rsp)
  400f0e: 74 20                         je      0x400f30 <phase_2+0x34>
  400f10: e8 25 05 00 00                callq   0x40143a <explode_bomb>
  400f15: eb 19                         jmp     0x400f30 <phase_2+0x34>
  400f17: 8b 43 fc                      movl    -0x4(%rbx), %eax
  400f1a: 01 c0                         addl    %eax, %eax
  400f1c: 39 03                         cmpl    %eax, (%rbx)
  400f1e: 74 05                         je      0x400f25 <phase_2+0x29>
  400f20: e8 15 05 00 00                callq   0x40143a <explode_bomb>
  400f25: 48 83 c3 04                   addq    $0x4, %rbx
  400f29: 48 39 eb                      cmpq    %rbp, %rbx
  400f2c: 75 e9                         jne     0x400f17 <phase_2+0x1b>
  400f2e: eb 0c                         jmp     0x400f3c <phase_2+0x40>
  400f30: 48 8d 5c 24 04                leaq    0x4(%rsp), %rbx
  400f35: 48 8d 6c 24 18                leaq    0x18(%rsp), %rbp
  400f3a: eb db                         jmp     0x400f17 <phase_2+0x1b>
  400f3c: 48 83 c4 28                   addq    $0x28, %rsp
  400f40: 5b                            popq    %rbx
  400f41: 5d                            popq    %rbp
  400f42: c3                            retq
```

`read_six_numbers`读六个数，第一个数为1，后面的数是前面的2倍。所以为`1 2 4 8 16 32`。

## phase_3

```shell
0000000000400f43 <phase_3>:
  400f43: 48 83 ec 18                   subq    $0x18, %rsp
  400f47: 48 8d 4c 24 0c                leaq    0xc(%rsp), %rcx
  400f4c: 48 8d 54 24 08                leaq    0x8(%rsp), %rdx
  400f51: be cf 25 40 00                movl    $0x4025cf, %esi         # imm = 0x4025CF
  400f56: b8 00 00 00 00                movl    $0x0, %eax
  400f5b: e8 90 fc ff ff                callq   0x400bf0 <__isoc99_sscanf@plt>
  400f60: 83 f8 01                      cmpl    $0x1, %eax
  400f63: 7f 05                         jg      0x400f6a <phase_3+0x27>
  400f65: e8 d0 04 00 00                callq   0x40143a <explode_bomb>
  400f6a: 83 7c 24 08 07                cmpl    $0x7, 0x8(%rsp)
  400f6f: 77 3c                         ja      0x400fad <phase_3+0x6a>
  400f71: 8b 44 24 08                   movl    0x8(%rsp), %eax
  400f75: ff 24 c5 70 24 40 00          jmpq    *0x402470(,%rax,8)
  400f7c: b8 cf 00 00 00                movl    $0xcf, %eax
  400f81: eb 3b                         jmp     0x400fbe <phase_3+0x7b>
  400f83: b8 c3 02 00 00                movl    $0x2c3, %eax            # imm = 0x2C3
  400f88: eb 34                         jmp     0x400fbe <phase_3+0x7b>
  400f8a: b8 00 01 00 00                movl    $0x100, %eax            # imm = 0x100
  400f8f: eb 2d                         jmp     0x400fbe <phase_3+0x7b>
  400f91: b8 85 01 00 00                movl    $0x185, %eax            # imm = 0x185
  400f96: eb 26                         jmp     0x400fbe <phase_3+0x7b>
  400f98: b8 ce 00 00 00                movl    $0xce, %eax
  400f9d: eb 1f                         jmp     0x400fbe <phase_3+0x7b>
  400f9f: b8 aa 02 00 00                movl    $0x2aa, %eax            # imm = 0x2AA
  400fa4: eb 18                         jmp     0x400fbe <phase_3+0x7b>
  400fa6: b8 47 01 00 00                movl    $0x147, %eax            # imm = 0x147
  400fab: eb 11                         jmp     0x400fbe <phase_3+0x7b>
  400fad: e8 88 04 00 00                callq   0x40143a <explode_bomb>
  400fb2: b8 00 00 00 00                movl    $0x0, %eax
  400fb7: eb 05                         jmp     0x400fbe <phase_3+0x7b>
  400fb9: b8 37 01 00 00                movl    $0x137, %eax            # imm = 0x137
  400fbe: 3b 44 24 0c                   cmpl    0xc(%rsp), %eax
  400fc2: 74 05                         je      0x400fc9 <phase_3+0x86>
  400fc4: e8 71 04 00 00                callq   0x40143a <explode_bomb>
  400fc9: 48 83 c4 18                   addq    $0x18, %rsp
  400fcd: c3                            retq
```

汇编为switch结构，打印`0x4025cf`,要求为两个整数。通过观察汇编可以看出有八种解法前一位switch选择，后面为对应选择的数。

```
(gdb) x/s 0x4025cf
0x4025cf:       "%d %d"
```

## phase_4

```shell 
000000000040100c <phase_4>:
  40100c: 48 83 ec 18                   subq    $0x18, %rsp
  401010: 48 8d 4c 24 0c                leaq    0xc(%rsp), %rcx
  401015: 48 8d 54 24 08                leaq    0x8(%rsp), %rdx
  40101a: be cf 25 40 00                movl    $0x4025cf, %esi         # imm = 0x4025CF
  40101f: b8 00 00 00 00                movl    $0x0, %eax
  401024: e8 c7 fb ff ff                callq   0x400bf0 <__isoc99_sscanf@plt>
  401029: 83 f8 02                      cmpl    $0x2, %eax
  40102c: 75 07                         jne     0x401035 <phase_4+0x29>
  40102e: 83 7c 24 08 0e                cmpl    $0xe, 0x8(%rsp)
  401033: 76 05                         jbe     0x40103a <phase_4+0x2e>
  401035: e8 00 04 00 00                callq   0x40143a <explode_bomb>
  40103a: ba 0e 00 00 00                movl    $0xe, %edx
  40103f: be 00 00 00 00                movl    $0x0, %esi
  401044: 8b 7c 24 08                   movl    0x8(%rsp), %edi
  401048: e8 81 ff ff ff                callq   0x400fce <func4>
  40104d: 85 c0                         testl   %eax, %eax
  40104f: 75 07                         jne     0x401058 <phase_4+0x4c>
  401051: 83 7c 24 0c 00                cmpl    $0x0, 0xc(%rsp)
  401056: 74 05                         je      0x40105d <phase_4+0x51>
  401058: e8 dd 03 00 00                callq   0x40143a <explode_bomb>
  40105d: 48 83 c4 18                   addq    $0x18, %rsp
  401061: c3                            retq
```

打印可以看出要求仍为两个整数。

```shell
(gdb) x/s 0x4025cf
0x4025cf:       "%d %d"
```

阅读汇编代码可以发现第二个数为0，第一个数`func4`有关。

```shell
0000000000400fce <func4>:
  400fce: 48 83 ec 08                   subq    $0x8, %rsp
  400fd2: 89 d0                         movl    %edx, %eax
  400fd4: 29 f0                         subl    %esi, %eax
  400fd6: 89 c1                         movl    %eax, %ecx
  400fd8: c1 e9 1f                      shrl    $0x1f, %ecx
  400fdb: 01 c8                         addl    %ecx, %eax
  400fdd: d1 f8                         sarl    %eax
  400fdf: 8d 0c 30                      leal    (%rax,%rsi), %ecx
  400fe2: 39 f9                         cmpl    %edi, %ecx
  400fe4: 7e 0c                         jle     0x400ff2 <func4+0x24>
  400fe6: 8d 51 ff                      leal    -0x1(%rcx), %edx
  400fe9: e8 e0 ff ff ff                callq   0x400fce <func4>
  400fee: 01 c0                         addl    %eax, %eax
  400ff0: eb 15                         jmp     0x401007 <func4+0x39>
  400ff2: b8 00 00 00 00                movl    $0x0, %eax
  400ff7: 39 f9                         cmpl    %edi, %ecx
  400ff9: 7d 0c                         jge     0x401007 <func4+0x39>
  400ffb: 8d 71 01                      leal    0x1(%rcx), %esi
  400ffe: e8 cb ff ff ff                callq   0x400fce <func4>
  401003: 8d 44 00 01                   leal    0x1(%rax,%rax), %eax
  401007: 48 83 c4 08                   addq    $0x8, %rsp
  40100b: c3                            retq
```

阅读上述汇编发现只有7可以满足返回值为0。

## phase_5

```shell 
0000000000401062 <phase_5>:
  401062: 53                            pushq   %rbx
  401063: 48 83 ec 20                   subq    $0x20, %rsp
  401067: 48 89 fb                      movq    %rdi, %rbx
  40106a: 64 48 8b 04 25 28 00 00 00    movq    %fs:0x28, %rax
  401073: 48 89 44 24 18                movq    %rax, 0x18(%rsp)
  401078: 31 c0                         xorl    %eax, %eax
  40107a: e8 9c 02 00 00                callq   0x40131b <string_length>
  40107f: 83 f8 06                      cmpl    $0x6, %eax
  401082: 74 4e                         je      0x4010d2 <phase_5+0x70>
  401084: e8 b1 03 00 00                callq   0x40143a <explode_bomb>
  401089: eb 47                         jmp     0x4010d2 <phase_5+0x70>
  40108b: 0f b6 0c 03                   movzbl  (%rbx,%rax), %ecx
  40108f: 88 0c 24                      movb    %cl, (%rsp)
  401092: 48 8b 14 24                   movq    (%rsp), %rdx
  401096: 83 e2 0f                      andl    $0xf, %edx
  401099: 0f b6 92 b0 24 40 00          movzbl  0x4024b0(%rdx), %edx
  4010a0: 88 54 04 10                   movb    %dl, 0x10(%rsp,%rax)
  4010a4: 48 83 c0 01                   addq    $0x1, %rax
  4010a8: 48 83 f8 06                   cmpq    $0x6, %rax
  4010ac: 75 dd                         jne     0x40108b <phase_5+0x29>
  4010ae: c6 44 24 16 00                movb    $0x0, 0x16(%rsp)
  4010b3: be 5e 24 40 00                movl    $0x40245e, %esi         # imm = 0x40245E
  4010b8: 48 8d 7c 24 10                leaq    0x10(%rsp), %rdi
  4010bd: e8 76 02 00 00                callq   0x401338 <strings_not_equal>
  4010c2: 85 c0                         testl   %eax, %eax
  4010c4: 74 13                         je      0x4010d9 <phase_5+0x77>
  4010c6: e8 6f 03 00 00                callq   0x40143a <explode_bomb>
  4010cb: 0f 1f 44 00 00                nopl    (%rax,%rax)
  4010d0: eb 07                         jmp     0x4010d9 <phase_5+0x77>
  4010d2: b8 00 00 00 00                movl    $0x0, %eax
  4010d7: eb b2                         jmp     0x40108b <phase_5+0x29>
  4010d9: 48 8b 44 24 18                movq    0x18(%rsp), %rax
  4010de: 64 48 33 04 25 28 00 00 00    xorq    %fs:0x28, %rax
  4010e7: 74 05                         je      0x4010ee <phase_5+0x8c>
  4010e9: e8 42 fa ff ff                callq   0x400b30 <__stack_chk_fail@plt>
  4010ee: 48 83 c4 20                   addq    $0x20, %rsp
```

可以发现谜底为长度为6的字符串。

```c++
(gdb) x/s 0x40245e
0x40245e:       "flyers"
(gdb) x/s 0x4024b0
0x4024b0 <array.3449>:  "maduiersnfotvbylSo you think you can stop the bomb with ctrl-c, do you?"
```

通过对比找出这几个字符的相对位置，然后对照ascii码后4位为那些数的组合就是答案（不唯一）。

## phase_6

```shell
00000000004010f4 <phase_6>:
  4010f4: 41 56                        	pushq	%r14
  4010f6: 41 55                        	pushq	%r13
  4010f8: 41 54                        	pushq	%r12
  4010fa: 55                           	pushq	%rbp
  4010fb: 53                           	pushq	%rbx
  4010fc: 48 83 ec 50                  	subq	$0x50, %rsp
  401100: 49 89 e5                     	movq	%rsp, %r13
  401103: 48 89 e6                     	movq	%rsp, %rsi
  401106: e8 51 03 00 00               	callq	0x40145c <read_six_numbers>
  40110b: 49 89 e6                     	movq	%rsp, %r14
  40110e: 41 bc 00 00 00 00            	movl	$0x0, %r12d
  401114: 4c 89 ed                     	movq	%r13, %rbp
  401117: 41 8b 45 00                  	movl	(%r13), %eax
  40111b: 83 e8 01                     	subl	$0x1, %eax
  40111e: 83 f8 05                     	cmpl	$0x5, %eax
  401121: 76 05                        	jbe	0x401128 <phase_6+0x34>
  401123: e8 12 03 00 00               	callq	0x40143a <explode_bomb>
  401128: 41 83 c4 01                  	addl	$0x1, %r12d
  40112c: 41 83 fc 06                  	cmpl	$0x6, %r12d
  401130: 74 21                        	je	0x401153 <phase_6+0x5f>
  401132: 44 89 e3                     	movl	%r12d, %ebx
  401135: 48 63 c3                     	movslq	%ebx, %rax
  401138: 8b 04 84                     	movl	(%rsp,%rax,4), %eax
  40113b: 39 45 00                     	cmpl	%eax, (%rbp)
  40113e: 75 05                        	jne	0x401145 <phase_6+0x51>
  401140: e8 f5 02 00 00               	callq	0x40143a <explode_bomb>
  401145: 83 c3 01                     	addl	$0x1, %ebx
  401148: 83 fb 05                     	cmpl	$0x5, %ebx
  40114b: 7e e8                        	jle	0x401135 <phase_6+0x41>
  40114d: 49 83 c5 04                  	addq	$0x4, %r13
  401151: eb c1                        	jmp	0x401114 <phase_6+0x20>
  401153: 48 8d 74 24 18               	leaq	0x18(%rsp), %rsi
  401158: 4c 89 f0                     	movq	%r14, %rax
  40115b: b9 07 00 00 00               	movl	$0x7, %ecx
  401160: 89 ca                        	movl	%ecx, %edx
  401162: 2b 10                        	subl	(%rax), %edx
  401164: 89 10                        	movl	%edx, (%rax)
  401166: 48 83 c0 04                  	addq	$0x4, %rax
  40116a: 48 39 f0                     	cmpq	%rsi, %rax
  40116d: 75 f1                        	jne	0x401160 <phase_6+0x6c>
  40116f: be 00 00 00 00               	movl	$0x0, %esi
  401174: eb 21                        	jmp	0x401197 <phase_6+0xa3>
  401176: 48 8b 52 08                  	movq	0x8(%rdx), %rdx
  40117a: 83 c0 01                     	addl	$0x1, %eax
  40117d: 39 c8                        	cmpl	%ecx, %eax
  40117f: 75 f5                        	jne	0x401176 <phase_6+0x82>
  401181: eb 05                        	jmp	0x401188 <phase_6+0x94>
  401183: ba d0 32 60 00               	movl	$0x6032d0, %edx         # imm = 0x6032D0
  401188: 48 89 54 74 20               	movq	%rdx, 0x20(%rsp,%rsi,2)
  40118d: 48 83 c6 04                  	addq	$0x4, %rsi
  401191: 48 83 fe 18                  	cmpq	$0x18, %rsi
  401195: 74 14                        	je	0x4011ab <phase_6+0xb7>
  401197: 8b 0c 34                     	movl	(%rsp,%rsi), %ecx
  40119a: 83 f9 01                     	cmpl	$0x1, %ecx
  40119d: 7e e4                        	jle	0x401183 <phase_6+0x8f>
  40119f: b8 01 00 00 00               	movl	$0x1, %eax
  4011a4: ba d0 32 60 00               	movl	$0x6032d0, %edx         # imm = 0x6032D0
  4011a9: eb cb                        	jmp	0x401176 <phase_6+0x82>
  4011ab: 48 8b 5c 24 20               	movq	0x20(%rsp), %rbx
  4011b0: 48 8d 44 24 28               	leaq	0x28(%rsp), %rax
  4011b5: 48 8d 74 24 50               	leaq	0x50(%rsp), %rsi
  4011ba: 48 89 d9                     	movq	%rbx, %rcx
  4011bd: 48 8b 10                     	movq	(%rax), %rdx
  4011c0: 48 89 51 08                  	movq	%rdx, 0x8(%rcx)
  4011c4: 48 83 c0 08                  	addq	$0x8, %rax
  4011c8: 48 39 f0                     	cmpq	%rsi, %rax
  4011cb: 74 05                        	je	0x4011d2 <phase_6+0xde>
  4011cd: 48 89 d1                     	movq	%rdx, %rcx
  4011d0: eb eb                        	jmp	0x4011bd <phase_6+0xc9>
  4011d2: 48 c7 42 08 00 00 00 00      	movq	$0x0, 0x8(%rdx)
  4011da: bd 05 00 00 00               	movl	$0x5, %ebp
  4011df: 48 8b 43 08                  	movq	0x8(%rbx), %rax
  4011e3: 8b 00                        	movl	(%rax), %eax
  4011e5: 39 03                        	cmpl	%eax, (%rbx)
  4011e7: 7d 05                        	jge	0x4011ee <phase_6+0xfa>
  4011e9: e8 4c 02 00 00               	callq	0x40143a <explode_bomb>
  4011ee: 48 8b 5b 08                  	movq	0x8(%rbx), %rbx
  4011f2: 83 ed 01                     	subl	$0x1, %ebp
  4011f5: 75 e8                        	jne	0x4011df <phase_6+0xeb>
  4011f7: 48 83 c4 50                  	addq	$0x50, %rsp
  4011fb: 5b                           	popq	%rbx
  4011fc: 5d                           	popq	%rbp
  4011fd: 41 5c                        	popq	%r12
  4011ff: 41 5d                        	popq	%r13
  401201: 41 5e                        	popq	%r14
  401203: c3                           	retq
```

这个汇编实在太长了，读不下去，具体为数据结构构成的链表的排序。