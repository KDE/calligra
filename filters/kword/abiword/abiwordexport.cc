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

#include <qmap.h>
#include <qiodevice.h>
#include <qtextstream.h>
#include <qdom.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <Q3CString>

#include <kdebug.h>
#include <kcodecs.h>
#include <kfilterdev.h>
#include <kgenericfactory.h>
#include <kimageio.h>

#include <KoPageLayout.h>
#include <KoFilterChain.h>
#include <KoPictureKey.h>

#include <KWEFStructures.h>
#include <KWEFUtil.h>
#include <KWEFBaseWorker.h>
#include <KWEFKWordLeader.h>

#include <abiwordexport.h>
#include <abiwordexport.moc>

class ABIWORDExportFactory : KGenericFactory<ABIWORDExport, KoFilter>
{
public:
    ABIWORDExportFactory(void) : KGenericFactory<ABIWORDExport, KoFilter> ("kwordabiwordexport")
    {}
protected:
    virtual void setupTranslations( void )
    {
        KGlobal::locale()->insertCatalog( "kofficefilters" );
    }
};

K_EXPORT_COMPONENT_FACTORY( libabiwordexport, ABIWORDExportFactory() )

class StyleMap : public QMap<QString,LayoutData>
{
public:
    StyleMap(void) {}
    ~StyleMap(void) {}
};

class AbiWordWorker : public KWEFBaseWorker
{
public:
    AbiWordWorker(void);
    virtual ~AbiWordWorker(void) { delete m_streamOut; delete m_ioDevice; }
public:
    virtual bool doOpenFile(const QString& filenameOut, const QString& to);
    virtual bool doCloseFile(void); // Close file in normal conditions
    virtual bool doOpenDocument(void);
    virtual bool doCloseDocument(void);
    virtual bool doFullParagraph(const QString& paraText, const LayoutData& layout,
        const ValueListFormatData& paraFormatDataList);
    virtual bool doOpenTextFrameSet(void); // AbiWord's <section>
    virtual bool doCloseTextFrameSet(void); // AbiWord's </section>
    virtual bool doFullPaperFormat(const int format,
        const double width, const double height, const int orientation); // Calc AbiWord's <papersize>
    virtual bool doFullPaperBorders (const double top, const double left,
        const double bottom, const double right); // Like KWord's <PAPERBORDERS>
    virtual bool doCloseHead(void); // Write <papersize>
    virtual bool doOpenStyles(void); // AbiWord's <styles>
    virtual bool doCloseStyles(void); // AbiWord's </styles>
    virtual bool doFullDefineStyle(LayoutData& layout); // AbiWord's <s></s>
    virtual bool doOpenSpellCheckIgnoreList (void); // AbiWord's <ignorewords>
    virtual bool doCloseSpellCheckIgnoreList (void); // AbiWord's </ignorewords>
    virtual bool doFullSpellCheckIgnoreWord (const QString& ignoreword); // AbiWord's <iw>
    virtual bool doFullDocumentInfo(const KWEFDocumentInfo& docInfo); // AbiWord's <metadata>
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
        const TextFormatting& formatData, const bool force) const;
    QString layoutToCss(const LayoutData& layoutOrigin,
        const LayoutData& layout, const bool force) const;
    QString escapeAbiWordText(const QString& strText) const;
    bool makeTable(const FrameAnchor& anchor);
    bool makePicture(const FrameAnchor& anchor);
    void writeAbiProps(const TextFormatting& formatLayout, const TextFormatting& format);
    void writePictureData(const QString& koStoreName, const QString& keyName);
    QString transformToTextDate(const QDateTime& dt);
private:
    QIODevice* m_ioDevice;
    QTextStream* m_streamOut;
    QString m_pagesize; // Buffer for the <pagesize> tag
    QMap<QString,KoPictureKey> m_mapPictureData;
    StyleMap m_styleMap;
    double m_paperBorderTop,m_paperBorderLeft,m_paperBorderBottom,m_paperBorderRight;
    bool m_inIgnoreWords; // true if <ignorewords> has been written
    KWEFDocumentInfo m_docInfo; // document information
};

AbiWordWorker::AbiWordWorker(void) : m_ioDevice(NULL), m_streamOut(NULL),
    m_paperBorderTop(0.0),m_paperBorderLeft(0.0),
    m_paperBorderBottom(0.0),m_paperBorderRight(0.0)
{
}

QString AbiWordWorker::escapeAbiWordText(const QString& strText) const
{
    // Escape quotes (needed in attributes)
    // Escape apostrophs (allowed by XML)
    return KWEFUtil::EscapeSgmlText(NULL,strText,true,true);
}

