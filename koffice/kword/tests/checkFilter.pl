#!/usr/bin/perl

foreach $in (<STDIN>) {
    if($in=~/^# Running normal tests... #$/) { $testsStarted=1; }
    elsif($testsStarted != 1 || $in=~/^$/) { print "$in"; }
    elsif($in=~/^check: (.*?)\[(\d+)\](| failed)$/) {
#print "++ $_ a: '$1', b: '$2'\n";
        $file = $1;
        $line = $2;
        $failed = $3;
        if($file ne $currentFile) {
            print "$file: ";
            $currentFile = $file;
        }
        if($lines ne "") {
            if($lines=~/\n$/) {
                $lines = "$lines...";
            } else {
                $lines = "$lines, ";
            }
        }
        if($failed ne "") { # it failed!
            $lines = "$lines\[F:$line\]";
        } else {
            $lines = "$lines$line";
        }
    }
    elsif($in=~/ ((\d+) test(|s) passed, (\d+) test(|s) failed)/) {
#print "passed: '$2' failed: '$4'\n";
        if($4 ne "0") {
            print "$lines\n";
        }
        print "$1\n";
        $lines = "";
        $currentFile = "";
    }
    elsif($currentFile eq "") {
        print $in;
    } else {
        # debug output in between lines..
        if(!($lines eq "" || $lines=~/\n$/)) {
            $lines = "$lines\n";
        }
        $lines = "$lines$in";
    }
}

if($lines ne "") {
    print "$lines\n";
    print $in;
    print "WARN: unexpected end of stream, check test $file after line $line\n";
}
