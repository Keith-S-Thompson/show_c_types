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
# If you want to add more information to the results.json file,
# add arguments to the "./show_c_types" command line.

########################################################################

# By default, we assume that the settings of certain predefined macros
# (__STDC_VERSION__, etc.) determine which types are supported.  This assumption
# is not always reliable; for example, some old compilers like VAXC don't
# support signed char; others do support long long without claiming C99 conformance.
#
# You can override the default assumptions by defining any of the following
# macros.  (The code only checks whether they're defined or not; their
# values are ignored.)
#     ENABLE_SIGNED_CHAR        (signed char exists
#     ENABLE_LONG_LONG          (long long and unsigned long long exist)
#     ENABLE_LONG_DOUBLE        (long double exists)
#     ENABLE_INTMAX_T           (<stdint.h> exists and defined intmax_t and uintmax_t)
#     ENABLE_BOOL               (<stdbool.h> exists and defines bool)
# You can specify that such types *don't* exist by defining any of the
# following macros:
#     DISABLE_SIGNED_CHAR
#     DISABLE_LONG_LONG
#     DISABLE_LONG_DOUBLE
#     DISABLE_INTMAX_T
#     DISABLE_BOOL
#
# By default, we assume that time_t and clock_t are integer types.
# Define either or both of the following macros to indicate that time_t
# and/or clock_t are floating-point types:
#     FLOATING_TIME_T           (time_t is a floating-point type)
#     FLOATING_CLOCK_T          (clock_t is a floating-point type)
#
# Update the definition of "CC" to set any of these macros.
# For many C compilers, the "-D" option can be used for this purpose, e.g.:
#     -DENABLE_BOOL _DENABLE_LONG_LONG
# For others, consult your compiler's documentation.

CC=gcc -std=c99

# CC_VERSION is a command that prints the name and version of the
# compiler, preferably on one line.
CC_VERSION=$(CC) --version | head -n 1

# "make" with no arguments creates the executable and runs it, creating "results.json".

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