bool AbiWordWorker::doOpenFile(const QString& filenameOut, const QString& )
{
    kDebug(30506) << "Opening file: " << filenameOut
        << " (in AbiWordWorker::doOpenFile)" << endl;
    //Find the last extension
    QString strExt;
    const int result=filenameOut.findRev('.');
    if (result>=0)
    {
        strExt=filenameOut.mid(result);
    }

    QString strMimeType; // Mime type of the compressor

    if ((strExt==".gz")||(strExt==".GZ")        //in case of .abw.gz (logical extension)
        ||(strExt==".zabw")||(strExt==".ZABW")) //in case of .zabw (extension used prioritary with AbiWord)
    {
        // Compressed with gzip
        strMimeType="application/x-gzip";
    }
    else if ((strExt==".bz2")||(strExt==".BZ2") //in case of .abw.bz2 (logical extension)
        ||(strExt==".bzabw")||(strExt==".BZABW")) //in case of .bzabw (extension used prioritary with AbiWord)
    {
        // Compressed with bzip2
        strMimeType="application/x-bzip2";
    }
    else
    {
        // No compression
        strMimeType="text/plain";
    }

    kDebug(30506) << "Compression: " << strMimeType << endl;

    m_ioDevice = KFilterDev::deviceForFile(filenameOut,strMimeType);

    if (!m_ioDevice)
    {
        kError(30506) << "No output file! Aborting!" << endl;
        return false;
    }

    if ( !m_ioDevice->open (QIODevice::WriteOnly) )
    {
        kError(30506) << "Unable to open output file! Aborting!" << endl;
        return false;
    }

    m_streamOut=new QTextStream(m_ioDevice);

    // We only export in UTF-8 (are there AbiWord ports that cannot read UTF-8? Be careful SVG uses UTF-8 too!)
    m_streamOut->setEncoding( QTextStream::UnicodeUTF8 );
    return true;
}

bool AbiWordWorker::doCloseFile(void)
{
    delete m_streamOut;
    m_streamOut=NULL;
    if (m_ioDevice)
        m_ioDevice->close();
    return (m_ioDevice);
}

bool AbiWordWorker::doOpenDocument(void)
{
    kDebug(30506)<< "AbiWordWorker::doOpenDocument" << endl;
    // Make the file header

    // First the XML header in UTF-8 version
    // (AbiWord and QT handle UTF-8 well, so we stay with this encoding!)
    *m_streamOut << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

    // NOTE: AbiWord CVS 2002-02-?? has a new DOCTYPE
    *m_streamOut << "<!DOCTYPE abiword PUBLIC \"-//ABISOURCE//DTD AWML 1.0 Strict//EN\"";
    *m_streamOut << " \"http://www.abisource.com/awml.dtd\">\n";

    // First magic: "<abiword"
    *m_streamOut << "<abiword";
    // AbiWord CVS 2002-02-23 defines a default namespace.
    *m_streamOut << " xmlns=\"http://www.abisource.com/awml.dtd\"";
    // As we do not use xmlns:awml, do we need to define it?
    // *m_streamOut << " xmlns:awml=\"http://www.abisource.com/awml.dtd\"";
    *m_streamOut << " xmlns:xlink=\"http://www.w3.org/1999/xlink\"";
    // AbiWord CVS 2002-02-22 defines other namespaces, which we are not using.
    // AbiWord CVS 2002-12-23 has no fileformat attribute anymore
    // ### TODO: add document language and document direction of writing.
    *m_streamOut << " xml:space=\"preserve\" version=\"1.1.2\" template=\"false\" styles=\"unlocked\">\n";
    // Second magic: "<!-- This file is an AbiWord document."
    // TODO/FIXME: write as much spaces as AbiWord does for the following line.
    *m_streamOut << "<!-- This file is an AbiWord document. -->\n";
    // We have chosen NOT to have the full comment header that AbiWord files normally have.
    // ### TODO: perhaps we should add the comment: do not edit the file
    *m_streamOut << "\n";


    return true;
}

void AbiWordWorker::writePictureData(const QString& koStoreName, const QString& keyName)
{
    kDebug(30506) << "AbiWordWorker::writeImageData" << endl;

    QByteArray image;

    QString strExtension(koStoreName.lower());
    const int result=koStoreName.findRev(".");
    if (result>=0)
    {
        strExtension=koStoreName.mid(result+1);
    }

    bool isImageLoaded=false;

    if (strExtension=="png")
    {
        isImageLoaded=loadSubFile(koStoreName,image);
    }
    else
    {
        // All other picture types must be converted to PNG
        //   (yes, even JPEG, SVG or WMF!)
        isImageLoaded=loadAndConvertToImage(koStoreName,strExtension,"PNG",image);
    }

    if (isImageLoaded)
    {
        *m_streamOut << "<d name=\"" << keyName << "\""
            << " base64=\"yes\""
            << " mime=\"image/png\">\n";

        Q3CString base64=KCodecs::base64Encode(image,true);

        *m_streamOut << base64 << "\n"; // QCString is taken as Latin1 by QTextStream

        *m_streamOut << "</d>\n";
    }
    else
    {
        kWarning(30506) << "Unable to load picture: " << koStoreName << endl;
    }
}

