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
#include "processors.h"

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


//Taken from QT docs
class StructureParser : public QXmlDefaultHandler
{
public:
    StructureParser(QDomElement node)
    {
        structureStack.setAutoDelete(true);
//        nodeStructure=node;
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
        indent = "";
        return TRUE;
    }
    virtual bool startElement( const QString&, const QString&, const QString& name, const QXmlAttributes& attributes);
    virtual bool endElement( const QString&, const QString& , const QString& qName);
    virtual bool characters ( const QString & ch );
private:
    bool startElementSection( const QXmlAttributes& attributes , QDomNode& nodeOut);
    QString indent;
    QStack<StackItem> structureStack;
//    QDomNode nodeStructure;
};

bool StructureParser :: startElementSection(const QXmlAttributes &attributes, QDomNode& nodeOut)
{
    //Search in output <FRAMESETS>
    QDomNodeList framesetsNodeList(nodeOut.toElement().elementsByTagName("FRAMESETS"));
    //Take first item, as they are only one <FRAMESETS> in KWord's documents
    if (!framesetsNodeList.count())
    {
        kdError()<<"AbiWord filter bailing out! No <FRAMESETS> tag found! "<< endl;
        return false;
    }
    kdDebug()<<"framesetsNodeList.count()= " << framesetsNodeList.count() << endl;
    QDomNode framesetsPluralElement(framesetsNodeList.item(0).toElement());
    kdDebug()<<"framesetsPluralElement= " << framesetsPluralElement.nodeName() << endl;
    if (framesetsPluralElement.isNull())
    {
        kdError()<<"AbiWord filter bailing out! Cannot access <FRAMESETS> tag!"<<endl;
        return false;
    }
    //As we have a new AbiWord <section>, we think we have a KWord <FRAMESET>
    QDomElement framesetElementOut=nodeOut.ownerDocument().createElement("FRAMESET");
    framesetElementOut.setAttribute("frameType",1);
    framesetElementOut.setAttribute("frameInfo",0);
    framesetElementOut.setAttribute("autoCreateNewFrame",1);
    framesetElementOut.setAttribute("removable",0);
    //Todo?  attribute "name"
    framesetsPluralElement.appendChild(framesetElementOut);
    
    QDomElement frameElementOut=nodeOut.ownerDocument().createElement("FRAME");
    frameElementOut.setAttribute("left",28);
    frameElementOut.setAttribute("top",42);
    frameElementOut.setAttribute("bottom",566);
    frameElementOut.setAttribute("right",798);
    frameElementOut.setAttribute("runaround",1);
    framesetElementOut.appendChild(frameElementOut);
    
    nodeOut=framesetElementOut;
    return true;
}

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
    indent += "*";
    QDomNode nodeOut=structureStack.current()->stackNode; //TODO: empty stack!
    QDomNode nodeOut2=structureStack.current()->stackNode2; //TODO: empty stack!
    StackItem *stackItem=new(StackItem); //TODO: memory failure recovery
    if ((name=="c")||(name=="C"))
    {
        stackItem->elementType=ElementTypeContent;
        stackItem->stackNode=nodeOut;   // <TEXT>
        stackItem->stackNode2=nodeOut2; // <FORMATS>
        stackItem->pos=structureStack.current()->pos; //Propagate the position upwards
    }
    else if ((name=="p")||(name=="P"))
    {//TODO: put the nodeOut.ownerDocument() together
        QDomElement paragraphElementOut=nodeOut.ownerDocument().createElement("PARAGRAPH");
        nodeOut.appendChild(paragraphElementOut);
        QDomElement textElementOut=nodeOut.ownerDocument().createElement("TEXT");
        paragraphElementOut.appendChild(textElementOut);
        QDomElement formatsPluralElementOut=nodeOut.ownerDocument().createElement("FORMATS");
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
    {
        startElementSection(attributes,nodeOut);
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
    indent.remove( 0, 1 );
    kdDebug() << indent << " </" << name << ">" << endl;
    // TODO: stack empty?
    StackItem *stackItem=structureStack.pop();
    if ((name=="c")||(name=="C"))
    {// TODO: verify consistancy with stack!
        stackItem->stackNode.toElement().normalize();
        structureStack.current()->pos=stackItem->pos; //Propagate the position downwards
    }
    else if ((name=="p")||(name=="P"))
    {// TODO: verify consistancy with stack!
        stackItem->stackNode.toElement().normalize();
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
    StackItem *stackItem=structureStack.current();
    QDomNode nodeOut=stackItem->stackNode; //TODO: empty stack!
    QDomNode nodeOut2=stackItem->stackNode2; //TODO: empty stack!
    if ((stackItem->elementType==ElementTypeContent) || (stackItem->elementType==ElementTypeParagraph))
    { // <c> or <p>
        // TODO: verify consistancy with stack!
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
    strHeader+="<FRAMESETS>\n";
    strHeader+="</FRAMESETS>\n";
    strHeader+="</DOC>\n";
       
    QDomDocument qDomDocumentOut(fileOut);
    qDomDocumentOut.setContent(strHeader);
    
    QFile in(fileIn);

    StructureParser handler(qDomDocumentOut.documentElement());
    
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
