// $Header$

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

#include <qmap.h>
#include <qiodevice.h>
#include <qtextstream.h>
#include <qdom.h>

#include <kdebug.h>
#include <kzip.h>

#include <koGlobal.h>
#include <koPictureKey.h>

#include <KWEFStructures.h>
#include <KWEFUtil.h>
#include <KWEFBaseWorker.h>
#include <KWEFKWordLeader.h>

#include "ExportFilter.h"

OOWriterWorker::OOWriterWorker(void) : m_streamOut(NULL),
    m_paperBorderTop(0.0),m_paperBorderLeft(0.0),
    m_paperBorderBottom(0.0),m_paperBorderRight(0.0), m_zip(NULL), m_pictureNumber(0),
    m_automaticParagraphStyleNumber(0), m_automaticTextStyleNumber(0),
    m_footnoteNumber(0)
{
}

QString OOWriterWorker::escapeOOText(const QString& strText) const
{
    // Escape quotes (needed in attributes)
    // Escape apostrophs (allowed by XML)
    return KWEFUtil::EscapeSgmlText(NULL,strText,true,true);
}

bool OOWriterWorker::doOpenFile(const QString& filenameOut, const QString& )
{
    kdDebug(30518) << "Opening file: " << filenameOut
        << " (in OOWriterWorker::doOpenFile)" << endl;

    m_zip=new KZip(filenameOut); // How to check failure?

    if (!m_zip->open(IO_WriteOnly))
    {
        kdError(30518) << "Could not open ZIP file for writing! Aborting!" << endl;
        delete m_zip;
        m_zip=NULL;
        return false;
    }

    m_streamOut=new QTextStream(m_contentBody, IO_WriteOnly);

    m_streamOut->setEncoding( QTextStream::UnicodeUTF8 );

    return true;
}

bool OOWriterWorker::zipPrepareWriting(const QString& name)
{
    if (!m_zip)
        return false;
    m_size=0;
    return m_zip->prepareWriting(name, QString::null, QString::null, 0);
}

bool OOWriterWorker::zipDoneWriting(void)
{
    if (!m_zip)
        return false;
    return m_zip->doneWriting(m_size);
}

bool OOWriterWorker::zipWriteData(const char* str)
{
    if (!m_zip)
        return false;
    const uint size=strlen(str);
    m_size+=size;
    return m_zip->writeData(str,size);
}

bool OOWriterWorker::zipWriteData(const QByteArray& array)
{
    if (!m_zip)
        return false;
    const uint size=array.size();
    m_size+=size;
    return m_zip->writeData(array.data(),size);
}

bool OOWriterWorker::zipWriteData(const QCString& cstr)
{
    if (!m_zip)
        return false;
    const uint size=cstr.length();
    m_size+=size;
    return m_zip->writeData(cstr.data(),size);
}

bool OOWriterWorker::zipWriteData(const QString& str)
{
    return zipWriteData(str.utf8());
}

void OOWriterWorker::writeStartOfFile(const QString& type)
{
    const bool noType=type.isEmpty();
    zipWriteData("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");

    zipWriteData("<!DOCTYPE office:document");
    if (!noType)
    {
        zipWriteData("-");
        zipWriteData(type);
    }
    zipWriteData(" PUBLIC \"-//OpenOffice.org//DTD OfficeDocument 1.0//EN\"");
    zipWriteData(" \"office.dtd\"");
    zipWriteData(">\n");

    zipWriteData("<office:document");
    if (!noType)
    {
        zipWriteData("-");
        zipWriteData(type);
    }

    // The name spaces used by OOWriter (those not used by this filter are commented out)
    zipWriteData(" xmlns:office=\"http://openoffice.org/2000/office\"");
    zipWriteData(" xmlns:style=\"http://openoffice.org/2000/style\"");
    zipWriteData(" xmlns:text=\"http://openoffice.org/2000/text\"");
    zipWriteData(" xmlns:table=\"http://openoffice.org/2000/table\"");
    zipWriteData(" xmlns:draw=\"http://openoffice.org/2000/drawing\"");
    zipWriteData(" xmlns:fo=\"http://www.w3.org/1999/XSL/Format\"");
    zipWriteData(" xmlns:xlink=\"http://www.w3.org/1999/xlink\"");
    //zipWriteData(" xmlns:number=\"http://openoffice.org/2000/datastyle\"");
    zipWriteData(" xmlns:svg=\"http://www.w3.org/2000/svg\"");
    //zipWriteData(" xmlns:chart=\"http://openoffice.org/2000/chart\"");
    //zipWriteData(" xmlns:dr3d=\"http://openoffice.org/2000/dr3d\"");
    //zipWriteData(" xmlns:math=\"http://www.w3.org/1998/Math/MathML"");
    //zipWriteData(" xmlns:form=\"http://openoffice.org/2000/form\"");
    //zipWriteData(" xmlns:script=\"http://openoffice.org/2000/script\"");

    zipWriteData(" office:class=\"text\"");

    // We are using an (OASIS) )extension compared to version 1.0, so we cannot write the version string.
    // (We do not even write it for context.xml and meta.xml, as OOWriter 1.0..1 does not like it in this case.)
    //zipWriteData("office:version=\"1.0\"");

    zipWriteData(">\n");
}