bool AbiWordWorker::doCloseDocument(void)
{
    // Before writing the <data> element,
    //  we must be sure that we have data and that we can retrieve it.

    if (m_kwordLeader && !m_mapPictureData.isEmpty())
    {
        *m_streamOut << "<data>\n";

        QMap<QString,KoPictureKey>::ConstIterator it;
		QMap<QString,KoPictureKey>::ConstIterator end(m_mapPictureData.end());
        // all images first
        for (it=m_mapPictureData.begin(); it!=end; ++it)
        {
            // Warning: do not mix up KWord's key and the iterator's key!
            writePictureData(it.key(),it.data().filename());
        }

        *m_streamOut << "</data>\n";
    }

    *m_streamOut << "</abiword>\n"; //Close the file for XML
    return true;
}

bool AbiWordWorker::doOpenTextFrameSet(void)
{
    *m_streamOut << "<section props=\"";
    *m_streamOut << "page-margin-top: ";
    *m_streamOut << m_paperBorderTop;
    *m_streamOut << "pt; ";
    *m_streamOut << "page-margin-left: ";
    *m_streamOut << m_paperBorderLeft;
    *m_streamOut << "pt; ";
    *m_streamOut << "page-margin-bottom: ";
    *m_streamOut << m_paperBorderBottom;
    *m_streamOut << "pt; ";
    *m_streamOut << "page-margin-right: ";
    *m_streamOut << m_paperBorderRight;
    *m_streamOut << "pt"; // Last one, so no semi-comma
    *m_streamOut << "\">\n";
    return true;
}

bool AbiWordWorker::doCloseTextFrameSet(void)
{
    *m_streamOut << "</section>\n";
    return true;
}

bool AbiWordWorker::doOpenStyles(void)
{
    *m_streamOut << "<styles>\n";
    return true;
}

bool AbiWordWorker::doCloseStyles(void)
{
    *m_streamOut << "</styles>\n";
    return true;
}

QString AbiWordWorker::textFormatToAbiProps(const TextFormatting& formatOrigin,
    const TextFormatting& formatData, const bool force) const
{
    // TODO: rename variable formatData
    QString strElement; // TODO: rename this variable

    // Font name
    QString fontName = formatData.fontName;
    if ( !fontName.isEmpty()
        && (force || (formatOrigin.fontName!=formatData.fontName)))
    {
        strElement+="font-family: ";
        strElement+= escapeAbiWordText(fontName); // TODO: add alternative font names
        strElement+="; ";
    }

    if (force || (formatOrigin.italic!=formatData.italic))
    {
        // Font style
        strElement+="font-style: ";
        if ( formatData.italic )
        {
            strElement+="italic";
        }
        else
        {
            strElement+="normal";
        }
        strElement+="; ";
    }

    if (force || ((formatOrigin.weight>=75)!=(formatData.weight>=75)))
    {
        strElement+="font-weight: ";
        if ( formatData.weight >= 75 )
        {
            strElement+="bold";
        }
        else
        {
            strElement+="normal";
        }
        strElement+="; ";
    }

    if (force || (formatOrigin.fontSize!=formatData.fontSize))
    {
        const int size=formatData.fontSize;
        if (size>0)
        {
            // We use absolute font sizes.
            strElement+="font-size: ";
            strElement+=QString::number(size,10);
            strElement+="pt; ";
        }
    }

    if (force || (formatOrigin.fgColor!=formatData.fgColor))
    {
        if ( formatData.fgColor.isValid() )
        {
            // Give colour
            strElement+="color: ";

            // No leading # (unlike CSS2)
            // We must have two hex digits for each colour channel!
            const int red=formatData.fgColor.red();
            strElement += QString::number((red&0xf0)>>4,16);
            strElement += QString::number(red&0x0f,16);

            const int green=formatData.fgColor.green();
            strElement += QString::number((green&0xf0)>>4,16);
            strElement += QString::number(green&0x0f,16);

            const int blue=formatData.fgColor.blue();
            strElement += QString::number((blue&0xf0)>>4,16);
            strElement += QString::number(blue&0x0f,16);

            strElement+="; ";
        }
    }

    if (force || (formatOrigin.bgColor!=formatData.bgColor))
    {
        if ( formatData.bgColor.isValid() )
        {
            // Give background colour
            strElement+="bgcolor: ";

            // No leading # (unlike CSS2)
            // We must have two hex digits for each colour channel!
            const int red=formatData.bgColor.red();
            strElement += QString::number((red&0xf0)>>4,16);
            strElement += QString::number(red&0x0f,16);

            const int green=formatData.bgColor.green();
            strElement += QString::number((green&0xf0)>>4,16);
            strElement += QString::number(green&0x0f,16);

            const int blue=formatData.bgColor.blue();
            strElement += QString::number((blue&0xf0)>>4,16);
            strElement += QString::number(blue&0x0f,16);

            strElement+="; ";
        }
    }

    if (force || (formatOrigin.underline!=formatData.underline)
        || (formatOrigin.strikeout!=formatData.strikeout))
    {
        strElement+="text-decoration: ";
        if ( formatData.underline )
        {
            strElement+="underline";
        }
        else if ( formatData.strikeout )
        {
            strElement+="line-through";
        }
        else
        {
            strElement+="none";
        }
        strElement+="; ";
    }

    return strElement;
}

