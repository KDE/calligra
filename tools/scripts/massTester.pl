#! /usr/bin/perl -w
use strict;

# This script runs a commnand on a list of input arguments that are supplied on
# the standard input.
# Graphical output is handled in a Xephyr window.
# Output of the command is given in a file results.txt
# If the argument list contains 'FILE' the argument will be put there instead of
# at the end.

# Examples:
# find -name '*.odt' | \
#     massTester.pl --withgui --timeout 3 words --export-pdf --export-filename dummy.pdf
#
# find -name '*.odt' | \
#     massTester.pl --withgui --timeout 3 koconverter --batch FILE out.odt

# Author: Jos van den Oever

my $timeout = 3; # seconds
my $withgui = 0;
my $DISPLAY = ":2";
my $outputfile = "results.txt";

# run the given command and return the exit status
# the first argument is the timeout in seconds
# the rest of the arguments form the command
sub testCommand {
	my $timeout = shift;
	my @command = @_;
	my $pid = fork;
	my $result = "undefined";
	eval {
		local $SIG{ALRM} = sub { die "alarm\n" };
		alarm $timeout;

		if (!$pid) {
			exec @command;
			exit;
		}
		waitpid $pid, 0;
		$result = $?;
		alarm 0;
	};
	if ($@ eq "alarm\n") {
		kill 9, $pid;
		$result = "timeout";
	}
	return $result;
}

sub makeCommand {
	my @command;
	my $file = shift;
	my $foundFILE = 0;
	foreach (@_) {
		if ($_ eq "FILE") {
			$foundFILE = 1;
			push @command, $file;
		} else {
			push @command, $_;
		}
	}
	if ($foundFILE == 0) {
		push @command, $file;
	}
	return @command;
}

# read the value for the current argument in the input array
sub readArgumentValue {
	my $argv = shift;
	my $name = @$argv[0];
	die "Provide value for $name.\n" unless defined @$argv[1];
	shift @$argv;
	return @$argv[0];
}

# parse arguments
while ($ARGV[0] && $ARGV[0] =~ m/^--/) {
	if ($ARGV[0] eq "--timeout") {
		$timeout = readArgumentValue \@ARGV;
	} elsif ($ARGV[0] eq "--withgui") {
		$withgui = 1;
	} else {
		die "Unknown argument $ARGV[0].\n";
	}
	shift;
}
die "No command supplied.\n" unless $#ARGV >= 0;

my @command = @ARGV;

# start Xephyr
my $xephyrpid;
if ($withgui == 1) {
	if (!($xephyrpid = fork)) {
		exec "Xephyr", "-noreset", $DISPLAY;
		exit;
	}
}
$ENV{'DISPLAY'} = $DISPLAY;
$ENV{'KDE_DEBUG'} = "0";
my @files = <STDIN>;

my %result;
foreach (@files) {
	my $file = $_;
	chomp $file;
	my @cmd = makeCommand $file, @command;
	$result{$file} = testCommand $timeout, @cmd;
}

open FH, "> $outputfile";
foreach (keys %result) {
	print FH $_."\t".$result{$_}."\n";
}
close FH;

# close Xephyr
if ($withgui == 1) {
	kill 15, $xephyrpid;
	waitpid $xephyrpid, 0;
}
