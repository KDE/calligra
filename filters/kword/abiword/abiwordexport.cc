// $Header$

/* This file is part of the KDE project
   Copyright (C) 2001 Nicolas GOUTTE <nicog@snafu.de>

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

#include <abiwordexport.h>
#include <abiwordexport.moc>

#include <qregexp.h>
#include <qtextstream.h>
#include <qdom.h>

#include <kdebug.h>
#include <kfilterdev.h>

#include <koGlobal.h>

#include <KWEFStructures.h>
#include <KWEFUtil.h>
#include <TagProcessing.h>
#include <KWEFBaseClass.h>
#include <ProcessDocument.h>


#include "kqiodevicegzip.h"

ABIWORDExport::ABIWORDExport(KoFilter *parent, const char *name) :
                     KoFilter(parent, name) {
}

// In KWord's AbiWord export filter, the class ClassExportFilterBase is more
//  a dummy for compatibility with KWord's HTML export filter.
class ClassExportFilterBase : public KWEFBaseClass
{
public:
    ClassExportFilterBase(void) {}
    virtual ~ClassExportFilterBase(void) {}
};

static QString FormatDataToAbiProps(FormatData& formatData)
{
    QString strElement; // TODO: rename this variable

    // Font name
    QString fontName = formatData.fontName;
    if ( !fontName.isEmpty() )
    {
        strElement+="font-family: ";
        strElement+=fontName; // TODO: add alternative font names
        strElement+="; ";
    }

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

    const int size=formatData.fontSize;
    if (size>0)
    {
        // We use absolute font sizes.
        strElement+="font-size: ";
        strElement+=QString::number(size,10);
        strElement+="pt; ";
    }

    if ( formatData.colour.isValid() )
    {
        // Give colour
        strElement+="color: ";

        // No leading # (unlike CSS2)
        //We must have two hex digits for each colour channel!
        const int red=formatData.colour.red();
        strElement += QString::number((red&0xf0)>>4,16);
        strElement += QString::number(red&0x0f,16);

        const int green=formatData.colour.green();
        strElement += QString::number((green&0xf0)>>4,16);
        strElement += QString::number(green&0x0f,16);

        const int blue=formatData.colour.blue();
        strElement += QString::number((blue&0xf0)>>4,16);
        strElement += QString::number(blue&0x0f,16);

        strElement+="; ";
    }
    if ( formatData.textbackgroundColour.isValid() )
    {
        // Give background colour
        strElement+="bgcolor: ";

        // No trailing # (unlike CSS2)
        //We must have two hex digits for each colour channel!
        const int red=formatData.colour.red();
        strElement += QString::number((red&0xf0)>>4,16);
        strElement += QString::number(red&0x0f,16);

        const int green=formatData.colour.green();
        strElement += QString::number((green&0xf0)>>4,16);
        strElement += QString::number(green&0x0f,16);

        const int blue=formatData.colour.blue();
        strElement += QString::number((blue&0xf0)>>4,16);
        strElement += QString::number(blue&0x0f,16);

        strElement+="; ";
    }

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
    return strElement;
}

// ProcessParagraphData () mangles the pure text through the
// formatting information stored in the FormatData list and prints it
// out to the export file.

static void ProcessParagraphData ( QString &paraText,
    ValueListFormatData &paraFormatDataList, QString &outputText,
    KWEFBaseClass* )
{
    if ( paraText.length () > 0 )
    {
        CreateMissingFormatData(paraText,paraFormatDataList);

        ValueListFormatData::Iterator  paraFormatDataIt;  //Warning: cannot use "->" with it!!

        QString partialText;

        for ( paraFormatDataIt = paraFormatDataList.begin ();
              paraFormatDataIt != paraFormatDataList.end ();
              paraFormatDataIt++ )
        {
            // Retrieve text and escape it
            partialText=EscapeXmlText(
                paraText.mid((*paraFormatDataIt).pos,(*paraFormatDataIt).len),
                true,true);

            if ((*paraFormatDataIt).missing)
            {
                // It's just normal text, so we do not need a <c> element!
                outputText += partialText;
            }
            else
            { // Text with properties, so use a <c> element!

                QString abiprops=FormatDataToAbiProps(*paraFormatDataIt);

                // Erase the last semi-comma (as in CSS2, semi-commas only separate instructions and do not terminate them)
                const int result=abiprops.findRev(";");

                if (result>=0)
                {
                    // Remove the last semi-comma and the space thereafter
                    abiprops.remove(result,2);
                }

                outputText += "<c props=\"";
                outputText += abiprops;
                outputText += "\">";
                outputText += partialText;
                outputText += "</c>";
            }
       }

    }

}

static void ProcessParagraphTag ( QDomNode myNode, void *, QString   &outputText, KWEFBaseClass* exportFilter )
{
    AllowNoAttributes (myNode);

    QString paraText;
    ValueListFormatData paraFormatDataList;
    LayoutData paraLayout;
    QValueList<TagProcessing> tagProcessingList;
    bool hardbreak=false; // Have we an hard break?

    tagProcessingList.append ( TagProcessing ( "TEXT",    ProcessTextTag,       (void *) &paraText           ) );
    tagProcessingList.append ( TagProcessing ( "FORMATS", ProcessFormatsTag,    (void *) &paraFormatDataList ) );
    tagProcessingList.append ( TagProcessing ( "LAYOUT",  ProcessLayoutTag,     (void *) &paraLayout         ) );
    ProcessSubtags (myNode, tagProcessingList, outputText,exportFilter);

    QString style; // Style attribute for <p> element
    QString props; // Props attribute for <p> element

#if 1
    style=paraLayout.styleName;
#else
    if ( paraLayout.counter.numbering == CounterData::NUM_CHAPTER )
    {
        const int depth = paraLayout.counter.depth + 1;
        // Note: .arg(strParaText) must remain last,
        //  as strParaText may contain an unwanted % + number sequence
        style = QString("Heading %1").arg(depth);
    }
    else
    {// We don't know the layout, so assume it's "Standard". It's better than to abort with an error!
        // TODO: use AbiWord's full style capacity
        style = "Normal";
    }
#endif

    // Check if the current alignment is a valid one for AbiWord.
    if ( (paraLayout.alignment == "left") || (paraLayout.alignment == "right")
        || (paraLayout.alignment == "center")  || (paraLayout.alignment == "justify"))
    {
        props += "text-align:";
        props += paraLayout.alignment;
        props += "; ";
    }
    else
    {
        kdWarning(30506) << "Unknown alignment: " << paraLayout.alignment << endl;
    }

    if ( !paraLayout.tabulator.isEmpty())
        props +="tabstops:"+paraLayout.tabulator+"; ";

    if ( paraLayout.indentLeft!=0.0 )
    {
        props += QString("margin-left:%1pt; ").arg(paraLayout.indentLeft);
    }

    if ( paraLayout.indentRight!=0.0 )
    {
        props += QString("margin-right:%1pt; ").arg(paraLayout.indentRight);
    }

    if ( paraLayout.indentFirst!=0.0 )
    {
        props += QString("text-indent:%1pt; ").arg(paraLayout.indentFirst);
    }

    if( paraLayout.marginBottom!=0.0)
    {
       props += QString("margin-bottom:%1pt; ").arg(paraLayout.marginBottom);
    }
    if( paraLayout.marginTop!=0.0  )
    {
       props += QString("margin-top:%1pt; ").arg(paraLayout.marginTop);
    }

    if ( !paraLayout.lineSpacingType )
    {
        // We have a custom line spacing (in points)
        props += QString("line-height:%1pt; ").arg(paraLayout.lineSpacing);
    }
    else if ( 15==paraLayout.lineSpacingType  )
    {
        props += "line-height:1.5; "; // One-and-half
    }
    else if ( 20==paraLayout.lineSpacingType  )
    {
        props += "line-height:2.0; "; // Two
    }
    else if ( paraLayout.lineSpacingType!=10  )
    {
        kdWarning(30506) << "Curious lineSpacingType: " << paraLayout.lineSpacingType << " (Ignoring!)" << endl;
    }

    // Add all AbiWord properties collected in the <FORMAT> element
    props += FormatDataToAbiProps(paraLayout.formatData);

    outputText += "<p";
    if (!style.isEmpty())
    {
        outputText += " style=\"";
        outputText += style;
        outputText += "\"";
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

        outputText += " props=\"";
        outputText += props;
        outputText += "\"";
    }
    outputText += ">";  //Warning: No trailing white space or else it's in the text!!!

    // Before processing the text, test if we have a page break
    if (paraLayout.pageBreakBefore)
    {
        // We have a page break before the paragraph
        outputText += "<pbr/>";
    }

    ProcessParagraphData ( paraText, paraFormatDataList, outputText, exportFilter );

    // Before closing the paragraph, test if we have a page break
    if (paraLayout.pageBreakAfter)
    {
        // We have a page break after the paragraph
        outputText += "<pbr/>";
    }

    outputText += "</p>\n";
}


static void ProcessFramesetTag ( QDomNode myNode, void *, QString   &outputText, KWEFBaseClass* exportFilter )
{
    int frameType=-1;
    int frameInfo=-1;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ( "frameType", "int", (void*) &frameType ) );
    attrProcessingList.append ( AttrProcessing ( "frameInfo", "int", (void*) &frameInfo) );
    attrProcessingList.append ( AttrProcessing ( "removable", "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "visible",   "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "name",      "", NULL ) );
    ProcessAttributes (myNode, attrProcessingList);

    if ((1==frameType) && (0==frameInfo))
    {   //Main text
        outputText+="<section>\n";

        QValueList<TagProcessing> tagProcessingList;
        tagProcessingList.append ( TagProcessing ( "FRAME",     NULL,                NULL ) );
        tagProcessingList.append ( TagProcessing ( "PARAGRAPH", ProcessParagraphTag, NULL ) );
        ProcessSubtags (myNode, tagProcessingList, outputText,exportFilter);

        outputText+="</section>\n";
    }
    //TODO: Treat the other types of frames (frameType)
}


static void ProcessFramesetsTag (QDomNode myNode, void *, QString   &outputText, KWEFBaseClass* exportFilter )
{
    AllowNoAttributes (myNode);

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "FRAMESET", ProcessFramesetTag, NULL ) );
    ProcessSubtags (myNode, tagProcessingList, outputText,exportFilter);
}

static void ProcessStyleTag (QDomNode myNode, void *, QString   &strStyles, KWEFBaseClass* exportFilter )
{
    kdDebug(30506) << "Entering ProcessStyleTag" << endl;

    AllowNoAttributes (myNode);

    LayoutData *layout = new LayoutData (); // TODO: memory error recovery

    QString dummy;

    ProcessLayoutTag(myNode, layout, dummy, exportFilter);

    strStyles+="<s name=\"";
    strStyles+=layout->styleName; // TODO: cook the style name to the standard style names in AbiWord
    strStyles+="\"";
    if ( layout->counter.numbering == CounterData::NUM_CHAPTER )
    {
        strStyles+=" level=\"";
        strStyles+=QString::number(layout->counter.depth+1,10);
        strStyles+="\"";
    }

    // Add all AbiWord properties collected in the <FORMAT> element
    QString abiprops = FormatDataToAbiProps(layout->formatData);

    const int result=abiprops.findRev(";");
    if (result>=0)
    {
        // Remove the last semi-comma and the space thereafter
        abiprops.remove(result,2);
    }

    //TODO: other layout things

    strStyles+=" props=\"";
    strStyles+=abiprops;  // Be careful that layout->abiprops might be empty!
    strStyles+="\"/>\n";

    delete layout;

    kdDebug(30506) << "Exiting ProcessStyleTag" << endl;
}

static void ProcessStylesPluralTag (QDomNode myNode, void *, QString &outputText, KWEFBaseClass* exportFilter )
{
    AllowNoAttributes (myNode);

    QString strStyles;

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "STYLE", ProcessStyleTag, NULL ) );
    ProcessSubtags (myNode, tagProcessingList, strStyles,exportFilter);

    if (!strStyles.isEmpty())
    {
        outputText+="<styles>\n";
        outputText+=strStyles;
        outputText+="</styles>\n";
    }
}

static void ProcessPaperTag (QDomNode myNode, void *, QString   &outputText, KWEFBaseClass*)
{

    int format=-1;
    int orientation=-1;
    double width=-1.0;
    double height=-1.0;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ( "format",          "int", (void*) &format ) );
    attrProcessingList.append ( AttrProcessing ( "width",           "double", (void*) &width ) );
    attrProcessingList.append ( AttrProcessing ( "height",          "double", (void*) &height ) );
    attrProcessingList.append ( AttrProcessing ( "orientation",     "int", (void*) &orientation ) );
    attrProcessingList.append ( AttrProcessing ( "columns",         "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "columnspacing",   "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "hType",           "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "fType",           "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "spHeadBody",      "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "spFootBody",      "", NULL ) );
    ProcessAttributes (myNode, attrProcessingList);

    AllowNoSubtags (myNode);

    outputText += "<pagesize ";

    switch (format)
    {
        // ISO A formats
        case PG_DIN_A0: // ISO A0
        {
            outputText += "pagetype=\"A0\" width=\"84.1\" height=\"118.0\" units=\"cm\" ";
            break;
        }
        case PG_DIN_A1: // ISO A1
        {
            outputText += "pagetype=\"A1\" width=\"59.4\" height=\"84.1\" units=\"cm\" ";
            break;
        }
        case PG_DIN_A2: // ISO A2
        {
            outputText += "pagetype=\"A2\" width=\"42.0\" height=\"59.4\" units=\"cm\" ";
            break;
        }
        case PG_DIN_A3: // ISO A3
        {
            outputText += "pagetype=\"A3\" width=\"29.7\" height=\"42.0\" units=\"cm\" ";
            break;
        }
        case PG_DIN_A4: // ISO A4
        {
            outputText += "pagetype=\"A4\" width=\"21.0\" height=\"29.7\" units=\"cm\" ";
            break;
        }
        case PG_DIN_A5: // ISO A5
        {
            outputText += "pagetype=\"A5\" width=\"14.8\" height=\"21.0\" units=\"cm\" ";
            break;
        }
        case PG_DIN_A6: // ISO A6
        {
            outputText += "pagetype=\"A6\" width=\"10.5\" height=\"14.8\" units=\"cm\" ";
            break;
        }
        // ISO B formats
        case PG_DIN_B0: // ISO B0
        {
            outputText += "pagetype=\"B0\" width=\"100.0\" height=\"141.0\" units=\"cm\" ";
            break;
        }
        case PG_DIN_B1: // ISO B1
        {
            outputText += "pagetype=\"B1\" width=\"70.7\" height=\"100.0\" units=\"cm\" ";
            break;
        }
        case PG_DIN_B2: // ISO B2
        {
            outputText += "pagetype=\"B2\" width=\"50.0\" height=\"70.7\" units=\"cm\" ";
            break;
        }
        case PG_DIN_B3: // ISO B3
        {
            outputText += "pagetype=\"B3\" width=\"35.3\" height=\"50.0\" units=\"cm\" ";
            break;
        }
        case PG_DIN_B4: // ISO B4
        {
            outputText += "pagetype=\"B4\" width=\"25.8\" height=\"35.3\" units=\"cm\" ";
            break;
        }
        case PG_DIN_B5: // ISO B5
        {
            outputText += "pagetype=\"B5\" width=\"17.6\" height=\"25.0\" units=\"cm\" ";
            break;
        }
        case PG_DIN_B6: // ISO B6
        {
            outputText += "pagetype=\"B6\" width=\"12.5\" height=\"17.6\" units=\"cm\" ";
            break;
        }
        // American formats
        case PG_US_LETTER: // US Letter
        {
            outputText += "pagetype=\"Letter\" width=\"8.5\" height=\"11.0\" units=\"inch\" ";
            break;
        }
        case PG_US_LEGAL: // US Legal
        {
            outputText += "pagetype=\"Legal\" width=\"8.5\" height=\"14.0\" units=\"inch\" ";
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
}

static void ProcessDocTag (QDomNode myNode, void *,  QString &outputText, KWEFBaseClass* exportFilter)
{
    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ( "editor",        "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "mime",          "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "syntaxVersion", "", NULL ) );
    ProcessAttributes (myNode, attrProcessingList);
    // TODO: verify syntax version and perhaps mime


    // We process the tag in some groups to have the order
    //  like the AbiWord file format wants it.

    QValueList<TagProcessing> tagProcessingList;

    // <styles>
    tagProcessingList.append ( TagProcessing ( "STYLES",      ProcessStylesPluralTag, NULL ) );
    ProcessSubtags (myNode, tagProcessingList, outputText,exportFilter);

    // TODO: list

    // <paper>
    tagProcessingList.clear();
    tagProcessingList.append ( TagProcessing ( "PAPER",       ProcessPaperTag,     NULL ) );
    ProcessSubtags (myNode, tagProcessingList, outputText,exportFilter);

    // the rest: <FRAMESETS> give <section>
    tagProcessingList.clear();
    tagProcessingList.append ( TagProcessing ( "ATTRIBUTES",  NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "FOOTNOTEMGR", NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "PIXMAPS",     NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "SERIALL",     NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "FRAMESETS",   ProcessFramesetsTag, NULL ) );
    ProcessSubtags (myNode, tagProcessingList, outputText,exportFilter);

    // TODO: <data>
}

bool ABIWORDExport::filter(const QString  &filenameIn,
                           const QString  &filenameOut,
                           const QString  &from,
                           const QString  &to,
                           const QString  &         )
{
    if ((from != "application/x-kword") || (to != "application/x-abiword"))
    {
        return false;
    }

    KoStore koStoreIn (filenameIn, KoStore::Read);

    if ( !koStoreIn.open ( "root" ) )
    {
        koStoreIn.close ();

        kdError(30506) << "Unable to open input file!" << endl;
        return false;
    }

    QByteArray byteArrayIn = koStoreIn.read ( koStoreIn.size () );
    koStoreIn.close ();

    //
    // Create a QIODevice for writing
    // TODO: replace it with KFilterDev
    //
    QIODevice* ioDevice=NULL;

    //Find the last extension
    QString strExt;
    const int result=filenameOut.findRev('.');
    if (result>=0)
    {
        strExt=filenameOut.mid(result);
    }

#if 0
    QString strMime; // Mime type of the compressor (default: unknown)

    if ((strExt==".gz")||(strExt==".GZ")        //in case of .abw.gz (logical extension)
        ||(strExt==".zabw")||(strExt==".ZABW")) //in case of .zabw (extension used prioritary with AbiWord)
    {
        // Compressed with gzip
        strMime="application/x-gzip";
        kdDebug(30506) << "Compression: gzip" << endl;
    }
    else if ((strExt==".bz2")||(strExt==".BZ2") //in case of .abw.bz2 (logical extension)
        ||(strExt==".bzabw")||(strExt==".BZABW")) //in case of .bzabw (extension used prioritary with AbiWord)
    {
        // Compressed with bzip2
        strMime="application/x-bzip2";
        kdDebug(30506) << "Compression: bzip2" << endl;
    }

    ioDevice = KFilterDev::deviceForFile(filenameOut,strMime);
#else
    if ((strExt==".gz")||(strExt==".GZ")        //in case of .abw.gz (standard extension)
        ||(strExt==".zabw")||(strExt==".ZABW")) //in case of .zabw (extension used prioritary with AbiWord)
    {// GZipped
        ioDevice=new KQIODeviceGZip(filenameOut);
    }
    else
    {// Uncompressed
        ioDevice=new QFile(filenameOut);
    }
#endif

    if (!ioDevice)
    {
        kdError(30506) << "Could not create QIODevice! Aborting!" << endl;
        return false;
    }

    // Open the
    if ( !ioDevice->open (IO_WriteOnly) )
    {
        kdError(30506) << "Unable to open output file! Aborting!" << endl;
        return false;
    }

    // Now that we have a QIODevice, make a QTextStream
    QTextStream streamOut(ioDevice);
    streamOut.setEncoding( QTextStream::UnicodeUTF8 );

    QDomDocument qDomDocumentIn;

    // let parse the buffer just read from the file
    qDomDocumentIn.setContent(byteArrayIn);

    QDomNode docNodeIn = qDomDocumentIn.documentElement ();

    // Create export filter (more a dummy for now!)
    ClassExportFilterBase* exportFilter=new ClassExportFilterBase;
    if (!exportFilter)
    {
        kdError(30506) << "No AbiWord filter created! Aborting! (Memory problem?)" << endl;
        return false;
    }

    // Make the file header

    // First the XML header in UTF-8 version
    // (AbiWord and QT handle UTF-8 well, so we stay with this encoding!)
    streamOut << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

    // NOTE: AbiWord CVS 2001-08-21 has now a DOCTYPE
    streamOut << "<!DOCTYPE abw PUBLIC \"-//ABISOURCE//DTD ABW 1.0 Strict//EN\"";
    streamOut << " \"http://www.abisource.com/awml.dtd\">\n";

    // First magic: "<abiword"
    streamOut << "<abiword version=\"unnumbered\" fileformat=\"1.0\">\n";
    // Second magic: "<!-- This file is an AbiWord document."
    streamOut << "<!-- This file is an AbiWord document. -->\n";
    // We have chosen NOT to have the full comment header that AbiWord files normally have.
    streamOut << "\n";

    // Put the rest of the information in the way AbiWord puts its debug info!

    // Say who we are (with the CVS revision number) in case we have a bug in our filter output!
    streamOut << "<!-- KWord_Export_Filter_Version =";
    QString strVersion("$Revision$");
    // Eliminate the dollar signs
    //  (We don't want that the version number changes if the AbiWord file is itself put in a CVS storage.)
    streamOut << strVersion.mid(10).replace(QRegExp("\\$"),""); // Note: double escape character (one for C++, one for QRegExp!)
    streamOut << " -->\n\n";

#if 1
    // Some "security" to see if I have forgotten to run "make install"
    // (Can be deleted when the filter will be stable.)
    kdDebug(30506) << "abiwordexport.cc " << __DATE__ " " __TIME__ << " " << strVersion << endl;
#endif

    QString stringBufOut;
    // Now that we have the header, we can do the real work!
    ProcessDocTag (docNodeIn, NULL, stringBufOut, exportFilter);
    streamOut << stringBufOut;

    // Add the tail of the file
    streamOut << "</abiword>\n"; //Close the file for XML

    // At the end, close the output file

    ioDevice->close();

    delete exportFilter;
    delete ioDevice;

    return true;
}

