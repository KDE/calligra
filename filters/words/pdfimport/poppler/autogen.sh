#!/bin/sh

# Package name
PKG_NAME=${PKG_NAME:-Poppler}
srcdir=$(dirname $0)

# Default version requirements
REQUIRED_GTK_DOC_VERSION=${REQUIRED_GTK_DOC_VERSION:-1.0}
REQUIRED_AUTOMAKE_VERSION=${REQUIRED_AUTOMAKE_VERSION:-1.7}

case $REQUIRED_AUTOMAKE_VERSION in
    1.4*) automake_progs="automake-1.4" ;;
    1.5*) automake_progs="automake-1.11 automake-1.10 automake-1.9 automake-1.8 automake-1.7 automake-1.6 automake-1.5" ;;
    1.6*) automake_progs="automake-1.11 automake-1.10 automake-1.9 automake-1.8 automake-1.7 automake-1.6" ;;
    1.7*) automake_progs="automake-1.11 automake-1.10 automake-1.9 automake-1.8 automake-1.7" ;;
    1.8*) automake_progs="automake-1.11 automake-1.10 automake-1.9 automake-1.8" ;;
    1.9*) automake_progs="automake-1.11 automake-1.10 automake-1.9" ;;
    1.10*) automake_progs="automake-1.11 automake-1.10" ;;
    1.11*) automake_progs="automake-1.11" ;;
esac

# Print types
boldface="`tput bold 2>/dev/null`"
normal="`tput sgr0 2>/dev/null`"
printbold() {
    echo $ECHO_N "$boldface"
    echo "$@"
    echo $ECHO_N "$normal"
}
printboldn() {
    echo -n $ECHO_N "$boldface"
    echo "$@"
    echo -n $ECHO_N "$normal"
}
printerr() {
    echo "$@" >&2
}		

# Usage:
#     compare_versions MIN_VERSION ACTUAL_VERSION
# returns true if ACTUAL_VERSION >= MIN_VERSION
compare_versions() {
    ch_min_version=$1
    ch_actual_version=$2
    ch_status=0
    IFS="${IFS=         }"; ch_save_IFS="$IFS"; IFS="."
    set $ch_actual_version
    for ch_min in $ch_min_version; do
        ch_cur=`echo $1 | sed 's/[^0-9].*$//'`; shift # remove letter suffixes
        if [ -z "$ch_min" ]; then break; fi
        if [ -z "$ch_cur" ]; then ch_status=1; break; fi
        if [ $ch_cur -gt $ch_min ]; then break; fi
        if [ $ch_cur -lt $ch_min ]; then ch_status=1; break; fi
    done
    IFS="$ch_save_IFS"
    return $ch_status
}

# Usage:
#     version_check PACKAGE VARIABLE CHECKPROGS MIN_VERSION SOURCE
# checks to see if the package is available
version_check() {
    vc_package=$1
    vc_variable=$2
    vc_checkprogs=$3
    vc_min_version=$4
    vc_source=$5
    vc_status=1

    vc_checkprog=`eval echo "\\$$vc_variable"`
    if [ -n "$vc_checkprog" ]; then
        printbold "using $vc_checkprog for $vc_package"
        return 0
    fi

    printbold "Checking for $vc_package >= $vc_min_version..."
    for vc_checkprog in $vc_checkprogs; do
        echo -n $ECHO_N "  Testing $vc_checkprog... "
        if $vc_checkprog --version < /dev/null > /dev/null 2>&1; then
            vc_actual_version=`$vc_checkprog --version | head -n 1 | \
                               sed 's/^.*[      ]\([0-9.]*[a-z]*\).*$/\1/'`
            if compare_versions $vc_min_version $vc_actual_version; then
                echo "found $vc_actual_version"
                # set variable
                eval "$vc_variable=$vc_checkprog"
                vc_status=0
                break
            else
                echo "too old (found version $vc_actual_version)"
            fi
        else
            echo "not found."
        fi
    done
    if [ "$vc_status" != 0 ]; then
	printerr
	printboldn "  Error: You must have $vc_package >= $vc_min_version"
	printboldn "         installed to build $PKG_NAME"
	printerr
	printerr "  Download the appropriate package for"
	printerr "  from your distribution or get the source tarball at"
	printerr "  $vc_source"
	printerr
    fi
    return $vc_status
}

want_gtk_doc=false
if [ ! "`echo $@ |grep -- --enable-gtk-doc`" = "" ]; then
    version_check gtk-doc GTKDOCIZE gtkdocize $REQUIRED_GTK_DOC_VERSION \
"http://ftp.gnome.org/pub/GNOME/sources/gtk-doc/" || DIE=1
    want_gtk_doc=true
fi

version_check automake AUTOMAKE "$automake_progs" $REQUIRED_AUTOMAKE_VERSION \
"http://ftp.gnu.org/pub/gnu/automake/automake-$REQUIRED_AUTOMAKE_VERSION.tar.gz" || DIE=1

printbold "Running autoreconf -v -i ..."
(cd $srcdir && autoreconf -v -i )

if $want_gtk_doc; then
    printbold "Running $GTKDOCIZE..."
    (cd $srcdir && $GTKDOCIZE --copy) || exit 1
fi

if test x$NOCONFIGURE = x; then
    printbold "Running $srcdir/configure $@ ..."
    $srcdir/configure $@
else
    printbold "Skipping configure process."
fi

