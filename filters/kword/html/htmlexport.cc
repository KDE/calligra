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

#include <htmlexport.h>
#include <htmlexport.moc>
#include <kdebug.h>
#include <qdom.h>
#include <qtextstream.h>

#undef DEBUG_KWORD_TAGS // Debugging KWord's tags and attributes

class ClassExportFilterBase
{
    public:
        ClassExportFilterBase(void) {}
        virtual ~ClassExportFilterBase(void) {}
    public: //Non-virtual
        const bool filter(const QString  &filenameIn, const QString  &filenameOut,
                          const QString  &from, const QString  &to, const QString& );
        QString& escapeText(QString& str) const;
    public: //virtual
        virtual bool isXML(void) {return false;}
        virtual QString getDocType(void) const = 0;
        virtual QString getHtmlOpeningTagExtraAttributes(void) const = 0;
    protected:
        QDomDocument qDomDocumentIn;
};

// Start processor's header

// The class TagProcessing and the two functions ProcessSubtags () and
// AllowNoSubtags () allow for easing parsing of subtags in the
// current tag. If don't expect any subtags you call AllowNoSubtags ().
// Otherwise you create a list of TagProcessing elements and pass that
// to ProcessSubtags () which will go through all subtags it can find,
// call the corresponding processing function, and do all the
// necessary error handling.

class TagProcessing
{
    public:
        TagProcessing ()
        {}

        TagProcessing (QString  n,
                       void     (*p)(QDomNode, void *, QString &, ClassExportFilterBase*),
                       void    *d) : name (n), processor (p), data (d)
        {}

        QString  name;
        void     (*processor)(QDomNode, void *, QString &, ClassExportFilterBase*);
        void    *data;
};

void ProcessSubtags     ( QDomNode                   parentNode,
                          QValueList<TagProcessing>  &tagProcessingList,
                          QString                    &outputText,
                          ClassExportFilterBase      *exportFilter);

void AllowNoSubtags ( QDomNode  myNode );


// The class AttrProcessing and the two functions ProcessAttributes ()
// and AllowNoSubtags () allow for easing parsing of the current tag's
// attributes. If don't expect any attributes you call AllowNoAttributes ().
// Otherwise you create a list of AttrProcessing elements and pass
// that to ProcessAttributes () which will go through all attributes
// it can find, retreive the value in the datatype defined, and do all
// the necessary error handling.


class AttrProcessing
{
    public:
        AttrProcessing ()
        {}

        AttrProcessing ( QString   n,
                         QString   t,
                         void     *d  ) : name (n), type (t), data (d)
        {}

        QString   name;
        QString   type;
        void     *data;
};

void ProcessAttributes ( QDomNode                     myNode,
                         QValueList<AttrProcessing>  &attrProcessingList );

void AllowNoAttributes ( QDomNode  myNode );

// End processor's header

// Start processor's functions

// The class TagProcessing and the two functions ProcessSubtags () and
// AllowNoSubtags () allow for easing parsing of subtags in the
// current tag. If don't expect any subtags you call AllowNoSubtags ().
// Otherwise you create a list of TagProcessing elements and pass that
// to ProcessSubtags () which will go through all subtags it can find,
// call the corresponding processing function, and do all the
// necessary error handling.

void ProcessSubtags ( QDomNode                    parentNode,
                      QValueList<TagProcessing>  &tagProcessingList,
                      QString                    &outputText,
                      ClassExportFilterBase      *exportFilter)
{
    QDomNode childNode;

    for ( childNode = parentNode.firstChild (); !childNode.isNull (); childNode = childNode.nextSibling () )
    {
//      if ( childNode.isElement () )   // doesn't work!!!
        if ( childNode.nodeType () == QDomNode::ElementNode )
        {
            bool found = false;

            QValueList<TagProcessing>::Iterator  tagProcessingIt;

            for ( tagProcessingIt = tagProcessingList.begin ();
                  !found && tagProcessingIt != tagProcessingList.end ();
                  tagProcessingIt++ )
            {
                if ( childNode.nodeName () == (*tagProcessingIt).name )
                {
                    found = true;

                    if ( (*tagProcessingIt).processor != NULL )
                    {
                        ((*tagProcessingIt).processor) ( childNode, (*tagProcessingIt).data, outputText, exportFilter );
                    }
                    else
                    {
//                      kdError () << "<para>ignoring " << childNode.nodeName ()
//                                      << " tag in " << parentNode.nodeName () << "!</para>" << endl;
                    }
                }
            }

            if ( !found )
            {
                kdError(30503) << "Unexpected tag " << childNode.nodeName ()
                               << " in " << parentNode.nodeName () << "!" << endl;
            }
        }
    }
}

