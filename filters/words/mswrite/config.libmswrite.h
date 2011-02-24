
/* This file is part of the LibMSWrite project
   Copyright (c) 2001-2003, 2007 Clarence Dang <clarencedang@users.sf.net>
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
   OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
   IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
   THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   LibMSWrite Project Website:
   http://sourceforge.net/projects/libmswrite/
*/

#ifndef NDEBUG
#if 0 // set to 1 if debugging LibMSWrite
#define DEBUG_HEADER
#define DEBUG_FORMATINFO
#define DEBUG_PARA
//#define DEBUG_PARA_TAB // tabulator
#define DEBUG_CHAR
#define DEBUG_FONT
#define DEBUG_PAGETABLE
#define DEBUG_PAGELAYOUT
#define DEBUG_OBJECT
#define DEBUG_IMAGE
#define DEBUG_INTERNALPARSER
#define DEBUG_INTERNALGENERATOR

#define CHECK_INTERNAL // more consistency checks - not required if LibMSWrite was "bug free"
#endif
#endif

