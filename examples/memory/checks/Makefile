all:
	gcc -ggdb memory_checks.c -o memory_checks

vuln:
	echo "Creating vulnerable progam"
	gcc -Wno-stack-protector -z execstack -D_FORTIFY_SOURCE=o -ggdb memory_checks.c -o memory_checks
debug: vuln
	gdb --args ./memory_checks
