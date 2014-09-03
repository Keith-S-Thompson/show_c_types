Copyright (C) 2014 Keith Thompson

`show_c_types` is released under GPL version 2 or later.  See the
header comments in `show_c_types.c` and `Makefile` and the file
`COPYING`.

This program uses a collection of very ugly macros to determine the
characteristics of predefined C types.  The results are printed to
stdout in something closely resembling [JSON](http://www.json.org/) format.

JSON doesn't permit a comma on the last element of a list.  To make
the logic simpler, this program prints final commas.  The output
can be translated to valid JSON using the included `fix-json.pl`
script (which is specialized for this program's output).

It's based on an earlier unreleased program that prints similar
information in plain text.

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

This should generate a new file with a `.json` suffix.  (Run `make
clean` first if necessary, e.g., if you've edited the `Makefile`.)

If you run the `show_c_types` program manually, it will write
its results to standard output.  The program accepts command-line
arguments of the form `key=value`, and will record those arguments
in the `"configuration"` node (see below).

The contents of the json output are intended to be human-readable and
self-explanatory, but they're subject to change in future releases.
The output consists of a single array, each element of which is a
single object with no nested sub-objects.  Each element contains
exactly one member with key "node_kind"; the value is currently one
of the following:

- `"comment"`
- `"configuration"`
- `"integer_type"`
- `"floating_type"`
- `"type"`
- `"header"`
- `"predefined_macros"`

The "configuration" record shows the version number of this program,
currently `"2014-09-02"`.

Currently, the `*.json` files in the `results/` directory are from
an earlier version of this program. I'll update them [Real Soon
Now](http://www.jargon.net/jargonfile/r/RealSoonNow.html).

If you've run this on a system for which I don't already have results
in the `results` directory, please e-mail the generated `*.json`
file as an attachment to the author.

I'll add more output files as I'm able to gather the information,
including manually converting the plain text output from the older
version of this program.

`dump_float.c` dumps selected floating-point values in hexadecimal;
this is useful for updating the `floating_looks_like()` function in
`show_c_types.c`.

Home page: https://github.com/Keith-S-Thompson/show_c_types

Author: Keith Thompson <Keith.S.Thompson@gmail.com>
