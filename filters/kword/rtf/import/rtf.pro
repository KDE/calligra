# $Id$
#
# This file is part of MIFParse.
# Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>


TEMPLATE = app
CONFIG = console debug qt
TARGET = rtf2kword
SOURCES =	main.cpp KRTFTokenizer.cpp KRTFParser.h KRTFFileParser.h	\
		KRTFHeaderParser.cpp KRTFDocumentParser.cpp
HEADERS =	KRTFTokenizer.h KRTFToken.h KRTFParser.cpp KRTFFileParser.h \
		KRTFHeaderParser.h KRTFDocumentParser.h
#DEFINES = PARSERDEBUG

