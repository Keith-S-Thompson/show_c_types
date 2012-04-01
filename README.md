`show_c_types` is released under GPL version 2 or later.  See the
header comment in `show_c_types.c` and the file `COPYING`.

This program uses a collection of very ugly macros to determine the
characteristics of predefined C types.  The results are printed to
stdout in [JSON](http://www.json.org/) format.

It's based on an earlier unreleased program that prints similar
information in plain text.

You may need to modify the `Makefile` to adjust the compiler command
as appropriate for your system.

To build:

    make show_c_types

To build and run:

    make

or

    make output

This should generate a new file `output.json`.

If you've run this on a system for which I don't already have results
in the `results` directory, please e-mail your `output.json` file as
an attachment to the author.

I'll add more output files as I'm able to gather the information,
including manually converting the plain text output from the older
version of this program.

Home page: https://github.com/Keith-S-Thompson/show_c_types

Author: Keith Thompson <Keith.S.Thompson@gmail.com>
