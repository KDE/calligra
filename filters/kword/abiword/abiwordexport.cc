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
#include <kdebug.h>
#include <qdom.h>

#include "processors.h"
#include "kqiodevicegzip.h"

ABIWORDExport::ABIWORDExport(KoFilter *parent, const char *name) :
                     KoFilter(parent, name) {
}

// Every tag has its own processing function. All of those functions
// have the same parameters since the functions are passed to
// ProcessSubtags throuch the TagProcessing class.  The top level
// function is ProcessDocTag and can be called with the node returned
// by QDomDocument::documentElement (). The tagData argument can be
// used to either pass variables down to the subtags or to allow
// subtags to return values. As a bare minimum the tag processing
// functions must handle the tag's attributes and the tag's subtags
// (which it can choose to ignore). Currently implemented is
// processing for the following tags and attributes:
//
// DOC
//   FRAMESETS
//     FRAMESET
//       PARAGRAPH
//          TEXT - Text Element
//          FORMATS
//            FORMAT id=1 pos= len=
//          LAYOUT
//            NAME value=


static void ProcessLayoutNameTag ( QDomNode myNode, void *tagData, QString & )
{
    QString *layout = (QString *) tagData;

    *layout = "";
    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ( "value", "QString", (void *) layout ) );
    ProcessAttributes (myNode, attrProcessingList);

    if ( *layout == "" )
    {
        *layout = "Standard";

        kdError(30506) << "Bad layout name value!" << endl;
    }

    AllowNoSubtags (myNode);
}


static void ProcessLayoutTag ( QDomNode myNode, void *tagData, QString &outputText )
{
    QString *layout = (QString *) tagData;

    AllowNoAttributes (myNode);

    *layout = "";
    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "NAME",      ProcessLayoutNameTag, (void *) layout ) );
    tagProcessingList.append ( TagProcessing ( "FOLLOWING", NULL,                 NULL            ) );
    tagProcessingList.append ( TagProcessing ( "COUNTER",   NULL,                 NULL            ) );
    tagProcessingList.append ( TagProcessing ( "FORMAT",    NULL,                 NULL            ) );
    tagProcessingList.append ( TagProcessing ( "TABULATOR", NULL,                 NULL            ) );
    tagProcessingList.append ( TagProcessing ( "FLOW",      NULL,                 NULL            ) );
    ProcessSubtags (myNode, tagProcessingList, outputText);
}

// FORMAT's subtags

static void ProcessItalicTag (QDomNode myNode, void* , QString& abiprops)
{
    int value=0;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ("value", "int", (void *)&value) );
    ProcessAttributes (myNode, attrProcessingList);

    if (value)
    {
        abiprops+="font-style:italic; "; // Note: Trailing space is important!
    }
}

static void ProcessUnderlineTag (QDomNode myNode, void* , QString& abiprops)
{
    int value=0;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ("value", "int", (void *)&value) );
    ProcessAttributes (myNode, attrProcessingList);

    if (value)
    {
        abiprops+="text-decoration:underline; "; // Note: Trailing space is important!
    }
}

static void ProcessWeightTag (QDomNode myNode, void* , QString& abiprops)
{
    int weight=50;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ("value", "int", (void *)&weight) );
    ProcessAttributes (myNode, attrProcessingList);

    if (weight>=75) //boldness threshold
    {
        abiprops += "font-weight:bold; "; // Note: Trailing space is important!
    }
    /* Not needed or am I wrong?
    else
    {
        abiprops += "font-weight:normal; "; // Note: Trailing space is important!
    }
    */
}

static void ProcessSizeTag (QDomNode myNode, void* , QString& abiprops)
{
    int size=-1;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ("value", "int", (void *)&size) );
    ProcessAttributes (myNode, attrProcessingList);

    if (size>0)
    {
        abiprops += "font-size:";
        abiprops += QString::number(size,10);
        abiprops += "pt"; // Don't forget the unit symbol!
        abiprops += "; "; // Note: Trailing space is important!
    }
}

static void ProcessFontTag (QDomNode myNode, void* , QString& abiprops)
{
    QString fontName;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ("name", "QString", (void *)&fontName) );
    ProcessAttributes (myNode, attrProcessingList);

    if (!fontName.isEmpty())
    {
        abiprops += "font-family:";
        abiprops += fontName; //TODO: font name translation
        abiprops += "; "; // Note: Trailing space is important!
    }
}

