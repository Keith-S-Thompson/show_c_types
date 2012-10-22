Copyright (C) 2012 Keith Thompson

`show_c_types` is released under GPL version 2 or later.  See the
header comments in `show_c_types.c` and `Makefile` and the file
`COPYING`.

This program uses a collection of very ugly macros to determine the
characteristics of predefined C types.  The results are printed to
stdout in [JSON](http://www.json.org/) format.

It's based on an earlier unreleased program that prints similar
information in plain text.

You may need to modify the `Makefile` to adjust the compiler command
as appropriate for your system.  See the header comment block there
for details.

To build:

    make show_c_types

To build and run:

    make

or

    make output

This should generate a new file with a `.json` suffix.  (Run `make
clean` first if necessary, e.g., if you've edited the `Makefile`.)
The contents of the output file are subject to change in future
releases.  The configuration record shows the version number of this
program.

If you've run this on a system for which I don't already have results
in the `results` directory, please e-mail the generated `*.json`
file as an attachment to the author.

I'll add more output files as I'm able to gather the information,
including manually converting the plain text output from the older
version of this program.

Home page: https://github.com/Keith-S-Thompson/show_c_types

Author: Keith Thompson <Keith.S.Thompson@gmail.com>