void OOWriterWorker::writeStylesXml(void)
{
    if (!m_zip)
        return;

    zipPrepareWriting("styles.xml");

    writeStartOfFile("styles");

    zipWriteData( " <office:font-decls>\n");
    for (QMap<QString,QString>::ConstIterator it=m_fontNames.begin(); it!=m_fontNames.end(); it++)
    {
        zipWriteData("  <style:font-decl style:name=\"");
        zipWriteData(escapeOOText(it.key()));
        zipWriteData("\" fo:font-family=\"");
        zipWriteData(escapeOOText(it.key()));
        zipWriteData("\" ");
        zipWriteData(it.data()); // already in XML, so do not escape
        zipWriteData(" />\n");
    }
    zipWriteData(" </office:font-decls>\n");

    zipWriteData(m_styles);

    zipWriteData(" <office:automatic-styles>\n");
    zipWriteData("  <style:page-master style:name=\"pm1\">\n");

    zipWriteData("   <style:properties ");

    zipWriteData(" fo:page-width=\"");
    zipWriteData(QString::number(m_paperWidth));
    zipWriteData("pt\" fo:page-height=\"");
    zipWriteData(QString::number(m_paperHeight));
    zipWriteData("pt\" ");

    zipWriteData("style:print-orientation=\"");
    if (1==m_paperOrientation)
    {
        zipWriteData("landscape");
    }
    else
    {
        zipWriteData("portrait");
    }

    zipWriteData("\" fo:margin-top=\"");
    zipWriteData(QString::number(m_paperBorderTop));
    zipWriteData("pt\" fo:margin-bottom=\"");
    zipWriteData(QString::number(m_paperBorderBottom));
    zipWriteData("pt\" fo:margin-left=\"");
    zipWriteData(QString::number(m_paperBorderLeft));
    zipWriteData("pt\" fo:margin-right=\"");
    zipWriteData(QString::number(m_paperBorderRight));
    zipWriteData("pt\"/>\n");

    zipWriteData("  </style:page-master>\n");
    zipWriteData(" </office:automatic-styles>\n");

    zipWriteData(" <office:master-styles>\n");
    zipWriteData("  <style:master-page style:name=\"Standard\" style:page-master-name=\"pm1\" />\n");
    zipWriteData(" </office:master-styles>\n");

    zipWriteData( "</office:document-styles>\n" );

    zipDoneWriting();
}

void OOWriterWorker::writeContentXml(void)
{
    if (!m_zip)
        return;

    zipPrepareWriting("content.xml");

    writeStartOfFile("content");

    zipWriteData( " <office:font-decls>\n");
    for (QMap<QString,QString>::ConstIterator it=m_fontNames.begin(); it!=m_fontNames.end(); it++)
    {
        zipWriteData("  <style:font-decl style:name=\"");
        zipWriteData(escapeOOText(it.key()));
        zipWriteData("\" fo:font-family=\"");
        zipWriteData(escapeOOText(it.key()));
        zipWriteData("\" ");
        zipWriteData(it.data()); // already in XML, so do not escape
        zipWriteData(" />\n");
    }
    zipWriteData(" </office:font-decls>\n");

    zipWriteData(" <office:automatic-styles>\n");
    zipWriteData(m_contentAutomaticStyles);
    zipWriteData(" </office:automatic-styles>\n");

    zipWriteData(m_contentBody);

    zipWriteData( "</office:document-content>\n" );

    zipDoneWriting();
}

