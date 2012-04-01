#!/usr/bin/perl

use strict;
use warnings;

use JSON ();
use Data::Dumper ();

open my $JSON, '-|', './show_c_types';
my $json = join('', <$JSON>);
close $JSON;

my $data = JSON::decode_json($json);

print Data::Dumper::Dumper($data);
