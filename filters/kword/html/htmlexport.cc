// $Header$

/*
   This file is part of the KDE project
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

/*
    Important changes

    04 Feb 2001  Nicolas GOUTTE <nicog@snafu.de>
        Re-write of HTML export
        (Replaces the old HTML export code completely)

*/

#include <qtextstream.h>
#include <qdom.h>

#include <kdebug.h>

#include <htmlexport.h>
#include <htmlexport.moc>

#include "ExportTagProcessing.h"
#include "ExportDialog.h"
#include "ExportFilterBase.h"

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


static void ProcessLayoutNameTag ( QDomNode myNode, void *tagData, QString &, ClassExportFilterBase* )
{
    LayoutData *layout = (LayoutData *) tagData;

    layout->styleName = "";
    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ( "value", "QString", (void *) &layout->styleName ) );
    ProcessAttributes (myNode, attrProcessingList);

    if ( layout->styleName == "" )
    {
        layout->styleName = "Standard";
        kdError(30503) << "Bad layout name value!" << endl;
    }

    AllowNoSubtags (myNode);
}

static void ProcessLayoutFlowTag ( QDomNode myNode, void *tagData, QString &, ClassExportFilterBase* )
{
    LayoutData *layout = (LayoutData *) tagData;

    layout->alignment = "";
    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ( "align", "QString", (void *) &layout->alignment ) );
    ProcessAttributes (myNode, attrProcessingList);

    AllowNoSubtags (myNode);
}

static void ProcessCounterTag ( QDomNode myNode, void *tagData, QString &, ClassExportFilterBase* )
{
    CounterData *counter = (CounterData *) tagData;
    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ( "numberingtype", "int", (void *) &counter->numbering ) );
    attrProcessingList.append ( AttrProcessing ( "type", "int", (void *) &counter->style ) );
    attrProcessingList.append ( AttrProcessing ( "depth", "int", (void *) &counter->depth ) );
    attrProcessingList.append ( AttrProcessing ( "start", "int", (void *) &counter->start ) );
    attrProcessingList.append ( AttrProcessing ( "lefttext", "QString", (void *) &counter->lefttext ) );
    attrProcessingList.append ( AttrProcessing ( "righttext", "QString", (void *) &counter->righttext ) );
    attrProcessingList.append ( AttrProcessing ( "bullet", "int", (void *) &counter->customCharacter ) );
    attrProcessingList.append ( AttrProcessing ( "bulletfont", "QString", (void *) &counter->customFont ) );
    ProcessAttributes (myNode, attrProcessingList);

    AllowNoSubtags (myNode);
}


// FORMAT's subtags

static void ProcessItalicTag (QDomNode myNode, void* formatDataPtr , QString&, ClassExportFilterBase*)
{
    FormatData* formatData = (FormatData*) formatDataPtr;

    int value=0;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ("value", "int", (void *)&value) );
    ProcessAttributes (myNode, attrProcessingList);

    formatData->italic=(value!=0);
}

static void ProcessUnderlineTag (QDomNode myNode, void* formatDataPtr , QString&, ClassExportFilterBase*)
{
    FormatData* formatData = (FormatData*) formatDataPtr;

    int value=0;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ("value", "int", (void *)&value) );
    ProcessAttributes (myNode, attrProcessingList);

    formatData->underline=(value!=0);
}

static void ProcessStrikeOutTag (QDomNode myNode, void* formatDataPtr , QString&, ClassExportFilterBase*)
{
    FormatData* formatData = (FormatData*) formatDataPtr;

    int value=0;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ("value", "int", (void *)&value) );
    ProcessAttributes (myNode, attrProcessingList);

    formatData->strikeout=(value!=0);
}

static void ProcessWeightTag (QDomNode myNode, void* formatDataPtr , QString&, ClassExportFilterBase*)
{
    FormatData* formatData = (FormatData*) formatDataPtr;

    int weight=50;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ("value", "int", (void *)&weight) );
    ProcessAttributes (myNode, attrProcessingList);

    formatData->weight=weight;
}

static void ProcessSizeTag (QDomNode myNode, void* formatDataPtr , QString&, ClassExportFilterBase*)
{
    FormatData* formatData = (FormatData*) formatDataPtr;

    int size=-1;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ("value", "int", (void *)&size) );
    ProcessAttributes (myNode, attrProcessingList);

    formatData->fontSize=size;
}

static void ProcessFontTag (QDomNode myNode, void* formatDataPtr , QString&, ClassExportFilterBase* )
{
    FormatData* formatData = (FormatData*) formatDataPtr;

    QString fontName;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ("name", "QString", (void *)&fontName) );
    ProcessAttributes (myNode, attrProcessingList);

    formatData->fontName=fontName;
}

static void ProcessColorTag (QDomNode myNode, void* formatDataPtr , QString&, ClassExportFilterBase*)
{
    FormatData* formatData = (FormatData*) formatDataPtr;

    int red,green,blue;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ("red"   , "int", (void *)&red   ) );
    attrProcessingList.append ( AttrProcessing ("green" , "int", (void *)&green ) );
    attrProcessingList.append ( AttrProcessing ("blue"  , "int", (void *)&blue  ) );
    ProcessAttributes (myNode, attrProcessingList);

    formatData->colour.setRgb(red, green, blue);
}

static void ProcessVertAlignTag (QDomNode myNode, void* formatDataPtr , QString&, ClassExportFilterBase*)
{
    FormatData* formatData = (FormatData*) formatDataPtr;

    int value=-1;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ("value", "int", (void *)&value) );
    ProcessAttributes (myNode, attrProcessingList);

    formatData->verticalAlignment=value;
}

