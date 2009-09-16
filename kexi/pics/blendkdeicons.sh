#!/bin/sh
#
# Generates series of blended icons
#
# Copyright (C) 2004 Jarosław Staniek <staniek@kde.org>
#

usage() {
	echo "USAGE: $0 <modifier_icon> <suffix> <icon> <icon> ..
example: $0 action-new_sign _newobj action_table action_query
will result with:
cr16-action-table.png blended using cr16-action-new_sign.png
  and saved to cr16-action-table_newobj.png,
cr22-action-table.png blended using cr22-action-new_sign.png
  and saved to cr22-action-table_newobj.png,
cr16-action-query.png blended using cr16-action-new_sign.png
  and saved to cr16-action-query_newobj.png,
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
		mod_file="cr"$size"-"$mod".png"
		if [ -f "$mod_file" ] ; then
			for i in `ls "cr"$size"-"$icon".png" 2> /dev/null` ; do
				blendicons "$i" "$mod_file" `echo $i | sed "s/\.png/"$suffix".png/"`
			done
		fi
	done
	shift
	icon=$1
done