bool OOWriterWorker::doCloseFile(void)
{
    kdDebug(30518)<< "OOWriterWorker::doCloseFile" << endl;
    if (m_zip)
    {
        writeContentXml();
        writeStylesXml();
        m_zip->close();
    }

    delete m_zip;
    m_zip=NULL;
    return true;
}

bool OOWriterWorker::doOpenDocument(void)
{
    kdDebug(30518)<< "OOWriterWorker::doOpenDocument" << endl;

    *m_streamOut << " <office:body>\n";

    return true;
}

bool OOWriterWorker::doCloseDocument(void)
{
    *m_streamOut << " </office:body>\n";
    return true;
}

QString OOWriterWorker::textFormatToStyle(const TextFormatting& formatOrigin,
    const TextFormatting& formatData, const bool force, QString& key)
{
    // TODO: rename variable formatData
    QString strElement; // TODO: rename this variable

    // Font name
    QString fontName = formatData.fontName;
    declareFont(fontName);
    if ( !fontName.isEmpty()
        && (force || (formatOrigin.fontName!=formatData.fontName)))
    {
        strElement+="style:font-name=\"";
        strElement+= escapeOOText(fontName);
        strElement+="\" ";
        key += fontName;
    }

    key += ",";

    if (force || (formatOrigin.italic!=formatData.italic))
    {
        // Font style
        strElement+="fo:font-style=\"";
        if ( formatData.italic )
        {
            strElement+="italic";
            key+='I';
        }
        else
        {
            strElement+="normal";
            key+='N';
        }
        strElement+="\" ";
    }

    key += ",";

    if (force || ((formatOrigin.weight>=75)!=(formatData.weight>=75)))
    {
        strElement+="fo:font-weight=\"";
        if ( formatData.weight >= 75 )
        {
            strElement+="bold";
            key+='B';
        }
        else
        {
            strElement+="normal";
            key+='N';
        }
        strElement+="\" ";
    }

    key += ",";

    if (force || (formatOrigin.fontSize!=formatData.fontSize))
    {
        const int size=formatData.fontSize;
        if (size>0)
        {
            strElement+="fo:font-size=\"";
            strElement+=QString::number(size,10);
            strElement+="pt\" ";
            key+=QString::number(size,10);
        }
    }

    key += ",";

    if (force || (formatOrigin.fgColor!=formatData.fgColor))
    {
        if ( formatData.fgColor.isValid() )
        {
            strElement+="fo:color=\"";
            strElement+=formatData.fgColor.name();
            strElement+="\" ";
            key+=formatData.fgColor.name();
        }
    }

    key += ",";

    if (force || (formatOrigin.bgColor!=formatData.bgColor))
    {
        if ( formatData.bgColor.isValid() )
        {
            strElement+="fo:background-color=\"";
            strElement+=formatData.bgColor.name();
            strElement+="\" ";
            key+=formatData.bgColor.name();
        }
    }

    key += ",";

    if (force || (formatOrigin.underline!=formatData.underline)
        || (formatOrigin.strikeout!=formatData.strikeout))
    {
        strElement+="fo:text-decoration=\"";
        if ( formatData.underline )
        {
            strElement+="underline";
            key+='U';
        }
        else if ( formatData.strikeout )
        {
            strElement+="line-through";
            key+='T';
        }
        else
        {
            strElement+="none";
            key+='N';
        }
        strElement+="\" ";
    }

    return strElement.stripWhiteSpace(); // Remove especially trailing spaces
}