static void ProcessSingleFormatTag (QDomNode myNode, void *tagData, QString &, ClassExportFilterBase* exportFilter)
{
    FormatData *formatData = (FormatData*) tagData;

    int formatId (-1);

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ( "id",  "int", (void *) &formatId       ) );
    attrProcessingList.append ( AttrProcessing ( "pos", "int", (void *) &formatData->pos ) );
    attrProcessingList.append ( AttrProcessing ( "len", "int", (void *) &formatData->len ) );
    ProcessAttributes (myNode, attrProcessingList);

    if ( formatId != 1 )
    {
        kdError(30503) << "Unexpected FORMAT attribute id value " << formatId << "!" << endl;
    }

    if ( formatData->pos == -1 || formatData->len == -1 )
    {
        formatData->pos = 0;
        formatData->len = 0;

        kdError(30503) << "Missing formatting!" << endl;
    }

    if ( 6 == formatId )
    {// <FORMAT id=6> have no length but has one character in <TEXT>
        //TODO: verifiy that KWord 0.9 still does it!
        formatData->realLen=1;
    }
    else
    {
        formatData->realLen=formatData->len;
    }

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "ITALIC",    ProcessItalicTag,   (void*) formatData ) );
    tagProcessingList.append ( TagProcessing ( "UNDERLINE", ProcessUnderlineTag,(void*) formatData ) );
    tagProcessingList.append ( TagProcessing ( "STRIKEOUT", ProcessStrikeOutTag,(void*) formatData ) );
    tagProcessingList.append ( TagProcessing ( "WEIGHT",    ProcessWeightTag,   (void*) formatData ) );
    tagProcessingList.append ( TagProcessing ( "SIZE",      ProcessSizeTag,     (void*) formatData ) );
    tagProcessingList.append ( TagProcessing ( "FONT",      ProcessFontTag,     (void*) formatData ) );
    tagProcessingList.append ( TagProcessing ( "COLOR",     ProcessColorTag,    (void*) formatData ) );
    tagProcessingList.append ( TagProcessing ( "VERTALIGN", ProcessVertAlignTag,(void*) formatData ) );

    QString strDummy;

    ProcessSubtags (myNode, tagProcessingList, strDummy, exportFilter);

}

static void ProcessIndentsTag (QDomNode myNode, void* tagData , QString&, ClassExportFilterBase*)
{
    LayoutData *layout = (LayoutData *) tagData;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ("first" , "double", (void *)&layout->indentFirst ) );
    attrProcessingList.append ( AttrProcessing ("left"  , "double", (void *)&layout->indentLeft  ) );
    attrProcessingList.append ( AttrProcessing ("right" , "double", (void *)&layout->indentRight ) );
    ProcessAttributes (myNode, attrProcessingList);
}

static void ProcessLayoutTag ( QDomNode myNode, void *tagData, QString &outputText, ClassExportFilterBase* exportFilter )
{
    LayoutData *layout = (LayoutData *) tagData;

    AllowNoAttributes (myNode);

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "NAME",        ProcessLayoutNameTag,     (void*) layout  ) );
    tagProcessingList.append ( TagProcessing ( "FOLLOWING",   NULL,                     NULL            ) );
    tagProcessingList.append ( TagProcessing ( "COUNTER",     ProcessCounterTag,        (void*) &layout->counter ) );
    tagProcessingList.append ( TagProcessing ( "FORMAT",      ProcessSingleFormatTag,   (void*) &layout->formatData ) );
    tagProcessingList.append ( TagProcessing ( "TABULATOR",   NULL,                     NULL            ) );
    tagProcessingList.append ( TagProcessing ( "FLOW",        ProcessLayoutFlowTag,     (void*) layout  ) );
    tagProcessingList.append ( TagProcessing ( "INDENTS",     ProcessIndentsTag,        (void*) layout  ) );
    tagProcessingList.append ( TagProcessing ( "OFFSETS",     NULL,                     NULL            ) );
    tagProcessingList.append ( TagProcessing ( "LINESPACING", NULL,                     NULL            ) );
    tagProcessingList.append ( TagProcessing ( "PAGEBREAKING",NULL,                     NULL            ) );
    ProcessSubtags (myNode, tagProcessingList, outputText, exportFilter);
}

static void ProcessFormatTag (QDomNode myNode, void *tagData, QString & outputText, ClassExportFilterBase* exportFilter)
{
    // To use in <FORMATS> elements

    ValueListFormatData *formatDataList = (ValueListFormatData *) tagData;

    FormatData formatData (-1,-1);

    ProcessSingleFormatTag(myNode,(void*) &formatData, outputText, exportFilter);

    formatDataList->append (formatData);
}


static void ProcessFormatsTag ( QDomNode myNode, void *tagData, QString &outputText, ClassExportFilterBase* exportFilter )
{
    ValueListFormatData *formatDataList = (ValueListFormatData *) tagData;

    AllowNoAttributes (myNode);

    (*formatDataList).clear ();
    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "FORMAT", ProcessFormatTag, (void *) formatDataList ) );
    ProcessSubtags (myNode, tagProcessingList, outputText, exportFilter);
}


static void ProcessTextTag ( QDomNode myNode, void *tagData, QString &, ClassExportFilterBase*)
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

QString ClassExportFilterBase::getHtmlOpeningTagExtraAttributes(void) const
{
	if (isXML())
	{
    	// XHTML must return an extra attribute defining its namespace (in the <html> opening tag)
	    return " xmlns=\"http://www.w3.org/1999/xhtml\""; // Leading space is important!
	}
	return QString::null;
}

static void CreateMissingFormatData(QString &paraText, ValueListFormatData &paraFormatDataList)
{
    ValueListFormatData::Iterator  paraFormatDataIt;
    int lastPos=0; // last position

    paraFormatDataIt = paraFormatDataList.begin ();
    while (paraFormatDataIt != paraFormatDataList.end ())
    {
        if ((*paraFormatDataIt).pos>lastPos)
        {
            //We must add a FormatData
            FormatData formatData(lastPos,(*paraFormatDataIt).pos-lastPos);
            formatData.missing=true;
            paraFormatDataList.insert(paraFormatDataIt,formatData);
        }
        lastPos=(*paraFormatDataIt).pos+(*paraFormatDataIt).realLen;
        paraFormatDataIt++; // To the next one, please!
    }
    // Add the last one if needed
    if ((int)paraText.length()>lastPos)
    {
        FormatData formatData(lastPos,paraText.length()-lastPos);
        formatData.missing=true;
        paraFormatDataList.append(formatData);
    }
}

