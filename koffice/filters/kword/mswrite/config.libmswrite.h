/* This file is part of the LibMSWrite Library
   Copyright (C) 2001-2003 Clarence Dang <clarencedang@users.sourceforge.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License Version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License Version 2 for more details.

   You should have received a copy of the GNU Library General Public License
   Version 2 along with this library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.

   LibMSWrite Project Website:
   http://sourceforge.net/projects/libmswrite/
*/

#ifndef NDEBUG
	#if 0	// set to 1 if debugging LibMSWrite
		#define DEBUG_HEADER
		#define DEBUG_FORMATINFO
		#define DEBUG_PARA
		//#define DEBUG_PARA_TAB	// tabulator
		#define DEBUG_CHAR
		#define DEBUG_FONT
		#define DEBUG_PAGETABLE
		#define DEBUG_PAGELAYOUT
		#define DEBUG_OBJECT
		#define DEBUG_IMAGE
		#define DEBUG_INTERNALPARSER
		#define DEBUG_INTERNALGENERATOR

		#define CHECK_INTERNAL	// more consistency checks - not required if LibMSWrite was "bug free"
	#endif
#endif