bool AbiWordWorker::makeTable(const FrameAnchor& anchor)
{
#if 0
    *m_streamOut << "</p>\n"; // Close previous paragraph ### TODO: do it correctly like for HTML
    *m_streamOut << "<table>\n";
#endif

    Q3ValueList<TableCell>::ConstIterator itCell;
    for (itCell=anchor.table.cellList.begin();
        itCell!=anchor.table.cellList.end(); itCell++)
    {
#if 0
        // ### TODO: rowspan, colspan

        // AbiWord seems to work by attaching to the cell borders
        *m_streamOut << "<cell props=\"";
        *m_streamOut << "left-attach:" << (*itCell).col << "; ";
        *m_streamOut << "right-attach:" << (*itCell).col + 1 << "; ";
        *m_streamOut << "top-attach:" << (*itCell).row << "; ";
        *m_streamOut << "bot-attach:" << (*itCell).row + 1;
        *m_streamOut << "\">\n";
#endif
        if (!doFullAllParagraphs(*(*itCell).paraList))
        {
            return false;
        }
#if 0
        *m_streamOut << "</cell>\n";
#endif
    }
#if 0
    *m_streamOut << "</table>\n";
    *m_streamOut << "<p>\n"; // Re-open the "previous" paragraph ### TODO: do it correctly like for HTML
#endif
    return true;
}

bool AbiWordWorker::makePicture(const FrameAnchor& anchor)
{
    kDebug(30506) << "New image/clipart: " << anchor.picture.koStoreName
        << " , " << anchor.picture.key.toString() << endl;

    const double height=anchor.frame.bottom - anchor.frame.top;
    const double width =anchor.frame.right  - anchor.frame.left;

    // TODO: we are only using the filename, not the rest of the key
    // TODO:  (bad if there are two images of the same name, but of a different key)
    *m_streamOut << "<image dataid=\"" << anchor.picture.key.filename() << "\"";
    *m_streamOut << " props= \"height:" << height << "pt;width:" << width << "pt\"";
    *m_streamOut << "/>"; // NO end of line!
    // TODO: other props for image

    m_mapPictureData[anchor.picture.koStoreName]=anchor.picture.key;

    return true;
}

void AbiWordWorker::writeAbiProps (const TextFormatting& formatLayout, const TextFormatting& format)
{
    QString abiprops=textFormatToAbiProps(formatLayout,format,false);

    // Erase the last semi-comma (as in CSS2, semi-commas only separate instructions and do not terminate them)
    const int result=abiprops.findRev(";");

    if (result>=0)
    {
        // Remove the last semi-comma and the space thereafter
        abiprops.remove(result,2);
    }

    if (!abiprops.isEmpty())
    {
        *m_streamOut << " props=\"" << abiprops << "\"";
    }
}

void AbiWordWorker::processNormalText ( const QString &paraText,
    const TextFormatting& formatLayout,
    const FormatData& formatData)
{
    // Retrieve text and escape it
    QString partialText=escapeAbiWordText(paraText.mid(formatData.pos,formatData.len));

    // Replace line feeds by line breaks
    int pos;
    while ((pos=partialText.find(QChar(10)))>-1)
    {
        partialText.replace(pos,1,"<br/>");
    }

    if (formatData.text.missing)
    {
        // It's just normal text, so we do not need a <c> element!
        *m_streamOut << partialText;
    }
    else
    { // Text with properties, so use a <c> element!
        *m_streamOut << "<c";
        writeAbiProps(formatLayout,formatData.text);
        *m_streamOut << ">" << partialText << "</c>";
    }
}

