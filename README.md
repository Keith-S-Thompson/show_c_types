Copyright (C) 2023 Keith Thompson

`show_c_types` is released under GPL version 2 or later.  See the
header comments in `show_c_types.c` and `Makefile` and the file
`COPYING`.

This program uses a collection of very ugly macros to determine the
characteristics of predefined C types.  The results are printed to
stdout in "ini" format, described in <https://en.wikipedia.org/wiki/INI_file>.

Earlier versions of this program printed similar information in plain text
(not released), and then in something close to JSON.

You may need to modify the `Makefile` to adjust the compiler command
as appropriate for your system, or you may need to build and execute
the program manually.  See the header comment block in the `Makefile`
for details.

To build:

    make show_c_types

To build and run:

    make

or

    make output

This should generate a new file with a `.ini` suffix.
(Run `make clean` first if necessary, e.g., if you've edited the `Makefile`.)

If you run the `show_c_types` program manually, it will write
its results to standard output.  The program accepts command-line
arguments of the form `key=value`, and will record those arguments
in the `"configuration"` node (see below).

The contents of the ini output are intended to be human-readable and
self-explanatory, but they're subject to change in future releases.
The output consists of a sequence of sections.

The "configuration" record shows the version number of this program,
currently `"2023-11-10"`.

The `results_0` directory (previously named `old-results`) contains
results from an older version of this program that printed results
in plain text.

`results_1` contains results from a later version that generated
pseudo-JSON output.

`results` contains results from the current version.

If you've run this on a system for which I don't already have results
in the `results` directory, please e-mail the generated `*.ini`
file as an attachment to the author.

I'll add more output files as I'm able to gather the information,
including manually converting the plain text and JSON output from
the older version of this program.

`dump_float.c` dumps selected floating-point values in hexadecimal;
this is useful for updating the `floating_looks_like()` function in
`show_c_types.c`.

Home page: https://github.com/Keith-S-Thompson/show_c_types

Author: Keith Thompson <Keith.S.Thompson@gmail.com>
