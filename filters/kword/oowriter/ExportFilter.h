/* This file is part of the KDE project
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

/*
   This file is based on the old file:
    /home/kde/koffice/filters/kword/ascii/asciiexport.cc

   The old file was copyrighted by
    Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
    Copyright (c) 2000 ID-PRO Deutschland GmbH. All rights reserved.
                       Contact: Wolf-Michael Bolle <Bolle@ID-PRO.de>

   The old file was licensed under the terms of the GNU Library General Public
   License version 2.
*/

#ifndef _EXPORTFILTER_H
#define _EXPORTFILTER_H

#include <KWEFStructures.h>
#include <KWEFUtil.h>
#include <KWEFBaseWorker.h>
#include <KWEFKWordLeader.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <QTextStream>
#include <QByteArray>

#define STRICT_OOWRITER_VERSION_1

class KZip;

class StyleMap : public QMap<QString,LayoutData>
{
public:
    StyleMap(void) {}
    ~StyleMap(void) {}
};

class OOWriterWorker : public KWEFBaseWorker
{
public:
    OOWriterWorker(void);
    virtual ~OOWriterWorker(void) { delete m_streamOut; }
public:
    /// What is the type of the frameset anchor
    enum AnchorType
    {
        AnchorUnknown = 0, // ### TODO: is this really needed?
        AnchorInlined, ///< The frameset is inlined
        AnchorNonInlined, ///< the frameset is not inlined
        AnchorTextImage ///< This is a text image (KWord 0.8; inlined; only for pictures)
    };
public:
    virtual bool doOpenFile(const QString& filenameOut, const QString& to);
    virtual bool doCloseFile(void); ///< Close file in normal conditions
    virtual bool doOpenDocument(void);
    virtual bool doCloseDocument(void);
    virtual bool doFullParagraph(const QString& paraText, const LayoutData& layout,
        const ValueListFormatData& paraFormatDataList);
    virtual bool doFullPaperFormat(const int format,
        const double width, const double height, const int orientation);
    virtual bool doFullPaperBorders (const double top, const double left,
        const double bottom, const double right);
    /**
     * Other data of KWord's \<PAPER\> which are not in @see doFullPaperFormat
     * @since 1.4
     */
    virtual bool doFullPaperFormatOther ( const int columns, const double columnspacing, const int numPages );        
    virtual bool doOpenStyles(void);
    virtual bool doCloseStyles(void);
    virtual bool doFullDefineStyle(LayoutData& layout);
    virtual bool doFullDocumentInfo(const KWEFDocumentInfo& docInfo);
    virtual bool doVariableSettings(const VariableSettingsData& vs);
    virtual bool doOpenBody (void);
    virtual bool doDeclareNonInlinedFramesets( Q3ValueList<FrameAnchor>& pictureAnchors, Q3ValueList<FrameAnchor>& tableAnchors ); ///< @since 1.4
private:
    void processParagraphData (const QString& paraText,
        const TextFormatting& formatLayout,
        const ValueListFormatData& paraFormatDataList);
    void processNormalText ( const QString& paraText,
        const TextFormatting& formatLayout,
        const FormatData& formatData);
    void processFootnote( const VariableData& variable );
    void processNote( const VariableData& variable );
    void processVariable ( const QString& paraText,
        const TextFormatting& formatLayout,
        const FormatData& formatData);
    void processAnchor ( const QString& paraText,
        const TextFormatting& formatLayout,
        const FormatData& formatData);
    void processTextImage ( const QString&,
        const TextFormatting& formatLayout,
        const FormatData& formatData);
    QString textFormatToStyle(const TextFormatting& formatOrigin,
        const TextFormatting& formatData, const bool force, QString& key);
    QString layoutToParagraphStyle(const LayoutData& layoutOrigin,
        const LayoutData& layout, const bool force, QString& styleKey);
    QString escapeOOText(const QString& strText) const;
    QString escapeOOSpan(const QString& strText) const;
    QString cellToProperties( const TableCell& cell, QString& key) const;
    bool makeTableRows( const QString& tableName, const Table& table, int firstRowNumber );
    bool makeTable( const FrameAnchor& anchor, const AnchorType anchorType );
    bool makePicture( const FrameAnchor& anchor, const AnchorType anchorType );
    bool convertUnknownPicture(const QString& name, const QString& extension, QByteArray& image);
    void declareFont(const QString& fontName);
    void writeFontDeclaration(void);
    void writeContentXml(void);
    void writeStylesXml(void);
    void writeMetaXml(void);
    void writeStartOfFile(const QString& type);
    /**
     * For automatic styles: create the name from @p prefix and from the increased @p counter
     * (Yes, the counter is increased by the method.)
     */
    QString makeAutomaticStyleName(const QString& prefix, ulong& counter) const;
private: // ZIP methods
    bool zipPrepareWriting(const QString& name);
    bool zipDoneWriting(void);
    bool zipWriteData(const char* str);
    bool zipWriteData(const QByteArray& array);
    bool zipWriteData(const QString& str); ///< Assumes UTF-8
private:
    QTextStream* m_streamOut;
    QString m_pagesize;
    StyleMap m_styleMap;
    double m_paperBorderTop,m_paperBorderLeft,m_paperBorderBottom,m_paperBorderRight;
    KWEFDocumentInfo m_docInfo; ///< document information
    QByteArray m_contentBody; ///< office:body element of content.xml
    KZip* m_zip; ///< Output OOWriter file
    QMap<QString,QString> m_fontNames; ///< List of used font names (the data() are extra attributes for font declaration time.)

    ulong m_pictureNumber; ///< Number of picture (increment *before* use)
    ulong m_automaticParagraphStyleNumber; ///< Number of paragraph-based automatic styles (increment *before* use)
    ulong m_automaticTextStyleNumber; ///< Number of text-based automatic styles (increment *before* use)
    ulong m_footnoteNumber; ///< Number of footnote (for text:id) (increment *before* use)
    ulong m_tableNumber; ///< Number of table (for table:name) (increment *before* use)
    ulong m_textBoxNumber; ///< Number of a textbox (for draw:text-box) (increment *before* use)
    

    QString m_styles; ///< Normal paragraph styles (in OO format)
    QString m_contentAutomaticStyles; ///< Automatic styles for content.xml (in OO format)

    uint m_size; ///< Size of ZIP entry
    int m_paperFormat;
    double m_paperWidth;
    double m_paperHeight;
    int m_paperOrientation;

    QMap<QString,QString> m_mapTextStyleKeys; ///< Map of keys to automatic text styles
    QMap<QString,QString> m_mapParaStyleKeys; ///< Map of keys to automatic paragraph styles
    VariableSettingsData m_varSet; ///< KWord's \<VARIABLESETTINGS\>
    int m_numPages; ///< Number of pages @note if the input file comes from a filter, this data is often missing.
    double m_columnspacing; ///< Spacing between columns
    int m_columns; ///< Number of columns
private: // Variable that would need a link/glue from libexport
    Q3ValueList<FrameAnchor> m_nonInlinedPictureAnchors; ///< Pseudo-anchors for non-inlined anchors  \todo: connection to libexport
    Q3ValueList<FrameAnchor> m_nonInlinedTableAnchors; ///< Pseudo-anchors for non-inlined tables  \todo: connection to libexport

};
#endif // _EXPORTFILTER_H
