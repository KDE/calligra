#!/bin/bash
################################################################################
# sql_keywords.sh
#
# Generate sets of driver-specific keywords.
# This program generates files that can be used as part of KexiDB drivers
# that list keywords specific to that driver, i.e. words that have to be
# escaped if they are to be used as identifiers in the database.
#
# It extracts keywords from the lexer of the DB sources, deletes keywords that
# are already going to be escaped because they are part of Kexi's SQL dialect,
# and writes the resulting keywords to a "char *keywords[]" construct in a .cpp
# file that can then be used in the driver.
#
# To use:
# Put three DB source archives in the current directory in proper order, e.g.:
# sqlite-src-3080403.zip from http://www.sqlite.org/download.html 
# mysql-5.6.17.tar.gz from http://dev.mysql.com/downloads/mysql/
# postgresql-9.3.4.tar.gz from http://www.postgresql.org/ftp/source/
# 
#
# Run script with 4 arguments in order {sqlite-archive} {mysql-archive}{pgsql-archive} {srcPath}
# (this order required) for example
#./sql_keywords.sh sqlite-src-35.zip mysql-5.3.17.tar.gz postgresql-9.3.4.tar.gz ~/kde4/src/calligra
#
# Sed, awk, grep have been used without much thought -
# CHECK THE OUTPUT BEFORE INCLUDING IT IN A DRIVER!
#
# Copyright (C) 2014 Wojciech Kosowicz <pcellix@gmail.com>
#
# Based on the original script by Martin Ellis <martin.ellis@kdemail.net>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA 02110-1301, USA.

sqlArchive="$1"
mysqlArchive="$2"
postgresqlArchive="$3"
sourceDir=`realpath $4`
tmpDir=`mktemp -d`

if [ $# -lt 4 ] || [[ ! $sqlArchive == *sqlite*.zip ]] || [ ! -r ${sqlArchive} ] \
  || [[ ! $mysqlArchive == *mysql*.tar.gz ]] || [ ! -r ${mysqlArchive} ] \
  || [[ ! $postgresqlArchive == *postgresql*.tar.gz ]] || [ ! -r ${postgresqlArchive} ] \
  || [ ! -w $sourceDir/kexi/kexidb/drivers/pqxx/pqxxkeywords.cpp ] \
  || [ ! -w $sourceDir/libs/db/drivers/sqlite/sqlitekeywords.cpp ] \
  || [ ! -w $sourceDir/kexi/kexidb/drivers/mysql/mysqlkeywords.cpp ] \
  || [ ! -w $sourceDir/libs/db/keywords.cpp ] ; then
  echo "Please specify {sqlite-archive} {mysql-archive} {pgsql-archive} {srcPatch} arguments and \
    ensure that you have read rights (archives) and write (sources) for these files"
  exit 1
fi

set -e
progname="sql_keywords.sh"

################################################################################
# C++ file generator
# params : array   - scoped datatype and name of the array to generate
#          include - a file to include (or "" if none)
#          inFile  - file containing raw keywords
#          outfile - file to write

header () {
  local array="$1"
  local include="$2"
  local inFile="$3"
  local outFile="$4"
  echo "Writing keywords in $inFile to $outFile"
  cat <<EOF1 > "$outFile";
/* This file is part of the KDE project
   Copyright (C) 2004 Martin Ellis <martin.ellis@kdemail.net>
   Copyright (C) 2004 Jarosław Staniek <staniek@kde.org>
   Copyright (C) 2014 Wojciech Kosowicz <pcellix@gmail.com>

   This file has been automatically generated from
   calligra/kexi/tools/sql_keywords/$progname and
   $inFile.

   Please edit the $progname, not this file!

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
EOF1
  if [ "$include" != "" ] ; then
    echo "#include <$include>" >> "$outFile"
  fi
  cat <<EOF2 >> "$outFile";

namespace KexiDB {
  ${array}[] = {
EOF2
}

body() {
  local inFile="$1"
  local outFile="$2"
  awk '/^[a-zA-Z_0-9]*/ { print "\t\t\""$$1"\","; } ' "$inFile" >> "$outFile"
}

footer() {
  local outFile="$1"
  cat <<EOF >> "$outFile";
		0
  };
}
EOF

}