void AbiWordWorker::processVariable ( const QString&,
    const TextFormatting& formatLayout,
    const FormatData& formatData)
{
    if (0==formatData.variable.m_type)
    {
        // As AbiWord's field is inflexible, we cannot make the date custom
        *m_streamOut << "<field type=\"date_ntdfl\"";
        writeAbiProps(formatLayout,formatData.text);
        *m_streamOut << "/>";
    }
    else if (2==formatData.variable.m_type)
    {
        // As AbiWord's field is inflexible, we cannot make the time custom
        *m_streamOut << "<field type=\"time\"";
        writeAbiProps(formatLayout,formatData.text);
        *m_streamOut << "/>";
    }
    else if (4==formatData.variable.m_type)
    {
        // As AbiWord's field is inflexible, we cannot make the time custom
        QString strFieldType;
        if (formatData.variable.isPageNumber())
        {
            strFieldType="page_number";
        }
        else if (formatData.variable.isPageCount())
        {
            strFieldType="page_count";
        }
        if (strFieldType.isEmpty())
        {
            // Unknown subtype, therefore write out the result
            *m_streamOut << formatData.variable.m_text;
        }
        else
        {
            *m_streamOut << "<field type=\"" << strFieldType <<"\"";
            writeAbiProps(formatLayout,formatData.text);
            *m_streamOut << "/>";
        }
    }
    else if (9==formatData.variable.m_type)
    {
        // A link
        *m_streamOut << "<a xlink:href=\""
            << escapeAbiWordText(formatData.variable.getHrefName())
            << "\"><c";  // In AbiWord, an anchor <a> has always a <c> child
        writeAbiProps(formatLayout,formatData.text);
        *m_streamOut << ">"
            << escapeAbiWordText(formatData.variable.getLinkName())
            << "</c></a>";
    }
#if 0
                else if (11==(*paraFormatDataIt).variable.m_type)
                {
                    // Footnote
                    QString value = (*paraFormatDataIt).variable.getFootnoteValue();
                    bool automatic = (*paraFormatDataIt).variable.getFootnoteAuto();
                    Q3ValueList<ParaData> *paraList = (*paraFormatDataIt).variable.getFootnotePara();
                    if( paraList )
                    {
                        QString fstr;
                        Q3ValueList<ParaData>::ConstIterator it;
                        for (it=paraList->begin();it!=paraList->end();it++)
                            fstr += ProcessParagraphData( (*it).text, (*it).layout,(*it).formattingList);
                        str += "{\\super ";
                        str += automatic ? "\\chftn " : value;
                        str += "{\\footnote ";
                        str += "{\\super ";
                        str += automatic ? "\\chftn " : value;
                        str += fstr;
                        str += " }";
                        str += " }";
                        str += " }";
                    }
                }
#endif
    else
    {
        // Generic variable
        *m_streamOut << formatData.variable.m_text;
    }
}

void AbiWordWorker::processAnchor ( const QString&,
    const TextFormatting& /*formatLayout*/, //TODO
    const FormatData& formatData)
{
    // We have an image or a table
    if ( (2==formatData.frameAnchor.type) // <IMAGE> or <PICTURE>
        || (5==formatData.frameAnchor.type) ) // <CLIPART>
    {
        makePicture(formatData.frameAnchor);
    }
    else if (6==formatData.frameAnchor.type)
    {
        makeTable(formatData.frameAnchor);
    }
    else
    {
        kWarning(30506) << "Unsupported anchor type: "
            << formatData.frameAnchor.type << endl;
    }
}

void AbiWordWorker::processParagraphData ( const QString &paraText,
    const TextFormatting& formatLayout,
    const ValueListFormatData &paraFormatDataList)
{
    if ( paraText.length () > 0 )
    {
        ValueListFormatData::ConstIterator  paraFormatDataIt;

        for ( paraFormatDataIt = paraFormatDataList.begin ();
              paraFormatDataIt != paraFormatDataList.end ();
              paraFormatDataIt++ )
        {
            if (1==(*paraFormatDataIt).id)
            {
                processNormalText(paraText, formatLayout, (*paraFormatDataIt));
            }
            else if (4==(*paraFormatDataIt).id)
            {
                processVariable(paraText, formatLayout, (*paraFormatDataIt));
            }
            else if (6==(*paraFormatDataIt).id)
            {
                processAnchor(paraText, formatLayout, (*paraFormatDataIt));
            }
        }
    }
}