#ifdef DEBUG_KWORD_TAGS
// Version for debugging (process all sub tags)
void AllowNoSubtags ( QDomNode  myNode )
{
    QString outputText;
    QValueList<TagProcessing> tagProcessingList;
    ProcessSubtags (myNode, tagProcessingList, outputText, NULL);
}
#else
// Normal version: no subtags expected, so do not search any!
void AllowNoSubtags ( QDomNode )
{
}
#endif

// The class AttrProcessing and the two functions ProcessAttributes ()
// and AllowNoSubtags () allow for easing parsing of the current tag's
// attributes. If don't expect any attributes you call AllowNoAttributes ().
// Otherwise you create a list of AttrProcessing elements and pass
// that to ProcessAttributes () which will go through all attributes
// it can find, retreive the value in the datatype defined, and do all
// the necessary error handling.

void ProcessAttributes ( QDomNode                     myNode,
                         QValueList<AttrProcessing>  &attrProcessingList )
{
    QDomNamedNodeMap myAttribs ( myNode.toElement ().attributes () );

    int i, n;
    n = myAttribs.length ();

    for ( i = 0; i < n; i++ )
    {
        QDomAttr myAttrib ( myAttribs.item (i).toAttr () );

        if ( !myAttrib.isNull () )
        {
            bool found = false;

            QValueList<AttrProcessing>::Iterator attrProcessingIt;

            for ( attrProcessingIt = attrProcessingList.begin ();
                  !found && attrProcessingIt != attrProcessingList.end ();
                  attrProcessingIt++ )
            {
                if ( myAttrib.name () == (*attrProcessingIt).name )
                {
                    found = true;

                    if ( (*attrProcessingIt).data != NULL )
                    {
                        if ( (*attrProcessingIt).type == "QString" )
                        {
                            *((QString *) (*attrProcessingIt).data) = myAttrib.value ();
                        }
                        else if ( (*attrProcessingIt).type == "int" )
                        {
                            *((int *) (*attrProcessingIt).data) = myAttrib.value ().toInt ();
                        }
                        else
                        {
                            kdError(30503) << "Unexpected data type " << (*attrProcessingIt).type << " in " <<
                                           myNode.nodeName () << " attribute " << (*attrProcessingIt).name
                                           << "!" << endl;
                        }
                    }
                    else
                    {
//                      kdError(30503) << "<para>ignoring " << myNode.tagName << " attribute "
//                                     << (*attrProcessingIt).name << "!</para>" << endl;
                    }
                }
            }

            if ( !found )
            {
                kdError(30503) << "Unexpected attribute " << myAttrib.name () << " in " <<
                                  myNode.nodeName () << "!" << endl;
            }
        }
    }
}

#ifdef DEBUG_KWORD_TAGS
// Version for debugging (process all attributes)
void AllowNoAttributes ( QDomNode  myNode )
{
    QValueList<AttrProcessing> attrProcessingList;
    ProcessAttributes (myNode, attrProcessingList);
}
#else
// Normal version: no attributes expected, so do not process any!
void AllowNoAttributes ( QDomNode )
{
}
#endif

// End processor's functions

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


// FormatData is a container for data retreived from the FORMAT tag
// and its subtags to be used in the PARAGRAPH tag.