static void ProcessColorTag (QDomNode myNode, void* , QString& abiprops)
{
    int red,green,blue;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ("red"   , "int", (void *)&red   ) );
    attrProcessingList.append ( AttrProcessing ("green" , "int", (void *)&green ) );
    attrProcessingList.append ( AttrProcessing ("blue"  , "int", (void *)&blue  ) );
    ProcessAttributes (myNode, attrProcessingList);

    abiprops += "color:";

    //We must have two hex digits for each colour channel!
    abiprops += QString::number((red&0xf0)>>4,16);
    abiprops += QString::number(red&0x0f,16);

    abiprops += QString::number((green&0xf0)>>4,16);
    abiprops += QString::number(green&0x0f,16);

    abiprops += QString::number((blue&0xf0)>>4,16);
    abiprops += QString::number(blue&0x0f,16);

    abiprops += "; "; // Note: Trailing space is important!
}

static void ProcessVertAlignTag (QDomNode myNode, void* , QString& abiprops)
{
    int value=-1;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ("value", "int", (void *)&value) );
    ProcessAttributes (myNode, attrProcessingList);

    if (1==value)
    {
        abiprops += "text-position:subscript";
    }
    else if (2==value)
    {
        abiprops += "text-position:superscript";
    }
    // if the value is not the one of the two mentioned then we consider that we have nothing special!
}

// FormatData is a container for data retreived from the FORMAT tag
// and its subtags to be used in the PARAGRAPH tag.

class FormatData
{
    public:
        FormatData ()
        {}
        FormatData ( int p,
                     int l  ) : pos (p), len (l)
        {}

        int pos; // Start of text to which this format applies
        int len; // Len of text to which this format applies

        QString abiprops; // Value of the "props" attribute
                          // of Abiword's "<c>"	tag
};


static void ProcessFormatTag (QDomNode myNode, void *tagData, QString &)
{
    QValueList<FormatData> *formatDataList = (QValueList<FormatData> *) tagData;

    int formatId (-1);
    FormatData formatData (-1,-1);
    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ( "id",  "int", (void *) &formatId       ) );
    attrProcessingList.append ( AttrProcessing ( "pos", "int", (void *) &formatData.pos ) );
    attrProcessingList.append ( AttrProcessing ( "len", "int", (void *) &formatData.len ) );
    ProcessAttributes (myNode, attrProcessingList);

    if ( formatId != 1 )
    {
        kdError(30506) << "Unexpected FORMAT attribute id value " << formatId << "!" << endl;
    }

    if ( formatData.pos == -1 || formatData.len == -1 )
    {
        formatData.pos = 0;
        formatData.len = 0;

        kdError(30506) << "Missing formatting!" << endl;
    }

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "ITALIC",    ProcessItalicTag, NULL ) );
    tagProcessingList.append ( TagProcessing ( "UNDERLINE", ProcessUnderlineTag, NULL ) );
    tagProcessingList.append ( TagProcessing ( "WEIGHT",    ProcessWeightTag, NULL ) );
    tagProcessingList.append ( TagProcessing ( "SIZE",      ProcessSizeTag, NULL ) );
    tagProcessingList.append ( TagProcessing ( "FONT",      ProcessFontTag, NULL ) );
    tagProcessingList.append ( TagProcessing ( "COLOR",     ProcessColorTag,    NULL ) );
    tagProcessingList.append ( TagProcessing ( "VERTALIGN", ProcessVertAlignTag,NULL ) );

    //Now let's the sub tags fill in the AbiWord's "props" attribute
    ProcessSubtags (myNode, tagProcessingList, formatData.abiprops);

    //Find the last semi-comma
    int result=formatData.abiprops.findRev(";");

    if (result>=0)
    {
        // Remove the last semi-comma and the space thereafter
        formatData.abiprops.remove(result,2);
    }

    formatDataList->append (formatData);
}


static void ProcessFormatsTag ( QDomNode myNode, void *tagData, QString &outputText )
{
    QValueList<FormatData> *formatDataList = (QValueList<FormatData> *) tagData;

    AllowNoAttributes (myNode);

    (*formatDataList).clear ();
    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "FORMAT", ProcessFormatTag, (void *) formatDataList ) );
    ProcessSubtags (myNode, tagProcessingList, outputText);
}