QString AbiWordWorker::layoutToCss(const LayoutData& layoutOrigin,
    const LayoutData& layout, const bool force) const
{
    QString props;

    if (force || (layoutOrigin.alignment!=layout.alignment))
    {
        // Check if the current alignment is a valid one for AbiWord.
        if ((layout.alignment == "left") || (layout.alignment == "right")
            || (layout.alignment == "center")  || (layout.alignment == "justify"))
        {
            props += "text-align:";
            props += layout.alignment;
            props += "; ";
        }
        else if (layout.alignment == "auto")
        {
            // We assume a left alignment as AbiWord is not really bi-di (and this filter even less.)
            props += "text-align:left; ";
        }
        else
        {
            kWarning(30506) << "Unknown alignment: " << layout.alignment << endl;
        }
    }

    // TODO/FIXME: what if all tabulators must be erased?
#if 0
    // DEPRECATED!
    if (!layout.tabulator.isEmpty()
        && (force || (layoutOrigin.tabulator!=layout.tabulator)))
    {
        props += "tabstops:";
        props += layout.tabulator;
        props += "; ";
    }
#endif
    if (!layout.tabulatorList.isEmpty()
        && (force || (layoutOrigin.tabulatorList!=layout.tabulatorList) ))
    {
        props += "tabstops:";
        bool first=true;
        TabulatorList::ConstIterator it;
		TabulatorList::ConstIterator end(layout.tabulatorList.end());
        for (it=layout.tabulatorList.begin();it!=end;++it)
        {
            if (first)
            {
                first=false;
            }
            else
            {
                props += ",";
            }
            props += QString::number((*it).m_ptpos);
            props += "pt";

            switch ((*it).m_type)
            {
                case 0:  props += "/L"; break;
                case 1:  props += "/C"; break;
                case 2:  props += "/R"; break;
                case 3:  props += "/D"; break;
                default: props += "/L";
            }

            props += "0"; // No filling
        }
        props += "; ";
    }

    if ((layout.indentLeft>=0.0)
        && (force || (layoutOrigin.indentLeft!=layout.indentLeft)))
    {
        props += QString("margin-left:%1pt; ").arg(layout.indentLeft);
    }

    if ((layout.indentRight>=0.0)
        && (force || (layoutOrigin.indentRight!=layout.indentRight)))
    {
        props += QString("margin-right:%1pt; ").arg(layout.indentRight);
    }

    if (force || (layoutOrigin.indentLeft!=layout.indentLeft))
    {
        props += "text-indent: ";
        props += QString::number(layout.indentFirst);
        props += "pt; ";
    }

    if ((layout.marginBottom>=0.0)
        && ( force || ( layoutOrigin.marginBottom != layout.marginBottom ) ) )
    {
       props += QString("margin-bottom:%1pt; ").arg(layout.marginBottom);
    }

    if ((layout.marginTop>=0.0)
        && ( force || ( layoutOrigin.marginTop != layout.marginTop ) ) )
    {
       props += QString("margin-top:%1pt; ").arg(layout.marginTop);
    }

    if (force
        || ( layoutOrigin.lineSpacingType != layout.lineSpacingType )
        || ( layoutOrigin.lineSpacing != layout.lineSpacing ) )
    {
        switch ( layout.lineSpacingType )
        {
        case LayoutData::LS_CUSTOM:
            {
                // We have a custom line spacing (in points). However AbiWord cannot do it, so transform in "at-least"
                props += "line-height=:";
                props += QString::number( layout.lineSpacing ); // ### TODO: rounding?
                props += "pt+; ";
                break;
            }
        case LayoutData::LS_SINGLE:
            {
                props += "line-height:1.0; "; // One
                break;
            }
        case LayoutData::LS_ONEANDHALF:
            {
                props += "line-height:1.5; "; // One-and-half
                break;
            }
        case LayoutData::LS_DOUBLE:
            {
                props += "line-height:2.0; "; // Two
                break;
            }
        case LayoutData::LS_MULTIPLE:
            {
                props += "line-height:";
                props += QString::number( layout.lineSpacing ); // ### TODO: rounding?
                props += "; ";
                break;
            }
        case LayoutData::LS_FIXED:
            {
                // We have a fixed line height (in points)
                props += "line-height:";
                props += QString::number( layout.lineSpacing ); // ### TODO: rounding?
                props += "pt; ";
                break;
            }
        case LayoutData::LS_ATLEAST:
            {
                // We have an "at-least" line height (in points)
                props += "line-height=:";
                props += QString::number( layout.lineSpacing ); // ### TODO: rounding?
                props += "pt+; "; // The + makes the difference
                break;
            }
        default:
            {
                kWarning(30506) << "Unsupported lineSpacingType: " << layout.lineSpacingType << " (Ignoring!)" << endl;
                break;
            }
        }
    }

    // Add all AbiWord properties collected in the <FORMAT> element
    props += textFormatToAbiProps(layoutOrigin.formatData.text,layout.formatData.text,force);

    return props;
}

bool AbiWordWorker::doFullParagraph(const QString& paraText, const LayoutData& layout,
    const ValueListFormatData& paraFormatDataList)
{
    QString style=layout.styleName;

    const LayoutData& styleLayout=m_styleMap[style];

    QString props=layoutToCss(styleLayout,layout,false);

    *m_streamOut << "<p";
    if (!style.isEmpty())
    {
        *m_streamOut << " style=\"" << EscapeXmlText(style,true,true) << "\"";
    }
    if (!props.isEmpty())
    {
        // Find the last semi-comma
        // Note: as in CSS2, semi-commas only separates instructions (like in PASCAL) and do not terminate them (like in C)
        const int result=props.findRev(";");
        if (result>=0)
        {
            // Remove the last semi-comma and the space thereafter
            props.remove(result,2);
        }

        *m_streamOut << " props=\"" << props << "\"";
    }
    *m_streamOut << ">";  //Warning: No trailing white space or else it's in the text!!!

    // Before processing the text, test if we have a page break
    if (layout.pageBreakBefore)
    {
        // We have a page break before the paragraph
        *m_streamOut << "<pbr/>";
    }

    processParagraphData(paraText, layout.formatData.text, paraFormatDataList);

    // Before closing the paragraph, test if we have a page break
    if (layout.pageBreakAfter)
    {
        // We have a page break after the paragraph
        *m_streamOut << "<pbr/>";
    }

    *m_streamOut << "</p>\n";
    return true;
}

