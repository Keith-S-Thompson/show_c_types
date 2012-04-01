#!/usr/bin/perl

use strict;
use warnings;

#
# Invoke ./show_c_types and parse the output as JSON.
# Prints "ok" if the output is syntactically valid JSON.
# This does not verify the content.
# Requires the Perl JSON module.
#

use JSON ();
# use Data::Dumper ();

open my $JSON, '-|', './show_c_types';
my $json = join('', <$JSON>);
close $JSON;

my $data = JSON::decode_json($json);

# print Data::Dumper::Dumper($data);

print "ok\n";

