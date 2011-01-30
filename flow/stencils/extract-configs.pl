#!/bin/perl

use strict;
use Archive::Zip;
use XML::Simple;
use IO::File;
use Carp;

my $dirname;

#
#   Check for one argument: the name of the OpenOffice.org document
#
if (scalar @ARGV != 1) {
	croak("Usage: $0 document");
}

#
#   Extract the document into xml files
#
print "Extracting $ARGV[0]...";
($dirname) = $ARGV[0] =~ m/(.+)\.odg/;
my $zip = new Archive::Zip($ARGV[0]);
if (!$zip) {
	croak("$ARGV[0] cannot be opened as a .ZIP file");
} else {
	$zip->extractTree(undef, "$dirname/");
}

#
#   Export the config file from the document
chdir $dirname;
my $fh = IO::File->new("content.xml");
my $xml = XMLin($fh);
my $root = $xml->{'office:drawing'};

if ($root) {
	print "XMLin opened\n";
}
if ($root->{'draw:g'}) {
	print "Test output:	$root->{'draw:g'}\n";
}
chdir "../";