static void ProcessParagraphTag ( QDomNode myNode, void *, QString   &outputText, ClassExportFilterBase* exportFilter)
{
    AllowNoAttributes (myNode);

    QString paraText;
    ValueListFormatData paraFormatDataList;
    LayoutData paraLayout;
    QValueList<TagProcessing> tagProcessingList;

    tagProcessingList.append ( TagProcessing ( "TEXT",    ProcessTextTag,       (void *) &paraText           ) );
    tagProcessingList.append ( TagProcessing ( "FORMATS", ProcessFormatsTag,    (void *) &paraFormatDataList ) );
    tagProcessingList.append ( TagProcessing ( "LAYOUT",  ProcessLayoutTag,     (void *) &paraLayout         ) );
    tagProcessingList.append ( TagProcessing ( "HARDBRK", NULL,                 NULL) ); // Not documented!
    ProcessSubtags (myNode, tagProcessingList, outputText, exportFilter);

    QString strParaText;

    exportFilter->ProcessParagraphData ( paraText, paraFormatDataList, strParaText);

    if (strParaText.isEmpty())
    {
        //An empty paragraph is not allowed in HTML, so add a non-breaking space!
        strParaText="&nbsp;";
    }

    // As KWord has only one depth of lists, we can process lists very simply.
    if ( paraLayout.counter.numbering == CounterData::NUM_LIST )
    {
        if (exportFilter->inList)
        {
            // We are in a list but does it have the right type?
            if ( paraLayout.counter.style!=exportFilter->typeList)
            {
                // No, then close the previous list
                if (exportFilter->orderedList)
                {
                    outputText+="</ol>\n";
                }
                else
                {
                    outputText+="</ul>\n";
                }
                exportFilter->inList=false; // We are not in a list anymore
            }
        }

        // Are we still in a list?
        if (!exportFilter->inList)
        {
            // We are not yet part of a list
            exportFilter->inList=true;
            outputText+=exportFilter->getStartOfListOpeningTag(paraLayout.counter.style,exportFilter->orderedList);
            exportFilter->typeList=paraLayout.counter.style;
        }
        // TODO: with Cascaded Style Sheet, we could add the exact counter type we want
        outputText += exportFilter->getParagraphElement("li",strParaText,paraLayout);
    }
    else
    {
        if (exportFilter->inList)
        {
            // The previous paragraphs were in a list, so we have to close it
            if (exportFilter->orderedList)
            {
                outputText+="</ol>\n";
            }
            else
            {
                outputText+="</ul>\n";
            }
            exportFilter->inList=false;
        }
        if ( paraLayout.counter.numbering == CounterData::NUM_CHAPTER )
        {
            QString strDepth=QString("h%1").arg(paraLayout.counter.depth + 1);
            outputText += exportFilter->getParagraphElement(strDepth,strParaText,paraLayout);
        }
        else
        {
            outputText += exportFilter->getParagraphElement("p",strParaText,paraLayout);
        }
    }
}


static void ProcessFramesetTag ( QDomNode myNode, void *, QString   &outputText, ClassExportFilterBase* exportFilter)
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

        QValueList<TagProcessing> tagProcessingList;
        tagProcessingList.append ( TagProcessing ( "FRAME",     NULL,                NULL ) );
        tagProcessingList.append ( TagProcessing ( "PARAGRAPH", ProcessParagraphTag, NULL ) );
        ProcessSubtags (myNode, tagProcessingList, outputText, exportFilter);

        // Are we still in a list?
        if (exportFilter->inList)
        {
            // We are in a list, so close it!
            if (exportFilter->orderedList)
            {
                outputText+="</ol>\n";
            }
            else
            {
                outputText+="</ul>\n";
            }
            exportFilter->inList=false;
        }
    }
    //TODO: Treat the other types of frames (frameType)
}


static void ProcessFramesetsTag (QDomNode myNode, void *, QString   &outputText, ClassExportFilterBase* exportFilter)
{
    AllowNoAttributes (myNode);

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "FRAMESET", ProcessFramesetTag, NULL ) );
    ProcessSubtags (myNode, tagProcessingList, outputText, exportFilter);
}

static void ProcessDocTag (QDomNode myNode, void *,  QString &outputText, ClassExportFilterBase* exportFilter)
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
    tagProcessingList.append ( TagProcessing ( "STYLES",      NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "PIXMAPS",     NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "SERIALL",     NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "FRAMESETS",   ProcessFramesetsTag, NULL ) );
    ProcessSubtags (myNode, tagProcessingList, outputText, exportFilter);
}

// ClassExportFilterBase

QString ClassExportFilterBase::escapeText(const QString& strIn) const
{
    QString strReturn;
    QChar ch;

    for (uint i=0; i<strIn.length(); i++)
    {
        ch=strIn[i];
        switch (ch.unicode())
        {
        case 38: // &
            {
                strReturn+="&amp;";
                break;
            }
        case 60: // <
            {
                strReturn+="&lt;";
                break;
            }
        case 62: // >
            {
                strReturn+="&gt;";
                break;
            }
        case 34: // "
            {
                strReturn+="&quot;";
                break;
            }
        // NOTE: the apostrophe ' is not escaped,
        // NOTE:  as HTML does not define &apos; by default (only XML/XHTML does)
        default:
            {
                // TODO: verify that the character ch can be expressed in the
                // TODO:  encoding in which we will write the HTML file.
                strReturn+=ch;
                break;
            }
        }
    }

    return strReturn;
}

bool ClassExportFilterBase::filter(const QString  &filenameIn, const QString  &filenameOut)
{

    KoStore koStoreIn (filenameIn, KoStore::Read);

    QByteArray byteArrayIn;

    QString strTitle;
    if ( koStoreIn.open ( "documentinfo.xml" ) )
    {
        byteArrayIn = koStoreIn.read ( koStoreIn.size () );
        koStoreIn.close ();

        QDomDocument qDomDocumentInfo;
        qDomDocumentInfo.setContent (byteArrayIn);

        // Search <title> element (be carefull: there are two of them!)
        QDomNodeList docNodeList = qDomDocumentInfo.elementsByTagName("title");

        // Now find the <title> element that is child of a <about> element
        for (uint i=0; i<docNodeList.count(); i++)
        {
            QDomNode node=docNodeList.item(i);
            kdDebug(30502) << " Parent name: " << node.parentNode().nodeName() << endl;
            if (node.parentNode().nodeName()=="about")
            {
                // We have the one we want!
                //  Therefore retrieve text of element (may be empty!)
                strTitle=node.toElement().text();
                kdDebug(30502) << "Found new title " << strTitle << endl;
            }
        }
    }
    else
    {
        // Note: we do not worry too much if we cannot open the document info!
        kdWarning(30502) << "Unable to open documentinfo.xml sub-file!" << endl;
    }

    if ( !koStoreIn.open ( "root" ) )
    {
        koStoreIn.close ();

        kdError(30503) << "Unable to open input file!" << endl;
        return false;
    }

    byteArrayIn = koStoreIn.read ( koStoreIn.size () );
    koStoreIn.close ();

    // let parse the buffer just read from the file
    qDomDocumentIn.setContent(byteArrayIn);

    QDomNode docNodeIn = qDomDocumentIn.documentElement ();

    //Now all is ready to write to a file
    QFile fileOut (filenameOut);

    if ( !fileOut.open (IO_WriteOnly) )
    {
        fileOut.close ();

        kdError(30503) << "Unable to open output file!" << endl;
        return false;
    }

    //Use a QTextStream, so we do not have a big buffer using plenty of memory.
    QTextStream streamOut(&fileOut);

    streamOut.setEncoding( QTextStream::UnicodeUTF8 ); // TODO: possibility of choosing other encodings

    // Make the file header

    if (isXML())
    {   //Write out the XML declaration
        streamOut << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
    }
    // write <!DOCTYPE
    streamOut << getDocType() << endl;

    // No "lang" or "xml:lang" attribute for <html>, as we do not know in which language the document is!
    streamOut << "<html"<< getHtmlOpeningTagExtraAttributes() << ">" << endl;

    streamOut << "<head>" << endl;

    // Declare that we are using UTF-8
    streamOut << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\""<< (isXML()?" /":"") << ">" << endl;

    // Say who we are (with the CVS revision number) in case we have a bug in our filter output!
    QString strVersion("$Revision$");
    // Eliminate the dollar signs
    //  (We don't want that the version number changes if the HTML file is itself put in a CVS storage.)
    streamOut << "<meta name=\"Generator\" content=\"KWord HTML Export Filter Version ="
              << strVersion.mid(10).replace(QRegExp("\\$"),"") // Note: double escape character (one for C++, one for QRegExp!)
              << "\""<< (isXML()?" /":"") // X(HT)ML closes empty elements, HTML not!
              << ">" << endl;

    if (strTitle.isEmpty())
    {
        // We have still no title for the document,
        //  therefore make one from the file name
        strTitle=filenameOut;
        const int result=strTitle.findRev("/");
        if (result>=0)
        {
            strTitle=strTitle.mid(result+1);
        }
    }

    streamOut << "<title>"<< escapeText(strTitle) <<"</title>" << endl;  // <TITLE> is mandatory in HTML 4.01 !

    //TODO: transform documentinfo.xml into many <META> elements (at least the author!)

    streamOut << getStyleElement(); //Includes an end of line at the end if it is not empty.

    streamOut << "</head>" << endl;

    streamOut << "<body" << getBodyOpeningTagExtraAttributes() << ">" << endl;

    // Now that we have the header, we can do the real work!
    QString stringBufOut;
    ProcessDocTag (docNodeIn, NULL, stringBufOut, this);
    streamOut << stringBufOut;

    // Add the tail of the file
    streamOut << "</body>\n</html>\n";

    fileOut.close();
    return true;

}

