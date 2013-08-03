#!/usr/bin/perl

if ( @ARGV != 1 ) {
  print STDERR "Missing arg: filename\n";
  exit 1;
}

$file = $ARGV[0];

$file =~ /^(.*)\.[^\.]*$/;
$filebase = $1;

$file_h = "$filebase.h";
$file_cpp = "$filebase.cpp";

$kcfgc = $file . "c";

$cmd = "./kconfig_compiler $file $kcfgc";

#print "CMD $cmd\n";

if ( system( $cmd ) != 0 ) {
  print STDERR "Unable to run kconfig_compiler\n";
  exit 1;
}

checkfile( $file_h );
checkfile( $file_cpp );

exit 0;

sub checkfile()
{
  my $file = shift;

  $file =~ /\/([^\/]*)$/;
  my $filename = $1;

  print "Checking '$filename':\n";

  my @ref;
  if ( !open( REF, "$file.ref" ) ) {
    print STDERR "Unable to open $file.ref\n";
    exit 1;
  }
  while( <REF> ) {
    push @ref, $_;
  }
  close REF;

  if ( !open( READ, $filename ) ) {
    print STDERR "Unable to open $filename\n";
    exit 1;
  }

  $error = 0;
  $i = 0;
  $line = 1;
  while( <READ> ) {
    $out = $_;
    $ref = @ref[$i++];

    if ( $out ne $ref ) {
      $error++;
      print "  Line $line: Expected        : $ref";
      print "  Line $line: Compiler output : $out";
    }
    
    $line++;
  }

  close READ;

  if ( $error > 0 ) {
    print "\n  FAILED: $error errors found.\n";
    if ( $error > 5 ) {
      system( "diff -u $file.ref $filename" ); 
    }
    exit 1;
  } else {
    print "  OK\n";
  }
}
