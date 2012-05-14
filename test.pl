#!/usr/bin/perl

use strict;
use warnings;

#
# With no arguments, invoke ./show_c_types and parse the output as JSON.
# With one argument, open the specified file and parse as JSON.
# Prints "ok" if the output is syntactically valid JSON.
# This does not verify the content.
# Requires the Perl JSON module.
#
# An initial "-dump" argument invokes Data::Dumper.
#

use JSON ();
use Data::Dumper ();

my $Dump = 0;
if (scalar @ARGV > 0 and $ARGV[0] eq '-dump') {
    $Dump = 1;
    shift;
}

my $JSON;
if (scalar @ARGV == 0) {
    open $JSON, '-|', './show_c_types' or die "./show_c_types: $!\n";
}
else {
    open $JSON, '<', $ARGV[0] or die "$ARGV[0]: $!\n";
}
my $json = join('', <$JSON>);
close $JSON;

my $data = JSON::decode_json($json);

if ($Dump) {
    print Data::Dumper::Dumper($data);
}

print "ok\n";

