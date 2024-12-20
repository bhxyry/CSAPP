# CSAPP: 程序的机器级表示

对于本章的指令类信息不在赘述，主要记述相关实现机理和思想。在写的过程中，太过追求完美主义，任何想穷尽总结本章的所有内容。感觉对于确实哪怕很小的细节都是损失，然而这也造成了知识和信息密度过大，反而让文章没有了条理，失去了本文意义。故此在本文中仅列出出一些本章的重要骨架，不在深究细枝末节。



开始时要求程序猿可以编写汇编代码，现在要求程序猿可以阅读编译器产生的代码。学习汇编程序最好的方法--逆向工程。IA32(intel architecture)32位，x86-64是其64位继承。

> 精通细节是理解更深和更基本概念的先决条件。

## 程序编码

#### gcc编译C程序过程

扩展源代码(扩展include、扩展宏)---汇编代码(编译器)---目标文件（汇编器）---可执行代码（连接器）

#### 机器代码两层抽象

- 每条指令按汇编程序的顺序执行（实际执行更精细复杂）。
- 虚拟内存

#### 机器代码可见的处理器状态

- PC（程序计数器）
- 整数寄存器文件
- 条件码寄存器
- 向量寄存器

#### 反汇编器（disassembler）

将机器代码转化成类似于汇编代码的格式。

#### 其他细节

汇编代码格式：ATT与Intel。

可在C程序中插入汇编程序。为什么？（汇编程序可以访问C语言访问不到的机器特性，例子见网络旁注）。

## 数据格式

#### 16个64位寄存器

 [IMG_5796.HEIC](image/IMG_5796.HEIC) 

#### 指令操作数

1. 立即数
2. 寄存器
3. 内存引用

​	有效地址：$Imm+R_b[r_b]+R_i[r_i]\times s$，r_b基址寄存器、r_i变址寄存器、s比例因子。

对于本章的指令类信息不在赘述，主要记述相关实现机理和思想。

## 控制

高级语言中除了顺序执行程序外，还有两个十分重要的程序执行结构--**条件结构和循环结构**

### 条件结构

思想：条件寄存器存储最近一次运算的状态+跳转指令修改PC值。

两种实现方式

- 条件控制

条件满足时按照一条路执行，不满足时按另一条路执行。

汇编翻译格式

```c
t = test-expr;
if(!t)
	goto false;
then-statement
goto done;
false:
	else-statement;
done
```

- 条件传送

计算一个条件分支中两个分枝的结果，根据条件从两个结果中选择一个。并不是所有条件结构都能按条件传送进行编译。

汇编翻译格式

```c
v = then-statement;
ve = else-statement;
t = test-expr;
if (!t) v = ve;
```

在现代的CPU中后者性能优于前者，为什么？

因为现代CPU都是使用流水线来获得高性能，即通过多条指令的重叠执行来调高效率。而分枝模型必须在分枝条件计算处理后决定执行方向，这样降低了效率。现代处理器采用分枝预测逻辑选一条分枝执行，提高重叠率。这个方法也有缺点即预测错误会导致较大的效率损失。条件传送将两个分枝都执行，这样控制流不受限于条件数据，相对多执行分枝的损失小于控制流预测错误损失，所以条件传送由于条件控制。

### 循环结构

思路：条件测试和跳转组合

两种翻译格式

- Jump to middle (只有一处条件测试)

```c
	goto test
loop: 
	body-statement;
test:
	t = test-expr;
	if(t)
		goto loop;
```

- Guarded-do（初始无跳转测试）

```c
t = test-expr;
if (!t)
	goto done;
loop:
	body-statement;
	t = test-expr;
	if (t)
		goto loop;
```

### switch 语句

思路：维护一个跳转表（存储分枝程序）。和很长if-else语句区别，switch语句执行时间于分枝数无关。

以空间换取时间。

## 过程

即C语言中的函数调用，要有三大机制支持：

1. 传递控制
2. 传递数据
3. 分配和释放空间

- 传递控制

​	思路：将PC设为函数的起始地址，将当前地址压入栈中。

- 数据传送

​	思路：通过寄存器（最多6个）和栈来传递

- 栈上的局部存储

     	-  局部变量

     	-  对于个变量使用&，因为必须有一个地址。
     	-  在函数执行完后释放

- 局部存储空间

 	保存函数调用前寄存器的值。

​	调用者保存寄存器和被调用者保存寄存器

## 数组

思想：编译器会对指针运算自行进行收缩。

动态数组：与变长栈帧有关。

## 异质的数据结构

#### 结构（struct）

讲个不同类型组合在一起，实现和数组的机理差不多。

数据对齐：对于不同类型要空出一些内存空间以保证内存大小为2的幂的倍数，以提高内存访问效率。

#### 联合（Union）

对一块内存用不同类型访问，可以用来查看不同数据类型的位模式。

## 缓冲区溢出

对数组的越界的写操作会破坏存储在栈中的状态信息。

黑客利用这一漏洞可以让程序执行本来不愿执行的程序。（例如将返回地址改为攻击函数的地址）。

### 安全措施

1. 栈随机化

    让函数调用的栈帧地址随机。这样黑客就不知道攻击代码所在的位置。但是黑客的应对技巧在攻击代码前插入NOP操作（什么都不会做，会让PC+1）这样只要随机碰到一个NOP，PC就会加至攻击代码的首行。称为“**空操作雪橇**”。

2. 栈破坏检测

​	在栈中引入一种栈保护者机制。就是在返回地址前插入一个金丝雀（canary）值，若这个值被改变值，认为返回地址已遭到破坏不执行。

3. 限制可执行代码区域

​	限制黑客插入攻击代码的能力，将内存区域权限管理。

## 变长栈帧

在变长数组时会用到，在栈帧开始的地方设一个帧指针，用于访问变长区域以前的内容。程序结束时，将栈指针设为帧指针。