static void ProcessLayoutNameTag ( QDomNode myNode, void *tagData, QString &, ClassExportFilterBase* )
{
    QString *layout = (QString *) tagData;

    *layout = "";
    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ( "value", "QString", (void *) layout ) );
    ProcessAttributes (myNode, attrProcessingList);

    if ( *layout == "" )
    {
        *layout = "Standard";

        kdError(30503) << "Bad layout name value!" << endl;
    }

    AllowNoSubtags (myNode);
}


static void ProcessLayoutTag ( QDomNode myNode, void *tagData, QString &outputText, ClassExportFilterBase* exportFilter )
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
    ProcessSubtags (myNode, tagProcessingList, outputText, exportFilter);
}

class FormatData
{
    public:
        FormatData ()
        { init(); }
        FormatData ( int p,
                     int l  ) : pos (p), len (l)
        { init(); }

        QString fontName;

        int  pos; // Start of text to which this format applies
        int  len; // Len of text to which this format applies
        int  weight;
        int  fontSize;
        int  colourRed;
        int  colourGreen;
        int  colourBlue;
        int  verticalAlignment;

        bool italic;
        bool underline;
    private:
        void init()
        {
            //Initiate member variables
            // (initiate all variables, even those to 0!)
            weight=0;
            fontSize=-1;
            colourRed=0;
            colourGreen=0;
            colourBlue=0;
            verticalAlignment=0;
            italic=false;
            underline=false;
        }
};

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

    formatData->colourRed=red;
    formatData->colourGreen=green;
    formatData->colourBlue=blue;
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


static void ProcessFormatTag (QDomNode myNode, void *tagData, QString &, ClassExportFilterBase* exportFilter)
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
        kdError(30503) << "Unexpected FORMAT attribute id value " << formatId << "!" << endl;
    }

    if ( formatData.pos == -1 || formatData.len == -1 )
    {
        formatData.pos = 0;
        formatData.len = 0;

        kdError(30503) << "Missing formatting!" << endl;
    }

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "ITALIC",    ProcessItalicTag,   (void*) &formatData ) );
    tagProcessingList.append ( TagProcessing ( "UNDERLINE", ProcessUnderlineTag,(void*) &formatData ) );
    tagProcessingList.append ( TagProcessing ( "WEIGHT",    ProcessWeightTag,   (void*) &formatData ) );
    tagProcessingList.append ( TagProcessing ( "SIZE",      ProcessSizeTag,     (void*) &formatData ) );
    tagProcessingList.append ( TagProcessing ( "FONT",      ProcessFontTag,     (void*) &formatData ) );
    tagProcessingList.append ( TagProcessing ( "COLOR",     ProcessColorTag,    (void*) &formatData ) );
    tagProcessingList.append ( TagProcessing ( "VERTALIGN", ProcessVertAlignTag,(void*) &formatData ) );

    QString strDummy;

    ProcessSubtags (myNode, tagProcessingList, strDummy, exportFilter);

    formatDataList->append (formatData);
}


