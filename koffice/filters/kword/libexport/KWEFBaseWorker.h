/*
   This file is part of the KDE project
   Copyright 2001, 2002, 2003, 2004 Nicolas GOUTTE <goutte@kde.org>

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

#ifndef KWEF_BASEWORKER_H
#define KWEF_BASEWORKER_H

#include <qstring.h>
#include <qiodevice.h>

#include <KWEFStructures.h>
#include <koffice_export.h>

class KWEFKWordLeader;

/**
 * Base worker of the libexport system
 */
class KOFFICEFILTER_EXPORT KWEFBaseWorker
{
    public:
        KWEFBaseWorker(void) : m_kwordLeader(NULL) {}
        virtual ~KWEFBaseWorker(void) {}
    public:
        void registerKWordLeader(KWEFKWordLeader* leader);
    public: // callbacks to Leader
        bool loadSubFile(const QString& fileName, QByteArray& array) const;
        QIODevice* getSubFileDevice(const QString& fileName) const;
    public: // indirect callbacks to Leader:
        QImage loadAndConvertToImage(const QString& strName, const QString& inExtension) const;
        bool loadAndConvertToImage(const QString& strName, const QString& inExtension, const QString& outExtension, QByteArray& image) const;
    public: // leader/worker functions
        virtual bool doOpenFile (const QString& filenameOut, const QString& to);
        virtual bool doCloseFile (void); // Close file in normal conditions
        virtual bool doAbortFile (void); // Close file after errors
        virtual bool doOpenDocument (void); // Like HTML's <html>
        virtual bool doCloseDocument (void); // Like HTML's </html>
        virtual bool doOpenTextFrameSet (void); // Like AbiWord's <section>
        virtual bool doCloseTextFrameSet (void); // Like AbiWord's </section>
        virtual bool doFullDocumentInfo (const KWEFDocumentInfo &docInfo);
	virtual bool doVariableSettings (const VariableSettingsData &varSettings);
        virtual bool doFullDocument (const QValueList<ParaData> &);
        virtual bool doFullAllParagraphs (const QValueList<ParaData>& paraList);
        virtual bool doFullParagraph(const QString& paraText, const LayoutData& layout,
            const ValueListFormatData& paraFormatDataList);
        virtual bool doFullPaperFormat (const int format,
            const double width, const double height, const int orientation); ///< Like AbiWord's \<papersize\>
        virtual bool doFullPaperBorders (const double top, const double left,
            const double bottom, const double right); ///< Like KWord's \<PAPERBORDERS\>
        /**
         * Other data of KWord's \<PAPER\> which are not in @see doFullPaperFormat
         * @since 1.4
         */
        virtual bool doFullPaperFormatOther ( const int columns, const double columnspacing, const int numPages );
        virtual bool doPageInfo(const int headerType, const int footerType);
        virtual bool doHeader(const HeaderData& header);
        virtual bool doFooter(const FooterData& footer);
        virtual bool doOpenHead (void); ///< Like HTML's \<HEAD\>
        virtual bool doCloseHead (void); ///< Like HTML's \</HEAD\>
        virtual bool doOpenBody (void); ///< Like HTML's \<BODY\>
        virtual bool doCloseBody (void); ///< Like HTML's \</BODY\>
        virtual bool doOpenStyles (void); ///< Like HTML's \<style\>
        virtual bool doCloseStyles (void); ///< Like HTML's \</style\>
        virtual bool doFullDefineStyle (LayoutData& layout); ///< Defines a single style
        virtual bool doOpenSpellCheckIgnoreList (void); ///< like AbiWord's \<ignorewords\> and KWord's \<SPELLCHECKIGNORELIST\>
        virtual bool doCloseSpellCheckIgnoreList (void); ///< like AbiWord's \</ignorewords\> and Kwords \</SPELLCHECKIGNORELIST\>
        virtual bool doFullSpellCheckIgnoreWord (const QString& ignoreword); ///< like AbiWord's \<iw\> and Kwords \</SPELLCHECKIGNOREWORD\>
        virtual bool doDeclareNonInlinedFramesets( QValueList<FrameAnchor>& pictureAnchors, QValueList<FrameAnchor>& tableAnchors ); ///< @since 1.4
    protected:
        KWEFKWordLeader* m_kwordLeader;
};

#endif /* KWEF_BASEWORKER_H */
