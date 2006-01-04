#!/bin/sh

for i in *.h *.c ; do cat $i | grep -v "\$Id" > $i.tmp && mv $i.tmp $i ; done
