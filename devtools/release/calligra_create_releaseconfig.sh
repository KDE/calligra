#! /bin/bash
#
# usage: calligra_create_releaseconfig.sh <version>
#
# This script does:
# - Clones the git repository int a temporary directory
# - Checks out the version (tag)
# - Extracts pot file names form Message.sh files
# - Removes the temporary git clone
# - Creates a config.ini file for create_tarball_kf5.rb script
#
# This means the repository will be downloaded twice, but it means we can use a standard create_tarball_kf5.rb
# so we do not risk messing things up for everybody else.
#
if [[ $# -ne 1 ]]; then
    echo "Usage: calligra_create_releaseconfig.sh <version>"
    exit 1
fi

version=$1

gitdir="./tmp"
rm -rf "$gitdir"
mkdir "$gitdir"

echo "Clone git repository to extract po file names..."
here=$PWD
cd "$gitdir"
git archive --remote git://anongit.kde.org/calligra.git "$version" | tar -x
cd "$here"

#
# Grab the names of generated pot files from the Messages.sh files
#
# The 'grep MSGCAT' treats messages in scripting (krossmoduleplan,pot, etc), which has a line like this:
# ${MSGCAT} -F "$podir/${POT_CPP}" "$podir/${POT_PY}" --use-first > $podir/krossmoduleplan.pot
#
# The other treats all else, with a line like this:
# kundo2_aware_xgettext calligra.pot $LIST
#
# Any deviation from these patterns will cause problems
#
echo "Extract po file names..."
po_list=$(find $gitdir -name 'Messages.sh' | while read messagefile; do
    if grep -q 'MSGCAT' $messagefile
    then cat $messagefile | grep 'MSGCAT' | cut -d'>' -f2 | cut -d'/' -f2 | cut -d'.' -f1 | sort -f | uniq
    else cat $messagefile | grep '\.pot' | cut -d' ' -f2 | cut -d'.' -f1 | sort -f | uniq
    fi
done)

# replace newline with ',' to make a comma separated list
po_list=${po_list//$'\n'/,}
#echo $po_list
if [ -z "$po_list" ]; then
    echo "No po file names extracted"
    exit 3
fi

# do not need this anymore
rm -rf $gitdir

echo "Create config.ini file..."

echo "[calligra]" > config.ini
echo "gitModule   = yes" >> config.ini
echo "gitTag      = $version" >> config.ini
echo "mainmodule  = calligra" >> config.ini
echo "submodule   = calligra" >> config.ini
echo "version     = $version" >> config.ini
echo "translations= yes" >> config.ini
echo "docs        = no" >> config.ini
echo "kde_release = no" >> config.ini
echo "custompo    = $po_list" >> config.ini

