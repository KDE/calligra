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
    m_paperBorderBottom(0.0),m_paperBorderRight(0.0), m_zip(NULL)
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

void OOWriterWorker::writeContentXml(void)
{
    if (!m_zip)
        return;

    zipPrepareWriting("content.xml");    
        
    zipWriteData("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    zipWriteData("<!DOCTYPE office:document-content>\n");
    zipWriteData("<office:document-content>\n");

    
    zipWriteData( " <office:fonts-decls>\n");
    for (QStringList::ConstIterator it=m_fontNames.begin(); it!=m_fontNames.end(); it++)
    {
        zipWriteData("  <style:font-decl style:name=\"");
        zipWriteData(escapeOOText(*it));
        zipWriteData("\" fo:font-family=\"");
        zipWriteData(escapeOOText(*it));
        // ### TODO: pitch
        zipWriteData("\" />\n");
    }
    zipWriteData(" </office:fonts-decls>\n");
    
    // TODO: styles

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
        m_zip->close();
    }
        
    delete m_zip;
    m_zip=NULL;
    return true;
}

bool OOWriterWorker::doOpenDocument(void)
{
    kdDebug(30518)<< "OOWriterWorker::doOpenDocument" << endl;

    *m_streamOut << "<office:body>\n";

    return true;
}

bool OOWriterWorker::doCloseDocument(void)
{
    *m_streamOut << "</office:body>\n";
    return true;
}

QString OOWriterWorker::textFormatToAbiProps(const TextFormatting& formatOrigin,
    const TextFormatting& formatData, const bool force)
{
    // TODO: rename variable formatData
    QString strElement; // TODO: rename this variable

    // Font name
    QString fontName = formatData.fontName;
    declareFont(fontName);
    if ( !fontName.isEmpty()
        && (force || (formatOrigin.fontName!=formatData.fontName)))
    {
        strElement+="fo:font-family=\"";
        strElement+= escapeOOText(fontName); // TODO: add alternative font names
        strElement+="\" ";
    }

    if (force || (formatOrigin.italic!=formatData.italic))
    {
        // Font style
        strElement+="fo:font-style=\"";
        if ( formatData.italic )
        {
            strElement+="italic";
        }
        else
        {
            strElement+="normal";
        }
        strElement+="\" ";
    }

    if (force || ((formatOrigin.weight>=75)!=(formatData.weight>=75)))
    {
        strElement+="fo:font-weight=\"";
        if ( formatData.weight >= 75 )
        {
            strElement+="bold";
        }
        else
        {
            strElement+="normal";
        }
        strElement+="\" ";
    }

    if (force || (formatOrigin.fontSize!=formatData.fontSize))
    {
        const int size=formatData.fontSize;
        if (size>0)
        {
            // We use absolute font sizes.
            strElement+="fo:font-size=\"";
            strElement+=QString::number(size,10);
            strElement+="pt\" ";
        }
    }

    if (force || (formatOrigin.fgColor!=formatData.fgColor))
    {
        if ( formatData.fgColor.isValid() )
        {
            // Give colour
            strElement+="fo:color=\"";
            strElement+=formatData.fgColor.name();
            strElement+="\" ";
        }
    }

    if (force || (formatOrigin.bgColor!=formatData.bgColor))
    {
        if ( formatData.bgColor.isValid() )
        {
            // Give background colour
            strElement+="fo:background-color=\"";
            strElement+=formatData.bgColor.name();
            strElement+="\" ";
        }
    }

    if (force || (formatOrigin.underline!=formatData.underline)
        || (formatOrigin.strikeout!=formatData.strikeout))
    {
        strElement+="fo:text-decoration=\"";
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
        strElement+="\" ";
    }

    return strElement.stripWhiteSpace(); // Remove especially trailing spaces
}

