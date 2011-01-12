#!/bin/sh

prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=${prefix}
exec_prefix_set=no

usage()
{
	cat <<EOF
Usage: wv2-config [OPTIONS] [LIBRARIES]
Options:
	[--prefix[=DIR]]
	[--exec-prefix[=DIR]]
	[--version]
	[--libs]
	[--cflags]
Libraries/Headers:
	wv2
EOF
	exit $1
}

if test $# -eq 0; then
	usage 1 1>&2
fi

lib_wv2=yes

while test $# -gt 0; do
  case "$1" in
  -*=*) optarg=`echo "$1" | sed 's/[-_a-zA-Z0-9]*=//'` ;;
  *) optarg= ;;
  esac

  case $1 in
    --prefix=*)
      prefix=$optarg
      if test $exec_prefix_set = no ; then
        exec_prefix=$optarg
      fi
      ;;
    --prefix)
      echo_prefix=yes
      ;;
    --exec-prefix=*)
      exec_prefix=$optarg
      exec_prefix_set=yes
      ;;
    --exec-prefix)
      echo_exec_prefix=yes
      ;;
    --version)
      echo @WV2_MAJOR_VERSION@.@WV2_MINOR_VERSION@.@WV2_MICRO_VERSION@
      ;;
    --cflags)
      echo_cflags=yes
      ;;
    --libs)
      echo_libs=yes
      ;;
    wv2)
      lib_wv2=yes
      ;;
    *)
      usage 1 1>&2
      ;;
  esac
  shift
done

if test "$echo_prefix" = "yes"; then
	echo $prefix
fi

if test "$echo_exec_prefix" = "yes"; then
	echo $exec_prefix
fi

wv2_libs="@WV2_LDFLAGS@ -lwv2 @WV2_LIBS@"

if test "$echo_cflags" = "yes"; then
	includes="@WV2_CFLAGS@"
	if test "$lib_wv2" = "yes"; then
		includes="-I${prefix}/include $includes"
	fi
	echo $includes
fi

if test "$echo_libs" = "yes"; then
	libdirs=-L${exec_prefix}/lib@LIB_SUFFIX@
	my_wv2_libs=
	for i in $wv2_libs ; do
		if test "x$i" != "x-L${exec_prefix}/lib@LIB_SUFFIX@" ; then
			if test -z "$my_wv2_libs" ; then
				my_wv2_libs="$i"
			else
				my_wv2_libs="$my_wv2_libs $i"
			fi
		fi
	done

	echo $libdirs $my_wv2_libs
fi      