bool OOWriterWorker::makeTable(const FrameAnchor& anchor)
{
#if 0
    *m_streamOut << "</abiword:p>\n"; // Close previous paragraph ### TODO: do it correctly like for HTML
    *m_streamOut << "<table:table>\n";
    // ### TODO: table:column
    // ### TODO: automatic styles

    QValueList<TableCell>::ConstIterator itCell;

    for (itCell=anchor.table.cellList.begin();
        itCell!=anchor.table.cellList.end(); itCell++)
    {
        // ### TODO: rowspan, colspan

        // AbiWord seems to work by attaching to the cell borders
        *m_streamOut << "<table:table-cell Table:value-type=\"string\">\n";

        if (!doFullAllParagraphs(*(*itCell).paraList))
        {
            return false;
        }

        *m_streamOut << "</table:table-cell>\n";
    }

    *m_streamOut << "</table:table>\n";
    *m_streamOut << "<abiword:p>\n"; // Re-open the "previous" paragraph ### TODO: do it correctly like for HTML
#endif
    return true;
}

bool OOWriterWorker::makePicture(const FrameAnchor& anchor)
{
    kdDebug(30518) << "New picture: " << anchor.picture.koStoreName
        << " , " << anchor.picture.key.toString() << endl;

    QString koStoreName(anchor.picture.koStoreName);

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
    else if ((strExtension=="jpg") || (strExtension=="jpeg"))
    {
        isImageLoaded=loadSubFile(koStoreName,image);
        strExtension="jpg";
    }
    else if ((strExtension=="gif")
        || (strExtension=="tif") || (strExtension=="tiff"))
        // ### TODO: Which other image formats does OOWriter support directly?
    {
        isImageLoaded=loadSubFile(koStoreName,image);
    }
    else
    {
        // All other picture types must be converted to PNG
        isImageLoaded=loadAndConvertToImage(koStoreName,strExtension,"PNG",image);
        strExtension="png";
    }

    if (!isImageLoaded)
    {
        kdWarning(30518) << "Unable to load picture: " << koStoreName << endl;
        return true;
    }

    kdDebug(30518) << "Picture loaded: " << koStoreName << endl;

    const double height=anchor.frame.bottom - anchor.frame.top;
    const double width =anchor.frame.right  - anchor.frame.left;

     // We need a 32 digit hex value of the picture number
     // Please note: it is an exact 32 digit value, truncated if the value is more than 512 bits wide. :-)
    QString number;
    number.fill('0',32);
    number += QString::number(++m_pictureNumber,16); // in hex

    QString ooName("Pictures/");
    ooName += number.right(32);
    ooName += '.';
    ooName += strExtension;

    kdDebug(30518) << "Picture " << koStoreName << " => " << ooName << endl;

    // TODO: we are only using the filename, not the rest of the key
    // TODO:  (bad if there are two images of the same name, but of a different key)
    *m_streamOut << "<draw:image draw:name=\"" << anchor.picture.key.filename() << "\"";
    *m_streamOut << " draw:style-name=\"Graphics\""; // ### TODO: should be an automatic "graphic" style name instead
    *m_streamOut << " text:anchor-type=\"paragraph\"";
    *m_streamOut << " svg:height=\"" << height << "pt\" svg:width=\"" << width << "pt\"";
    *m_streamOut << " draw:z-index=\"0\" xlink:href=\"#" << ooName << "\"";
    *m_streamOut << " xlink:type=\"simple\" xlink:show=\"embed\" xlink:actuate=\"onLoad\"";
    *m_streamOut << "/>"; // NO end of line!

    if (m_zip)
    {
#if 0
        // ### FIXME Why is the following line not working? (It makes unzip having problems with meta.xml)
        m_zip->writeFile(ooName,QString::null, QString::null, image.size(), image.data());
#else
        zipPrepareWriting(ooName);
        zipWriteData( image );
        zipDoneWriting();
#endif
    }

    return true;
}