//
// ClassExportFilterHtmlTransitional (normal HTML 4.01 Transitional)
//

class ClassExportFilterHtmlTransitional : public ClassExportFilterBase
{
    public:
        ClassExportFilterHtmlTransitional(void) {}
        virtual ~ClassExportFilterHtmlTransitional(void) {}
    public: //virtual
        virtual QString getDocType(void) const;
        virtual QString getBodyOpeningTagExtraAttributes(void) const;
        virtual void ProcessParagraphData ( QString &paraText, ValueListFormatData &paraFormatDataList, QString &outputText);
        virtual QString getStartOfListOpeningTag(const CounterData::Style typeList, bool& ordered);
        virtual QString getParagraphElement(const QString& strTag, const QString& strParagraphText, LayoutData& layout);
};

QString ClassExportFilterHtmlTransitional::getDocType(void) const
{
    // We are TRANSITIONAL, as we want to use tags like <FONT>, <U> and explicit colours.
    return "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">";
}

QString ClassExportFilterHtmlTransitional::getBodyOpeningTagExtraAttributes(void) const
{
    // Define the background colour as white!
    return " bgcolor=\"#FFFFFF\""; // Leading space is important!
}

void ClassExportFilterHtmlTransitional::ProcessParagraphData ( QString &paraText, ValueListFormatData &paraFormatDataList, QString &outputText)
{
    if (! paraText.isEmpty() )
    {
        CreateMissingFormatData(paraText,paraFormatDataList);

        ValueListFormatData::Iterator  paraFormatDataIt;  //Warning: cannot use "->" with it!!

        QString partialText;

        for ( paraFormatDataIt = paraFormatDataList.begin ();
              paraFormatDataIt != paraFormatDataList.end ();
              paraFormatDataIt++ )
        {
            //Retrieve text
            partialText=paraText.mid ( (*paraFormatDataIt).pos, (*paraFormatDataIt).len );

            if ((*paraFormatDataIt).missing)
            {   // Format is not issued from KWord.
				//  So it is only the text (no additional formating.)
                if (outputText==" ")
                {//Just a space as text. Therefore we must use a non-breaking space.
                    outputText += "&nbsp;";
                }
                else
                {
                    //Code all possible predefined HTML entities
                    outputText += escapeText(partialText);
                }
                continue; // And back to the loop
            }

            // TODO: first and last characters of partialText should not be a space (white space problems!)
            // TODO: replace multiple spaces into non-breaking spaces!
            // Opening elements

            QString fontName = (*paraFormatDataIt).fontName;

            // Will a potential <font> tag have attributes?
            QString fontAttributes;

            if ( !fontName.isEmpty() )
            {
                fontAttributes+=" face=\"";
                fontAttributes+=fontName; // TODO: add alternative font names
                fontAttributes+="\"";
            }
            // Give the font size relatively (be kind with people with impered vision)
            // TODO: option to give absolute font sizes
            int size=(*paraFormatDataIt).fontSize;
            // 12pt is considered the normal size // TODO: relative to layout!
            if (size>0)
            {
                size /= 4;
                size -= 3;
                //if (size<-4) size=-4; // Cannot be triggered
                if (size>4) size=4;
                if (size)
                {
                    fontAttributes+=" size=\""; // in XML numbers must be quoted!
                    if (size>0)
                    {
                        fontAttributes+="+";
                    }
                    fontAttributes+=QString::number(size,10);
                    fontAttributes+="\"";
                }
            }
            if ( (*paraFormatDataIt).colour.isValid() )
            {
                // Give colour
                fontAttributes+=" color=\"";
                // QColor::name() does all the job :)
                fontAttributes+=(*paraFormatDataIt).colour.name();
                fontAttributes+="\"";
            }

            if (!fontAttributes.isEmpty())
            {
                // We have font attributes, so we must have a <font> element
                outputText+="<font";
                outputText+=fontAttributes;
                outputText+=">";
            }

            if ( (*paraFormatDataIt).weight >= 75 )
            {
                outputText+="<b>";
            }
            if ( (*paraFormatDataIt).italic )
            {
                outputText+="<i>";
            }
            if ( (*paraFormatDataIt).underline )
            {
                outputText+="<u>";
            }
            if ( (*paraFormatDataIt).strikeout )
            {
                outputText+="<s>";
            }
            if ( 1==(*paraFormatDataIt).verticalAlignment )
            {
                outputText+="<sub>"; //Subscript
            }
            if ( 2==(*paraFormatDataIt).verticalAlignment )
            {
                outputText+="<sup>"; //Superscript
            }

            // The text
            if (outputText==" ")
            {//Just a space as text. Therefore we must use a non-breaking space.
                outputText += "&nbsp;";
            }
            else
            {
                //Code all possible predefined HTML entities
                outputText += escapeText(partialText);
            }
            // Closing elements

            if ( 2==(*paraFormatDataIt).verticalAlignment )
            {
                outputText+="</sup>"; //Superscript
            }
            if ( 1==(*paraFormatDataIt).verticalAlignment )
            {
                outputText+="</sub>"; //Subscript
            }
            if ( (*paraFormatDataIt).strikeout )
            {
                outputText+="</s>";
            }
            if ( (*paraFormatDataIt).underline )
            {
                outputText+="</u>";
            }
            if ( (*paraFormatDataIt).italic )
            {
                outputText+="</i>";
            }
            if ( (*paraFormatDataIt).weight >= 75 )
            {
                outputText+="</b>";
            }

            if (!fontAttributes.isEmpty())
            {
                outputText+="</font>";
            }
        }
    }
}

