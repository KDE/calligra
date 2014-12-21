#!/bin/sh
# Shows services that have the same name as other services

function desktop_files()
{
    find . -name \*.desktop
}

function duplicates()
{
    for f in $(desktop_files) ; do
        grep "^Type=Service\$" $f -q && a=`grep ^Name= $f` && echo $a
    done | sort | uniq -d | while read a ; do echo -n "^$a\$|" ; done
}

filter=$(duplicates)__dummy__
egrep "$filter" $(desktop_files)
