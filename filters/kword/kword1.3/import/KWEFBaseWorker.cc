//

/*
   This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003, 2004 Nicolas GOUTTE <goutte@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/


#include "KWEFStructures.h"
#include "KWEFBaseWorker.h"


//
// Define all methods returning true
//

#define DO_TRUE_DEFINITION(string) \
    bool KWEFBaseWorker::string \
    {\
        return true;\
    }

DO_TRUE_DEFINITION (doFullDocumentInfo (const KWEFDocumentInfo&))
DO_TRUE_DEFINITION (doVariableSettings (const VariableSettingsData &))
DO_TRUE_DEFINITION (doFullPaperFormat (const int, const double, const double, const int))
DO_TRUE_DEFINITION (doFullPaperBorders (const double, const double, const double, const double))
DO_TRUE_DEFINITION (doPageInfo(int,int))
DO_TRUE_DEFINITION (doOpenHead (void))
DO_TRUE_DEFINITION (doCloseHead (void))
DO_TRUE_DEFINITION (doOpenBody (void))
DO_TRUE_DEFINITION (doCloseBody (void))
DO_TRUE_DEFINITION (doOpenStyles (void))
DO_TRUE_DEFINITION (doCloseStyles (void))
DO_TRUE_DEFINITION (doFullDefineStyle (LayoutData&))
DO_TRUE_DEFINITION (doOpenSpellCheckIgnoreList (void))
DO_TRUE_DEFINITION (doCloseSpellCheckIgnoreList (void))
DO_TRUE_DEFINITION (doFullSpellCheckIgnoreWord (const QString&))
DO_TRUE_DEFINITION (doHeader(const HeaderData&))
DO_TRUE_DEFINITION (doFooter(const FooterData&))

