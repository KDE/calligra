# $Id$
#
# This file is part of MIFParse.
# Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>


TEMPLATE = parser
CONFIG = console debug
LEXINPUT = mif.l
YACCINPUT = mif.y
TARGET = mif2kword
PARSER = parser.cpp
SOURCES =	$$PARSER \
			backend.cpp	\
			keyword.cpp \
			treebuild_document.cpp \
			treebuild_pgf.cpp \
			treebuild_font.cpp \
			treebuild_tabstop.cpp \
			treebuild_variable.cpp \
			treebuild_xref.cpp	\
			treebuild_general.cpp	\
			treebuild_fnote.cpp	\
			treebuild_notes.cpp	\
			treebuild_para.cpp	\
			treebuild_textflow.cpp	\
			generate_xml.cpp	\
			generate_xml_textflow.cpp \
			generate_xml_para.cpp
HEADERS =	backend.h \
			treebuild_document.h \
			treebuild_pgf.h	\
			treebuild_font.h	\
			treebuild_tabstop.h	\
			treebuild_variable.h \
			treebuild_xref.h	\
			treebuild_general.h	\
			treebuild_fnote.h	\
			treebuild_notes.h	\
			treebuild_para.h	\
			treebuild_textflow.h	\
			generate_xml.h	\
			generate_xml_textflow.h \
			generate_xml_para.h
#DEFINES = PARSERDEBUG

