//

/*
   This file is part of the KDE project
   Copyright (C) 2001. 2002, 2004 Nicolas GOUTTE <goutte@kde.org>

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

#ifndef KWEF_KWORDLEADER_H
#define KWEF_KWORDLEADER_H

#include <qiodevice.h>

#include <koFilterChain.h>

#include "KWEFStructures.h"

class KWEFKWordLeader
{
    public:
        KWEFKWordLeader(void) : m_chain(NULL), m_hType(0), m_fType(0) {}
        virtual ~KWEFKWordLeader(void) {}

    public:
        KoFilter::ConversionStatus convert( KoFilterChain* chain,
            const QCString& from, const QCString& to);
    public: // "callbacks"
        virtual bool loadSubFile(const QString& fileName, QByteArray& array);
        virtual QIODevice* getSubFileDevice(const QString& fileName) const;
    public: // public leader/worker functions (DO NOT use in your own code!)
        virtual bool doFullDocumentInfo (const KWEFDocumentInfo &docInfo);
        virtual bool doVariableSettings (const VariableSettingsData &varSettings);
        virtual bool doFullDocument (const QValueList<ParaData> &);
        virtual bool doPageInfo (const int headerType, const int footerType);
        virtual bool doFullPaperFormat (const int format, const double width, const double height, const int orientation);
        virtual bool doFullPaperBorders (const double top, const double left, const double bottom, const double right);
        virtual bool doOpenHead (void);
        virtual bool doCloseHead (void);
        virtual bool doOpenBody (void);
        virtual bool doCloseBody (void);
        virtual bool doOpenStyles (void);
        virtual bool doCloseStyles (void);
        virtual bool doFullDefineStyle (LayoutData& layout);
        virtual bool doOpenSpellCheckIgnoreList (void);
        virtual bool doCloseSpellCheckIgnoreList (void);
        virtual bool doFullSpellCheckIgnoreWord (const QString& ignoreword);
        virtual bool doHeader(const HeaderData&);
        virtual bool doFooter(const FooterData&);

        QValueList<FootnoteData> footnoteList;  // ### TODO: does this need to be public?

        virtual void setHeaderType(int hType) { m_hType = hType; }
        virtual void setFooterType(int fType) { m_fType = fType; }
        virtual int headerType() { return m_hType; }
        virtual int footerType() { return m_fType; }

    protected: // leader/worker functions
        virtual bool doOpenFile (const QString& filenameOut, const QString& to)=0;
        virtual bool doCloseFile (void)=0;
        virtual bool doAbortFile (void);
        virtual bool doOpenDocument (void)=0;
        virtual bool doCloseDocument (void)=0;
    private:
        KoFilterChain* m_chain;
        int m_hType, m_fType;
};

#endif /* KWEF_KWORDLEADER_H */
