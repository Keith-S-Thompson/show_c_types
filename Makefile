# Copyright (C) 2012 Keith Thompson

# This file is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses/>.

# Adjust the definition of "CC" and "CC_VERSION" as needed for your system.
# Enable C99 or higher conformance if possible.
# If you want to add more information to the `config.guess`.ini file,
# add arguments to the "./show_c_types" command line.

########################################################################

# By default, we assume that the settings of certain predefined macros
# (__STDC_VERSION__, etc.) determine which types are supported.  This
# assumption is not always reliable; for example, some old compilers like
# VAXC don't support signed char; others do support long long without
# claiming C99 conformance.
#
# You can override the default assumptions by defining any of the following
# macros.  (The code only checks whether they're defined or not; their
# values are ignored.)
#     ENABLE_SIGNED_CHAR        (signed char exists)
#     ENABLE_LONG_LONG          (long long and unsigned long long exist)
#     ENABLE_LONG_DOUBLE        (long double exists)
#     ENABLE_STDINT_H           (a conforming <stdint.h> exists)
#     ENABLE_STDBOOL_H          (a conforming <stdbool.h> exists)
# You can specify that such types *don't* exist by defining any of the
# following macros:
#     DISABLE_SIGNED_CHAR
#     DISABLE_LONG_LONG
#     DISABLE_LONG_DOUBLE
#     DISABLE_STDINT_H
#     DISABLE_STDBOOL_H
#
# Update the definition of "CC" to set any of these macros.
# For many C compilers, the "-D" option can be used for this purpose, e.g.:
#     -DENABLE_STDBOOL_H -DENABLE_LONG_LONG
# For others, consult your compiler's documentation.

CC=gcc -std=c11 -pedantic
# For gcc with the C90 standard:
# CC=gcc -std=c90 -pedantic
# For gcc with the C11 standard:
# CC=gcc -std=c11 -pedantic
# For Solaris with gcc:
# CC=gcc -std=c99 -pedantic -m32
# CC=gcc -std=c99 -pedantic -m64
# For Solaris with cc:
# CC=cc -Xc -xc99=%all,no%lib -m32 -DENABLE_LONG_LONG
# CC=cc -Xc -xc99=%all,no%lib -m64 -DENABLE_LONG_LONG

# CC_VERSION is a command that prints the name and version of the
# compiler, preferably on one line.
CC_VERSION=$(CC) --version | head -n 1
# For Solaris with cc:
# CC_VERSION=cc -V 2>&1 | head -n 1

# "make" with no arguments creates the executable and runs it, creating
# "`./result-file-name`.ini", for example, "i686-pc-linux-gnu.ini"

OUTPUT=`./result-file-name`

output:         $(OUTPUT)

show_c_types:	show_c_types.o
	$(CC) show_c_types.o -o show_c_types

show_c_types.o:	show_c_types.c
	$(CC) -c show_c_types.c

$(OUTPUT):   show_c_types
	@echo "Creating $(OUTPUT)"
	./show_c_types config.guess="`./config.guess`" compiler="`$(CC_VERSION)`" compile_command="$(CC)" > $(OUTPUT)

test:           $(OUTPUT)
	./test.pl $(OUTPUT)

clean:
	rm -f show_c_types show_c_types.o $(OUTPUT)