QString ClassExportFilterHtmlTransitional::getStartOfListOpeningTag(const CounterData::Style typeList, bool& ordered)
{
    QString strResult;
    switch (typeList)
    {
    case CounterData::STYLE_NONE: // HTML cannot express STYLE_NONE
    case CounterData::STYLE_CUSTOMBULLET: // We cannot keep the custom type/style
    default:
        {
            orderedList=false;
            strResult="<ul>\n";
            break;
        }
    case CounterData::STYLE_CIRCLEBULLET:
        {
            orderedList=false;
            strResult="<ul type=\"circle\">\n";
            break;
        }
    case CounterData::STYLE_SQUAREBULLET:
        {
            orderedList=false;
            strResult="<ul type=\"square\">\n";
            break;
        }
    case CounterData::STYLE_DISCBULLET:
        {
            orderedList=false;
            strResult="<ul type=\"disc\">\n";
            break;
        }
    case CounterData::STYLE_NUM:
        {
            orderedList=true;
            strResult="<ol type=\"1\">\n";
            break;
        }
    case CounterData::STYLE_ALPHAB_L:
        {
            orderedList=true;
            strResult="<ol type=\"a\">\n";
            break;
        }
    case CounterData::STYLE_ALPHAB_U:
        {
            orderedList=true;
            strResult="<ol type=\"A\">\n";
            break;
        }
    case CounterData::STYLE_ROM_NUM_L:
        {
            orderedList=true;
            strResult="<ol type=\"i\">\n";
            break;
        }
    case CounterData::STYLE_ROM_NUM_U:
        {
            orderedList=true;
            strResult="<ol type=\"I\">\n";
            break;
        }
    case CounterData::STYLE_CUSTOM:
        {
            // We cannot keep the custom type/style
            orderedList=true;
            strResult="<ol>\n";
            break;
        }
    }
    return strResult;
}

QString ClassExportFilterHtmlTransitional::getParagraphElement(const QString& strTag, const QString& strParagraphText, LayoutData& layout)
{
    QString strAlign;

    if (strTag!="li")
    {
        // We do not set "left" explicitly, since KWord cannot do bi-di
        if (( layout.alignment== "right") || (layout.alignment=="center") || (layout.alignment=="justify"))
        {
            strAlign=QString(" align=\"%1\"").arg(layout.alignment);
        }
    }

    QString strElement;
    strElement+='<';
    strElement+=strTag;
    strElement+=strAlign;
    strElement+='>';

   // Opening elements

    QString fontName = layout.formatData.fontName;

    // Will a potential <font> tag have attributes?
    QString fontAttributes;

    if ( !fontName.isEmpty() )
    {
        fontAttributes+=" face=\"";
        fontAttributes+=fontName; // TODO: add alternative font names
        fontAttributes+="\"";
    }
#if 0
    // FIXME: using "size=+-" make things ugly. So for now, it is deactivated

    // Give the font size relatively (be kind with people with impered vision)
    // TODO: option to give absolute font sizes
    int size=layout.formatData.fontSize;
    // 12pt is considered the normal size // TODO: relative to layout!
    if (size>0)
    {
        size /= 4;
        size -= 3;
        //if (size<-4) size=-4; // Cannot be triggered
        if (size>4) size=4;
        if (size)
        {
            fontAttributes+=" size=\""; // in XML numbers must be quoted!
            if (size>0)
            {
                fontAttributes+="+";
            }
            fontAttributes+=QString::number(size,10);
            fontAttributes+="\"";
        }
    }
#endif
    if ( layout.formatData.colour.isValid() )
    {
        // Give colour
        fontAttributes+=" color=\"";
        // QColor::name() does all the job :)
        fontAttributes+=layout.formatData.colour.name();
        fontAttributes+="\"";
    }

    if (!fontAttributes.isEmpty())
    {
        // We have font attributes, so we must have a <font> element
        strElement+="<font";
        strElement+=fontAttributes;
        strElement+=">";
    }

    // Allow bold only if tag is not a heading!
    const bool bold=((layout.formatData.weight >= 75) & (strTag[0]!='h'));

    if ( bold )
    {
        strElement+="<b>";
    }
    if ( layout.formatData.italic )
    {
        strElement+="<i>";
    }
    if ( layout.formatData.underline )
    {
        strElement+="<u>";
    }
    if ( layout.formatData.strikeout )
    {
        strElement+="<s>";
    }
    if ( 1==layout.formatData.verticalAlignment )
    {
        strElement+="<sub>"; //Subscript
    }
    if ( 2==layout.formatData.verticalAlignment )
    {
        strElement+="<sup>"; //Superscript
    }

    // The text
    strElement+=strParagraphText;

    // Closing elements

    if ( 2==layout.formatData.verticalAlignment )
    {
        strElement+="</sup>"; //Superscript
    }
    if ( 1==layout.formatData.verticalAlignment )
    {
        strElement+="</sub>"; //Subscript
    }
    if ( layout.formatData.strikeout )
    {
        strElement+="</s>";
    }
    if ( layout.formatData.underline )
    {
        strElement+="</u>";
    }
    if ( layout.formatData.italic )
    {
        strElement+="</i>";
    }
    if ( bold )
    {
        strElement+="</b>";
    }

    if (!fontAttributes.isEmpty())
    {
        strElement+="</font>";
    }

    strElement+="</";
    strElement+=strTag;
    strElement+=">\n";
    return strElement;
}

//
// ClassExportFilterHtmlTransitional (normal XHTML 1.0 Transitional)
//

class ClassExportFilterXHtmlTransitional : public ClassExportFilterHtmlTransitional
{
    public:
        ClassExportFilterXHtmlTransitional(void) {}
        virtual ~ClassExportFilterXHtmlTransitional(void) {}
    public: //virtual
        virtual bool isXML(void) const {return true;}
        virtual QString getDocType(void) const;
};

