#!/usr/bin/perl -w

use strict;

# A small script to convert current-style KOffice tar storages to storages
# compatible with KOffice 1.0 and KOffice 1.1(.1)

# Holds the directory tree
my @rootdir;
my $tmpdir = "/tmp/kofficeconverter";
# Holds the soucre/dest of the files to fix
my @needFixing;

# Walk the whole archive and collect information about the files
# This creates one array, containing another array for every directory
# we found (recursively).
sub explore {
  my($path) = @_;
  my(@dir);

  print "   Exploring: $path\n";
  chdir($path);
  opendir(DIR, $path) || die "Couldn't open the directory: $!";
  my @contents = readdir(DIR);
  my $i = 0;
  foreach(@contents) {
    if($_ eq "." || $_ eq "..") {
      next;      # we're not intersted in . and ..
    }
    if(-d $_) {
      $dir[$i] = [ $_, $path, [ explore($path . '/' . $_) ] ];
      chdir($path);    # back to the directory where we come from
    }
    else {
      $dir[$i] = $_;
    }
    $i = $i + 1;
  }
  closedir(DIR);
  return @dir;
}

# Dumps the scary datastructure we built
sub dumpTree {
  my(@dir) = @_;
  foreach(@dir) {
    if(ref($_) eq 'ARRAY') {
      print $_->[0], "   (", $_->[1], ")\n";
      dumpTree(@{$_->[2]});
    }
    else {
      print $_ . "\n";
    }
  }
}

# Finds the files we have to fix
sub findCandidates {
  my($dref, $currentdir, $parentdir) = @_;
  my @dir = @{$dref};
  #print "current: $currentdir, parentdir: $parentdir\n";
  foreach(@dir) {
    if(ref($_) eq 'ARRAY') {
      #print $_->[0], "   (", $_->[1], ")\n";
      findCandidates(\@{$_->[2]}, $_->[0], $_->[1]);
    }
    else {
      if($_ =~ m/maindoc\.xml/) {
	my $source = $parentdir . '/' . $currentdir . "/maindoc.xml";
	my $dest = $parentdir . '/' . $currentdir . ".xml";
	push(@needFixing, [ $source, $dest ]);
      }
    }
  }
}

# No need to move around elements of the root directory
sub findMainDocuments {
  foreach(@rootdir) {
    if(ref($_) eq 'ARRAY') {
      findCandidates(\@{$_->[2]}, $_->[0], $_->[1]);
    }
  }
}

# Walks through all the documents and fixes links
sub fixLinks {
  for my $item (@needFixing) {
    my $prefix = substr $item->[0], length($tmpdir)+1;
    $prefix =~ m,^(.*?)(maindoc\.xml),;
    $prefix = "tar:/" . $1;
    open(SOURCE, "<$item->[0]") || die "Couldn't open the source file: $!\n";
    open(DEST, ">$item->[1]") || die "Couldn't open the destination file: $!\n";
    while(<SOURCE>) {
      if(m/(\s*\<object\s+mime=\"[^\"]*\"\s+url=\")([^\"]*)(\".*)/) {
	print DEST $1, $prefix, $2, $3, "\n";
	next;
      }
      elsif(m/(\s*\<OBJECT\s+mime=\"[^\"]*\"\s+url=\")([^\"]*)(\".*)/) {
	print DEST $1, $prefix, $2, $3, "\n";
	next;
      }
      print DEST $_;
    }
    close(SOURCE);
    close(DEST);
  }
}

# Get rid of the moved files
sub removeOldFiles {
  foreach(@needFixing) {
    system("rm -rf $_->[0]");
  }
}

# Special case for the main document as we have to use a temporary
# file and stuff like that. We only have to fix part references here.
sub fixMainDocument {
  open(SOURCE, "<$tmpdir/maindoc.xml");
  open(DEST, ">$tmpdir/tmp.xml");
  while(<SOURCE>) {
    if(m/(\s*\<object\s+mime=\"[^\"]*\"\s+url=\")([^\"]*)(\".*)/) {
      print DEST $1, "tar:/", $2, $3, "\n";
      next;
    }
    elsif(m/(\s*\<OBJECT\s+mime=\"[^\"]*\"\s+url=\")([^\"]*)(\".*)/) {
      print DEST $1, "tar:/", $2, $3, "\n";
      next;
    }
    print DEST $_;
  }
  close(SOURCE);
  close(DEST);
  system("mv $tmpdir/tmp.xml $tmpdir/maindoc.xml");
}

##################################################
# The execution starts here
##################################################
if($#ARGV != 1) {
    print "Script to convert current storages to KOffice 1.0 compatible ones.\n";
    print "Usage: perl storage.pl <inputfile> <outputfile>\n";
    exit(1);
}

# remember where we came from
chomp(my $cwd = `pwd`);

# clean up properly
system("rm -rf $tmpdir");
mkdir $tmpdir || die "Couldn't create tmp directory: $!\n";

print "Uncompressing the archive...\n";
system("tar -C $tmpdir -xzf $ARGV[0]");

print "Browsing the directory structure...\n";
@rootdir = explore($tmpdir);

# debugging
#dumpTree(@rootdir);

print "Find candidates for moving...\n";
findMainDocuments();

print "Moving and fixing relative links...\n";
fixLinks();
removeOldFiles();
fixMainDocument();

print "Creating the archive...\n";
chdir($tmpdir);
system("tar czf tmp.tgz *");
system("mv tmp.tgz $cwd/$ARGV[1]");

print "Cleaning up...\n";
# clean up properly
system("rm -rf $tmpdir");

print "Done.\n";
