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
    StackItemElementType elementType;
    QDomNode stackNode;
};


//Taken from QT docs
class StructureParser : public QXmlDefaultHandler
{
public:
    StructureParser(QDomElement node)
    {
        structureStack.setAutoDelete(true);
        nodeStructure=node;
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
    QDomNode nodeStructure;
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

bool StructureParser :: startElement( const QString&, const QString&, const QString& name, const QXmlAttributes& attributes)
{
    kdDebug() << indent << " <" << name << ">" << endl;
    indent += "*";
    QDomNode nodeOut=structureStack.current()->stackNode; //TODO: empty stack!
    StackItem *stackItem=new(StackItem); //TODO: memory failure recovery
    if (name=="c")
    {
        stackItem->elementType=ElementTypeContent;
        stackItem->stackNode=nodeOut;
    }
    else if (name=="p")
    {
        QDomElement paragraphElementOut=nodeOut.ownerDocument().createElement("PARAGRAPH");
        nodeOut.appendChild(paragraphElementOut);
        QDomElement textElementOut=nodeOut.ownerDocument().createElement("TEXT");
        paragraphElementOut.appendChild(textElementOut);
        stackItem->elementType=ElementTypeParagraph;
        stackItem->stackNode=textElementOut;
    }
    else if (name=="section")
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
    if (name=="c")
    {// TODO: verify consistancy with stack!
        stackItem->stackNode.toElement().normalize();
    }
    else if (name=="p")
    {// TODO: verify consistancy with stack!
        stackItem->stackNode.toElement().normalize();
    }
    // Do nothing yet
    delete stackItem;        
    return true;
}

bool StructureParser :: characters ( const QString & ch )
{
    kdDebug() << indent << " :" << ch << ":" << endl;
    StackItem *stackItem=structureStack.current();
    QDomNode nodeOut=stackItem->stackNode; //TODO: empty stack!
    if (stackItem->elementType==ElementTypeContent)
    {// TODO: verify consistancy with stack!
        nodeOut.appendChild(nodeOut.ownerDocument().createTextNode(ch));
    }
    else if (stackItem->elementType==ElementTypeParagraph)
    {// TODO: verify consistancy with stack!
        nodeOut.appendChild(nodeOut.ownerDocument().createTextNode(ch));
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
       
    
    QFile in(fileIn);
    //ToDo: verify if the encoding of the file is really UTF-8
    //For now, we arbitrarily decide that Qt can handle it!!
    QDomDocument qDomDocumentOut(fileOut);
    qDomDocumentOut.setContent(strHeader);
    
    StructureParser handler(qDomDocumentOut.documentElement());
    QXmlInputSource source(in);
    QXmlSimpleReader reader;
    reader.setContentHandler( &handler );
    reader.parse( source );
   
    kdError() << "Filter for AbiWord import is not ready yet" << endl; //Todo: remove it!

    kdDebug()<< qDomDocumentOut.toCString() << endl << "Now importing to KWord!" << endl;

    //Todo: verify that the output document is valid for KWord, as KWord hates <FRAMESETS/>
            
    KoStore out=KoStore(QString(fileOut), KoStore::Write);
    if(!out.open("root"))
    {
        kdError() << "Unable to open output file!" << endl;
        out.close();
        return false;
    }

    //Write the document!
    QCString strOut=qDomDocumentOut.toCString();
    out.write((const char*)strOut, strOut.length());
    out.close();
    
    return true;
}
