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
        KWEFKWordLeader(void) : m_syntaxVersion(-1), m_oldSyntax(false), m_numPages(-1),
            m_columns( 1 ), m_columnspacing( 0.0 ),
            m_chain(NULL), m_hType(0), m_fType(0) {}
        virtual ~KWEFKWordLeader(void) {}

    public:
        KoFilter::ConversionStatus convert( KoFilterChain* chain,
            const QCString& from, const QCString& to);
    public:
        void createBookmarkFormatData( ParaData& paraData );
    protected:
        bool loadSubFile(const QString& fileName, QByteArray& array);
        QIODevice* getSubFileDevice(const QString& fileName) const;
        QImage loadAndConvertToImage(const QString& strName, const QString& inExtension) const;
        bool loadAndConvertToImage(const QString& strName, const QString& inExtension, const QString& outExtension, QByteArray& image) const;

    public: // public leader/worker functions (DO NOT use in the worker!)
        virtual bool doFullDocumentInfo (const KWEFDocumentInfo &docInfo)=0;
        virtual bool doVariableSettings (const VariableSettingsData &varSettings)=0;
        virtual bool doPageInfo (const int headerType, const int footerType);
        virtual bool doFullPaperFormat (const int format, const double width, const double height, const int orientation)=0;
        virtual bool doFullPaperBorders (const double top, const double left, const double bottom, const double right)=0;
        virtual bool doOpenHead (void);
        virtual bool doCloseHead (void);
        virtual bool doOpenBody (void);
        virtual bool doCloseBody (void);
        virtual bool doOpenStyles (void)=0;
        virtual bool doCloseStyles (void)=0;
        virtual bool doFullDefineStyle (LayoutData& layout)=0;
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

        virtual bool doFullAllParagraphs (const QValueList<ParaData>& paraList);

    protected: // leader/worker functions
        virtual bool doOpenFile (const QString& filenameOut, const QString& to)=0;
        virtual bool doCloseFile (void)=0;
        virtual bool doAbortFile (void);
        virtual bool doOpenDocument (void)=0;
        virtual bool doCloseDocument (void)=0;
        virtual bool doFullParagraph(const QString& paraText, const LayoutData& layout,
            const ValueListFormatData& paraFormatDataList)=0;

    public:
        /// -1: unknown, 1: KWord 0.8, 2: KWord 1.1 & 1.2, 3: KWord 1.3
        int m_syntaxVersion;
        /// true if oldSyntax (KWord 0.8)
        bool m_oldSyntax;
        int m_numPages;
        int m_columns;
        double m_columnspacing;
        /// Name of the current frame set
        QString m_currentFramesetName;
        /// Number of paragraph in each frameset (for bookmarks)
        QMap<QString,int> m_paraCountMap;
        /// List of bookmarks
        QValueList<Bookmark> m_bookmarkList;
        QStringList m_unanchoredFramesets; ///< List of framesets where an anchor was searched but not found (DEBUG)
        QValueList<FrameAnchor> m_nonInlinedPictureAnchors; ///< Pseudo-anchors for non-inlined anchors
        QValueList<FrameAnchor> m_nonInlinedTableAnchors; ///< Pseudo-anchors for non-inlined tables
    private:
        KoFilterChain* m_chain;
        int m_hType, m_fType;
};

#endif /* KWEF_KWORDLEADER_H */