static void ProcessFormatsTag ( QDomNode myNode, void *tagData, QString &outputText, ClassExportFilterBase* exportFilter )
{
    QValueList<FormatData> *formatDataList = (QValueList<FormatData> *) tagData;

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

// ProcessParagraphData () mangles the pure text through the
// formatting information stored in the FormatData list and prints it
// out to the export file.

static void ProcessParagraphData ( QString &paraText, QValueList<FormatData> &paraFormatDataList, QString &outputText, ClassExportFilterBase* exportFilter)
{
    if (! paraText.isEmpty() )
    {

        QValueList<FormatData>::Iterator  paraFormatDataIt;  //Warning: cannot use "->" with it!!

        QString partialText;

        for ( paraFormatDataIt = paraFormatDataList.begin ();
              paraFormatDataIt != paraFormatDataList.end ();
              paraFormatDataIt++ )
        {
            //Retrieve text
            partialText=paraText.mid ( (*paraFormatDataIt).pos, (*paraFormatDataIt).len );
            //Code all possible predefined HTML entities
            exportFilter->escapeText(partialText);

            // TODO: first and last characters of partialText should not be a space (white space problems!)
            // TODO: replace multiples spaces in non-breaking spaces!
            // Opening elements
            if ( (*paraFormatDataIt).weight > 50 )
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

            // The text
            outputText += partialText;

            // Closing elements
            if ( (*paraFormatDataIt).underline )
            {
                outputText+="</u>";
            }
            if ( (*paraFormatDataIt).italic )
            {
                outputText+="</i>";
            }
            if ( (*paraFormatDataIt).weight > 50 )
            {
                outputText+="</b>";
            }
       }

    }
/*
    //We must have two hex digits for each colour channel!
     += QString::number((red&0xf0)>>4,16);
     += QString::number(red&0x0f,16);

     += QString::number((green&0xf0)>>4,16);
     += QString::number(green&0x0f,16);

     += QString::number((blue&0xf0)>>4,16);
     += QString::number(blue&0x0f,16);
*/
}


static void ProcessParagraphTag ( QDomNode myNode, void *, QString   &outputText, ClassExportFilterBase* exportFilter)
{
    AllowNoAttributes (myNode);

    QString paraText;
    QValueList<FormatData> paraFormatDataList;
    QString paraLayout;
    QValueList<TagProcessing> tagProcessingList;

    tagProcessingList.append ( TagProcessing ( "TEXT",    ProcessTextTag,       (void *) &paraText           ) );
    tagProcessingList.append ( TagProcessing ( "FORMATS", ProcessFormatsTag,    (void *) &paraFormatDataList ) );
    tagProcessingList.append ( TagProcessing ( "LAYOUT",  ProcessLayoutTag,     (void *) &paraLayout         ) );
    tagProcessingList.append ( TagProcessing ( "HARDBRK", NULL,                 NULL) ); // Not documented!
    ProcessSubtags (myNode, tagProcessingList, outputText, exportFilter);

    QString strParaText;
    ProcessParagraphData ( paraText, paraFormatDataList, strParaText, exportFilter );
    if (strParaText.isEmpty())
    {
        //An empty paragraph is not allowed in HTML, so add a non-breaking space!
        strParaText="&nbsp;";
    }

    if ( paraLayout == "Head 1" )
    {
        outputText += "<h1>";
        outputText +=strParaText;
        outputText += "</h1>\n";
    }
    else if ( paraLayout == "Head 2" )
    {
        outputText += "<h2>";
        outputText +=strParaText;
        outputText += "</h2>\n";
    }
    else if ( paraLayout == "Head 3" )
    {
        outputText += "<h3>";  //Warning: No trailing white space or else it's in the text!!!
        outputText +=strParaText;
        outputText += "</h3>\n";
    }
    /*
    else if ( paraLayout == "Bullet List" )
    {
        outputText += "<p>"; //TODO
        outputText +=strParaText;
        outputText += "</p>\n";
    }
    else if ( paraLayout == "Enumerated List" )
    {
        outputText += "<p>"; //TODO
        outputText +=strParaText;
        outputText += "</p>\n";
    }
    else if ( paraLayout == "Alphabetical List" )
    {
        outputText += "<p>"; //TODO
        outputText +=strParaText;
        outputText += "</p>\n";
    }
    */
    else
    {// We don't know the layout, so assume it's "Standard". It's better than to abort with an error!
        outputText += "<p>";
        outputText +=strParaText;
        outputText += "</p>\n";
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

const QString strAmp ("&amp;");
const QString strLt  ("&lt;");
const QString strGt  ("&gt;");
//const QString strApos("&apos;");  //Only predefined in XHTML
const QString strQuot("&quot;");

const QRegExp regExpAmp ("&");
const QRegExp regExpLt  ("<");
const QRegExp regExpGt  (">");
//const QRegExp regExpApos("'");    //Only predefined in XHTML
const QRegExp regExpQuot("\"");


QString& ClassExportFilterBase::escapeText(QString& str) const
{// TODO: escape text that cannot be encoded in the current encoding!
    //Code all possible predefined HTML entities
    str.replace (regExpAmp , strAmp)
       .replace (regExpLt  , strLt)
       .replace (regExpGt  , strGt)
       .replace (regExpQuot, strQuot);
    return str;
}

const bool ClassExportFilterBase::filter(const QString  &filenameIn, const QString  &filenameOut,
                                         const QString& , const QString& , const QString& )
{

    KoStore koStoreIn (filenameIn, KoStore::Read);

    if ( !koStoreIn.open ( "root" ) )
    {
        koStoreIn.close ();

        kdError(30503) << "Unable to open input file!" << endl;
        return false;
    }

    QByteArray byteArrayIn = koStoreIn.read ( koStoreIn.size () );
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
              << "\""<< (isXML()?" /":"") << ">" << endl;

    // Put the filename as HTML title // TODO: take the real title from documentinfo.xml (if any!)
    QString strTitle(filenameOut);
    const int result=strTitle.findRev("/");
    if (result>=0)
    {
        strTitle=strTitle.mid(result+1);
    }
    streamOut << "<title>"<< escapeText(strTitle) <<"</title>\n";  // <TITLE> is mandatory in HTML 4.01 !

    //TODO: transform documentinfo.xml into many <META> elements (at least the author!)

    streamOut << "</head>" << endl << "<body>" << endl;

    // Now that we have the header, we can do the real work!
    QString stringBufOut;
    ProcessDocTag (docNodeIn, NULL, stringBufOut, this);
    streamOut << stringBufOut;

    // Add the tail of the file
    streamOut << "</body>" << endl << "</html>" << endl;

    fileOut.close (); //Really close the file
    return true;

}

// ClassExportFilterHtmlTransitional (normal HTML 4.01 Transitional)

class ClassExportFilterHtmlTransitional : public ClassExportFilterBase
{
    public:
        ClassExportFilterHtmlTransitional(void) {}
        virtual ~ClassExportFilterHtmlTransitional(void) {}
    public: //virtual
        virtual QString getDocType(void) const;
        virtual QString getHtmlOpeningTagExtraAttributes(void) const { return QString::null; }
};

QString ClassExportFilterHtmlTransitional::getDocType(void) const
{
    // We are TRANSITIONAL, as we want to use tags like <FONT>, <U> and explicit colours.
    return "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">";
}


// ClassExportFilterHtmlTransitional (normal XHTML 1.0 Transitional)

class ClassExportFilterXHtmlTransitional : public ClassExportFilterBase
{
    public:
        ClassExportFilterXHtmlTransitional(void) {}
        virtual ~ClassExportFilterXHtmlTransitional(void) {}
    public: //virtual
        virtual bool isXML(void) {return true;}
        virtual QString getDocType(void) const;
        virtual QString getHtmlOpeningTagExtraAttributes(void) const;
};

QString ClassExportFilterXHtmlTransitional::getDocType(void) const
{
    // We are TRANSITIONAL, as we want to use tags like <FONT>, <U> and explicit colours.
    // Note "html" is lower-case in XHTML, while "DOCTYPE" and "!PUBLIC" are upper-case!
    return "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"DTD/xhtml1-transitional.dtd\">";
}

QString ClassExportFilterXHtmlTransitional::getHtmlOpeningTagExtraAttributes(void) const
{
    // XHTML must return an extra attribute defining its namespace (in the <html> opening tag)
    return " xmlns=\"http://www.w3.org/1999/xhtml\""; // Leading space is important!

}
// HTMLExport
HTMLExport::HTMLExport(KoFilter *parent, const char *name) :
                     KoFilter(parent, name) {
}

const bool HTMLExport::filter(const QString  &filenameIn,
                               const QString  &filenameOut,
                               const QString  &from,
                               const QString  &to,
                               const QString  &param)
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

    ClassExportFilterBase* exportFilter=NULL;

    if (1)
    { //HTML 4.01 Transitional
        exportFilter=new ClassExportFilterHtmlTransitional;
    }
    else if (0)
    { //XHTML 1.0 Transitional
        exportFilter=new ClassExportFilterXHtmlTransitional;
    }
    //TODO memory failure recovery

    // Do the work!
    const bool result = exportFilter->filter(filenameIn,filenameOut,from,to,param);

    delete exportFilter;

    return result;
}
