// $Header$

/*
   This file is part of the KDE project
   Copyright (C) 2001 Nicolas GOUTTE <nicog@snafu.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <config.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <abiwordimport.h>
#include <abiwordimport.moc>
#include <kdebug.h>
#include <qxml.h>
#include <qdom.h>
#include <qstack.h>
#include <zlib.h>
#include <ktempfile.h>
#include "processors.h"

// *Note for the reader of this code*
// Tags in lower case (e.g. <c>) are AbiWord's ones.
// Tags in upper case (e.g. <TEXT>) are KWord's ones.

class AbiProps
{
public:
    AbiProps() {};
    AbiProps(QString newName, void* newValue) : name(newName), value(newValue) {};
    virtual ~AbiProps() {};
    QString name;
    void* value;
};

// Treat the "props" attribute of AbiWord's tags and split it in spearates names and values
static void TreatAbiProps(QString strProps,QValueList<AbiProps> &abiPropsList)
{
    if (strProps.isEmpty())
        return;
    
    QString name,value;
    bool notFinished=true;
    int position=0;
    int result;
    while (notFinished)
    {
        //Find next name and its value
        result=strProps.find(':',position);
        if (result==-1)
        {
            name=strProps.mid(position).stripWhiteSpace();
            value="";
            notFinished=false;
        }
        else
        {
            name=strProps.mid(position,result-position).stripWhiteSpace();
            position=result+1;
            result=strProps.find(';',position);
            if (result==-1)
            {
                value=strProps.mid(position).stripWhiteSpace();
                notFinished=false;
            }
            else
            {
                value=strProps.mid(position,result-position).stripWhiteSpace();
                position=result+1;
            }
        }
        kdDebug(30506) << "========== (Property :" << name << "=" << value <<":)"<<endl;
        //Now treat the name and the value that we have just found
        QValueList<AbiProps>::Iterator iterator;
        for (iterator=abiPropsList.begin();iterator!=abiPropsList.end();iterator++)
        {
            if (name==(*iterator).name)
            {
                if((*(iterator)).value)
                {
                    void *pointer=(*(iterator)).value;
                    *((QString*) pointer)=value;
                }
                break;
            }
        }
    }
}

enum StackItemElementType{
    ElementTypeUnknown  = 0,
    ElementTypeBottom   = 1, // Bottom of the stack
    ElementTypeAbiWord  = 2, // <abiword>
    ElementTypeSection  = 3, // <section>
    ElementTypeParagraph= 4, // <p>
    ElementTypeContent  = 5  // <c>
};

class StackItem
{
public:
    StackItem()
    {
        fontName="times"; //Default font
        fontSize=0; //No explicit font size
        italic=false;
        bold=false;
        underline=false;
        red=0;
        green=0;
        blue=0;
        textPosition=0;
    }
    ~StackItem()
    {
    }
public:
    StackItemElementType elementType;
    QDomNode    stackNode,stackNode2;
    QString     fontName;
    int         fontSize;
    int         pos; //Position
    bool        italic;
    bool        bold;
    bool        underline;
    int         red;
    int         green;
    int         blue;
    int         textPosition; //Normal (0), subscript(1), superscript (2)
};


class StructureParser : public QXmlDefaultHandler
{
public:
    StructureParser(QDomElement node)
    {
        structureStack.setAutoDelete(true);
        mainFramesetElement=node;
        StackItem *stackItem=new(StackItem); //TODO: memory failure recovery
        stackItem->elementType=ElementTypeBottom;
        stackItem->stackNode=node;
        structureStack.push(stackItem); //Security item (not to empty the stack)
    }
    virtual ~StructureParser()
    {
        structureStack.clear();
    }
    bool startDocument()
    {
        indent = "";  //DEBUG
        return true;
    }
    virtual bool startElement( const QString&, const QString&, const QString& name, const QXmlAttributes& attributes);
    virtual bool endElement( const QString&, const QString& , const QString& qName);
    virtual bool characters ( const QString & ch );
private:
    QString indent; //DEBUG
    QStack<StackItem> structureStack;
    QDomElement mainFramesetElement;
};

#define WRITE_LAYOUT 1
#ifdef WRITE_LAYOUT
static void FillStandardLayout(QDomElement& layoutElement)
{//TODO: put the layoutElement.ownerDocument() together
    QDomElement element;
    element=layoutElement.ownerDocument().createElement("NAME");
    element.setAttribute("value","Standard");
    layoutElement.appendChild(element);

    element=layoutElement.ownerDocument().createElement("FOLLOWING");
    element.setAttribute("value","Standard");
    layoutElement.appendChild(element);

    QDomElement formatElement=layoutElement.ownerDocument().createElement("FORMAT");
    layoutElement.appendChild(formatElement);

    element=layoutElement.ownerDocument().createElement("FONT");
    element.setAttribute("name","times");
    formatElement.appendChild(element);

    element=layoutElement.ownerDocument().createElement("TABULATOR");
    element.setAttribute("mmpos","64.2055");
    element.setAttribute("ptpos","182");
    element.setAttribute("inchpos","2.52778");
    layoutElement.appendChild(element);

    element=layoutElement.ownerDocument().createElement("TABULATOR");
    element.setAttribute("mmpos","128.764");
    element.setAttribute("ptpos","365");
    element.setAttribute("inchpos","5.06944");
    layoutElement.appendChild(element);
}
#endif

bool StructureParser :: startElement( const QString&, const QString&, const QString& name, const QXmlAttributes& attributes)
{
    //Warning: be careful that the element names can be lower case or upper case (not very XML)
    kdDebug(30506) << indent << " <" << name << ">" << endl; //DEBUG
    indent += "*"; //DEBUG

    if (structureStack.isEmpty())
    {
        kdError(30506) << "Stack is empty!! Aborting! (in StructureParser::startElement)" << endl;
        return false;
    }

    QDomNode nodeOut=structureStack.current()->stackNode;
    QDomNode nodeOut2=structureStack.current()->stackNode2;
    // Create a new stack element copying the top of the stack.
    StackItem *stackItem=new StackItem(*structureStack.current());
    //TODO: memory failure recovery
    if ((name=="c")||(name=="C"))
    {
        // <c> elements can be nested in <p> elements or in other <c> elements
        // AbiWord does not use it but explicitely allows external programs to write AbiWord files with nested <c> elements!
        if ((structureStack.current()->elementType==ElementTypeParagraph)
                ||(structureStack.current()->elementType==ElementTypeContent))
        {
            QValueList<AbiProps> abiPropsList;
            // Initialize the QStrings with the previous values of the properties they represent!
            QString strFontStyle(stackItem->italic?"italic":"");
            QString strWeight(stackItem->bold?"bold":"");
            QString strDecoration(stackItem->underline?"underline":"");
            QString strTextPosition("old");
            QString strColour("old");
            QString strFontSize("old");
            QString strFontFamily(stackItem->fontName);

            abiPropsList.append( AbiProps("font-style",&strFontStyle));
            abiPropsList.append( AbiProps("font-weight",&strWeight));
            abiPropsList.append( AbiProps("text-decoration",&strDecoration));
            abiPropsList.append( AbiProps("text-position",&strTextPosition));
            abiPropsList.append( AbiProps("color",&strColour));
            abiPropsList.append( AbiProps("font-size",&strFontSize));
            abiPropsList.append( AbiProps("font-family",&strFontFamily));
            kdDebug(30506)<< "========== props=\"" << attributes.value("props") << "\"" << endl;
            // Treat the props attributes in the two available flavors: lower case and upper case.
            TreatAbiProps(attributes.value("props"),abiPropsList);
            TreatAbiProps(attributes.value("PROPS"),abiPropsList);
            stackItem->elementType=ElementTypeContent;
            stackItem->stackNode=nodeOut;   // <TEXT>
            stackItem->stackNode2=nodeOut2; // <FORMATS>
            stackItem->pos=structureStack.current()->pos; //Propagate the position

            stackItem->italic=(strFontStyle=="italic");
            stackItem->bold=(strWeight=="bold");
            // underline is the only font-decoration available in KWord
            stackItem->underline=(strDecoration=="underline"); // Underline is the only text decoration that KWord can do!
            if (strTextPosition=="subscript")
            {
                stackItem->textPosition=1;
            }
            else if (strTextPosition=="superscript")
            {
                stackItem->textPosition=2;
            }
            else if (strTextPosition!="old")
            {
                // we have any other new value, assume it means normal!
                stackItem->textPosition=0;
            }
            if (!strColour.isEmpty() && (strColour!="old"))
            {
                // we have a new colour, so decode it!
                long int colour=strColour.toLong(NULL,16);
                stackItem->red  =(colour&0xFF0000)>>16;
                stackItem->green=(colour&0x00FF00)>>8;
                stackItem->blue =(colour&0x0000FF);
            }
            if (!strFontSize.isEmpty() && (strFontSize!="old"))
            {
                // TODO: font-size (e.g. 10pt)
                int size=0;
                int ch; // digit value of the character
                for (int pos=0;;pos++)
                {
                    ch=strFontSize.at(pos).digitValue();
                    if (ch==-1)
                    {
                        // Not a digit
                        break;
                    }
                    else
                    {
                        size*=10;
                        size+=ch;
                    }
                }
                // TODO: verify that the unit of the font size is really "pt"
                if (size>0)
                {
                    stackItem->fontSize=size;
                }
            }
            if (!strFontFamily.isEmpty() && (strFontFamily!="old"))
            {
                // TODO: transform the font-family in a font we have on the system on which KWord runs.
                stackItem->fontName=strFontFamily;
            }
        }
        else
        {//we are not nested correctly, so consider it a parse error!
            kdError(30506) << "Abiword Import: parse error <c> tag not nested in neither a <p>  nor a <c> tag" << endl;
            return false;
        }
    }
    else if ((name=="p")||(name=="P"))
    {
        //We use mainFramesetElement here to be not dependant that <section> has happend before
        QDomElement paragraphElementOut=mainFramesetElement.ownerDocument().createElement("PARAGRAPH");
        mainFramesetElement.appendChild(paragraphElementOut);
        QDomElement textElementOut=mainFramesetElement.ownerDocument().createElement("TEXT");
        paragraphElementOut.appendChild(textElementOut);
        QDomElement formatsPluralElementOut=mainFramesetElement.ownerDocument().createElement("FORMATS");
        paragraphElementOut.appendChild(formatsPluralElementOut);

#ifdef WRITE_LAYOUT
        QDomElement layoutElementOut=nodeOut.ownerDocument().createElement("LAYOUT");
        paragraphElementOut.appendChild(layoutElementOut);
        FillStandardLayout(layoutElementOut);
#endif

        stackItem->elementType=ElementTypeParagraph;
        stackItem->stackNode=textElementOut; // <TEXT>
        stackItem->stackNode2=formatsPluralElementOut; // <FORMATS>
        stackItem->pos=0; // No text characters yet
    }
    else if ((name=="section")||(name=="SECTION"))
    {//Not really needed, as it is the default behaviour for now!
        //TODO: non main text sections (e.g. footers)
        stackItem->elementType=ElementTypeSection;
        stackItem->stackNode=nodeOut;
    }
    else
    {
        stackItem->elementType=ElementTypeUnknown;
        stackItem->stackNode=nodeOut;
    }
    structureStack.push(stackItem);
    return true;
}

bool StructureParser :: endElement( const QString&, const QString& , const QString& name)
{
    indent.remove( 0, 1 ); // DEBUG
    kdDebug(30506) << indent << " </" << name << ">" << endl;

    if (structureStack.isEmpty())
    {
        kdError(30506) << "Stack is empty!! Aborting! (in StructureParser::endElement)" << endl;
        return false;
    }

    StackItem *stackItem=structureStack.pop();
    if ((name=="c")||(name=="C"))
    {
        if (stackItem->elementType==ElementTypeContent)
        {
            stackItem->stackNode.toElement().normalize();
            structureStack.current()->pos=stackItem->pos; //Propagate the position
        }
        else
        {
            kdError(30506) << "Wrong element type!! Aborting! (</c> in StructureParser::endElement)" << endl;
            return false;
        }
    }
    else if ((name=="p")||(name=="P"))
    {
        if (stackItem->elementType==ElementTypeParagraph)
        {
            stackItem->stackNode.toElement().normalize();
        }
        else
        {
            kdError(30506) << "Wrong element type!! Aborting! (</p> in StructureParser::endElement)" << endl;
            return false;
        }
    }
    // Do nothing yet
    delete stackItem;
    return true;
}

bool StructureParser :: characters ( const QString & ch )
{
    // DEBUG start
    if (ch=="\n")
    {
        kdDebug(30506) << indent << " (LINEFEED)" << endl;
    }
    else
    {
        kdDebug(30506) << indent << " :" << ch << ":" << endl;
    }
    // DEBUG end
    if (structureStack.isEmpty())
    {
        kdError(30506) << "Stack is empty!! Aborting! (in StructureParser::characters)" << endl;
        return false;
    }
    StackItem *stackItem=structureStack.current();
    QDomNode nodeOut=stackItem->stackNode;
    QDomNode nodeOut2=stackItem->stackNode2;
    if ((stackItem->elementType==ElementTypeContent) || (stackItem->elementType==ElementTypeParagraph))
    { // <c> or <p>
        nodeOut.appendChild(nodeOut.ownerDocument().createTextNode(ch));

        QDomElement formatElementOut=nodeOut.ownerDocument().createElement("FORMAT");
        formatElementOut.setAttribute("id",1); // Normal text!
        formatElementOut.setAttribute("pos",stackItem->pos); // Start position
        formatElementOut.setAttribute("len",ch.length()); // Start position
        nodeOut2.appendChild(formatElementOut); //Append to <FORMATS>
        stackItem->pos+=ch.length(); // Adapt new starting position

        //Note: the <FONT> tag is mandatory for KWord
        QDomElement fontElementOut=nodeOut.ownerDocument().createElement("FONT");
        fontElementOut.setAttribute("name",stackItem->fontName); // Font name
        formatElementOut.appendChild(fontElementOut); //Append to <FORMAT>

        if (stackItem->fontSize)
        {
            QDomElement fontElementOut=nodeOut.ownerDocument().createElement("SIZE");
            fontElementOut.setAttribute("value",stackItem->fontSize);
            formatElementOut.appendChild(fontElementOut); //Append to <FORMAT>
        }

        if (stackItem->italic)
        {
            QDomElement fontElementOut=nodeOut.ownerDocument().createElement("ITALIC");
            fontElementOut.setAttribute("value",1);
            formatElementOut.appendChild(fontElementOut); //Append to <FORMAT>
        }

        if (stackItem->bold)
        {
            QDomElement fontElementOut=nodeOut.ownerDocument().createElement("WEIGHT");
            fontElementOut.setAttribute("value",75);
            formatElementOut.appendChild(fontElementOut); //Append to <FORMAT>
        }

        if (stackItem->underline)
        {
            QDomElement fontElementOut=nodeOut.ownerDocument().createElement("UNDERLINE");
            fontElementOut.setAttribute("value",1);
            formatElementOut.appendChild(fontElementOut); //Append to <FORMAT>
        }

        if (stackItem->textPosition)
        {
            QDomElement fontElementOut=nodeOut.ownerDocument().createElement("VERTALIGN");
            fontElementOut.setAttribute("value",stackItem->textPosition);
            formatElementOut.appendChild(fontElementOut); //Append to <FORMAT>
        }

        if (stackItem->red || stackItem->green || stackItem->blue)
        {
            QDomElement fontElementOut=nodeOut.ownerDocument().createElement("COLOR");
            fontElementOut.setAttribute("red",stackItem->red);
            fontElementOut.setAttribute("green",stackItem->green);
            fontElementOut.setAttribute("blue",stackItem->blue);
            formatElementOut.appendChild(fontElementOut); //Append to <FORMAT>
        }

    }

    return true;
}


static QDomElement createMainFramesetElement(QDomDocument& qDomDocumentOut)
{
    QDomElement framesetsPluralElementOut=qDomDocumentOut.createElement("FRAMESETS");
    qDomDocumentOut.documentElement().appendChild(framesetsPluralElementOut);

    //As we have a new AbiWord <section>, we think we have a KWord <FRAMESET>
    QDomElement framesetElementOut=qDomDocumentOut.createElement("FRAMESET");
    framesetElementOut.setAttribute("frameType",1);
    framesetElementOut.setAttribute("frameInfo",0);
    framesetElementOut.setAttribute("autoCreateNewFrame",1);
    framesetElementOut.setAttribute("removable",0);
    //Todo?  attribute "name"
    framesetsPluralElementOut.appendChild(framesetElementOut);
    
    QDomElement frameElementOut=qDomDocumentOut.createElement("FRAME");
    frameElementOut.setAttribute("left",28);
    frameElementOut.setAttribute("top",42);
    frameElementOut.setAttribute("bottom",566);
    frameElementOut.setAttribute("right",798);
    frameElementOut.setAttribute("runaround",1);
    framesetElementOut.appendChild(frameElementOut);

    return framesetElementOut; // return the main <FRAMESET> where the body text will be under.
}

ABIWORDImport::ABIWORDImport(KoFilter *parent, const char *name) :
                     KoFilter(parent, name) {
}

const bool ABIWORDImport::filter(const QString &fileIn, const QString &fileOut,
                               const QString& from, const QString& to,
                               const QString &)
{
    if ((to != "application/x-kword") || (from != "application/x-abiword"))
        return false;


    //The warning is serious!! (KWord crashes if it does not find a few things in its XML code)
    kdDebug(30506)<<"WARNING: AbiWord to KWord Import filter can crash KWord!!"<<endl;

    QString fileToParseName(fileIn); //Name of the file to parse
    bool otherFile=false; //Do we have an intermediary file;

    //Test if the file is gzipped

    //At first, find the last extension
    QString strExt;
    const int result=fileIn.findRev('.');
    if (result>=0)
    {
        strExt=fileIn.mid(result);
    }

    kdDebug(30506) << "AbiWord Filter: -" << strExt << "-" << endl;

    if ((strExt==".gz")||(strExt==".GZ")        //in case of .abw.gz (logical extension)
        ||(strExt==".zabw")||(strExt==".ZABW")) //in case of .zabw (extension used prioritary with AbiWord)
    {   //The input file is compressed, so we cannot treat it directly

        const int bufferSize=1024;
        char buffer[bufferSize];

        //Open another temporary file (in the method of KoFilterManager::import)
        KTempFile tempFileIn;
        if (tempFileIn.status())
        { //An error has occured, so abort!
            return false;
        }
        QString tempFileInName=tempFileIn.name();
        gzFile gzfile=gzopen(fileIn.local8Bit(),"rb");
        if (!gzfile)
        {
            kdError(30506) << "Could not open gzipped file! Aborting!" << endl;
            return false;
        }
        for (;;)
        {
            const int result=gzread(gzfile,buffer,bufferSize);
            if (result>0)
            {
                const int result2=tempFileIn.file()->writeBlock(buffer,result);
                if (result==result2)
                {
                    continue;
                }
                else
                {
                    // error!
                    kdError(30506) << "Cannot write temp file! Aborting!" << endl;
                    gzclose(gzfile);
                    return false;
                }
            }
            else if (!result)
            {
                // end of file
                break;
            }
            else
            {
                // error!
                kdError(30506) << "Error reading gzipped file! Aborting!" << endl;
                gzclose(gzfile);
                return false;
            }
        }
        gzclose(gzfile);
        kdDebug(30506)<< "Gzipped file uncompressed!" << endl;

        fileToParseName=tempFileIn.name();
        otherFile=true;
    }

    //Initiate QDomDocument (TODO: is there are better way?)
    QString strHeader("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    strHeader+="<DOC editor=\"KWord\" mime=\"application/x-kword\" syntaxVersion=\"1\" >\n";
    strHeader+="<ATTRIBUTES processing=\"0\" standardpage=\"1\" hasHeader=\"0\" hasFooter=\"0\" unit=\"mm\" />\n";
    strHeader+="<PAPER format=\"1\" ptWidth=\"595\" ptHeight=\"841\" mmWidth =\"210\" mmHeight=\"297\"";
    strHeader+="inchWidth =\"8.26772\" inchHeight=\"11.6929\" orientation=\"0\" columns=\"1\" ptColumnspc=\"2\"";
    strHeader+="mmColumnspc=\"1\" inchColumnspc=\"0.0393701\" hType=\"0\" fType=\"0\"";
    strHeader+="ptHeadBody=\"9\" ptFootBody=\"9\" mmHeadBody=\"3.5\" mmFootBody=\"3.5\"";
    strHeader+="inchHeadBody=\"0.137795\" inchFootBody=\"0.137795\">\n";
    strHeader+="<PAPERBORDERS mmLeft=\"10\" mmTop=\"15\" mmRight=\"10\" mmBottom=\"15\"";
    strHeader+="ptLeft=\"28\" ptTop=\"42\" ptRight=\"28\" ptBottom=\"42\"";
    strHeader+="inchLeft=\"0.393701\" inchTop=\"0.590551\" inchRight=\"0.393701\" inchBottom=\"0.590551\" />\n";
    strHeader+="</PAPER>\n";
    strHeader+="</DOC>\n";

    QDomDocument qDomDocumentOut(fileOut);
    qDomDocumentOut.setContent(strHeader);


    StructureParser handler(createMainFramesetElement(qDomDocumentOut));

    //For now, we arbitrarily decide that Qt can handle the encoding in which the file was written!!
    QXmlSimpleReader reader;
    reader.setContentHandler( &handler );

    // The input file is now uncompressed, so we may handle it directly
    QFile in(fileToParseName);
    QXmlInputSource source(in);
    if (!reader.parse( source ))
    {
        kdError(30506) << "AbiWord Import: Parsing unsuccessful. Aborting!" << endl;
        return false;
    }

    if (otherFile)
    {
        // Unlink the intermediary file (Note: file is not deleted if there was a parsing error!)
        in.remove();
    }


    KoStore out=KoStore(fileOut, KoStore::Write);
    if(!out.open("root"))
    {
        kdError(30506) << "AbiWord Import unable to open output file!" << endl;
        out.close();
        return false;
    }

    //Write the document!
    QCString strOut=qDomDocumentOut.toCString();
    out.write((const char*)strOut, strOut.length());
    out.close();

#if 0
    kdDebug(30506) << qDomDocumentOut.toString();
#endif

    kdDebug(30506) << "Now importing to KWord!" << endl;

    return true;
}
