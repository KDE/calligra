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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KWEF_KWORDLEADER_H
#define KWEF_KWORDLEADER_H

#include <qiodevice.h>

#include <KoFilterChain.h>

#include "KWEFStructures.h"
#include "KWEFBaseWorker.h"
#include <koffice_export.h>

class KOFFICEFILTER_EXPORT KWEFKWordLeader
{
    public:
        KWEFKWordLeader(void);
        KWEFKWordLeader(KWEFBaseWorker* newWorker);
        virtual ~KWEFKWordLeader(void);
    public:
        void setWorker ( KWEFBaseWorker *newWorker );
        KWEFBaseWorker *getWorker(void) const;
        KoFilter::ConversionStatus convert( KoFilterChain* chain,
            const QCString& from, const QCString& to);
    public: // ### TODO: where to put in the end?
        void createBookmarkFormatData( ParaData& paraData );
    public: // callbacks
        bool loadSubFile(const QString& fileName, QByteArray& array);
        QIODevice* getSubFileDevice(const QString& fileName);
    public: // public leader/worker functions (DO NOT use in your own code!)
        bool doFullDocumentInfo (const KWEFDocumentInfo &docInfo);
	bool doVariableSettings (const VariableSettingsData &varSettings);
        bool doFullDocument (const QValueList<ParaData> &);
	bool doPageInfo (const int headerType, const int footerType);
        bool doFullPaperFormat (const int format, const double width, const double height, const int orientation);
        bool doFullPaperBorders (const double top, const double left, const double bottom, const double right);
        /**
         * Other data of KWord's \<PAPER\> which are not in @see doFullPaperFormat
         * @since 1.4
         */
        bool doFullPaperFormatOther ( const int columns, const double columnspacing, const int numPages );
        bool doOpenHead (void);
        bool doCloseHead (void);
        bool doOpenBody (void);
        bool doCloseBody (void);
        bool doOpenStyles (void);
        bool doCloseStyles (void);
        bool doFullDefineStyle (LayoutData& layout);
        bool doOpenSpellCheckIgnoreList (void);
        bool doCloseSpellCheckIgnoreList (void);
        bool doFullSpellCheckIgnoreWord (const QString& ignoreword);
        bool doHeader(const HeaderData&);
        bool doFooter(const FooterData&);
        bool doDeclareNonInlinedFramesets( QValueList<FrameAnchor>& pictureAnchors, QValueList<FrameAnchor>& tableAnchors ); ///< @since 1.4

        QValueList<FootnoteData> footnoteList;

        void setHeaderType(int hType) { m_hType = hType; }
        void setFooterType(int fType) { m_fType = fType; }
        int headerType() { return m_hType; }
        int footerType() { return m_fType; }

    protected: // leader/worker functions
        bool doOpenFile (const QString& filenameOut, const QString& to);
        bool doCloseFile (void);
        bool doAbortFile (void);
        bool doOpenDocument (void);
        bool doCloseDocument (void);
    public:
        /// -1: unknown, 1: KWord 0.8, 2: KWord 1.1 & 1.2, 3: KWord 1.3
        int m_syntaxVersion;
        /// true if oldSyntax (KWord 0.8)
        bool m_oldSyntax;
        /// Name of the current frame set
        QString m_currentFramesetName;
        /// Number of paragraph in each frameset (for bookmarks)
        QMap<QString,int> m_paraCountMap;
        /// List of bookmarks
        QValueList<Bookmark> m_bookmarkList;
        QStringList m_unanchoredFramesets; ///< List of framesets where an anchor was searched but not found (DEBUG) @since 1.4
        QValueList<FrameAnchor> m_nonInlinedPictureAnchors; ///< Pseudo-anchors for non-inlined anchors @since 1.4
        QValueList<FrameAnchor> m_nonInlinedTableAnchors; ///< Pseudo-anchors for non-inlined tables @since 1.4
    private:
        KWEFBaseWorker *m_worker;
        KoFilterChain* m_chain;
        int m_hType, m_fType;
};

#endif /* KWEF_KWORDLEADER_H */
