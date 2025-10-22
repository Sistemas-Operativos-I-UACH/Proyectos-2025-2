all:
	gcc -ggdb memory_checks.c -o memory_checks
	gcc -ggdb dynamic_memory.c -o dynamic_memory

dyn:
	gcc -ggdb dynamic_memory.c -o dynamic_memory
vuln:
	echo "Creating vulnerable progam"
	gcc -Wno-stack-protector -z execstack -D_FORTIFY_SOURCE=o -ggdb memory_checks.c -o memory_checks

dyn_vuln:
	echo "Creating vulnerable progam"
	gcc -Wno-stack-protector -z execstack -D_FORTIFY_SOURCE=o -ggdb dynamic_memory.c -o dynamic_memory

debug: vuln
	gdb --args ./memory_checks

debug_dyn: dyn
	gdb --args ./dynamic_memory

debug_dyn_vuln: dyn_vuln
	gdb --args ./dynamic_memory