QString ClassExportFilterXHtmlTransitional::getDocType(void) const
{
    // We are TRANSITIONAL, as we want to use tags like <FONT>, <U> and explicit colours.
    // Note "html" is lower-case in XHTML, while "DOCTYPE" and "!PUBLIC" are upper-case!
    return "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"DTD/xhtml1-transitional.dtd\">";
}

//
// ClassExportFilterHtmlSpartan (HTML 4.01 Strict, only document structure, no (HTML-)deprecated formattings)
//

class ClassExportFilterHtmlSpartan : public ClassExportFilterBase
{
    public:
        ClassExportFilterHtmlSpartan (void) {}
        virtual ~ClassExportFilterHtmlSpartan (void) {}
    public: //virtual
        virtual QString getDocType(void) const;
        virtual QString getBodyOpeningTagExtraAttributes(void) const;
        virtual void ProcessParagraphData ( QString &paraText, ValueListFormatData &paraFormatDataList, QString &outputText);
        virtual QString getStartOfListOpeningTag(const CounterData::Style typeList, bool& ordered);
        virtual QString getParagraphElement(const QString& strTag, const QString& strParagraphText, LayoutData& layout);
};

QString ClassExportFilterHtmlSpartan::getDocType(void) const
{
    // We are STRICT
    return "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">";
}

QString ClassExportFilterHtmlSpartan::getBodyOpeningTagExtraAttributes(void) const
{
    return QString::null;
}

void ClassExportFilterHtmlSpartan::ProcessParagraphData ( QString &paraText, ValueListFormatData &paraFormatDataList, QString &outputText)
{
    if (! paraText.isEmpty() )
    {

        CreateMissingFormatData(paraText,paraFormatDataList);

        ValueListFormatData::Iterator  paraFormatDataIt;  //Warning: cannot use "->" with it!!

        QString partialText;

        for ( paraFormatDataIt = paraFormatDataList.begin ();
              paraFormatDataIt != paraFormatDataList.end ();
              paraFormatDataIt++ )
        {
            //Retrieve text
            partialText=paraText.mid ( (*paraFormatDataIt).pos, (*paraFormatDataIt).len );

            if ((*paraFormatDataIt).missing)
            {   //Format is not issued from KWord. Therefore is only the layout
                // So it is only the text
                if (outputText==" ")
                {//Just a space as text. Therefore we must use a non-breaking space.
                    outputText += "&nbsp;";
                }
                else
                {
                    //Code all possible predefined HTML entities
                    outputText += escapeText(partialText);
                }
                continue; // And back to the loop
            }

            // TODO: first and last characters of partialText should not be a space (white space problems!)
            // TODO: replace multiples spaces in non-breaking spaces!
            // Opening elements
            // <sub> and <sup> are not really considered being formatting by HTML, so we keep them here!
            if ( 1==(*paraFormatDataIt).verticalAlignment )
            {
                outputText+="<sub>"; //Subscript
            }
            if ( 2==(*paraFormatDataIt).verticalAlignment )
            {
                outputText+="<sup>"; //Superscript
            }

            // The text
            if (outputText==" ")
            {//Just a space as text. Therefore we must use a non-breaking space.
                outputText += "&nbsp;";
            }
            else
            {
                //Code all possible predefined HTML entities
                outputText += escapeText(partialText);
            }

            // Closing elements
            if ( 2==(*paraFormatDataIt).verticalAlignment )
            {
                outputText+="</sup>"; //Superscript
            }
            if ( 1==(*paraFormatDataIt).verticalAlignment )
            {
                outputText+="</sub>"; //Subscript
            }
        }
    }
}

QString ClassExportFilterHtmlSpartan::getStartOfListOpeningTag(const CounterData::Style typeList, bool& ordered)
{
    QString strResult;
    switch (typeList)
    {
    case CounterData::STYLE_NONE:
    default:
        {
            orderedList=false;
            strResult="<ul>\n";
            break;
        }
    case CounterData::STYLE_NUM:
    case CounterData::STYLE_ALPHAB_L:
    case CounterData::STYLE_ALPHAB_U:
    case CounterData::STYLE_ROM_NUM_L:
    case CounterData::STYLE_ROM_NUM_U:
    case CounterData::STYLE_CUSTOM:
        {
            orderedList=true;
            strResult="<ol>\n";
            break;
        }
    }
    return strResult;
}

QString ClassExportFilterHtmlSpartan::getParagraphElement(const QString& strTag, const QString& strParagraphText, LayoutData&)
{
    QString strElement;
    strElement+='<';
    strElement+=strTag;
    strElement+='>';
    strElement+=strParagraphText;
    strElement+="</";
    strElement+=strTag;
    strElement+=">\n";
    return strElement;
}

//
// ClassExportFilterXHtmlSpartan (HTML 4.01 Strict, only document structure, no (HTML-)deprecated formattings)
//

class ClassExportFilterXHtmlSpartan : public ClassExportFilterHtmlSpartan
{
    public:
        ClassExportFilterXHtmlSpartan (void) {}
        virtual ~ClassExportFilterXHtmlSpartan (void) {}
    public: //virtual
        virtual bool isXML(void) const {return true;}
        virtual QString getDocType(void) const;
};

QString ClassExportFilterXHtmlSpartan::getDocType(void) const
{
    // We are STRICT
    return "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"DTD/xhtml1-strict.dtd\">";
}

// ClassExportFilterHtmlStyle (HTML 4.01 Strict, style using CSS2, no style sheets)
class ClassExportFilterHtmlStyle : public ClassExportFilterBase
{
    public:
        ClassExportFilterHtmlStyle (void) {}
        virtual ~ClassExportFilterHtmlStyle (void) {}
    public: //virtual
        virtual QString getDocType(void) const;
        virtual QString getBodyOpeningTagExtraAttributes(void) const;
        virtual void ProcessParagraphData ( QString &paraText, ValueListFormatData &paraFormatDataList, QString &outputText);
        virtual QString getStyleElement(void);
        virtual QString getStartOfListOpeningTag(const CounterData::Style typeList, bool& ordered);
        virtual QString getParagraphElement(const QString& strTag, const QString& strParagraphText, LayoutData& layout);
};

QString ClassExportFilterHtmlStyle::getDocType(void) const
{
    // We are STRICT
    return "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">";
}

