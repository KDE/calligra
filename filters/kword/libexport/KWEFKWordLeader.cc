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
   Part of this code comes from the old file:
    /home/kde/koffice/filters/kword/ascii/asciiexport.cc

   The old file was copyrighted by
    Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
    Copyright (c) 2000 ID-PRO Deutschland GmbH. All rights reserved.
                       Contact: Wolf-Michael Bolle <Bolle@ID-PRO.de>

   The old file was licensed under the terms of the GNU Library General Public
   License version 2.
*/

#include <qdom.h>

#include <kdebug.h>

#include <koStore.h>

#include "KWEFStructures.h"
#include "TagProcessing.h"
#include "KWEFBaseClass.h"
#include "ProcessDocument.h"
#include "KWEFBaseWorker.h"
#include "KWEFKWordLeader.h"

void ProcessParagraphTag ( QDomNode      myNode,
                           void          *,
                           QString       &outputText,
                           KWEFBaseClass *exportFilter )
{
    AllowNoAttributes (myNode);

    QString paraText;
    ValueListFormatData paraFormatDataList;
    LayoutData layout;

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "TEXT",    ProcessTextTag,    (void *) &paraText           ) );
    tagProcessingList.append ( TagProcessing ( "FORMATS", ProcessFormatsTag, (void *) &paraFormatDataList ) );
    tagProcessingList.append ( TagProcessing ( "LAYOUT",  ProcessLayoutTag,  (void *) &layout         ) );
    ProcessSubtags (myNode, tagProcessingList, outputText, exportFilter);

    KWEFKWordLeader* leader=(KWEFKWordLeader*) exportFilter;

    CreateMissingFormatData(paraText, paraFormatDataList);
    leader->doFullParagraph(paraText, layout, paraFormatDataList);

}

void ProcessFramesetTag ( QDomNode      myNode,
                          void          *,
                          QString       &outputText,
                          KWEFBaseClass *exportFilter )
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
        KWEFKWordLeader* leader=(KWEFKWordLeader*) exportFilter;
        leader->doOpenTextFrameSet();
        QValueList<TagProcessing> tagProcessingList;
        tagProcessingList.append ( TagProcessing ( "FRAME",     NULL,                NULL ) );
        tagProcessingList.append ( TagProcessing ( "PARAGRAPH", ProcessParagraphTag, NULL ) );
        ProcessSubtags (myNode, tagProcessingList, outputText, exportFilter);
        leader->doCloseTextFrameSet();
    }
    //TODO: Treat the other types of frames (frameType)
}

void ProcessFramesetsTag ( QDomNode      myNode,
                           void          *,
                           QString       &outputText,
                           KWEFBaseClass *exportFilter )
{
    AllowNoAttributes (myNode);

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "FRAMESET", ProcessFramesetTag, NULL ) );
    ProcessSubtags (myNode, tagProcessingList, outputText, exportFilter);
}

void ProcessDocTag ( QDomNode       myNode,
                     void           *,
                     QString        &outputText,
                     KWEFBaseClass  *exportFilter )
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

void KWEFKWordLeader::setWorker(KWEFBaseWorker* newWorker)
{
    m_worker=newWorker;
}

KWEFBaseWorker*  KWEFKWordLeader::getWorker(void) const
{
    return m_worker;
}


bool KWEFKWordLeader::doOpenFile(const QString& filenameOut, const QString& to)
{
    if (m_worker)
        return m_worker->doOpenFile(filenameOut,to);
    // As it would be the first method to be called, warn if worker is NULL
    kdError() << "No Worker! (in KWEFKWordLeader::doOpenFile)" << endl;
    return false;
}

bool KWEFKWordLeader::doCloseFile(void)
{
    if (m_worker)
        return m_worker->doCloseFile();
    return false;
}

bool KWEFKWordLeader::doAbortFile(void)
{
    if (m_worker)
        return m_worker->doAbortFile();
    return false;
}

bool KWEFKWordLeader::doOpenDocument(void)
{
    if (m_worker)
        return m_worker->doOpenDocument();
    return false;
}

bool KWEFKWordLeader::doCloseDocument(void)
{
    if (m_worker)
        return m_worker->doCloseDocument();
    return false;
}

bool KWEFKWordLeader::doFullParagraph(QString& paraText, LayoutData& layout, ValueListFormatData& paraFormatDataList)
{
    if (m_worker)
        return m_worker->doFullParagraph(paraText, layout, paraFormatDataList);
    return false;
}

bool KWEFKWordLeader::doOpenTextFrameSet(void)
{
    if (m_worker)
        return m_worker->doOpenTextFrameSet();
    return false;
}

bool KWEFKWordLeader::doCloseTextFrameSet(void)
{
    if (m_worker)
        return m_worker->doCloseTextFrameSet();
    return false;
}

bool KWEFKWordLeader::doFullDocumentInfo(QDomDocument& info)
{
    if (m_worker)
        return m_worker->doFullDocumentInfo(info);
    return false;
}

bool KWEFKWordLeader::filter(const QString& filenameIn, const QString& filenameOut,
    const QString& from, const QString& to, const QString&)
{
    if ( from != "application/x-kword" )
    {
        return false;
    }

    KoStore koStoreIn (filenameIn, KoStore::Read);

    if (!doOpenFile(filenameOut,to))
    {
        kdError() << "Worker could not open export file! Aborting!" << endl;
        return false;
    }

    if ( koStoreIn.open ( "documentinfo.xml" ) )
    {
        QByteArray infoArrayIn = koStoreIn.read ( koStoreIn.size () );
        koStoreIn.close ();

        QDomDocument qDomDocumentInfo;
        qDomDocumentInfo.setContent (infoArrayIn);

        doFullDocumentInfo(qDomDocumentInfo);
    }
    else
    {
        // Note: we do not worry too much if we cannot open the document info!
        kdWarning() << "Unable to open documentinfo.xml sub-file!" << endl;
    }

    if ( !koStoreIn.open ( "root" ) )
    {
        // TODO: read a untarred KWord file (useful with koconverter)
        kdError() << "Unable to open input file!" << endl;
        doAbortFile();
        return false;
    }

    QByteArray byteArrayIn = koStoreIn.read ( koStoreIn.size () );
    koStoreIn.close ();

    QDomDocument qDomDocumentIn;

    if (!qDomDocumentIn.setContent (byteArrayIn))
    {
        kdError() << "Parsing error! Aborting!" << endl;
        doAbortFile();
        return false;
    }

    if (!doOpenDocument())
    {
        kdError() << "Worker could not open document! Aborting!" << endl;
        doAbortFile();
        return false;
    }

    QDomNode docNode = qDomDocumentIn.documentElement ();

    QString stringBufOut; // Dummy!

    ProcessDocTag (docNode, NULL, stringBufOut, this);

    if (!doCloseDocument())
    {
        kdError() << "Worker could not close document! Aborting!" << endl;
        doAbortFile();
        return false;
    }

    if (!doCloseFile())
    {
        kdError() << "Worker could not close export file! Aborting!" << endl;
        return false;
    }

    return true;
}