static void ProcessTextTag ( QDomNode myNode, void *tagData, QString &)
{
    QString *tagText = (QString *) tagData;

    QDomText myText ( myNode.firstChild ().toText () );

    if ( !myText.isNull () )
    {
        *tagText = myText.data ();
    }
    else
    {
        *tagText = "";
    }

    AllowNoAttributes (myNode);

    AllowNoSubtags (myNode);
}

static void ProcessHardBreakTag ( QDomNode myNode, void *tagData, QString &)
{   // <HRDBREAK> not documented!
    bool *hardBreak = (bool *) tagData;

    int frameBreak=0; // Break in frame or page (if I have understand well!) (Default: no page break)

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ("frame", "int", (void*) frameBreak));
    ProcessAttributes (myNode, attrProcessingList);

    AllowNoSubtags (myNode);

    *hardBreak=(1==frameBreak);
}

// ProcessParagraphData () mangles the pure text through the
// formatting information stored in the FormatData list and prints it
// out to the export file.

static void ProcessParagraphData ( QString &paraText, QValueList<FormatData> &paraFormatDataList, QString &outputText )
{
    const QString strAmp ("&amp;");
    const QString strLt  ("&lt;");
    const QString strGt  ("&gt;");
    const QString strApos("&apos;");
    const QString strQuot("&quot;");

    const QRegExp regExpAmp ("&");
    const QRegExp regExpLt  ("<");
    const QRegExp regExpGt  (">");
    const QRegExp regExpApos("'");
    const QRegExp regExpQuot("\"");

    if (paraFormatDataList.isEmpty())
    {
        // No <FORMAT> tags were found, it is just normal text!
        if (!paraText.isEmpty())
        {
            QString str=paraText;
            //Code all possible predefined XML entities
            str.replace (regExpAmp , strAmp); //Must be the first!!
            str.replace (regExpLt  , strLt);
            str.replace (regExpGt  , strGt);
            str.replace (regExpApos, strApos);
            str.replace (regExpQuot, strQuot);
            outputText += str;
        }
    }
    else if ( paraText.length () > 0 )
    {
        QValueList<FormatData>::Iterator  paraFormatDataIt;  //Warning: cannot use "->" with it!!

        QString partialText;

        for ( paraFormatDataIt = paraFormatDataList.begin ();
              paraFormatDataIt != paraFormatDataList.end ();
              paraFormatDataIt++ )
        {
            //Retrieve text
            partialText=paraText.mid ( (*paraFormatDataIt).pos, (*paraFormatDataIt).len );
            //Code all possible predefined XML entities
            partialText.replace (regExpAmp , strAmp); //Must be the first!!
            partialText.replace (regExpLt  , strLt);
            partialText.replace (regExpGt  , strGt);
            partialText.replace (regExpApos, strApos);
            partialText.replace (regExpQuot, strQuot);
            // TODO: AbiWord tries to be 7bit clean! (So may be other replacements are needed!)

            if ((*paraFormatDataIt).abiprops.isEmpty())
            {
                //It's just normal text, so there is no "props" attribute
                //Note: you must use a <c> tag!
                outputText += "<c>";
            }
            else
            { //Text with properties
                outputText += "<c props=\"";
                outputText += (*paraFormatDataIt).abiprops;
                outputText += "\">";
            }
            outputText += partialText;
            outputText += "</c>";
       }

    }

}