void ClassExportFilterHtmlStyle::ProcessParagraphData ( QString &paraText, ValueListFormatData &paraFormatDataList, QString &outputText)
{
    if (! paraText.isEmpty() )
    {

        CreateMissingFormatData(paraText,paraFormatDataList);

        ValueListFormatData::Iterator  paraFormatDataIt;  //Warning: cannot use "->" with it!!

        QString partialText;

        for ( paraFormatDataIt = paraFormatDataList.begin ();
              paraFormatDataIt != paraFormatDataList.end ();
              paraFormatDataIt++ )
        {
            //Retrieve text
            partialText=paraText.mid ( (*paraFormatDataIt).pos, (*paraFormatDataIt).len );

            if ((*paraFormatDataIt).missing)
            {   //Format is not issued from KWord. Therefore is only the layout
                // So it is only the text
                if (outputText==" ")
                {//Just a space as text. Therefore we must use a non-breaking space.
                    outputText += "&nbsp;";
                }
                else
                {
                    //Code all possible predefined HTML entities
                    outputText += escapeText(partialText);
                }
                continue; // And back to the loop
            }

            // TODO: first and last characters of partialText should not be a space (white space problems!)
            // TODO: replace multiples spaces in non-breaking spaces!
            // Opening elements
            outputText+="<span style=\"";

            // Font name
            QString fontName = (*paraFormatDataIt).fontName;
            if ( !fontName.isEmpty() )
            {
                outputText+="font-family: ";
                outputText+=fontName; // TODO: add alternative font names
                outputText+="; ";
            }

            // Font style
            outputText+="font-style: ";
            if ( (*paraFormatDataIt).italic )
            {
                outputText+="italic";
            }
            else
            {
                outputText+="normal";
            }
            outputText+="; ";

            outputText+="font-weight: ";
            if ( (*paraFormatDataIt).weight >= 75 )
            {
                outputText+="bold";
            }
            else
            {
                outputText+="normal";
            }
            outputText+="; ";

            const int size=(*paraFormatDataIt).fontSize;
            if (size>0)
            {
                // We use absolute font sizes.
                outputText+="font-size: ";
                outputText+=QString::number(size,10);
                outputText+="pt; ";
            }

            if ( (*paraFormatDataIt).colour.isValid() )
            {
                // Give colour
                outputText+="color: ";
                // QColor::name() does all the job :)
                outputText+=(*paraFormatDataIt).colour.name();
                outputText+="; ";
            }

            outputText+="text-decoration: ";
            if ( (*paraFormatDataIt).underline )
            {
                outputText+="underline";
            }
            else if ( (*paraFormatDataIt).strikeout )
            {
                outputText+="line-through";
            }
            else
            {
                outputText+="none";
            }
            //outputText+="; ";
            outputText+="\">"; // close span opening tag
            if ( 1==(*paraFormatDataIt).verticalAlignment )
            {
                outputText+="<sub>"; //Subscript
            }
            if ( 2==(*paraFormatDataIt).verticalAlignment )
            {
                outputText+="<sup>"; //Superscript
            }

            // The text
            if (outputText==" ")
            {//Just a space as text. Therefore we must use a non-breaking space.
                outputText += "&nbsp;";
            }
            else
            {
                //Code all possible predefined HTML entities
                outputText += escapeText(partialText);
            }

			// Closing elements
            if ( 2==(*paraFormatDataIt).verticalAlignment )
            {
                outputText+="</sup>"; //Superscript
            }
            if ( 1==(*paraFormatDataIt).verticalAlignment )
            {
                outputText+="</sub>"; //Subscript
            }
            outputText+="</span>";
        }
    }
}

QString ClassExportFilterHtmlStyle::getStyleElement(void)
{
    QString str;
    str="<style type=\"text/css\">\n";
    str+="<!--\n"; //Put the style under comments to increase the compatibility with old browsers
    str+="BODY { background-color: #FFFFFF }\n";
    str+="-->\n";
    str+="</style>\n";
    return str;
}

QString ClassExportFilterHtmlStyle::getBodyOpeningTagExtraAttributes(void) const
{
    return QString::null;
}

QString ClassExportFilterHtmlStyle::getStartOfListOpeningTag(const CounterData::Style typeList, bool& ordered)
{
    QString strResult;
    switch (typeList)
    {
    case CounterData::STYLE_CUSTOMBULLET: // We cannot keep the custom type/style
    default:
        {
            orderedList=false;
            strResult="<ul>\n";
            break;
        }
    case CounterData::STYLE_NONE:
        {
            orderedList=false;
            strResult="<ul style=\"list-style-type:none\">\n";
            break;
        }
    case CounterData::STYLE_CIRCLEBULLET:
        {
            orderedList=false;
            strResult="<ul style=\"list-style-type:circle\">\n";
            break;
        }
    case CounterData::STYLE_SQUAREBULLET:
        {
            orderedList=false;
            strResult="<ul style=\"list-style-type:square\">\n";
            break;
        }
    case CounterData::STYLE_DISCBULLET:
        {
            orderedList=false;
            strResult="<ul style=\"list-style-type:disc\">\n";
            break;
        }
    case CounterData::STYLE_NUM:
        {
            orderedList=true;
            strResult="<ol style=\"list-style-type:decimal\">\n";
            break;
        }
    case CounterData::STYLE_ALPHAB_L:
        {
            orderedList=true;
            strResult="<ol style=\"list-style-type:lower-alpha\">\n";
            break;
        }
    case CounterData::STYLE_ALPHAB_U:
        {
            orderedList=true;
            strResult="<ol style=\"list-style-type:upper-alpha\">\n";
            break;
        }
    case CounterData::STYLE_ROM_NUM_L:
        {
            orderedList=true;
            strResult="<ol style=\"list-style-type:lower-roman\">\n";
            break;
        }
    case CounterData::STYLE_ROM_NUM_U:
        {
            orderedList=true;
            strResult="<ol style=\"list-style-type:upper-roman\">\n";
            break;
        }
    case CounterData::STYLE_CUSTOM:
        {
            // We cannot keep the custom type/style
            orderedList=true;
            strResult="<ol>\n";
            break;
        }
    }
    return strResult;
}

