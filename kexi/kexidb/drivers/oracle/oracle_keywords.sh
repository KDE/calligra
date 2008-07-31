#!/bin/bash
#################################################################
# oracle_keywords.sh
#
# Generate oraclekeywords.cpp
# It generates oraclekeywords.cpp from file list_of_keywords.txt,
# this file contains a set of keywords defined by oracle,
# one per line.
#
# Obviously, a better knowledge of shell scripting would be fine,
# but I hope this will do.
#

# I do not know why it won't work
# Destination file
#FILE = "oraclekeywords.cpp"
# Keyword file
#KW_FILE = "list_of_keywords.txt"

# 1, headers and so
echo "/* This file is part of the KDE project
   Copyright (C) 2008 Julia Sanchez-Simon <hithwen@gmail.com>
   Copyright (C) 2008 Miguel Angel Aragüez-Rey <fizban87@gmail.com>

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

#include <oracledriver.h>

namespace KexiDB {
  const char* OracleDriver::keywords[] = {" > oraclekeywords.cpp

# 2, The list of keywords
while read line
do
	echo "		\"$line\"," >> oraclekeywords.cpp
done < list_of_keywords.txt

# 3, Array and namespace closing braces
echo "		0
	};
}" >> oraclekeywords.cpp

# 4, Show count of keywords (not really needed here)
wc list_of_keywords.txt