static void ProcessParagraphTag ( QDomNode myNode, void *, QString   &outputText )
{
    AllowNoAttributes (myNode);

    QString paraText;
    QValueList<FormatData> paraFormatDataList;
    QString paraLayout;
    QValueList<TagProcessing> tagProcessingList;
    bool hardbreak=false; // Have we an hard break?
	
    tagProcessingList.append ( TagProcessing ( "TEXT",    ProcessTextTag,       (void *) &paraText           ) );
    tagProcessingList.append ( TagProcessing ( "FORMATS", ProcessFormatsTag,    (void *) &paraFormatDataList ) );
    tagProcessingList.append ( TagProcessing ( "LAYOUT",  ProcessLayoutTag,     (void *) &paraLayout         ) );
    tagProcessingList.append ( TagProcessing ( "HARDBRK", ProcessHardBreakTag,  (void *) &hardbreak          ) ); // Not documented!
    ProcessSubtags (myNode, tagProcessingList, outputText);

    if (hardbreak)
    {
        // Add an AbiWord page break
        outputText+="<pbr/>\n";
    }

    //Note: AbiWord at the state of version 0.7.12 cannot use styles yet but styles are defined in the file format!

#if 0
    if ( paraLayout == "Head 1" )
    {
        outputText += "<p style=\"Heading 1\">";  //Warning: No trailing white space or else it's in the text!!!
        ProcessParagraphData ( paraText, paraFormatDataList, outputText );
    }
    else if ( paraLayout == "Head 2" )
    {
        outputText += "<p style=\"Heading 2\">";  //Warning: No trailing white space or else it's in the text!!!
        ProcessParagraphData ( paraText, paraFormatDataList, outputText );
    }
    else if ( paraLayout == "Head 3" )
    {
        outputText += "<p style=\"Heading 3\">";  //Warning: No trailing white space or else it's in the text!!!
    }
    /*
    else if ( paraLayout == "Bullet List" )
    {
        outputText += "<p>"; //TODO
    }
    else if ( paraLayout == "Enumerated List" )
    {
        outputText += "<p>"; //TODO
    }
    else if ( paraLayout == "Alphabetical List" )
    {
        outputText += "<p>"; //TODO
    }
    */
    else
    {// We don't know the layout, so assume it's "Standard". It's better than to abort with an error!
        outputText += "<p style=\"Normal\">";  //Warning: No trailing white space or else it's in the text!!!
    }
#else
    outputText += "<p>";  //Warning: No trailing white space or else it's in the text!!!
#endif
    ProcessParagraphData ( paraText, paraFormatDataList, outputText );
    outputText += "</p>\n";
}


static void ProcessFramesetTag ( QDomNode myNode, void *, QString   &outputText )
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
        ProcessSubtags (myNode, tagProcessingList, outputText);

        outputText+="</section>\n";
    }
    //TODO: Treat the other types of frames (frameType)
}


static void ProcessFramesetsTag (QDomNode myNode, void *, QString   &outputText )
{
    AllowNoAttributes (myNode);

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "FRAMESET", ProcessFramesetTag, NULL ) );
    ProcessSubtags (myNode, tagProcessingList, outputText);
}

static void ProcessStyleTag (QDomNode myNode, void *, QString   &outputText )
{
    AllowNoAttributes (myNode);

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "NAME",          NULL, NULL ) );
    tagProcessingList.append ( TagProcessing ( "FOLLOWING",     NULL, NULL ) );
    tagProcessingList.append ( TagProcessing ( "FLOW",          NULL, NULL ) );
    tagProcessingList.append ( TagProcessing ( "INDENTS",       NULL, NULL ) );
    tagProcessingList.append ( TagProcessing ( "COUNTER",       NULL, NULL ) );
    tagProcessingList.append ( TagProcessing ( "LINESPACING",   NULL, NULL ) );
    tagProcessingList.append ( TagProcessing ( "LEFTBORDER",    NULL, NULL ) );
    tagProcessingList.append ( TagProcessing ( "RIGHTBORDER",   NULL, NULL ) );
    tagProcessingList.append ( TagProcessing ( "TOPBORDER",     NULL, NULL ) );
    tagProcessingList.append ( TagProcessing ( "BOTTOMBORDER",  NULL, NULL ) );
    tagProcessingList.append ( TagProcessing ( "FORMAT",        NULL, NULL ) );
    ProcessSubtags (myNode, tagProcessingList, outputText);
}

static void ProcessStylesPluralTag (QDomNode myNode, void *, QString   &outputText )
{
    AllowNoAttributes (myNode);

    // We have the advantage that for styles, KWord and AbiWord have nearly the same way.
    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "STYLE", ProcessStyleTag, NULL ) );
    ProcessSubtags (myNode, tagProcessingList, outputText);
}

