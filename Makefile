#
# Adjust the definition of "CC" and "CC_VERSION" as needed for your system.
# Enable C99 or higher conformance if possible.
# If you want to add more information to the results.json file,
# add arguments to the "./show_c_types" command line.
#

CC=gcc -std=c99
CC_VERSION=gcc --version | head -n 1

target:         output

show_c_types:	show_c_types.o
	$(CC) show_c_types.o -o show_c_types

show_c_types.o:	show_c_types.c
	$(CC) -c show_c_types.c

output:         results.json

results.json:   show_c_types
	./show_c_types config.guess="`./config.guess`" compiler="`$(CC_VERSION)`" compile_command="$(CC)" > results.json

test:           show_c_types
	./test.pl

clean:
	rm -f show_c_types show_c_types.o results.json