################################################################################
# Keyword comparison functions
# Globals: keywords
# readKeywords
# params: filename - file of keywords to read
# sets:   keywords - array of keywords in the file
readKeywords () {
  local filename="$1"
  local kexiSQL="$2"
  while read keyword
  do
    keywords+=($keyword)
  done < "$filename"
}

# compareKeywords
# reads: kexiSQL -
#        driverSQL
# sets:  keywords - driver keywords that are not keywords in Kexi
compareKeywords () {
  for(( i=0; i < ${#driverSQL[@]}; i++ )) ; do
    found="no"
    for(( j=0; j < ${#kexiSQL[@]}; j++ )) ; do
      if [ "${driverSQL[$i]}" == "${kexiSQL[$j]}" ] ; then
        found="yes"
      fi
    done
    if [ "$found" == "no" ] ; then
      keywords+=("${driverSQL[$i]}")
    fi
  done
}


# getDriverKeywords
# params : kexi -
#          driver -
#          outFile -
getDriverKeywords () {
  local kexi="$1"
  local driver="$2"
  local outFile="$3"

  declare -a kexiSQL
  declare -a driverSQL

  echo "Looking for driver-specific keywords in \"$driver\""
  readKeywords $kexi
  for(( i=0; i < ${#keywords[@]}; i++ )) ; do
    kexiSQL[$i]=${keywords[$i]}
  done

  unset keywords

  readKeywords $driver
  for(( i=0; i < ${#keywords[@]}; i++ )) ; do
    driverSQL[$i]=${keywords[$i]}
  done
  unset keywords

  compareKeywords
  echo "Writing driver-specific keywords for \"$driver\" to \"$outFile\""
  rm -f $outFile
  for(( i=0; i < ${#keywords[@]}; i++ )) ; do
    echo ${keywords[$i]} >> $outFile
  done
  unset keywords
}
################################################################################


################################################################################
# Kexi lexer

checkKexiKeywords () {
  local scanner="../../../libs/db/parser/sqlscanner.l"
  if [ -r "$scanner" ] ; then
    echo "Getting Kexi keywords"
    grep '^(\?"[a-zA-Z_0-9]' "$scanner" | \
       sed 's/(\?"\([^"]*\)"[^"]*/\1\n/g' | \
       awk '/^[a-zA-Z_0-9]+$/ {print $1;}' | \
       sort | uniq > "$tmpDir/kexi.all"
    awk '/^[a-zA-Z_0-9]+$/ {print $1;}' kexi_reserved >> "$tmpDir/kexi.all"
  fi
}

################################################################################
# DB lexer functions
# These functions munge the extracted lexers from DBs and write the collected
# keywords to file

# getSQLiteKeywords
# params : inFile  - SQLite3 lexer file
#          outFile - all SQLite3 keywords
getSQLiteKeywords () {
  local inFile="$1"
  local outFile="$2"

  echo "Getting SQLite keywords ($inFile -> $outFile)"
    sed -n '/^static Keyword aKeywordTable/,/};/p' $inFile | \
    awk -F '"' '{print $2}' | \
    sed '/^$/d' > $outFile
}

getPostgreSQLKeywords () {
  local inFile="$1"
  local outFile="$2"

  echo "Getting PostgreSQL keywords ($inFile -> $outFile)"
    sed -n '/PG_KEYWORD(/p' $inFile | awk -F ',' '{print $2}' > $outFile
}

# getMySQLKeywords
# params : inFile  - MySQL lexer file
#          outFile - all MySQL keywords
getMySQLKeywords () {
  local inFile="$1"
  local outFile="$2"

  echo "Getting MySQL keywords ($inFile -> $outFile)"
  sed -n '/^static SYMBOL symbols/,/};/p' $inFile | \
    awk '/  { "[a-zA-Z_0-9]*"/ { print $2;}' | \
    sed 's/"\(.*\)".*/\1/g' > $outFile
}

################################################################################
# DB archives functions
# These functions extract the lexer files from the DB source archive

# extracted
# params : archive - archive containing backend DB source
#          file - file in archive containing DB's lexer
extract () {
  local archive="$1"
  local file="$2"

  echo "Getting file \"$file\" from \"$archive\""
  if [[ ${archive} == *tar.gz ]]; then
      tar -zxf "$archive" -C "$tmpDir" "$file"
  elif [[ ${archive} == *zip ]]; then
      unzip "$archive" "$file" -d "$tmpDir"
  fi
}

# checkArchives
checkArchives () {
  local pathInArchive
  local appName
  local appver

  pathInArchive="tool/mkkeywordhash.c"
  appName="SQLite"
  filePrefix="sqlite"
  appVer=`basename $sqlArchive | awk -F '.zip' '{print $1}'`
  extract "$sqlArchive" "$appVer/$pathInArchive"
  getSQLiteKeywords "$tmpDir/$appVer/$pathInArchive" "$tmpDir/$appVer.all"

  if [ "$tmpDir/$appVer.all" -nt "$tmpDir/$appVer.new" ] ; then
    getDriverKeywords "$tmpDir/kexi.all" "$tmpDir/$appVer.all" "$tmpDir/$appVer.new"
    header "const char* const ${appName}Driver::keywords" "${filePrefix}driver.h" "$tmp/$appVer/$pathInArchive" "$tmpDir/${filePrefix}keywords.cpp"
    body   "$tmpDir/$appVer.new" "$tmpDir/${filePrefix}keywords.cpp"
    footer "$tmpDir/${filePrefix}keywords.cpp"
  fi

  pathInArchive="sql/lex.h"
  appName="MySql"
  filePrefix="mysql"
  appVer=`basename $mysqlArchive | awk -F '.tar.gz' '{print $1}'`
  extract "$mysqlArchive" "$appVer/$pathInArchive"
  getMySQLKeywords "$tmpDir/$appVer/$pathInArchive" "$tmpDir/$appVer.all"


  if [ "$tmpDir/$appVer.all" -nt "$tmpDir/$appVer.new" ] ; then
    getDriverKeywords "$tmpDir/kexi.all" "$tmpDir/$appVer.all" "$tmpDir/$appVer.new"
    header "const char* const ${appName}Driver::keywords" "${filePrefix}driver.h" "$tmp/$appVer/$pathInArchive" "$tmpDir/${filePrefix}keywords.cpp"
    body   "$tmpDir/$appVer.new" "$tmpDir/${filePrefix}keywords.cpp"
    footer "$tmpDir/${filePrefix}keywords.cpp"
  fi

  pathInArchive="src/include/parser/kwlist.h"
  appName="pqxxSql"
  filePrefix="pqxx"
  appVer=`basename $postgresqlArchive | awk -F '.tar.gz' '{print $1}'`
  extract "$postgresqlArchive" "$appVer/$pathInArchive"
  getPostgreSQLKeywords "$tmpDir/$appVer/$pathInArchive" "$tmpDir/$appVer.all"

  if [ "$tmpDir/$appVer.all" -nt "$tmpDir/$appVer.new" ] ; then
    getDriverKeywords "$tmpDir/kexi.all" "$tmpDir/$appVer.all" "$tmpDir/$appVer.new"
    header "const char* ${appName}Driver::keywords" "${filePrefix}driver.h" "$tmp/$appVer/$pathInArchive" "$tmpDir/${filePrefix}keywords.cpp"
    body   "$tmpDir/$appVer.new" "$tmpDir/${filePrefix}keywords.cpp"
    footer "$tmpDir/${filePrefix}keywords.cpp"
  fi
}

# moveKeywordFiles
moveKeywordFiles () {
    mv $tmpDir/pqxxkeywords.cpp $sourceDir/kexi/kexidb/drivers/pqxx/
    mv $tmpDir/sqlitekeywords.cpp $sourceDir/libs/db/drivers/sqlite/
    mv $tmpDir/mysqlkeywords.cpp $sourceDir/kexi/kexidb/drivers/mysql/
    mv $tmpDir/keywords.cpp $sourceDir/libs/db/
}

checkKexiKeywords
src=`printf "calligra/libs/db/parser/sqlscanner.l\n"\
" * and calligra/kexi/tools/sql_keywords/kexi_reserved"`
header "const char* DriverPrivate::kexiSQLKeywords" "driver_p.h" "$src" "${tmpDir}/keywords.cpp"
body "${tmpDir}/kexi.all" "${tmpDir}/keywords.cpp"
footer "${tmpDir}/keywords.cpp"

checkArchives $1 $2 $3 $4
moveKeywordFiles
wc -l $tmpDir/*.all $tmpDir/*.new | awk '{print $2" "$1}' |sort|awk '{print $1"\t"$2}'