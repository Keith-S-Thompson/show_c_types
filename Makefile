#
# Adjust the definition of "CC" as needed for your system.
# If you want to add more information to the results.json file,
# please add arguments to the "./show_c_types" command line.  #
#

CC=gcc -std=c99

all:            show_c_types

show_c_types:	show_c_types.o
	$(CC) show_c_types.o -o show_c_types

show_c_types.o:	show_c_types.c
	$(CC) -c show_c_types.c

output:        results.json

results.json:   show_c_types
	./show_c_types config.guess="`./config.guess`" compiler="`gcc --version | head -n 1`" compile="$(CC)" > results.json

clean:
	rm -f show_c_types show_c_types.o results.json
