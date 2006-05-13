/*
   This file is part of the KDE project
   Copyright 2001, 2002 Nicolas GOUTTE <goutte@kde.org>

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

#ifndef EXPORTFILTERFULLPOWER_H
#define EXPORTFILTERFULLPOWER_H

#include <q3valuestack.h>
#include <q3valuelist.h>
#include <QStringList>
//Added by qt3to4:
#include <QTextStream>

#include <KWEFBaseWorker.h>

class ListInfo
{
public:
    CounterData::Style m_typeList; ///< What is the style of the current list (undefined, if we are not in a list)
    bool m_orderedList; ///< Is the current list ordered or not (undefined, if we are not in a list)
};

class RTFWorker : public KWEFBaseWorker
{
public:
    RTFWorker(void);
    virtual ~RTFWorker(void) { delete m_streamOut; delete m_ioDevice; }
public:
    virtual bool doOpenFile(const QString& filenameOut, const QString& to);
    virtual bool doCloseFile(void); ///< Close file in normal conditions
    virtual bool doOpenDocument(void);
    virtual bool doCloseDocument(void);
    virtual bool doFullParagraph(const QString& paraText, const LayoutData& layout,
        const ValueListFormatData& paraFormatDataList);
    virtual bool doFullDocumentInfo(const KWEFDocumentInfo& docInfo);
    virtual bool doOpenTextFrameSet(void);
    virtual bool doCloseTextFrameSet(void);
    virtual bool doFullPaperFormat(const int format,
        const double width, const double height, const int orientation);
    virtual bool doFullPaperBorders (const double top, const double left,
        const double bottom, const double right);
    virtual bool doFullDefineStyle(LayoutData& layout);
    virtual bool doHeader(const HeaderData& header);
    virtual bool doFooter(const FooterData& footer);
    virtual bool doVariableSettings(const VariableSettingsData& vs);

private:
    QString openSpan(const FormatData& formatOrigin, const FormatData& format);
    QString closeSpan(const FormatData& formatOrigin, const FormatData& format);
    /** 
     * Encodes the @p text into
     * RTF seven bit ASCII. This affects any 8 bit characters.
     * They are encoded either with \\' or with \\u
     * @return the escaped string
     */
    QString escapeRtfText ( const QString& text ) const;
    QString ProcessParagraphData ( const QString &paraText,
        const LayoutData& layout, const ValueListFormatData &paraFormatDataList);
    QString formatTextParagraph(const QString& strText,
        const FormatData& formatOrigin, const FormatData& format);
    QString makeTable(const FrameAnchor& anchor);
    bool convertUnknownPicture(const QString& strName, const QString& extension, QByteArray& image);
    QString makeImage(const FrameAnchor& anchor);
    void writeFontData(void);
    void writeColorData(void);
    void writeStyleData(void);
    QString textFormatToRtf(const TextFormatting& formatOrigin,
        const TextFormatting& formatData, const bool force);
    QString layoutToRtf(const LayoutData& layoutOrigin,
        const LayoutData& layout, const bool force);
    QString lookupFont(const QString& markup, const QString& fontName);
    QString lookupColor(const QString& markup, const QColor& color);
    QString lookupStyle(const QString& styleName, LayoutData& returnLayout);
    QString writeRow(const QString& textCellHeader, const QString& rowText, const FrameData& frame);
    QString writeBorder(const char whichBorder, const int borderWidth, const QColor& color);
protected:
    QIODevice* m_ioDevice;
    QTextStream* m_streamOut;
    QString m_eol;
    QString m_textDocInfo;
    QString m_textPage;
    QString m_textBody;
    QString m_fileName; ///< Name of the output file
    Q3ValueStack<ListInfo> m_listStack; ///< Stack for list information
    QStringList m_fontList;
    Q3ValueList<QColor> m_colorList;
    Q3ValueList<LayoutData> m_styleList;
    bool m_inTable;
    bool m_paperOrientation;
    double m_paperWidth, m_paperHeight;
    double m_paperMarginTop, m_paperMarginLeft, m_paperMarginBottom, m_paperMarginRight;
    QString m_prefix;
    int m_startPageNumber;
};

#endif /* EXPORTFILTERFULLPOWER_H */
