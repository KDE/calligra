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

#ifndef KWEF_BASEWORKER_H
#define KWEF_BASEWORKER_H

#include <qstring.h>
#include <qiodevice.h>

#include "KWEFStructures.h"
#include "KWEFKWordLeader.h"


class KWEFBaseWorker : public KWEFKWordLeader
{
    public:
        KWEFBaseWorker(void) {}
        virtual ~KWEFBaseWorker(void) {}
    public: // leader/worker functions
        virtual bool doFullDocumentInfo (const KWEFDocumentInfo &docInfo);
    	virtual bool doVariableSettings (const VariableSettingsData &varSettings);
        virtual bool doFullPaperFormat (const int format,
            const double width, const double height, const int orientation); // Like AbiWord's <papersize>
        virtual bool doFullPaperBorders (const double top, const double left,
            const double bottom, const double right); // Like KWord's <PAPERBORDERS>
        virtual bool doPageInfo(const int headerType, const int footerType);
        virtual bool doHeader(const HeaderData& header);
        virtual bool doFooter(const FooterData& footer);
        virtual bool doOpenHead (void); // Like HTML's <HEAD>
        virtual bool doCloseHead (void); // Like HTML's </HEAD>
        virtual bool doOpenBody (void); // Like HTML's <BODY>
        virtual bool doCloseBody (void); // Like HTML's </BODY>
        virtual bool doOpenStyles (void); // Like HTML's <style>
        virtual bool doCloseStyles (void); // Like HTML's </style>
        virtual bool doFullDefineStyle (LayoutData& layout); // Defines a single style
        virtual bool doOpenSpellCheckIgnoreList (void); // like AbiWord's <ignorewords> and KWord's <SPELLCHECKIGNORELIST>
        virtual bool doCloseSpellCheckIgnoreList (void); // like AbiWord's </ignorewords> and Kwords </SPELLCHECKIGNORELIST>
        virtual bool doFullSpellCheckIgnoreWord (const QString& ignoreword); // like AbiWord's <iw> and Kwords </SPELLCHECKIGNOREWORD>
};

#endif /* KWEF_BASEWORKER_H */
