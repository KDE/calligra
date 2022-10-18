#!/usr/bin/perl
#
# Script per a processar entre formats les entrades del llistat de barbarismes de l'Abiword
# <http://www.abisource.com/lxr/source/abispell/barbarisms/ca-ES-barbarism.xml>
#
# Copyright (c) 2003 by Antoni Bella Perez <bella5@teleline.es>.
#
#       This program is free software; you can redistribute it and or
#       modify it under the terms of the GNU General Public License as
#       published by the Free Software Foundation; either version 2 of
#       the License, or (at your option) any later version.
#
#       This program is distributed in the hope that it will be useful, but
#       WITHOUT ANY WARRANTY; without even the implied warranty of
#       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#       General Public License for more details.
#
#       You should have received a copy of the GNU General Public License along
#       with this program; if not, write to the Free Software Foundation,
#       Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#

open FILE_IN, "ca-ES-barbarism.xml" || die "Error: no es pot obrir el fitxer per a lectura!";
open FILE_OUT, ">ca_ES.tmp";

while($linia = <FILE_IN>) {
    chop $linia;
    my ($barbarism) = ($linia =~ m,barbarism word=(.*?)&gt,ms);
    my ($suggestion) = ($linia =~ m,suggestion word=(.*?)/&gt,ms);
    if ( $barbarism ) {
        print FILE_OUT "  <item find=$barbarism ";
    }
    if ( $suggestion ) {
        print FILE_OUT "replace=$suggestion />\n";
    }
}

close FILE_IN;
close FILE_OUT;

qx(cat ca_ES.tmp | sort -u > ca_ES+`date +%d-%m-%Y`.txt; rm ca_ES.tmp);
