#! /bin/sh
#
# A shell script to post-process doxy-generated files; the purpose
# is to make the menu on the left in the file match the actually
# generated files (ie. leave out namespaces if there are none).
#
# Usage: doxyndex.sh <toplevel-apidocs-dir> <relative-html-output-directory>
#
# Typically, this means $(top_builddir)/apidocs and something like
# libfoo/html for the output. For the top-level dig, set relative-html
# to "." . In non-top directories, both <!-- menu --> and <!-- gmenu -->
# are calculated and replaced. Top directories get an empty <!-- menu -->
# if any.

WRKDIR="$1/$2"
TOPDIR=`echo "$2" | sed -e 's+[^/][^/]*/+../+g' -e 's+html$+..+'`
echo "Postprocessing files in $WRKDIR ($TOPDIR)"

# Special case top-level to have an empty MENU.
if test "x$2" = "x." ; then
MENU=""
else
MENU="<ul>"

# This is a list of pairs, with / separators so we can use basename
# and dirname (a crude shell hack) to split them into parts. For
# each, if the file part exists (as a html file) tack it onto the
# MENU variable as a <li> with link.
for i in "Main Page/index" \
	"Modules/modules" \
	"Namespace List/namespaces" \
	"Class Hierarchy/hierarchy" \
	"Alphabetical List/classes" \
	"Class List/annotated" \
	"File List/files" \
	"Namespace Members/namespacemembers" \
	"Class Members/functions" \
	"Related Pages/pages"
do
	NAME=`dirname "$i"`
	FILE=`basename "$i"`
	test -f "$WRKDIR/$FILE.html" && MENU="$MENU<li><a href=\"$FILE.html\">$NAME</a></li>"
done

MENU="$MENU</ul>"
fi


# Get the list of global Menu entries.
GMENU=`cat "$1"/subdirs | tr -d '\n'`

PMENU=`grep '<!-- pmenu' "$WRKDIR/index.html" | sed -e 's+.*pmenu *++' -e 's+ *-->++' | awk '{ c=split($0,a,"/"); for (j=1; j<=c; j++) { printf " / <a href=\""; if (j==c) { printf("."); } for (k=j; k<c; k++) { printf "../"; } if (j<c) { printf("../html/index.html"); } printf "\">%s</a>\n" , a[j]; } }' | tr -d '\n'`

# Now substitute in the MENU in every file. This depends
# on HTML_HEADER (ie. header.html) containing the <!-- menu --> comment.  
for i in "$WRKDIR"/*.html 
do 
	sed -e "s+<!-- menu -->+$MENU+" -e "s+<!-- gmenu -->+$GMENU+" -e "s+<!-- pmenu.*-->+$PMENU+" < "$i"  | sed -e "s+@topdir@+$TOPDIR+g" > "$i.new" && mv "$i.new" "$i" 
done

