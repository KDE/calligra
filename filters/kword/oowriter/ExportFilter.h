//

/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 Nicolas GOUTTE <goutte@kde.org>

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
    virtual bool doOpenFile(const QString& filenameOut, const QString& to);
    virtual bool doCloseFile(void); // Close file in normal conditions
    virtual bool doOpenDocument(void);
    virtual bool doCloseDocument(void);
    virtual bool doFullParagraph(const QString& paraText, const LayoutData& layout,
        const ValueListFormatData& paraFormatDataList);
    virtual bool doFullPaperFormat(const int format,
        const double width, const double height, const int orientation); // Calc AbiWord's <papersize>
    virtual bool doFullPaperBorders (const double top, const double left,
        const double bottom, const double right); // Like KWord's <PAPERBORDERS>
    virtual bool doOpenStyles(void); // AbiWord's <office:styles>
    virtual bool doCloseStyles(void); // AbiWord's </office:styles>
    virtual bool doFullDefineStyle(LayoutData& layout); // AbiWord's <style:style/>
    virtual bool doFullDocumentInfo(const KWEFDocumentInfo& docInfo); // <office:meta/>
private:
    void processParagraphData (const QString& paraText,
        const TextFormatting& formatLayout,
        const ValueListFormatData& paraFormatDataList);
    void processNormalText ( const QString& paraText,
        const TextFormatting& formatLayout,
        const FormatData& formatData);
    void processVariable ( const QString& paraText,
        const TextFormatting& formatLayout,
        const FormatData& formatData);
    void processAnchor ( const QString& paraText,
        const TextFormatting& formatLayout,
        const FormatData& formatData);
    QString textFormatToAbiProps(const TextFormatting& formatOrigin,
        const TextFormatting& formatData, const bool force);
    QString layoutToParagraphStyle(const LayoutData& layoutOrigin,
        const LayoutData& layout, const bool force);
    QString escapeOOText(const QString& strText) const;
    bool makeTable(const FrameAnchor& anchor);
    bool makePicture(const FrameAnchor& anchor);
    bool convertUnknownPicture(const QString& name, const QString& extension, QByteArray& image);
    void writeAbiProps(const TextFormatting& formatLayout, const TextFormatting& format);
    void declareFont(const QString& fontName);
    void writeContentXml(void);
    void writeStylesXml(void);
    void writeStartOfFile(const QString& type);
private: // ZIP methids
    bool zipPrepareWriting(const QString& name);
    bool zipDoneWriting(void);
    bool zipWriteData(const char* str);
    bool zipWriteData(const QByteArray& array);
    bool zipWriteData(const QCString& cstr);
    bool zipWriteData(const QString& str); // Assumes UTF-8
private:
    QTextStream* m_streamOut;
    QString m_pagesize;
    StyleMap m_styleMap;
    double m_paperBorderTop,m_paperBorderLeft,m_paperBorderBottom,m_paperBorderRight;
    KWEFDocumentInfo m_docInfo; // document information
    QByteArray m_contentBody; // office:body element of content.xml
    KZip* m_zip;
    QStringList m_fontNames; // List of used font names
    ulong m_pictureNumber; // Number of picture (increment *before* use)
    QString m_styles;
    uint m_size; // Size of ZIP entry
    int m_paperFormat;
    double m_paperWidth;
    double m_paperHeight;
    int m_paperOrientation;
};
#endif // _EXPORTFILTER_H
