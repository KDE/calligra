#! /bin/env perl -w
use strict;
use File::Path;
use LWP::UserAgent;
use HTML::LinkExtor;
use URI;
use URI::URL;
use URI::Escape;

# This script downloads MS Office files for you. You should provide a search
# term which is used to get a list of document.
# To download 20 presentations about pears do:
#    downloadMSOfficeDocuments.pl 20 pear ppt
#
# Copyright 2009 Jos van den Oever <jos@vandenoever.info>

if ($#ARGV != 2 || $ARGV[0] !~ m/^\d+$/ || $ARGV[1] !~ m/^\w+$/) {
	die "Usage: $0 number term type\n";
}

my $maxresults = $ARGV[0];
my $term = $ARGV[1];
my $type = $ARGV[2];
my $maxjobs = 10;

my %mimetypes = (
	"ppt", "application/vnd.ms-powerpoint",
	"doc", "application/msword",
	"xls", "application/vnd.ms-excel",
	"rtf", "application/rtf"
);

if (!defined $mimetypes{$type}) {
	die "Unknown type '$type'.\n";
}
my $mimetype = $mimetypes{$type};

my $ua = LWP::UserAgent->new;
$ua->timeout(10); # seconds
my $agentstring = 'Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)';
$ua->agent($agentstring);
my @pages;
sub callback {
	my($tag, %attr) = @_;
	# we only look closer at <a ...>
	return if $tag ne 'a';
	push(@pages, values %attr);
}
my $p = HTML::LinkExtor->new(\&callback);

my $runningjobs = 0;
sub startJob {
	my $uri = shift;
        if ($runningjobs >= $maxjobs) {
		wait;
		$runningjobs--;
 	}
	$runningjobs++;
	if (!fork()) {
		my $localuri = $uri;
		my $localua = LWP::UserAgent->new;
		$localua->timeout(10); # seconds
		$localua->agent($agentstring);
		my $res = $localua->request(HTTP::Request->new(HEAD => $localuri));
		if ($res->content_type() eq $mimetype) {
			my $filename = uri_unescape($localuri);
			$filename =~ s#^http://##;
			$filename = uri_escape($filename, '/:\!&*$?;:= \'"');
			print $localuri."\n";
			$ua->get($localuri, ':content_file'  => $filename);
		}
		exit;
 	}
}

my @jobs;
my %done;
sub addJob {
	my $uri = shift;
	if (exists $done{$uri}) {
		return;
	}
	$done{$uri} = 1;
        push @jobs, $uri;
        while (@jobs && $runningjobs < $maxjobs) {
		startJob(pop @jobs);
		#sleep 1;
	}
}

for (my $start=0; $start < $maxresults; $start = $start + 100) {
	if ($start > 0) {
		sleep 3; # do not query search engine too often
	}
	@pages = ();
	my $url = "http://www.google.com/search?q=$term+filetype:$type&start=$start&num=100";
	my $res = $ua->request(HTTP::Request->new(GET => $url), sub {$p->parse($_[0])});
	foreach (@pages) {
		my $uri = $_;
		if ($uri =~ m/^http/ && $uri !~ m/\.google\./) {
			addJob($uri);
		}
	}
}

# keep the queue filled
while (@jobs) {
	startJob(pop @jobs);
}

# wait for jobs to finish
my $pid;
do {
	$pid = wait;
} while ($pid != -1);
