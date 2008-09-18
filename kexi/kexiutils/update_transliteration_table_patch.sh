#!/bin/bash
#
#   Copyright (C) 2007 Jaroslaw Staniek <js@iidea.pl>
#
#   This program is free software; you can redistribute it and/or
#   modify it under the terms of the GNU General Public
#   License as published by the Free Software Foundation; either
#   version 2 of the License, or (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; see the file COPYING.  If not, write to
#   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
#   Boston, MA 02110-1301, USA.

#
# Updates transliteration_table.h.patch file using the original 
# transliteration_table.h.bz2 file.
#

# 1. Create an original transliteration file by reversing changes

bzcat transliteration_table.cpp.bz2 > transliteration_table.cpp.orig || exit 1
patch -p0 -R transliteration_table.cpp.orig < transliteration_table.cpp.patch  || exit 1

# 2. Update transliteration_table.cpp.patch file

diff -u transliteration_table.cpp.orig transliteration_table.cpp \
 > transliteration_table.cpp.patch  || exit 1

