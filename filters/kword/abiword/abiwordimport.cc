/* $Header$ */

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
   This file is based on the old file:
    koffice/filters/kword/ascii/asciiimport.cc

   The old file was copyrighted by 
    Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
    Copyright (C) 2000 Michael Johnson <mikej@xnet.com>

   The old file was licensed under the terms of the GNU Library General Public
   License version 2.
*/

#include <config.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <abiwordimport.h>
#include <abiwordimport.moc>
#include <kdebug.h>
#include <qdom.h>
#include "processors.h"

static void ProcessParagraphTag (QDomNode nodeIn, void *,  QDomNode & nodeOut)
{   // <p>
    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ( "props", "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "PROPS", "", NULL ) ); //Note: XML is case sensitive!
    ProcessAttributes (nodeIn, attrProcessingList);

#if 0
    QValueList<TagProcessingDom> tagProcessingList;
    tagProcessingList.append ( TagProcessingDom ( "c",             NULL,   NULL ) );
    tagProcessingList.append ( TagProcessingDom ( "i",             NULL,   NULL ) );
    // <image> is the same as <i> but obsolete
    tagProcessingList.append ( TagProcessingDom ( "image",         NULL,   NULL ) ); 
    ProcessSubtags (nodeIn, tagProcessingList, nodeOut);
#endif
    
    QDomElement paragraphElementIn=nodeIn.toElement();
    QString strResult=paragraphElementIn.text(); //Retrieve text inside <p> </p> and all sub tags.
    //Todo: retrieve all the informations correctly (white space problem!!)
    QDomElement paragraphElementOut=nodeOut.ownerDocument().createElement("PARAGRAPH");
    nodeOut.appendChild(paragraphElementOut);
    QDomElement textElementOut=nodeOut.ownerDocument().createElement("TEXT");
    paragraphElementOut.appendChild(textElementOut);
    textElementOut.appendChild(nodeOut.ownerDocument().createTextNode(strResult));
}

static void ProcessSectionTag (QDomNode nodeIn, void *,  QDomNode & nodeOut)
{   //<section>
    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ( "props", "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "PROPS", "", NULL ) ); //Note: XML is case sensitive!
    ProcessAttributes (nodeIn, attrProcessingList);

    //Search in output <FRAMESETS>
    QDomNodeList framesetsNodeList(nodeOut.toElement().elementsByTagName("FRAMESETS"));
    //Take first item, as they are only one <FRAMESETS> in KWord's documents
    if (!framesetsNodeList.count())
    {
        kdError()<<"AbiWord filter bailing out! No <FRAMESETS> tag found! "<< endl;
        return;
    }
    kdDebug()<<"framesetsNodeList.count()= " << framesetsNodeList.count() << endl;
    QDomNode framesetsPluralElement(framesetsNodeList.item(0).toElement());
    kdDebug()<<"framesetsPluralElement= " << framesetsPluralElement.nodeName() << endl;
    if (framesetsPluralElement.isNull())
    {
        kdError()<<"AbiWord filter bailing out! Cannot access <FRAMESETS> tag!"<<endl;
        return;
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

    QValueList<TagProcessingDom> tagProcessingList;
    tagProcessingList.append ( TagProcessingDom ( "p",             ProcessParagraphTag,   NULL ) );
    ProcessSubtagsDom (nodeIn, tagProcessingList, framesetElementOut);
}

static void ProcessAbiWordTag (QDomNode nodeIn, void *,  QDomNode & nodeOut)
{   // <abiword>
    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ( "version", "", NULL ) );
    ProcessAttributes (nodeIn, attrProcessingList);

    QValueList<TagProcessingDom> tagProcessingList;
    tagProcessingList.append ( TagProcessingDom ( "section",       ProcessSectionTag,   NULL ) );
    tagProcessingList.append ( TagProcessingDom ( "style",         NULL,                NULL ) );
    tagProcessingList.append ( TagProcessingDom ( "data",          NULL,                NULL ) );
    ProcessSubtagsDom (nodeIn, tagProcessingList, nodeOut);
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

    // Note: we implement the import similary to the export.
    // We have something like an XML-file, haven't we?
    
    QFile in(fileIn);
    if(!in.open(IO_ReadOnly))
    {
        kdError() << "Unable to open input file!" << endl;
        in.close();
        return false;
    }

    //The warning is serious!! (KWord crashes if it does not find a few things in its XML code)
    kdDebug()<<"AbiWord to KWord Import filter: WARNING: the following code can crash KWord!!"<<endl;
    
    //ToDo: verify if the encoding of the file is really UTF-8
    //For now, we arbitrarily decide it is or that Qt can handle it!!
    
#if 1
    QDomDocument qDomDocumentIn; //For parsing the XML
    
    qDomDocumentIn.setContent(&in); //Feed it with the file content!
    in.close(); //Hopefully there is no errors!
#else
    QByteArray byteArrayIn;
    in.ReadBlock((char*) byteArrayIn, in.size());
    in.close(); //Hopefully there is no errors!
    
    //ToDo: verify if the encoding of the file is really UTF-8
    //For now, we arbitrarily decide it is!!
    
    QDomDocument qDomDocumentIn; //For parsing the XML
    
    qDomDocumentIn.setContent(byteArrayIn); //Feed it with the file content!
#endif       
   
    kdError() << "Filter for AbiWord import is not ready yet" << endl; //Todo: remove it!

    QDomDocument qDomDocumentOut("abiword");

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
       
    qDomDocumentOut.setContent(strHeader);
    
    QDomElement elementOut=qDomDocumentOut.documentElement();
    kdDebug()<<elementOut.tagName();

    ProcessAbiWordTag(qDomDocumentIn.documentElement(),NULL,elementOut);

    kdDebug()<< qDomDocumentOut.toCString() << endl << "Now importing to KWord!" << endl;
        
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