bool OOWriterWorker::makeTable(const FrameAnchor& anchor)
{
#if 0
    *m_streamOut << "</abiword:p>\n"; // Close previous paragraph ### TODO: do it correctly like for HTML
    *m_streamOut << "<abiword:table>\n";

    QValueList<TableCell>::ConstIterator itCell;



    for (itCell=anchor.table.cellList.begin();
        itCell!=anchor.table.cellList.end(); itCell++)
    {
        // ### TODO: rowspan, colspan
       
        // AbiWord seems to work by attaching to the cell borders
        *m_streamOut << "<abiword:cell props=\"";
        *m_streamOut << "left-attach:" << (*itCell).col << "; ";
        *m_streamOut << "right-attach:" << (*itCell).col + 1 << "; ";
        *m_streamOut << "top-attach:" << (*itCell).row << "; ";
        *m_streamOut << "bot-attach:" << (*itCell).row + 1;
        *m_streamOut << "\">\n";
        
        if (!doFullAllParagraphs(*(*itCell).paraList))
        {
            return false;
        }
        
        *m_streamOut << "</abiword:cell>\n";
    }

    *m_streamOut << "</abiword:table>\n";
    *m_streamOut << "<abiword:p>\n"; // Re-open the "previous" paragraph ### TODO: do it correctly like for HTML

    return true;
#else
    return true;
#endif
}

