// $Header$

/*
   This file is part of the KDE project
   Copyright 2001, 2002 Nicolas GOUTTE <nicog@snafu.de>

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

#ifndef EXPORTFILTERFULLPOWER_H
#define EXPORTFILTERFULLPOWER_H

#include <qvaluestack.h>
#include <qvaluelist.h>
#include <qcolor.h>
#include <qstringlist.h>

#include <KWEFBaseWorker.h>

class ListInfo
{
public:
    CounterData::Style m_typeList; // What is the style of the current list (undefined, if we are not in a list)
    bool m_orderedList; // Is the current list ordered or not (undefined, if we are not in a list)
};

class RTFWorker : public KWEFBaseWorker
{
public:
    RTFWorker(void) : m_ioDevice(NULL), m_streamOut(NULL), m_eol("\r\n"), m_inTable(false) { }
    virtual ~RTFWorker(void) { }
public:
    virtual bool doOpenFile(const QString& filenameOut, const QString& to);
    virtual bool doCloseFile(void); // Close file in normal conditions
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

private:
    QString getStartOfListOpeningTag(const CounterData::Style typeList, bool& ordered);
    void openParagraph(const LayoutData& layout);
    void closeParagraph(const LayoutData& layout);
    void openSpan(const FormatData& formatOrigin, const FormatData& format);
    void closeSpan(const FormatData& formatOrigin, const FormatData& format);
    QString escapeRtfText ( const QString& text ) const;
    void ProcessParagraphData ( const QString& strTag, const QString &paraText,
        const LayoutData& layout, const ValueListFormatData &paraFormatDataList);
    void formatTextParagraph(const QString& strText,
        const FormatData& formatOrigin, const FormatData& format);
    bool makeTable(const FrameAnchor& anchor);
    bool makeImage(const FrameAnchor& anchor);
    void writeFontData(void);
    void writeColorData(void);
    void writeStyleData(void);
    QString textFormatToRtf(const TextFormatting& formatOrigin,
        const TextFormatting& formatData, const bool force);
    QString layoutToRtf(const LayoutData& layoutOrigin,
        const LayoutData& layout, const bool force);
    QString lookupFont(const QString& fontName);
    QString lookupColor(const QString& markup, const QColor& color);
    QString lookupStyle(const QString& styleName, LayoutData& returnLayout);

protected:
    QIODevice* m_ioDevice;
    QTextStream* m_streamOut;
    QString m_eol;
    QString m_textDocInfo;
    QString m_textPage;
    QString m_textBody;
    QString m_fileName; // Name of the output file
    QValueStack<ListInfo> m_listStack; // Stack for list information
    QStringList m_fontList;
    QValueList<QColor> m_colorList;
    QValueList<LayoutData> m_styleList;
    bool m_inTable;
    bool m_paperOrientation;
    double m_paperWidth, m_paperHeight;
    double m_paperMarginTop, m_paperMarginLeft, m_paperMarginBottom, m_paperMarginRight;
};

#endif /* EXPORTFILTERFULLPOWER_H */
