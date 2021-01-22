# Graphík Utility Package
This is the official repository of the Graphík Utility Package(GUP). It includes the Graphík programming language, which I specified based on a previous project(namely Flex++), the Graphík Lightweight Compiler(GLC), which compiles Graphík source code down to x64 Assembly, and the Graphík Graphical Calculator(GGC).<br>

This serves as my 1st semester programming project.<br>

The GLC compiler has been built entirely from scratch, using only the C programming language. It is pretty much just like any other compiler, meaning that it does the following steps: <br>
<p>
-Lexical analysis<br>
-Grammatical analysis(Parsing)<br>
-Semantic analysis
-Assembly generation
</p>

The compiler uses external software to assemble and link the resulting Assembly program and object file respectively.<br>
<p>
-Netwide Assembler(NASM)<br>
-GNU Linker(LD)
</p>

The compiler follows the Microsoft x64 calling convention, except that all function arguments are pushed onto the stack(They are not passed to registers).<br>
The stack is aligned according to the Microsoft x64 stack usage rules:<br>
```
There must be atleast 32 bytes of shadow space for function calls(not quoted).
```
<br>

A thorough documentary of the Graphik programming language will be coming soon, but as of now, you may use the documents I have provided in the `docs` folder in the `graphik` directory as a reference.<br>

Example program(using recursion):<br>
```
declare fib(n) begin
	if n == 0 begin
		ret 0
	end

	if n == 1 begin
		ret 1
	end

	lbr = invoke fib(n - 1)
	rbr = invoke fib(n - 2)

	ret lbr + rbr
end

declare main(none) begin
	cnt = 0

	while cnt < 20 begin
		f = invoke fib(cnt)
		plot cnt f

		cnt = cnt + 1
	end

	ret 0
end
```
<br>