bool OOWriterWorker::makePicture(const FrameAnchor& anchor)
{
    kdDebug(30518) << "New image/clipart: " << anchor.picture.koStoreName
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

    if (strExtension=="png") // Which other image formats does OOWriter support?
    {
        isImageLoaded=loadSubFile(koStoreName,image);
    }
    else if ((strExtension=="jpg") || (strExtension=="jpeg"))
    {
        isImageLoaded=loadSubFile(koStoreName,image);
        strExtension="jpg";
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
    
    const double height=anchor.bottom - anchor.top;
    const double width =anchor.right  - anchor.left;

    QString ooName("Picture/");
    QString number("00000000000000000000000000000000"); // 32 zeros
    number += QString::number(++m_pictureNumber,16); // in hex
    ooName += number.right(32);
    ooName+='.';
    ooName+=strExtension;
    
    // TODO: we are only using the filename, not the rest of the key
    // TODO:  (bad if there are two images of the same name, but of a different key)
    *m_streamOut << "<draw:image draw:name=\"" << anchor.picture.key.filename() << "\"";
    *m_streamOut << " text:anchor-type=\"paragraph\"";
    *m_streamOut << " svg:height=\"" << height << "pt\" svg:width=\"" << width << "pt\"";
    *m_streamOut << " draw:z-index=\"0\" xlink:href=\"#" << ooName << "\"";
    *m_streamOut << " xlink:type=\"simple\" xlink:show=\"embed\" xlink:actuate=\"onLoad\"";
    *m_streamOut << "/>"; // NO end of line!
    
    if (m_zip)
    {
        m_zip->writeFile(ooName,QString::null, QString::null, image.size(), image.data());
    }
    
    return true;
}

void OOWriterWorker::writeAbiProps (const TextFormatting& formatLayout, const TextFormatting& format)
{
    *m_streamOut << textFormatToAbiProps(formatLayout,format,false);
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
        partialText.replace(pos,1,"<test:line-break/>");
    }

    if (formatData.text.missing)
    {
        // It's just normal text, so we do not need a <c> element!
        *m_streamOut << partialText;
    }
    else
    { // Text with properties, so use a <c> element!
        *m_streamOut << "<text:span";
        writeAbiProps(formatLayout,formatData.text);
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
        // As AbiWord's field is inflexible, we cannot make the time custom
        *m_streamOut << "<text:time/>";
    }
    else if (4==formatData.variable.m_type)
    {
        // As AbiWord's field is inflexible, we cannot make the time custom
        QString strFieldType;
        if (formatData.variable.isPageNumber())
        {
            *m_streamOut << "<text:page-number/>";
        }
#if 0
        else if (formatData.variable.isPageCount())
        {
            *m_streamOut << "<text:page-number/>";
        }
#endif
        else
        {
            // Unknown subtype, therefore write out the result
            *m_streamOut << formatData.variable.m_text;
        }
    }
    else if (9==formatData.variable.m_type)
    {
        // A link (### TODO is the <text:span> really needed?)
        *m_streamOut << "<text:a xlink:href=\""
            << escapeOOText(formatData.variable.getHrefName())
            << " xlink:type=\"simple\" xlink:actuate=\"onRequest\"><text:span";
        writeAbiProps(formatLayout,formatData.text);
        *m_streamOut << ">"
            << escapeOOText(formatData.variable.getLinkName())
            << "</text:span></text:a>";
    }
#if 0
                else if (11==(*paraFormatDataIt).variable.m_type)
                {
                    // Footnote
                    QString value = (*paraFormatDataIt).variable.getFootnoteValue();
                    bool automatic = (*paraFormatDataIt).variable.getFootnoteAuto();
                    QValueList<ParaData> *paraList = (*paraFormatDataIt).variable.getFootnotePara();
                    if( paraList )
                    {
                        QString fstr;
                        QValueList<ParaData>::ConstIterator it;
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

QString OOWriterWorker::layoutToCss(const LayoutData& layoutOrigin,
    const LayoutData& layout, const bool force)
{
    QString props;

    if (force || (layoutOrigin.alignment!=layout.alignment))
    {
        // Check if the current alignment is a valid one for AbiWord.
        if ((layout.alignment == "left") || (layout.alignment == "right")
            || (layout.alignment == "center")  || (layout.alignment == "justify"))
        {
            props += "fo:text-align=\"";
            props += layout.alignment;
            props += "\" ";
        }
        else if (layout.alignment == "auto")
        {
            // ### TODO: bidi!
            props += "fo:text-align=\"left\" ";
        }
        else
        {
            kdWarning(30518) << "Unknown alignment: " << layout.alignment << endl;
        }
    }

#if 0
    // TODO/FIXME: what if all tabulators must be erased?
    if (!layout.tabulatorList.isEmpty()
        && (force || (layoutOrigin.tabulatorList!=layout.tabulatorList) ))
    {
        props += "<style:tabstops>";
        TabulatorList::ConstIterator it;
        for (it=layout.tabulatorList.begin();it!=layout.tabulatorList.end();it++)
        {
            props+="<style:tabstop style:position=\"";
            props += QString::number((*it).m_ptpos);
            props += "pt\"";

            switch ((*it).m_type)
            {
                case 0:  props += " style:type=\"left\""; break;
                case 1:  props += " style:type=\"center\""; break;
                case 2:  props += " style:type=\"right\""; break;
                //case 3:  props += "/D"; break;
                default: props += " style:type=\"left\""; break;
            }
            props +="/>"

        }
        props += "</style:tabstops>";
    }
#endif

    if ((layout.indentLeft>=0.0)
        && (force || (layoutOrigin.indentLeft!=layout.indentLeft)))
    {
        props += QString("fo:margin-left=\"%1pt\" ").arg(layout.indentLeft);
    }

    if ((layout.indentRight>=0.0)
        && (force || (layoutOrigin.indentRight!=layout.indentRight)))
    {
        props += QString("fo:margin-right=\"%1pt\" ").arg(layout.indentRight);
    }

    if (force || (layoutOrigin.indentLeft!=layout.indentLeft))
    {
        props += "fo:text-indent=\"";
        props += QString::number(layout.indentFirst);
        props += "\" ";
    }

    if ((layout.marginBottom>=0.0)
        && (force || (layoutOrigin.indentRight!=layout.indentRight)))
    {
       props += QString("fo:margin-bottom=\"%1pt\" ").arg(layout.marginBottom);
    }

    if ((layout.marginTop>=0.0)
        && (force || (layoutOrigin.indentRight!=layout.indentRight)))
    {
       props += QString("fo:margin-top=\"%1pt\" ").arg(layout.marginTop);
    }

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
    }
    else if ( 15==layout.lineSpacingType  )
    {
        props += "fo:line-height=\"1.5\" "; // One-and-half
    }
    else if ( 20==layout.lineSpacingType  )
    {
        props += "fo:line-height=\"2.0\" "; // Two
    }
    else if ( layout.lineSpacingType!=10  )
    {
        kdWarning(30518) << "Curious lineSpacingType: " << layout.lineSpacingType << " (Ignoring!)" << endl;
    }

    // Add all AbiWord properties collected in the <FORMAT> element
    props += textFormatToAbiProps(layoutOrigin.formatData.text,layout.formatData.text,force);

    return props;
}

bool OOWriterWorker::doFullParagraph(const QString& paraText, const LayoutData& layout,
    const ValueListFormatData& paraFormatDataList)
{
    QString style=layout.styleName;

    const LayoutData& styleLayout=m_styleMap[style];

    QString props=layoutToCss(styleLayout,layout,false);

    *m_streamOut << "<text:p";
    if (!style.isEmpty())
    {
        *m_streamOut << " text:style-name=\"" << EscapeXmlText(style,true,true) << "\"";
    }
    *m_streamOut << props;
    if (layout.pageBreakBefore)
    {
        // We have a page break before the paragraph
        *m_streamOut << " fo:page-break-before=\"page\"";
    }
    if (layout.pageBreakAfter)
    {
        // We have a page break after the paragraph
        *m_streamOut << " fo:page-break-after=\"page\"";
    }
    *m_streamOut << ">";  //Warning(AbiWord): No trailing white space or else it's in the text!!!


    processParagraphData(paraText, layout.formatData.text, paraFormatDataList);

    // Before closing the paragraph, test if we have a page break

    *m_streamOut << "</text:p>\n";
    return true;
}

bool OOWriterWorker::doOpenStyles(void)
{
    m_styles += " <office:styles>\n";
    return true;
}

bool OOWriterWorker::doFullDefineStyle(LayoutData& layout)
{
    //Register style in the style map
    m_styleMap[layout.styleName]=layout;

    m_styles += "  <style:style";

    m_styles += " style:name=\"" + EscapeXmlText(layout.styleName,true,true) + "\"";
    m_styles += " style:next-style-name=\"" + EscapeXmlText(layout.styleFollowing,true,true) + "\"";
    m_styles += ">";
#if 0
    if ( (layout.counter.numbering == CounterData::NUM_CHAPTER)
        && (layout.counter.depth<10) )
    {
        m_styles += " abiword:level=\"";
        m_styles += QString::number(layout.counter.depth+1,10);
        m_styles += << "\">";
    }
#endif
    m_styles += "<style:properties ";
     
    m_styles += layoutToCss(layout,layout,true);

    m_styles += "/>";

    m_styles += "</style:style>\n";

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
#if 0
    QString outputText ("<style:page-master-name>\n");
    
    outputText += "<style:properties ";

    outputText+=" fo:page-width=\"";
    outputText+=QString::number(width);
    outputText+="pt\" fo:page-height=\"";
    outputText+=QString::number(height);
    outputText+="pt\" ";

    outputText += "style:print-orientation=\"";
    if (1==orientation)
    {
        outputText += "landscape";
    }
    else
    {
        outputText += "portrait";
    }
    outputText += "\"/>\n";
    outputText += "</style:page-master-name>\n";

    m_pagesize=outputText;
#endif
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
    
        
    zipPrepareWriting("meta.xml");
    
    m_docInfo=docInfo;
    
    zipWriteData("<office:document-meta>\n");
    zipWriteData(" <office:meta>\n");
    
    // Say who we are (with the CVS revision number) in case we have a bug in our filter output!
    zipWriteData("  <meta:generator>KWord Export Filter");

    QString strVersion("$Revision$");
    // Remove the dollar signs
    //  (We don't want that the version number changes if the AbiWord file is itself put in a CVS storage.)
    zipWriteData(strVersion.mid(10).replace('$',""));

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
        // New font, so register it
        m_fontNames.append(fontName);
    }
}
