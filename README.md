# Graphík Utility Package
This is the official repository of the Graphík Utility Package(GUP). It includes the Graphík programming language, which I specified based on a previous project(namely Flex++), the Graphík Lightweight Compiler(GLC), which compiles Graphík source code down to x64 Assembly, and the Graphík Graphical Calculator(GGC).<br>

This serves as my 1st semester programming project.<br>

The GLC compiler has been built entirely from scratch, using only the C programming language. It is pretty much just like any other compiler, meaning that it does the following steps: <br>
<p>
-Lexical analysis<br>
-Grammatical analysis(Parsing)<br>
-Semantic analysis<br>
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

Using the calculator is a very simple process, therefore I won't go in too much depth here(simply because there isn't such).<br>
For the option `Enter an expression`, you enter a mathematical expression(which may or may not include parameters).<br>
Example:<br>
```
3 + 4 * (10 / 5 + 12 * 3) ^ 2
-----------------------------
x - (x / 10) ^ 3 / 6 + (x / 10) ^ 5 / 120
```

<br>
We are dividing the parameter by a certain value, to zoom in on the graphical representation of the respective function(otherwise it's quite zoomed out).<br>

The other two options expect a directory to be entered. If an invalid directory is entered, an error message box will pop up.<br>
To go back to the main menu, simply press the `Escape` key.<br>

The `Load Graphik output` command expects the directory of a text file, containing coordinates, to be plotted on the Cartesian plain.<br>
The coordinates should be positioned in exactly the same manner as Graphik output. Graphik is intended to be used here, although you may freely use other programming languages to achieve the same results.<br>

The `Load expression file` command expects a text file, containing mathematical expressions on each line, to be plotted on the same plain.<br>
Check the `expr.txt` file in the `samples` folder for reference.

