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
#include <klocale.h>
#include <kmessagebox.h>
#include "processors.h"

// *Note for the reader of this code*
// Tags in lower case (e.g. <c>) are AbiWord's ones.
// Tags in upper case (e.g. <TEXT>) are KWord's ones.

class AbiProps
{
public:
    AbiProps() {};
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
        kdDebug() << "========== (Property :" << name << "=" << value <<":)"<<endl;
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
        kdDebug()<< "> Creating StackItem " << (void*) this <<endl;
        propertyFontName="times"; //Default font
    }
    ~StackItem()
    {
        kdDebug()<< "< Destructing StackItem " << (void*) this <<endl;
    }
public:
    StackItemElementType    elementType;
    QDomNode                stackNode,stackNode2;
    QString                 propertyFontName;
    int                     pos; //Position
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
    kdDebug() << indent << " <" << name << ">" << endl;
    indent += "*"; //DEBUG
    
    if (structureStack.isEmpty())
    {
        kdError() << "Stack is empty!! Aborting! (in StructureParser::startElement)" << endl;
        return false;
    }

    QDomNode nodeOut=structureStack.current()->stackNode;
    QDomNode nodeOut2=structureStack.current()->stackNode2;
    StackItem *stackItem=new(StackItem); //TODO: memory failure recovery
    if ((name=="c")||(name=="C"))
    {
        // <c> tags can be nested in <p> tags or in other <c> tags
        if ((structureStack.current()->elementType==ElementTypeParagraph)
                ||(structureStack.current()->elementType==ElementTypeContent))
        {
            QValueList<AbiProps> abiPropsList;
            kdDebug()<< "========== props=\"" << attributes.value("props") << "\"" << endl;
            TreatAbiProps(attributes.value("props"),abiPropsList); //TODO: PROPS (upper case)
            stackItem->elementType=ElementTypeContent;
            stackItem->stackNode=nodeOut;   // <TEXT>
            stackItem->stackNode2=nodeOut2; // <FORMATS>
            stackItem->pos=structureStack.current()->pos; //Propagate the position
        }
        else
        {//we are not nested correctly, so consider it a parse error!
            kdError() << "Abiword Import: parse error <c> tag not nested in neither a <p>  nor a <c> tag" << endl;
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
    kdDebug() << indent << " </" << name << ">" << endl;

    if (structureStack.isEmpty())
    {
        kdError() << "Stack is empty!! Aborting! (in StructureParser::endElement)" << endl;
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
            kdError() << "Wrong element type!! Aborting! (</c> in StructureParser::endElement)" << endl;
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
            kdError() << "Wrong element type!! Aborting! (</p> in StructureParser::endElement)" << endl;
            return false;
        }
    }
    // Do nothing yet
    delete stackItem;        
    return true;
}

bool StructureParser :: characters ( const QString & ch )
{
    if (ch=="\n")
    {
        kdDebug() << indent << " (LINEFEED)" << endl;
    }
    else
    {
        kdDebug() << indent << " :" << ch << ":" << endl;
    }
    if (structureStack.isEmpty())
    {
        kdError() << "Stack is empty!! Aborting! (in StructureParser::characters)" << endl;
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
        
        QDomElement fontElementOut=nodeOut.ownerDocument().createElement("FONT");
        fontElementOut.setAttribute("name",stackItem->propertyFontName); // Font name
        formatElementOut.appendChild(fontElementOut); //Append to <FORMAT>
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
    kdDebug()<<"WARNING: AbiWord to KWord Import filter can crash KWord!!"<<endl;

    // Make the file header
    
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
    
    QFile in(fileIn);

    StructureParser handler(createMainFramesetElement(qDomDocumentOut));
    
    //TODO: verify if the encoding of the file is really UTF-8
    //For now, we arbitrarily decide that Qt can handle it!!
    
    QXmlInputSource source(in);
    QXmlSimpleReader reader;
    reader.setContentHandler( &handler );

    if (!reader.parse( source ))
    {
        kdError() << "AbiWord Import: Parsing unsuccessful. Aborting!";
        return false;
    }
   
    kdDebug()<< qDomDocumentOut.toCString() << endl << "Now importing to KWord!" << endl;

    KoStore out=KoStore(QString(fileOut), KoStore::Write);
    if(!out.open("root"))
    {
        kdError() << "AbiWord Import unable to open output file!" << endl;
        out.close();
        return false;
    }

    //Write the document!
    QCString strOut=qDomDocumentOut.toCString();
    out.write((const char*)strOut, strOut.length());
    out.close();
    
    return true;
}