void OOWriterWorker::processNormalText ( const QString &paraText,
    const TextFormatting& formatLayout,
    const FormatData& formatData)
{
    // Retrieve text and escape it
    QString partialText=escapeOOText(paraText.mid(formatData.pos,formatData.len));

    // Replace line feeds by line breaks
    int pos;
    while ((pos=partialText.find(QChar(10)))>-1)
    {
        partialText.replace(pos,1,"<text:line-break/>");
    }

    if (formatData.text.missing)
    {
        // It's just normal text, so we do not need a <text:span> element!
        *m_streamOut << partialText;
    }
    else
    { // Text with properties, so use a <text:span> element!
        *m_streamOut << "<text:span";

        QString styleKey;
        QString props ( textFormatToStyle(formatLayout,formatData.text,false,styleKey) );

        QMap<QString,QString>::ConstIterator it ( m_mapTextStyleKeys.find(styleKey) );
        kdDebug(30518) << "Searching text key: " << styleKey << endl;

        QString automaticStyle;
        if (it==m_mapTextStyleKeys.end())
        {
            // We have not any match, so we need a new automatic text style
            automaticStyle="T";
            automaticStyle += QString::number(++m_automaticTextStyleNumber,10); // ### TODO: verify that it is not a normal style
            kdDebug(30518) << "Creating automatic text style: " << automaticStyle << " key: " << styleKey << endl;
            m_mapTextStyleKeys[styleKey]=automaticStyle;

            m_contentAutomaticStyles += "  <style:style";
            m_contentAutomaticStyles += " style:name=\"" + escapeOOText(automaticStyle) + "\"";
            m_contentAutomaticStyles += " style:family=\"text\"";
            m_contentAutomaticStyles += ">\n";
            m_contentAutomaticStyles += "   <style:properties ";
            m_contentAutomaticStyles += props;
            m_contentAutomaticStyles += "/>\n";
            m_contentAutomaticStyles += "  </style:style>\n";
        }
        else
        {
            // We have a match, so use the already defined automatic text style
            automaticStyle=it.data();
            kdDebug(30518) << "Using automatic text style: " << automaticStyle << " key: " << styleKey << endl;
        }

        *m_streamOut << " text:style-name=\"" << escapeOOText(automaticStyle) << "\" ";

        *m_streamOut << ">" << partialText << "</text:span>";
    }
}

void OOWriterWorker::processVariable ( const QString&,
    const TextFormatting& formatLayout,
    const FormatData& formatData)
{
    if (0==formatData.variable.m_type)
    {
        *m_streamOut << "<text:date/>";
    }
    else if (2==formatData.variable.m_type)
    {
        *m_streamOut << "<text:time/>";
    }
    else if (4==formatData.variable.m_type)
    {
        if (formatData.variable.isPageNumber())
        {
            *m_streamOut << "<text:page-number text:select-page=\"current\"/>";
        }
        else if (formatData.variable.isPageCount())
        {
            *m_streamOut << "<text:page-count/>";
        }
        else
        {
            // Unknown subtype, therefore write out the result
            *m_streamOut << formatData.variable.m_text;
        }
    }
    else if (9==formatData.variable.m_type)
    {
        // A link
        *m_streamOut << "<text:a xlink:href=\""
            << escapeOOText(formatData.variable.getHrefName())
            << "\" xlink:type=\"simple\">"
            << escapeOOText(formatData.variable.getLinkName())
            << "</text:a>";
    }
    else if (11==formatData.variable.m_type)
    {
        // Footnote
        const QString value = formatData.variable.getFootnoteValue();
        //const bool automatic = formatData.variable.getFootnoteAuto();
        QValueList<ParaData> *paraList = formatData.variable.getFootnotePara();
        if( paraList )
        {
            *m_streamOut << "<text:footnote text:id=\"ft";
            *m_streamOut << (++m_footnoteNumber);
            *m_streamOut << "\">";
            *m_streamOut << "<text:footnote-citation>" << escapeOOText(value) << "</text:footnote-citation>";
            *m_streamOut << "<text:footnote-body>\n";

            doFullAllParagraphs(*paraList);

            *m_streamOut << "\n</text:footnote-body>";
            *m_streamOut << "</text:footnote>";

        }
    }
    else
    {
        // Generic variable
        *m_streamOut << formatData.variable.m_text;
    }
}

void OOWriterWorker::processAnchor ( const QString&,
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
        kdWarning(30518) << "Unsupported anchor type: "
            << formatData.frameAnchor.type << endl;
    }
}

