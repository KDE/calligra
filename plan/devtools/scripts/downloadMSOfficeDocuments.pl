#! /usr/bin/env perl
use warnings;
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
	"pdf", "application/pdf",
        "pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation" ,
        "xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet",
	"docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document",
	"doc", "application/msword",
	"xls", "application/vnd.ms-excel",
	"rtf", "application/rtf",
        "ods", "application/vnd.oasis.opendocument.spreadsheet",
        "odt", "application/vnd.oasis.opendocument.text",
        "odp", "application/vnd.oasis.opendocument.presentation"
);

if (!defined $mimetypes{$type}) {
	die "Unknown type '$type'.\n";
}
my $mimetype = $mimetypes{$type};

#used to dispatch web requests
my $ua = LWP::UserAgent->new;
$ua->timeout(10); # seconds
$ua->env_proxy;
my $agentstring = 'Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)';
$ua->agent($agentstring);
my @pages;
sub callback {
	my($tag, %attr) = @_;
	# we only look closer at <a ...>
	return if $tag ne 'a';
	push(@pages, values %attr);
}
#extracts links from an HTML document
my $p = HTML::LinkExtor->new(\&callback);

my $runningjobs = 0;
sub startJob {

#Shifts the first value of the array off and returns it, shortening the array by 1 and moving everything down. 
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
		$localua->env_proxy;
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
#Shifts the first value of the array off and returns it, shortening the array by 1 and moving everything down. 
	my $uri = shift;
	my $scheme = $uri->scheme;
	if (exists $done{$uri} || ($scheme ne "http" && $scheme ne "https")) {
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
	my $base = "http://www.google.com/";
	my $url = $base . "search?q=$term+filetype:$type&start=$start&num=100";
	my $res = $ua->request(HTTP::Request->new(GET => $url), sub {$p->parse($_[0])});
	foreach (@pages) {
		my $uri = URI->new_abs($_, $base);
		if ($uri->host =~ m/google/) {
			my @q = $uri->query_form;
			if (!@q) {
				next;
			}
			for (my $i = 0; $i <= @q; $i++) {
				$uri = URI->new_abs($q[$i], $base);
				if (($uri->scheme eq "http" ||
						$uri->scheme eq "https") &&
						$uri->host !~ m/google/) {
					addJob($uri);
				}
			}
		} else {
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
