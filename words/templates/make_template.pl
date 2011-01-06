#!/usr/bin/perl -w

# This script takes a KWord XML file as entry (as saved by KWord)
# and generates a KWord XML file that is ready to be used as a template.
# Usually one will run unzip first, to extract maindoc.xml from the document,
# and use that as the <inputFile>.
# David Faure <faure@kde.org>

die "Usage: $0 inputFile outputFile.kwt" unless ($#ARGV == 1);
open(IN, "<$ARGV[0]") or die "Can't open $ARGV[0] for reading";
open(OUT, ">$ARGV[1]") or die "Can't open $ARGV[1] for reading";

my $inStyle = 0;
my $styleName = '';
while(<IN>)
{
    $inStyle = 1 if ( /<STYLE/ );
    $inStyle = 0 if ( /<\/STYLE/ );
    $styleName = $1 if ( $inStyle && /<NAME value=\"(.*)\"/ );

    my $ok = 1;
    # FONT name must be taken from the KDE standard font -> removing
    # align=left must be removed so that auto is used instead, for RTL users
    $ok = 0 if ( /FONT name/ || /FLOW align=\"left\"/ );

    # Remove font size from Standard style, comes from KDE standard font
    $ok = 0 if ( $inStyle && ($styleName eq 'Standard') && /<SIZE value=\"12\"/ );

    if ( $ok ) {
        print OUT $_;
    }
}

close IN;
close OUT;
