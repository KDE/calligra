#!/usr/local/bin/perl -w
# This script automatically builds all the tests, runs them, and compares the
# with the correct output

# This array holds the programs we should test.
@programs = ("fxtest");
$failed=0;
$passed=0;

print "Automatically testing the programs.\n";
print "Building the tests...\n";
!(system("make check &> /dev/null")!=0) or die "Building the tests failed! Aborting.\n";
print "Build finished.\n\n";

foreach $p (@programs) {
    print "Testing: ", $p;
    !(system("./$p &> $p.out")) or die "Failed executing $p. Aborting.\n";
    if(system("diff --brief --ignore-matching-lines=^### $p.out $p.original &> /dev/null")) {
        print "   FAILED\n";
        $failed=$failed+1;
    }
    else {
        print "   OK\n";
        $passed=$passed+1;
    }
}

print "Tests Passed: $passed, Tests Failed: $failed\n";
print "Done.\n";
