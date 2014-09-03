#!/usr/bin/perl

# Input: The output of the show_c_types program, which is valid JSON
# except for the presence of trailing commas.
#
# Output: Valid JSON with the trailing commas removed.

# This is not a general-purpose JSON sanitizer.  It's specialized to
# handle the output of show_c_types.

use strict;
use warnings;

my @pending = ();
while (<>) {
    push @pending, $_;
    if (/^\[$/) {
        push @pending, qq(    {\n);
        push @pending, qq(        "node_kind" : "comment",\n);
        push @pending, qq(        "comment" : "Postprocessed by fix-json.pl"\n);
        push @pending, qq(    },\n);
    }
    if (scalar @pending >= 2) {
        if ( /},?$/ or /\]$/) {
            $pending[-2] =~ s/,$//;
        }
    }
    while (scalar @pending >= 2) {
        print shift @pending;
    }
}
print @pending;
