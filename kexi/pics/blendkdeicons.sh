#!/bin/sh
#
# Generates series of blended icons
# Uses ImageMagick's "composite" tool
#
# Copyright (C) 2004-2013 Jarosław Staniek <staniek@kde.org>
#

usage() {
	echo "USAGE: $0 <modifier_icon> <suffix> <icon> <icon> ..
example: $0 action-new_sign _newobj action_table action_query
will result with:
hi16-action-table.png blended using hi16-action-new_sign.png
  and saved to hi16-action-table_newobj.png,
hi22-action-table.png blended using hi22-action-new_sign.png
  and saved to hi22-action-table_newobj.png,
hi16-action-query.png blended using hi16-action-new_sign.png
  and saved to hi16-action-query_newobj.png,
etc..."
}

if [ $# -lt 3 ] ; then usage; exit 1; fi

mod=$1
shift
suffix=$1
shift

if [ -z "$mod" -o -z "$suffix" ] ; then
	usage
	exit 1
fi

icon=$1
while [ -n "$1" ] ; do
	for size in 16 22 32 ; do
		mod_file="hi"$size"-"$mod".png"
		if [ -f "$mod_file" ] ; then
			for i in `ls "hi"$size"-"$icon".png" 2> /dev/null` ; do
				composite "$mod_file" "$i" `echo $i | sed "s/\.png/"$suffix".png/"`
			done
		fi
	done
	shift
	icon=$1
done

