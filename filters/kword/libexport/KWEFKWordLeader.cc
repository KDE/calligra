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

#include <koGlobal.h>
#include <koStore.h>

#include "KWEFStructures.h"
#include "TagProcessing.h"
#include "KWEFBaseClass.h"
#include "ProcessDocument.h"
#include "KWEFBaseWorker.h"
#include "KWEFKWordLeader.h"

static void ProcessParagraphTag ( QDomNode      myNode,
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

static void ProcessFramesetTag ( QDomNode      myNode,
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

static void ProcessFramesetsTag ( QDomNode      myNode,
                           void          *,
                           QString       &outputText,
                           KWEFBaseClass *exportFilter )
{
    AllowNoAttributes (myNode);

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "FRAMESET", ProcessFramesetTag, NULL ) );
    ProcessSubtags (myNode, tagProcessingList, outputText, exportFilter);
}

static void ProcessStylesPluralTag (QDomNode myNode, void *, QString &outputText, KWEFBaseClass* exportFilter )
{
    AllowNoAttributes (myNode);

    QValueList<TagProcessing> tagProcessingList;
    //tagProcessingList.append ( TagProcessing ( "STYLE", ProcessStyleTag, NULL ) );
    ProcessSubtags (myNode, tagProcessingList, outputText,exportFilter);
}

static void ProcessPaperTag (QDomNode myNode, void *, QString   &outputText, KWEFBaseClass* exportFilter)
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

    KWEFKWordLeader* leader=(KWEFKWordLeader*) exportFilter;
    leader->doFullPaperFormat(format, width, height, orientation);
}

static void ProcessDocTag ( QDomNode       myNode,
                     void           *,
                     QString        &outputText,
                     KWEFBaseClass  *exportFilter )
{
    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ( "editor",        "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "mime",          "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "syntaxVersion", "", NULL ) );
    ProcessAttributes (myNode, attrProcessingList);
    // TODO: verify syntax version and perhaps mime

    QValueList<TagProcessing> tagProcessingList;

    KWEFKWordLeader* leader=(KWEFKWordLeader*) exportFilter;
    leader->doOpenHead();

    // At first, process <PAPER>, even if mostly the output will need to be delayed.
    tagProcessingList.append ( TagProcessing ( "PAPER",       ProcessPaperTag,     NULL ) );
    ProcessSubtags (myNode, tagProcessingList, outputText,exportFilter);

    tagProcessingList.clear();
    tagProcessingList.append ( TagProcessing ( "STYLES",      ProcessStylesPluralTag, NULL ) );
    ProcessSubtags (myNode, tagProcessingList, outputText,exportFilter);

    leader->doCloseHead();
    leader->doOpenBody();

    tagProcessingList.clear();
    // TODO: do all those tags still exist in KWord 1.2?
    tagProcessingList.append ( TagProcessing ( "PAPER",       NULL,                NULL ) ); // already done
    tagProcessingList.append ( TagProcessing ( "ATTRIBUTES",  NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "FOOTNOTEMGR", NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "STYLES",      NULL,                NULL ) ); // already done
    tagProcessingList.append ( TagProcessing ( "PIXMAPS",     NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "SERIALL",     NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "FRAMESETS",   ProcessFramesetsTag, NULL ) );
    ProcessSubtags (myNode, tagProcessingList, outputText, exportFilter);
    leader->doCloseBody();
}

void KWEFKWordLeader::setWorker(KWEFBaseWorker* newWorker)
{
    m_worker=newWorker;
}

KWEFBaseWorker*  KWEFKWordLeader::getWorker(void) const
{
    return m_worker;
}

// Short simple definition for methods with void parameter
#define DO_VOID_DEFINITION(string) \
    bool KWEFKWordLeader::##string() \
    {\
        if (m_worker) \
            return m_worker->##string(); \
        return false; \
    }

bool KWEFKWordLeader::doOpenFile(const QString& filenameOut, const QString& to)
{
    if (m_worker)
        return m_worker->doOpenFile(filenameOut,to);
    // As it would be the first method to be called, warn if worker is NULL
    kdError() << "No Worker! (in KWEFKWordLeader::doOpenFile)" << endl;
    return false;
}

DO_VOID_DEFINITION(doCloseFile)
DO_VOID_DEFINITION(doAbortFile)
DO_VOID_DEFINITION(doOpenDocument)
DO_VOID_DEFINITION(doCloseDocument)

bool KWEFKWordLeader::doFullParagraph(QString& paraText, LayoutData& layout, ValueListFormatData& paraFormatDataList)
{
    if (m_worker)
        return m_worker->doFullParagraph(paraText, layout, paraFormatDataList);
    return false;
}

DO_VOID_DEFINITION(doOpenTextFrameSet)
DO_VOID_DEFINITION(doCloseTextFrameSet)

bool KWEFKWordLeader::doFullDocumentInfo(QDomDocument& info)
{
    if (m_worker)
        return m_worker->doFullDocumentInfo(info);
    return false;
}

DO_VOID_DEFINITION(doOpenHead)
DO_VOID_DEFINITION(doCloseHead)
DO_VOID_DEFINITION(doOpenBody)
DO_VOID_DEFINITION(doCloseBody)

bool KWEFKWordLeader::doFullPaperFormat(const int format, const double width, const double height, const int orientation)
{
    if (m_worker)
        return m_worker->doFullPaperFormat(format, width, height, orientation);
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
