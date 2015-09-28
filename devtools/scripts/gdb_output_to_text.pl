#!/usr/bin/perl
while (<>) {
  if ( m/.* = [-\d]+ '(.+)'/ ) {
    my $a = $1;
    eval( "\$a=\"$a\"" );
    print $a;
  } elsif ( m/.* = -1/ ) {
    print "\n";
  }
}