bool AbiWordWorker::doFullDefineStyle(LayoutData& layout)
{
    //Register style in the style map
    m_styleMap[layout.styleName]=layout;

    *m_streamOut << "<s";

    // TODO: cook the style name to the standard style names in AbiWord
    *m_streamOut << " name=\"" << EscapeXmlText(layout.styleName,true,true) << "\"";
    *m_streamOut << " followedby=\"" << EscapeXmlText(layout.styleFollowing,true,true) << "\"";

    if ( (layout.counter.numbering == CounterData::NUM_CHAPTER)
        && (layout.counter.depth<10) )
    {
        *m_streamOut << " level=\"";
        *m_streamOut << QString::number(layout.counter.depth+1,10);
        *m_streamOut << "\"";
    }

    QString abiprops=layoutToCss(layout,layout,true);

    const int result=abiprops.findRev(";");
    if (result>=0)
    {
        // Remove the last semi-comma and the space thereafter
        abiprops.remove(result,2);
    }

    *m_streamOut << " props=\"" << abiprops << "\"";

    *m_streamOut << "/>\n";

    return true;
}

bool AbiWordWorker::doFullPaperFormat(const int format,
            const double width, const double height, const int orientation)
{
    QString outputText = "<pagesize ";

    switch (format)
    {
        // ISO A formats
        case PG_DIN_A0: // ISO A0
        case PG_DIN_A1: // ISO A1
        case PG_DIN_A2: // ISO A2
        case PG_DIN_A3: // ISO A3
        case PG_DIN_A4: // ISO A4
        case PG_DIN_A5: // ISO A5
        case PG_DIN_A6: // ISO A6
        // ISO B formats
        case PG_DIN_B0: // ISO B0
        case PG_DIN_B1: // ISO B1
        case PG_DIN_B2: // ISO B2
        case PG_DIN_B3: // ISO B3
        case PG_DIN_B4: // ISO B4
        case PG_DIN_B5: // ISO B5
        case PG_DIN_B6: // ISO B6
        // American formats
        case PG_US_LETTER: // US Letter
        case PG_US_LEGAL:  // US Legal
        {
            QString pagetype=KoPageFormat::formatString(KoFormat(format));
            outputText+="pagetype=\"";
            outputText+=pagetype;

            QString strWidth, strHeight, strUnits;
            KWEFUtil::GetNativePaperFormat(format, strWidth, strHeight, strUnits);
            outputText+="\" width=\"";
            outputText+=strWidth;
            outputText+="\" height=\"";
            outputText+=strHeight;
            outputText+="\" units=\"";
            outputText+=strUnits;
            outputText+="\" ";
            break;
        }
        case PG_US_EXECUTIVE: // US Executive (does not exists in AbiWord!)
        {
            // FIXME/TODO: AbiWord (CVS 2001-04-25) seems not to like custom formats, so avoid them for now!
#if 0
            outputText += "pagetype=\"Custom\" width=\"7.5\" height=\"10.0\" units=\"inch\" ";
#else
            // As replacement, use the slightly bigger "letter" format.
            outputText += "pagetype=\"Letter\" width=\"8.5\" height=\"11.0\" units=\"inch\" ";
#endif
            break;
        }
        // Other format not supported yet by AbiWord CVS 2001-04-25)
        case PG_DIN_A7: // ISO A7
        case PG_DIN_A8: // ISO A8
        case PG_DIN_A9: // ISO A9
        case PG_DIN_B10: // ISO B10
        // Other formats
        case PG_SCREEN: // Screen
        case PG_CUSTOM: // Custom
        default:
        {
             // FIXME/TODO: AbiWord (CVS 2001-04-25) seems not to like custom formats, so avoid them for now!
            if ((width<=1.0) || (height<=1.0) || true)
            {
                // Height or width is ridiculous, so assume A4 format
                outputText += "pagetype=\"A4\" width=\"21.0\" height=\"29.7\" units=\"cm\" ";
            }
            else
            {   // We prefer to use inches, as to limit rounding errors (page size is in points!)
                outputText += QString("pagetype=\"Custom\" width=\"%1\" height=\"%2\" units=\"inch\" ").arg(width/72.0).arg(height/72.0);
            }
            break;
        }
    }

    outputText += "orientation=\"";
    if (1==orientation)
    {
        outputText += "landscape";
    }
    else
    {
        outputText += "portrait";
    }
    outputText += "\" ";

    outputText += "page-scale=\"1.0\"/>\n"; // KWord has no page scale, so assume 100%

    m_pagesize=outputText;
    return true;
}

bool AbiWordWorker::doFullPaperBorders (const double top, const double left,
    const double bottom, const double right)
{
    m_paperBorderTop=top;
    m_paperBorderLeft=left;
    m_paperBorderBottom=bottom;
    m_paperBorderRight=right;
    return true;
}

bool AbiWordWorker::doCloseHead(void)
{
    if (!m_pagesize.isEmpty())
    {
        *m_streamOut << m_pagesize;
    }
    return true;
}

bool AbiWordWorker::doOpenSpellCheckIgnoreList (void)
{
    kDebug(30506) << "AbiWordWorker::doOpenSpellCheckIgnoreList" << endl;
    m_inIgnoreWords=false; // reset
    return true;
}