void OOWriterWorker::processParagraphData ( const QString &paraText,
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

QString OOWriterWorker::layoutToParagraphStyle(const LayoutData& layoutOrigin,
    const LayoutData& layout, const bool force, QString& styleKey)
{
    QString props; // Props has to remain empty, if there is no difference.

    styleKey += layout.styleName;
    styleKey += ',';

    if (force || (layoutOrigin.alignment!=layout.alignment))
    {
        // Check if the current alignment is a valid one for OOWriter.
        if ((layout.alignment == "left") || (layout.alignment == "right")
            || (layout.alignment == "center")  || (layout.alignment == "justify"))
        {
            props += "fo:text-align=\"";
            props += layout.alignment;
            props += "\" ";
            styleKey += layout.alignment[0].upper();
        }
        else if (layout.alignment == "auto")
        {
            props += "fo:text-align=\"left\" ";
            props += "style:text-auto-align=\"true\" "; // OASIS extension
            styleKey += "A";
        }
        else
        {
            kdWarning(30518) << "Unknown alignment: " << layout.alignment << endl;
        }
    }

    styleKey += ',';

    if ((layout.indentLeft>=0.0)
        && (force || (layoutOrigin.indentLeft!=layout.indentLeft)))
    {
        props += QString("fo:margin-left=\"%1pt\" ").arg(layout.indentLeft);
        styleKey += QString::number(layout.indentLeft);
    }

    styleKey += ',';

    if ((layout.indentRight>=0.0)
        && (force || (layoutOrigin.indentRight!=layout.indentRight)))
    {
        props += QString("fo:margin-right=\"%1pt\" ").arg(layout.indentRight);
        styleKey += QString::number(layout.indentRight);
    }

    styleKey += ',';

    if (force || (layoutOrigin.indentLeft!=layout.indentLeft))
    {
        props += "fo:text-indent=\"";
        props += QString::number(layout.indentFirst);
        props += "\" ";
        styleKey += QString::number(layout.indentFirst);
    }

    styleKey += ',';

    if ((layout.marginBottom>=0.0)
        && (force || (layoutOrigin.indentRight!=layout.indentRight)))
    {
       props += QString("fo:margin-bottom=\"%1pt\" ").arg(layout.marginBottom);
       styleKey += QString::number(layout.marginBottom);
    }

    styleKey += ',';

    if ((layout.marginTop>=0.0)
        && (force || (layoutOrigin.indentRight!=layout.indentRight)))
    {
       props += QString("fo:margin-top=\"%1pt\" ").arg(layout.marginTop);
       styleKey += QString::number(layout.marginTop);
    }

    styleKey += ',';

    // ### TODO: add support of at least, multiple...
    if (!force
        && (layoutOrigin.lineSpacingType==layoutOrigin.lineSpacingType)
        && (layoutOrigin.lineSpacing==layoutOrigin.lineSpacing))
    {
        // Do nothing!
    }
    else if (!layout.lineSpacingType)
    {
        // We have a custom line spacing (in points)
        props += QString("fo:line-height=\"%1pt\" ").arg(layout.lineSpacing);
        styleKey += QString::number(layout.lineSpacing);
    }
    else if ( 10==layout.lineSpacingType  )
    {
        styleKey += "100%"; // One
    }
    else if ( 15==layout.lineSpacingType  )
    {
        props += "fo:line-height=\"150%\" "; // One-and-half
        styleKey += "150%";
    }
    else if ( 20==layout.lineSpacingType  )
    {
        props += "fo:line-height=\"200%\" "; // Two
        styleKey += "200%";
    }
    else
    {
        kdWarning(30518) << "Curious lineSpacingType: " << layout.lineSpacingType << " (Ignoring!)" << endl;
    }

    styleKey += ',';

    if ( layout.pageBreakBefore )
    {
        // We have a page break before the paragraph
        props += "fo:page-break-before=\"page\" ";
        styleKey += 'B';
    }

    styleKey += ',';

    if ( layout.pageBreakAfter )
    {
        // We have a page break after the paragraph
        props += "fo:page-break-after=\"page\" ";
        styleKey += 'A';
    }

    styleKey += '@'; // A more visible seperator

    props += textFormatToStyle(layoutOrigin.formatData.text,layout.formatData.text,force,styleKey);

    props += ">";

    styleKey += '@'; // A more visible seperator

    // ### TODO/FIXME: what if all tabulators must be erased?
    if (!layout.tabulatorList.isEmpty()
        && (force || (layoutOrigin.tabulatorList!=layout.tabulatorList) ))
    {
        props += "\n    <style:tab-stops>\n";
        TabulatorList::ConstIterator it;
        for (it=layout.tabulatorList.begin();it!=layout.tabulatorList.end();it++)
        {
            props+="     <style:tab-stop style:position=\"";
            props += QString::number((*it).m_ptpos);
            props += "pt\"";
            styleKey += QString::number((*it).m_ptpos);
            switch ((*it).m_type)
            {
                case 0:  props += " style:type=\"left\""; styleKey += "L"; break;
                case 1:  props += " style:type=\"center\""; styleKey += "C"; break;
                case 2:  props += " style:type=\"right\""; styleKey += "R"; break;
                case 3:  props += " style:type=\"char\" style:char=\".\""; styleKey += "D"; break; // decimal
                default: props += " style:type=\"left\""; styleKey += "L"; break;
            }
            switch ((*it).m_filling) // ### TODO: check if the characters are right
            {
                case TabulatorData::TF_NONE: break;
                case TabulatorData::TF_DOT:  props += " style:leader-char=\".\""; break;
                case TabulatorData::TF_LINE: props += " style:leader-char=\"_\""; break;

                case TabulatorData::TF_DASH:
                case TabulatorData::TF_DASHDOT:
                case TabulatorData::TF_DASHDOTDOT: props += " style:leader-char=\"-\""; break;

                default: break;
            }
            props += "/>\n";
            styleKey +='/';
        }
        props += "    </style:tab-stops>\n   ";
    }

    return props;
}

bool OOWriterWorker::doFullParagraph(const QString& paraText, const LayoutData& layout,
    const ValueListFormatData& paraFormatDataList)
{
    const bool header = ( (layout.counter.numbering == CounterData::NUM_CHAPTER)
        && (layout.counter.depth<10) ); // ### TODO: Does OOWriter really limits to 10?

    if (header)
    {
        *m_streamOut << "  <text:h text:level=\"";
        *m_streamOut << QString::number(layout.counter.depth+1,10);
        *m_streamOut << "\" ";
    }
    else
        *m_streamOut << "  <text:p ";

    const LayoutData& styleLayout=m_styleMap[layout.styleName];

    QString styleKey;
    const QString props(layoutToParagraphStyle(styleLayout,layout,false,styleKey));

    QString actualStyle(layout.styleName);
    if (!props.isEmpty())
    {
        QMap<QString,QString>::ConstIterator it ( m_mapParaStyleKeys.find(styleKey) );
        kdDebug(30518) << "Searching paragraph key: " << styleKey << endl;

        QString automaticStyle;

        if (it==m_mapParaStyleKeys.end())
        {
            // We have additional properties, so we need an automatic style for the paragraph
            automaticStyle += "P";
            automaticStyle += QString::number(++m_automaticParagraphStyleNumber,10); // ### TODO: verify that it is not a normal style
            kdDebug(30518) << "Creating automatic paragraph style: " << automaticStyle << endl;
            m_mapParaStyleKeys[styleKey]=automaticStyle;

            m_contentAutomaticStyles += "  <style:style";
            m_contentAutomaticStyles += " style:name=\"" + escapeOOText(automaticStyle) + "\"";
            m_contentAutomaticStyles += " style:parent-style-name=\"" + escapeOOText(layout.styleName) + "\"";
            m_contentAutomaticStyles += " style:family=\"paragraph\" style:class=\"text\"";
            m_contentAutomaticStyles += ">\n";
            m_contentAutomaticStyles += "   <style:properties ";
            m_contentAutomaticStyles += props;
            m_contentAutomaticStyles += "</style:properties>\n";
            m_contentAutomaticStyles += "  </style:style>\n";
        }
        else
        {
            // We have a match, so use the already defined automatic paragraph style
            automaticStyle=it.data();
            kdDebug(30518) << "Using automatic paragraph style: " << automaticStyle << " key: " << styleKey << endl;
        }

        actualStyle=automaticStyle;
    }

    if (!actualStyle.isEmpty())
    {
        *m_streamOut << "text:style-name=\"" << escapeOOText(actualStyle) << "\" ";
    }
    else
    {   // SHould not happen
        kdWarning(30518) << "No style for a paragraph!" << endl;
    }

    *m_streamOut << ">";

    processParagraphData(paraText, layout.formatData.text, paraFormatDataList);

    // Before closing the paragraph, test if we have a page break

    if (header)
        *m_streamOut << "</text:h>\n";
    else
        *m_streamOut << "</text:p>\n";

    return true;
}

bool OOWriterWorker::doOpenStyles(void)
{
    m_styles += " <office:styles>\n";
    m_styles += "  <style:style style:name=\"Graphics\" style:family=\"graphics\">"; // ### TODO: what if Grpahics is a normal style
    m_styles += "   <style:properties/>";
    m_styles += "  </style:style>";
    return true;
}

bool OOWriterWorker::doFullDefineStyle(LayoutData& layout)
{
    //Register style in the style map
    m_styleMap[layout.styleName]=layout;

    m_styles += "  <style:style";

    m_styles += " style:name=\"" + EscapeXmlText(layout.styleName,true,true) + "\"";
    m_styles += " style:next-style-name=\"" + EscapeXmlText(layout.styleFollowing,true,true) + "\"";
    m_styles += " style:family=\"paragraph\" style:class=\"text\"";
    m_styles += ">\n";
    m_styles += "   <style:properties ";

    QString dummyKey; // Not needed
    m_styles += layoutToParagraphStyle(layout,layout,true,dummyKey);

    m_styles += "</style:properties>\n";
    m_styles += "  </style:style>\n";

    return true;
}

bool OOWriterWorker::doCloseStyles(void)
{
    m_styles += " </office:styles>\n";
    return true;
}


bool OOWriterWorker::doFullPaperFormat(const int format,
            const double width, const double height, const int orientation)
{
    m_paperFormat=format;
    m_paperWidth=width;
    m_paperHeight=height;
    m_paperOrientation=orientation;
    return true;
}

bool OOWriterWorker::doFullPaperBorders (const double top, const double left,
    const double bottom, const double right)
{
    m_paperBorderTop=top;
    m_paperBorderLeft=left;
    m_paperBorderBottom=bottom;
    m_paperBorderRight=right;
    return true;
}

bool OOWriterWorker::doFullDocumentInfo(const KWEFDocumentInfo& docInfo)
{
    if (!m_zip)
        return true;


    m_docInfo=docInfo;

    zipPrepareWriting("meta.xml");

    writeStartOfFile("meta");

    zipWriteData(" <office:meta>\n");

    // Tell who we are in case we have a bug in our filter output!
    zipWriteData("  <meta:generator>KWord's OOWriter Export Filter");
    zipWriteData(QString("$Revision$").mid(10).remove('$')); // has a leading and a trailing space.

    zipWriteData("</meta:generator>\n");

    if (!m_docInfo.title.isEmpty())
    {
        zipWriteData("  <dc:title>");
        zipWriteData(escapeOOText(m_docInfo.title));
        zipWriteData("</dc:title>\n");
    }
    if (!m_docInfo.abstract.isEmpty())
    {
        zipWriteData("  <dc:description>");
        zipWriteData(escapeOOText(m_docInfo.abstract));
        zipWriteData("</dc:description>\n");
    }

    QDateTime now (QDateTime::currentDateTime(Qt::UTC)); // current time in UTC
    zipWriteData("  <dc:date>");
    zipWriteData(escapeOOText(now.toString(Qt::ISODate)));
    zipWriteData("</dc:date>\n");

    zipWriteData(" </office:meta>\n");
    zipWriteData("</office:document-meta>\n");

    zipDoneWriting();

    return true;
}

void OOWriterWorker::declareFont(const QString& fontName)
{
    if (m_fontNames.find(fontName)==m_fontNames.end())
    {
        QString props;

        // Disabled, as QFontInfo::styleHint() cannot guess
#if 0
        QFont font(fontName);
        QFontInfo info(font);
        props+="style:font-family-generic=\""
        switch (info.styleHint())
        {
        case QFont::SansSerif:
        default:
            {
                props += "swiss";
                break;
            }
        case QFont::Serif:
            {
                props +=  "roman";
                break;
            }
        case QFont::Courier:
            {
                props +=  "modern";
                break;
            }
        case QFont::OldEnglish:
            {
                props +=  "decorative";
                break;
            }
        }
        props +="\" ";
#endif

        props +="style:font-pitch=\"variable\""; // ### TODO: check if font is variable or fixed
        // New font, so register it
        m_fontNames[fontName]=props;
    }
}