static void ProcessDocTag (QDomNode myNode, void *,  QString &outputText)
{
    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ( "editor",        "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "mime",          "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "syntaxVersion", "", NULL ) );
    ProcessAttributes (myNode, attrProcessingList);

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "PAPER",       NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "ATTRIBUTES",  NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "FOOTNOTEMGR", NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "STYLES",      ProcessStylesPluralTag, NULL ) );
    tagProcessingList.append ( TagProcessing ( "PIXMAPS",     NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "SERIALL",     NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "FRAMESETS",   ProcessFramesetsTag, NULL ) );
    ProcessSubtags (myNode, tagProcessingList, outputText);
}

static bool writeOutputFileUncompressed(const QString& filename, const QCString& strOut)
{
    //Now all is ready to write to a file
    QFile fileOut (filename);

    if ( !fileOut.open (IO_WriteOnly) )
    {
        kdError(30506) << "Unable to open output file!" << endl;
        return false;
    }

    //Warning: do not use QString::length() (as in asciiexport.cc) but QCString::length()
    // "QString::length()" gives the number of characters, not the number of bytes needed to represent them in UTF8!
    fileOut.writeBlock ( (const char *) strOut, strOut.length() ); //Write the file

    fileOut.close (); //Really close the file
    return true;
}

static bool writeOutputFileGZipped(const QString& filename, const QCString& strOut)
{
    //Now all is ready to write to a file
    KQIODeviceGZip fileOut (filename);

    if ( !fileOut.open (IO_WriteOnly) )
    {
        kdError(30506) << "Unable to open output file!" << endl;
        return false;
    }

    //Warning: do not use QString::length() (as in asciiexport.cc) but QCString::length()
    // "QString::length()" gives the number of characters, not the number of bytes needed to represent them in UTF8!
    fileOut.writeBlock ( (const char *) strOut, strOut.length() ); //Write the file

    fileOut.close (); //Really close the file
    return true;
}

const bool ABIWORDExport::filter(const QString  &filenameIn,
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

    QDomDocument qDomDocumentIn;

    // let parse the buffer just read from the file
    qDomDocumentIn.setContent(byteArrayIn);

    QDomNode docNodeIn = qDomDocumentIn.documentElement ();

    QString stringBufOut;

    // Make the file header

    // First the XML header in UTF-8 version
    // (AbiWord and QString handles UTF-8 well, so we stay with this encoding!)
    stringBufOut = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    // First magic: "<abiword"
    stringBufOut += "<abiword version=\"unnumbered\">\n";
    // Second magic: "<!-- This file is an AbiWord document."
    stringBufOut += "<!-- This file is an AbiWord document. -->\n";
    // QUESTION: Do we need the full four line comment header?
    stringBufOut += "\n";

    // Put the rest of the information in the way AbiWord puts its debug info!

    // Say who we are (with the CVS revision number) in case we have a bug in our filter output!
    stringBufOut += "<!-- KWord_Export_Filter_Version =";
    QString strVersion("$Revision$");
    // Eliminate the dollar signs
    //  (We don't want that the version number changes if the AbiWord file is itself put in a CVS storage.)
    stringBufOut += strVersion.mid(10).replace(QRegExp("\\$"),""); // Note: double escape character (one for C++, one for QRegExp!)
    stringBufOut += " -->\n\n";

#if 1
    // Some "security" to see if I have forgotten to run "make install"
    // (Can be deleted when the filter will be stable.)
    kdDebug(30506) << "abiwordexport.cc " << __DATE__ " " __TIME__ << " " << strVersion << endl;
#endif

    // Now that we have the header, we can do the real work!
    ProcessDocTag (docNodeIn, NULL, stringBufOut);

    // Add the tail of the file
    stringBufOut += "</abiword>\n"; //Close the file for XML

    QCString strOut=stringBufOut.utf8(); //Retrieve UTF8 info into a byte array

    bool success=false;

    //Choose if gzipped or not

    //At first, find the last extension
    QString strExt;
    const int result=filenameOut.findRev('.');
    if (result>=0)
    {
        strExt=filenameOut.mid(result);
    }

    if ((strExt==".gz")||(strExt==".GZ")        //in case of .abw.gz (standard extension)
        ||(strExt==".zabw")||(strExt==".ZABW")) //in case of .zabw (extension used prioritary with AbiWord)
    {// GZipped
        success=writeOutputFileGZipped(filenameOut,strOut);
    }
    else
    {// Uncompressed
        success=writeOutputFileUncompressed(filenameOut,strOut);
    }

    return success;
}