bool AbiWordWorker::doCloseSpellCheckIgnoreList (void)
{
    kDebug(30506) << "AbiWordWorker::doCloseSpellCheckIgnoreList" << endl;
    if (m_inIgnoreWords)
        *m_streamOut << "</ignorewords>\n";
    return true;
}

bool AbiWordWorker::doFullSpellCheckIgnoreWord (const QString& ignoreword)
{
    kDebug(30506) << "AbiWordWorker::doFullSpellCheckIgnoreWord: " << ignoreword << endl;
    if (!m_inIgnoreWords)
    {
        *m_streamOut << "<ignorewords>\n";
        m_inIgnoreWords=true;
    }
    *m_streamOut << " <iw>" << ignoreword << "</iw>\n";
    return true;
}

// Similar to QDateTime::toString, but guaranteed *not* to be translated
QString AbiWordWorker::transformToTextDate(const QDateTime& dt)
{
    if (dt.isValid())
    {
        QString result;

        const QDate date(dt.date());

        const char* dayName[7] = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };
        const int dow = date.dayOfWeek() - 1;
        if ((dow<0) || (dow>6))
            result += "Mon"; // Unknown day, rename it Monday.
        else
            result += dayName[dow];
        result += ' ';

        const char* monthName[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
        const int month = date.month() - 1;
        if ((month<0) || (month>11))
            result += "Jan"; // Unknown month, rename it January
        else
            result += monthName[month];
        result += ' ';

        QString temp;

        temp = "00";
        temp += QString::number(date.day(), 10);
        result += temp.right(2);
        result += ' ';

        const QTime time(dt.time());

        temp = "00";
        temp += QString::number(time.hour(), 10);
        result += temp.right(2);
        result += ':';

        temp = "00";
        temp += QString::number(time.minute(), 10);
        result += temp.right(2);
        result += ':';

        temp = "00";
        temp += QString::number(time.second(), 10);
        result += temp.right(2);
        result += ' ';

        temp = "0000";
        temp += QString::number(date.year(), 10);
        result += temp.right(4);

        return result;
    }
    else
    {
        // Invalid, so give back 1970-01-01
        return "Thu Jan 01 00:00:00 1970";
    }
}

bool AbiWordWorker::doFullDocumentInfo(const KWEFDocumentInfo& docInfo)
{
    m_docInfo=docInfo;

    *m_streamOut << "<metadata>\n";

    // First all Dublin Core data
    *m_streamOut << "<m key=\"dc.format\">application/x-abiword</m>\n";
    if (!m_docInfo.title.isEmpty())
    {
        *m_streamOut << "<m key=\"dc.title\">" << escapeAbiWordText(m_docInfo.title) << "</m>\n";
    }
    if (!m_docInfo.abstract.isEmpty())
    {
        *m_streamOut << "<m key=\"dc.description\">" << escapeAbiWordText(m_docInfo.abstract) << "</m>\n";
    }

    if ( !m_docInfo.keywords.isEmpty() )
    {
        *m_streamOut << "<m key=\"abiword.keywords\">" << escapeAbiWordText(m_docInfo.keywords) << "</m>\n";
    }
    if ( !m_docInfo.subject.isEmpty() )
    {
        *m_streamOut << "<m key=\"dc.subject\">" << escapeAbiWordText(m_docInfo.subject) << "</m>\n";
    }

    // Say who we are (with the CVS revision number) in case we have a bug in our filter output!
    *m_streamOut << "<m key=\"abiword.generator\">KWord Export Filter";

    QString strVersion("$Revision$");
    // Remove the dollar signs
    //  (We don't want that the version number changes if the AbiWord file is itself put in a CVS storage.)
    *m_streamOut << strVersion.mid(10).remove('$');

    *m_streamOut << "</m>\n";

    QDateTime now (QDateTime::currentDateTime(Qt::UTC)); // current time in UTC
    *m_streamOut << "<m key=\"abiword.date_last_changed\">"
         << escapeAbiWordText(transformToTextDate(now))
         << "</m>\n";

    *m_streamOut << "</metadata>\n";

    return true;
}


// ==========================================================================================

ABIWORDExport::ABIWORDExport(KoFilter */*parent*/, const char */*name*/, const QStringList &) :
                     KoFilter() {
}

KoFilter::ConversionStatus ABIWORDExport::convert( const QByteArray& from, const QByteArray& to )
{
    if ( to != "application/x-abiword" || from != "application/x-kword" )
    {
        return KoFilter::NotImplemented;
    }

    // We need KimageIO's help in AbiWordWorker::convertUnknownImage
    

    AbiWordWorker* worker=new AbiWordWorker();

    if (!worker)
    {
        kError(30506) << "Cannot create Worker! Aborting!" << endl;
        return KoFilter::StupidError;
    }

    KWEFKWordLeader* leader=new KWEFKWordLeader(worker);

    if (!leader)
    {
        kError(30506) << "Cannot create Worker! Aborting!" << endl;
        delete worker;
        return KoFilter::StupidError;
    }

    KoFilter::ConversionStatus result=leader->convert(m_chain,from,to);

    delete leader;
    delete worker;

    return result;
}
