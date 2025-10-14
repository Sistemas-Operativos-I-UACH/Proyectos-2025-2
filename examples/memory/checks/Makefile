all:
	gcc -ggdb memory_checks.c -o memory_checks

debug: all
	gdb --args ./memory_checks 123456