QString ClassExportFilterHtmlStyle::getParagraphElement(const QString& strTag, const QString& strParagraphText, LayoutData& layout)
{
    QString strElement;
    strElement+='<';
    strElement+=strTag;

    // Opening elements
    strElement+=" style=\"";

    // We do not set "left" explicitly, since KWord cannot do bi-di
    if (( layout.alignment== "right") || (layout.alignment=="center") || (layout.alignment=="justify"))
    {
        strElement+=QString("text-align:%1;").arg(layout.alignment);
    }

    if ( layout.indentLeft!=0.0 )
    {
        strElement+=QString("margin-left:%1pt;").arg(layout.indentLeft);
    }

    if ( layout.indentRight!=0.0 )
    {
        strElement+=QString("margin-right:%1pt;").arg(layout.indentRight);
    }

    if ( layout.indentFirst!=0.0 )
    {
        strElement+=QString("text-indent:%1pt;").arg(layout.indentFirst);
    }

    // Font name
    QString fontName = layout.formatData.fontName;
    if ( !fontName.isEmpty() )
    {
        strElement+="font-family: ";
        strElement+=fontName; // TODO: add alternative font names
        strElement+="; ";
    }

    // Font style
    strElement+="font-style: ";
    if ( layout.formatData.italic )
    {
        strElement+="italic";
    }
    else
    {
        strElement+="normal";
    }
    strElement+="; ";

    strElement+="font-weight: ";
    if ( layout.formatData.weight >= 75 )
    {
        strElement+="bold";
    }
    else
    {
        strElement+="normal";
    }
    strElement+="; ";

    const int size=layout.formatData.fontSize;
    if (size>0)
    {
        // We use absolute font sizes.
        strElement+="font-size: ";
        strElement+=QString::number(size,10);
        strElement+="pt; ";
    }

    if ( layout.formatData.colour.isValid() )
    {
        // Give colour
        strElement+="color: ";
        // QColor::name() does all the job :)
        strElement+=layout.formatData.colour.name();
        strElement+="; ";
    }

    strElement+="text-decoration: ";
    if ( layout.formatData.underline )
    {
        strElement+="underline";
    }
    else if ( layout.formatData.strikeout )
    {
        strElement+="line-through";
    }
    else
    {
        strElement+="none";
    }
    //strElement+="; ";
    strElement+="\">"; // close opening tag

    if ( 1==layout.formatData.verticalAlignment )
    {
        strElement+="<sub>"; //Subscript
    }
    if ( 2==layout.formatData.verticalAlignment )
    {
        strElement+="<sup>"; //Superscript
    }

    // The text
    strElement+=strParagraphText;
    // Closing elements

    if ( 2==layout.formatData.verticalAlignment )
    {
        strElement+="</sup>"; //Superscript
    }
    if ( 1==layout.formatData.verticalAlignment )
    {
        strElement+="</sub>"; //Subscript
    }

    strElement+="</";
    strElement+=strTag;
    strElement+=">\n";
    return strElement;
}

//
// ClassExportFilterXHtmlStyle (HTML 4.01 Strict, style using CSS2, no style sheets)
//

class ClassExportFilterXHtmlStyle : public ClassExportFilterHtmlStyle
{
    public:
        ClassExportFilterXHtmlStyle (void) {}
        virtual ~ClassExportFilterXHtmlStyle (void) {}
    public: //virtual
        virtual bool isXML(void) const {return true;}
        virtual QString getDocType(void) const;
        virtual QString getStyleElement(void);
};

QString ClassExportFilterXHtmlStyle::getDocType(void) const
{
    // We are STRICT
    return "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"DTD/xhtml1-strict.dtd\">";
}

QString ClassExportFilterXHtmlStyle::getStyleElement(void)
{
    //NOTE: in XHTML 1.0, you cannot put the style definition into HTML comments
    QString str;
    str="<style type=\"text/css\">\n";
    str+="BODY { background-color: #FFFFFF }\n"; //BODY in upper case or in lower case?
    str+="</style>\n";
    return str;
}

//
// HTMLExport
//

HTMLExport::HTMLExport(KoFilter *parent, const char *name) :
                     KoFilter(parent, name) {
}

bool HTMLExport::filter(const QString  &filenameIn,
                        const QString  &filenameOut,
                        const QString  &from,
                        const QString  &to,
                        const QString  &)
{
    if ((from != "application/x-kword") || (to != "text/html"))
    {
        return false;
    }

#if 1
    // Some "security" to see if I have forgotten to run "make install"
    // (Can be deleted when the filter will be stable.)
    kdDebug(30503) << "htmlexport.cc " << __DATE__ " " __TIME__ << " " << "$Revision$" << endl;
#endif

    HtmlExportDialog* dialog = new HtmlExportDialog();

    if (!dialog)
    {
        kdError(30503) << "Dialog has not been created Aborting!" << endl;
        return false;
    }

    if (!dialog->exec())
    {
        kdError(30503) << "Dialog was aborted! Aborting filter!" << endl;
        return false;
    }

    QString param=dialog->getState();

    ClassExportFilterBase* exportFilter=NULL;

    kdDebug(30503) << "Received parameters = :" << param << ":" << endl;

    if (param=="XHTML-SPARTAN")
    { // spartan XHTML 1.0
        kdDebug(30503) << "Spartan XHTML option" << endl;
        exportFilter=new ClassExportFilterXHtmlSpartan;
    }
    else if (param=="HTML-SPARTAN")
    { // spartan HTML 4.01
        kdDebug(30503) << "Spartan HTML option" << endl;
        exportFilter=new ClassExportFilterHtmlSpartan;
    }
    else if (param=="XHTML-TRANSITIONAL")
    { // transitional XHTML 1.0
        kdDebug(30503) << "Transitional XHTML option" << endl;
        exportFilter=new ClassExportFilterXHtmlTransitional;
    }
    else if (param=="HTML-TRANSITIONAL")
    { // transitional HTML 4.01
        kdDebug(30503) << "Transitional HTML option" << endl;
        exportFilter=new ClassExportFilterHtmlTransitional;
    }
    else if (param=="XHTML-STYLE")
    { // Style XHTML 1.0
        kdDebug(30503) << "Style XHTML option" << endl;
        exportFilter=new ClassExportFilterXHtmlStyle;
    }
    else if (param=="HTML-STYLE")
    { // Style HTML 4.01
        kdDebug(30503) << "Style HTML option" << endl;
        exportFilter=new ClassExportFilterHtmlStyle;
    }
    else if (param.contains("XHTML",false)>0)
    { //XHTML 1.0 Transitional
        kdDebug(30503) << "Unknown XHTML option" << endl;
        exportFilter=new ClassExportFilterXHtmlTransitional;
    }
    else if (param.contains("HTML",false)>0)
    { //XHTML 1.0 Transitional
        kdDebug(30503) << "Unknown HTML option" << endl;
        exportFilter=new ClassExportFilterHtmlTransitional;
    }
    else
    { // default: XHTML 1.0 Transitional
        kdDebug(30503) << "Unknown option" << endl;
        exportFilter=new ClassExportFilterXHtmlTransitional;
    }

    if (!exportFilter)
    {
        kdError(30503) << "No (X)HTML filter created! Aborting! (Memory problem?)" << endl;
        delete dialog;
        return false;
    }

    // Do the work!
    bool result = exportFilter->filter(filenameIn,filenameOut);

    delete exportFilter;
    delete dialog;

    return result;
}
