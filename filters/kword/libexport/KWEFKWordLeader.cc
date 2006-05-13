/*
   This file is part of the KDE project
   Copyright (C) 2001, 2002, 2004 Nicolas GOUTTE <goutte@kde.org>
   Copyright (c) 2001 IABG mbH. All rights reserved.
                      Contact: Wolf-Michael Bolle <Bolle@IABG.de>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

/*
   Part of this code comes from the old file:
    /home/kde/koffice/filters/kword/ascii/asciiexport.cc

   The old file was copyrighted by
    Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
    Copyright (c) 2000 ID-PRO Deutschland GmbH. All rights reserved.
                       Contact: Wolf-Michael Bolle <Wolf-Michael.Bolle@GMX.de>

   The old file was licensed under the terms of the GNU Library General Public
   License version 2.
*/

#include <QFile>
#include <qdom.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <QByteArray>

#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>

#include <KoGlobal.h>
#include <KoStore.h>

#include "KWEFStructures.h"
#include "TagProcessing.h"
#include "ProcessDocument.h"
#include "KWEFBaseWorker.h"
#include "KWEFKWordLeader.h"

KWEFKWordLeader::KWEFKWordLeader(void)
    : m_syntaxVersion(-1), m_oldSyntax(false), m_worker(NULL), m_chain(NULL), m_hType(0), m_fType(0)
{
}

KWEFKWordLeader::KWEFKWordLeader(KWEFBaseWorker* newWorker)
    : m_syntaxVersion(-1), m_oldSyntax(false), m_worker(newWorker), m_chain(NULL)
{
    if (newWorker) newWorker->registerKWordLeader(this);
}

KWEFKWordLeader::~KWEFKWordLeader(void)
{
}

static FrameAnchor *findAnchor ( const KoPictureKey& key,
                                 Q3ValueList<ParaData>& paraList )
{
    kDebug(30508) << "findAnchor " << key.toString() << endl;
    Q3ValueList<ParaData>::Iterator paraIt;

    for ( paraIt = paraList.begin (); paraIt != paraList.end (); ++paraIt )
    {
        ValueListFormatData::Iterator formattingIt;

        for ( formattingIt = (*paraIt).formattingList.begin ();
              formattingIt != (*paraIt).formattingList.end ();
              formattingIt++ )
        {
            if ( (*formattingIt).id              == 6    &&
                 (*formattingIt).frameAnchor.key == key )
            {
                kDebug(30508) << "Found anchor " << (*formattingIt).frameAnchor.key.toString() << endl;
                return &(*formattingIt).frameAnchor;
            }
        }
    }

    kWarning(30508) << "findAnchor returning NULL!" << endl;
    return NULL;
}

static void ProcessHardBrkTag ( QDomNode myNode, void* tagData, KWEFKWordLeader* )
{
    // <HARDBRK>
    bool* flag = (bool*) tagData;
    Q3ValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ( "frame", *flag );
    ProcessAttributes (myNode, attrProcessingList);
    if (*flag)
        kDebug(30508) << "<HARDBRK frame=\"1\"> found" << endl;
}

static void InsertBookmarkFormatData (const int pos, const QString& name, const bool start,
    ValueListFormatData &paraFormatDataList)
{
    ValueListFormatData::Iterator  paraFormatDataIt;

    FormatData book( start ? 1001 : 1002 , pos, 0 );
    book.variable.m_text = name;

    for (paraFormatDataIt = paraFormatDataList.begin ();
        paraFormatDataIt != paraFormatDataList.end ();
        paraFormatDataIt++)
    {
        if ( pos <= (*paraFormatDataIt).pos )
        {
            paraFormatDataList.insert ( paraFormatDataIt, book );
            return;

        }
        if ( ( pos > (*paraFormatDataIt).pos ) && ( pos < (*paraFormatDataIt).pos + (*paraFormatDataIt).len ) )
        {
            // Somewhere in the middle, we have to split the FormatData
            FormatData split ( *paraFormatDataIt );
            //const int oldlen = (*paraFormatDataIt).len;
            split.len = pos - (*paraFormatDataIt).pos;
            (*paraFormatDataIt).len -= split.len;
            (*paraFormatDataIt).pos = pos;
            paraFormatDataList.insert ( paraFormatDataIt, split );
            paraFormatDataList.insert ( paraFormatDataIt, book );
            return;
        }
    }

    // Still here? So we need to put the bookmark here:
    paraFormatDataList.append ( book );
}


void KWEFKWordLeader::createBookmarkFormatData( ParaData& paraData )
{
    const int paraCount = m_paraCountMap[ m_currentFramesetName ];

    Q3ValueList<Bookmark>::ConstIterator it;
    for (it = m_bookmarkList.begin(); it != m_bookmarkList.end(); ++it )
    {
        if ( (*(it)).m_frameset != m_currentFramesetName )
        {
            continue;
        }
        // As we always insert before, make first endings, then startings (problem is zero-length bookmark)
        if ( (*(it)).m_endparag == paraCount )
        {
            kDebug(30508) << "Paragraph: " << paraCount << " end: " << (*(it)).m_name << endl;
            InsertBookmarkFormatData( (*(it)).m_cursorIndexEnd, (*(it)).m_name, false, paraData.formattingList);

        }
        if ( (*(it)).m_startparag == paraCount )
        {
            kDebug(30508) << "Paragraph: " << paraCount << " begin: " << (*(it)).m_name << endl;
            InsertBookmarkFormatData( (*(it)).m_cursorIndexStart, (*(it)).m_name, true, paraData.formattingList);
        }
    }
}

static void ProcessParagraphTag ( QDomNode         myNode,
                                  void            *tagData,
                                  KWEFKWordLeader *leader )
{
#if 0
    kDebug (30508) << "ProcessParagraphTag () - Begin" << endl;
#endif

    Q3ValueList<ParaData> *paraList = (Q3ValueList<ParaData> *) tagData;

    AllowNoAttributes (myNode);

    // We need to adjust the paragraph number (0 if first)
    QMap<QString,int>::Iterator it = leader->m_paraCountMap.find( leader->m_currentFramesetName );
    if ( it == leader->m_paraCountMap.end() )
        leader->m_paraCountMap.insert( leader->m_currentFramesetName, 0 );
    else
        ++(*it);
    
    ParaData paraData;

    Q3ValueList<TagProcessing> tagProcessingList;
    tagProcessingList << TagProcessing ( "TEXT",    ProcessTextTag,     &paraData.text           )
                      << TagProcessing ( "FORMATS", ProcessFormatsTag,  &paraData.formattingList )
                      << TagProcessing ( "LAYOUT",  ProcessLayoutTag,   &paraData.layout         );
    
    if ( leader->m_oldSyntax )
    {
        tagProcessingList.append( TagProcessing( "HARDBRK", ProcessHardBrkTag, &paraData.layout.pageBreakBefore ) );
    }
    ProcessSubtags (myNode, tagProcessingList, leader);

    leader->createBookmarkFormatData( paraData );
    CreateMissingFormatData (paraData.text, paraData.formattingList);

    // TODO/FIXME: why !paraData.text.isEmpty()
    if ( paraData.formattingList.isEmpty () && !paraData.text.isEmpty () )
    {
        if ( paraData.layout.formatData.id == 1 )
        {
            paraData.formattingList << paraData.layout.formatData;
        }
        else
        {
            kWarning (30508) << "No useful FORMAT tag found for text in PARAGRAPH" << endl;
        }
    }


    *paraList << paraData;

#if 0
    kDebug (30508) << "ProcessParagraphTag () - End " << paraText << endl;
#endif
}


static void ProcessFrameTag ( QDomNode myNode, void *tagData,
    KWEFKWordLeader *leader )
{
    FrameAnchor* frameAnchor= (FrameAnchor*) tagData;

    int lRed=0, lBlue=0, lGreen=0;
    int rRed=0, rBlue=0, rGreen=0;
    int tRed=0, tBlue=0, tGreen=0;
    int bRed=0, bBlue=0, bGreen=0;
    int bkRed=255, bkBlue=255, bkGreen=255;

    frameAnchor->frame.lWidth=0.0;
    frameAnchor->frame.rWidth=0.0;
    frameAnchor->frame.tWidth=0.0;
    frameAnchor->frame.bWidth=0.0;

    Q3ValueList<AttrProcessing> attrProcessingList;
    attrProcessingList
        << AttrProcessing ( "right",        frameAnchor->frame.right  )
        << AttrProcessing ( "left",         frameAnchor->frame.left   )
        << AttrProcessing ( "top",          frameAnchor->frame.top    )
        << AttrProcessing ( "bottom",       frameAnchor->frame.bottom )

        << AttrProcessing ( "min-height",   frameAnchor->frame.minHeight )

        << AttrProcessing ( "runaround",      frameAnchor->frame.runaround         )
        << AttrProcessing ( "runaroundSide",  frameAnchor->frame.runaroundSide )
        << AttrProcessing ( "runaroundGap",   frameAnchor->frame.runaroundGap   )

        << AttrProcessing ( "autoCreateNewFrame",  frameAnchor->frame.autoCreateNewFrame )
        << AttrProcessing ( "newFrameBehavior",    frameAnchor->frame.newFrameBehavior   )
        << AttrProcessing ( "newFrameBehaviour", frameAnchor->frame.newFrameBehavior ) // Depreciated name

        << AttrProcessing ( "copy",       frameAnchor->frame.copy      )
        << AttrProcessing ( "sheetSide",  frameAnchor->frame.sheetSide )

        << AttrProcessing ( "lWidth",   frameAnchor->frame.lWidth )
        << AttrProcessing ( "rWidth",   frameAnchor->frame.rWidth )
        << AttrProcessing ( "tWidth",   frameAnchor->frame.tWidth )
        << AttrProcessing ( "bWidth",   frameAnchor->frame.bWidth )

        << AttrProcessing ( "lRed",     lRed   )
        << AttrProcessing ( "lGreen",   lGreen )
        << AttrProcessing ( "lBlue",    lBlue  )

        << AttrProcessing ( "rRed",     rRed   )
        << AttrProcessing ( "rGreen",   rGreen )
        << AttrProcessing ( "rBlue",    rBlue  )

        << AttrProcessing ( "tRed",     tRed   )
        << AttrProcessing ( "tGreen",   tGreen )
        << AttrProcessing ( "tBlue",    tBlue  )

        << AttrProcessing ( "bRed",     bRed   )
        << AttrProcessing ( "bGreen",   bGreen )
        << AttrProcessing ( "bBlue",    bBlue  )

        << AttrProcessing ( "lStyle",    frameAnchor->frame.lStyle )
        << AttrProcessing ( "rStyle",    frameAnchor->frame.rStyle )
        << AttrProcessing ( "tStyle",    frameAnchor->frame.tStyle )
        << AttrProcessing ( "bStyle",    frameAnchor->frame.bStyle )

        << AttrProcessing ( "bkRed",     bkRed   )
        << AttrProcessing ( "bkGreen",   bkGreen )
        << AttrProcessing ( "bkBlue",    bkBlue  )

        << AttrProcessing ( "bkStyle",    frameAnchor->frame.bkStyle )

        << AttrProcessing ( "bleftpt",     frameAnchor->frame.bleftpt   )
        << AttrProcessing ( "brightpt",    frameAnchor->frame.brightpt  )
        << AttrProcessing ( "btoppt",      frameAnchor->frame.btoppt    )
        << AttrProcessing ( "bbottompt",   frameAnchor->frame.bbottompt )
        ;

    if ( leader->m_oldSyntax )
    {
        attrProcessingList
            << AttrProcessing ( "bleftmm" )
            << AttrProcessing ( "bleftinch" )
            << AttrProcessing ( "brightmm" )
            << AttrProcessing ( "brightinch" )
            << AttrProcessing ( "btopmm" )
            << AttrProcessing ( "btopinch" )
            << AttrProcessing ( "bbottommm" )
            << AttrProcessing ( "bbottominch" )
            ;
    }
    
    ProcessAttributes (myNode, attrProcessingList);

    frameAnchor->frame.lColor.setRgb( lRed, lGreen, lBlue );
    frameAnchor->frame.rColor.setRgb( rRed, rGreen, rBlue );
    frameAnchor->frame.tColor.setRgb( tRed, tGreen, tBlue );
    frameAnchor->frame.bColor.setRgb( bRed, bGreen, bBlue );
    frameAnchor->frame.bkColor.setRgb( bkRed, bkGreen, bkBlue );

    AllowNoSubtags (myNode, leader);
}

static void ProcessPictureAnchor( QDomNode myNode, KWEFKWordLeader *leader, FrameAnchor* frameAnchor, const int frameType )
{
    frameAnchor->type = frameType;
 
    Q3ValueList<TagProcessing> tagProcessingList;
    tagProcessingList
        << TagProcessing ( "FRAME",   ProcessFrameTag, frameAnchor )
        << TagProcessing ( "PICTURE", ProcessImageTag, &frameAnchor->picture.key )
        << TagProcessing ( "IMAGE",   ProcessImageTag, &frameAnchor->picture.key )
        << TagProcessing ( "CLIPART", ProcessImageTag, &frameAnchor->picture.key )
        ;
    ProcessSubtags (myNode, tagProcessingList, leader);

    kDebug (30508) << "FRAMESET PICTURE KEY " << frameAnchor->picture.key.toString() << endl;

    frameAnchor->key = frameAnchor->picture.key;
}

static void ProcessTableAnchor( QDomNode myNode, KWEFKWordLeader *leader, FrameAnchor* frameAnchor,
    const int col, const int row, const int cols, const int rows )
{
    frameAnchor->type = 6; // Table

    Q3ValueList<ParaData> cellParaList;
    Q3ValueList<TagProcessing> tagProcessingList;
    tagProcessingList << TagProcessing ( "FRAME",     ProcessFrameTag,     frameAnchor   )
                        << TagProcessing ( "PARAGRAPH", ProcessParagraphTag, &cellParaList );
    ProcessSubtags (myNode, tagProcessingList, leader);

    frameAnchor->table.addCell (col, row, cols, rows, cellParaList, frameAnchor->frame);
}

static void ProcessFramesetTag ( QDomNode        myNode,
                                void            *tagData,
                                KWEFKWordLeader *leader )
{
#if 0
    kDebug (30508) << "ProcessFramesetTag () - Begin" << endl;
#endif

    Q3ValueList<ParaData> *paraList = (Q3ValueList<ParaData> *) tagData;

    int     frameType = -1;
    int     frameInfo = -1;
    int     col       = -1;
    int     row       = -1;
    int     cols      = -1;
    int     rows      = -1;
    QString grpMgr;

    const QString oldName ( leader->m_currentFramesetName );
    
    Q3ValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ( "name",        leader->m_currentFramesetName      )
                       << AttrProcessing ( "frameType",   frameType )
                       << AttrProcessing ( "frameInfo",   frameInfo )
                       << AttrProcessing ( "removable" )
                       << AttrProcessing ( "visible" )
                       << AttrProcessing ( "grpMgr",      grpMgr    )
                       << AttrProcessing ( "row",         row       )
                       << AttrProcessing ( "col",         col       )
                       << AttrProcessing ( "rows",        rows      )
                       << AttrProcessing ( "cols",        cols      )
                       << AttrProcessing ( "protectSize" )
                        ;
    ProcessAttributes (myNode, attrProcessingList);

    switch ( frameType )
    {
    case 1:
            if ( grpMgr.isEmpty () )
            {
                // As we do not support anything else than normal text, process only normal text.
                // TODO: Treat the other types of frames (frameType)
                if (frameInfo==0)
                {
                    // Normal Text
                    kDebug(30508) << "Processing Frameset: " << leader->m_currentFramesetName << endl;
                    Q3ValueList<TagProcessing> tagProcessingList;
                    tagProcessingList.append(TagProcessing ( "FRAME" ));
                    tagProcessingList.append(TagProcessing ( "PARAGRAPH", ProcessParagraphTag,  paraList ));
                    ProcessSubtags (myNode, tagProcessingList, leader);
                }
                else if (frameInfo==1)
                {
                    // header for first page
                    HeaderData header;
                    header.page = HeaderData::PAGE_FIRST;
                    Q3ValueList<TagProcessing> tagProcessingList;
                    tagProcessingList.append(TagProcessing ( "FRAME" ));
                    tagProcessingList.append(TagProcessing ( "PARAGRAPH", ProcessParagraphTag,  &header.para ));

                    ProcessSubtags (myNode, tagProcessingList, leader);
                    leader->doHeader( header );
                }
                else if (frameInfo==2)
                {
                    // header for even page
                    HeaderData header;
                    header.page = HeaderData::PAGE_EVEN;
                    Q3ValueList<TagProcessing> tagProcessingList;
                    tagProcessingList.append(TagProcessing ( "FRAME" ));
                    tagProcessingList.append(TagProcessing ( "PARAGRAPH", ProcessParagraphTag,  &header.para ));
                    ProcessSubtags (myNode, tagProcessingList, leader);
                    leader->doHeader( header );
                }
                else if (frameInfo==3)
                {
                    // header for odd page (or all page, if hType=0)
                    HeaderData header;
                    header.page = (leader->headerType() != 0 ) ? HeaderData::PAGE_ODD : HeaderData::PAGE_ALL;
                    Q3ValueList<TagProcessing> tagProcessingList;
                    tagProcessingList.append(TagProcessing ( "FRAME" ));
                    tagProcessingList.append(TagProcessing ( "PARAGRAPH", ProcessParagraphTag,  &header.para ));
                    ProcessSubtags (myNode, tagProcessingList, leader);
                    leader->doHeader( header );
                }
                else if (frameInfo==4)
                {
                    // footer for first page
                    FooterData footer;
                    footer.page = FooterData::PAGE_FIRST;
                    Q3ValueList<TagProcessing> tagProcessingList;
                    tagProcessingList.append(TagProcessing ( "FRAME" ));
                    tagProcessingList.append(TagProcessing ( "PARAGRAPH", ProcessParagraphTag,  &footer.para ));
                    ProcessSubtags (myNode, tagProcessingList, leader);
                    leader->doFooter( footer );
                }
                else if (frameInfo==5)
                {
                    // footer for even page
                    FooterData footer;
                    footer.page = FooterData::PAGE_EVEN;
                    Q3ValueList<TagProcessing> tagProcessingList;
                    tagProcessingList.append(TagProcessing ( "FRAME" ));
                    tagProcessingList.append(TagProcessing ( "PARAGRAPH", ProcessParagraphTag,  &footer.para ));
                    ProcessSubtags (myNode, tagProcessingList, leader);
                    leader->doFooter( footer );
                }
                else if (frameInfo==6)
                {
                    // footer for odd page (or all page, if fType=0)
                    FooterData footer;
                    footer.page = (leader->footerType() != 0) ? FooterData::PAGE_ODD : FooterData::PAGE_ALL;
                    Q3ValueList<TagProcessing> tagProcessingList;
                    tagProcessingList.append(TagProcessing ( "FRAME" ));
                    tagProcessingList.append(TagProcessing ( "PARAGRAPH", ProcessParagraphTag,  &footer.para ));
                    ProcessSubtags (myNode, tagProcessingList, leader);
                    leader->doFooter( footer );
                }
            }
            else
            {
                if ( col != -1 && row != -1 )
                {
                    if ( cols > 0 && rows > 0 )
                    {
#if 0
                        kDebug (30508) << "DEBUG - FRAMESET: table " << name << " col, row = "
                                        << col << ", " << row << ", Mgr = "<< grpMgr << endl;
#endif
                        FrameAnchor *frameAnchor = findAnchor (grpMgr, *paraList);
                        if ( frameAnchor )
                        {
                            ProcessTableAnchor( myNode, leader, frameAnchor, col, row, cols, rows );
                        }
                        else
                        {
                            bool found = false;
                            KoPictureKey key( grpMgr );
                            Q3ValueList<FrameAnchor>::Iterator it;
                            for ( it = leader->m_nonInlinedTableAnchors.begin(); it !=  leader->m_nonInlinedTableAnchors.end(); ++it )
                            {
                                if ( (*it).key == key )
                                {
                                    kDebug(30508) << "Found pseudo-anchor for table: " << (*it).key.toString() << endl;
                                    found = true;
                                    break;
                                }
                            }
                            
                            if ( found )
                            {
                                ProcessTableAnchor( myNode, leader, &(*it), col, row, cols, rows );
                            }
                            else
                            {
                                kWarning(30508) << "Table anchor not found: " << grpMgr << endl;
                                FrameAnchor anchor;
                                ProcessTableAnchor( myNode, leader, &anchor, col, row, cols, rows );
                                anchor.key = key; // Needed, so that the pseudo-anchor can be found again
                                leader->m_nonInlinedTableAnchors << anchor;
                                leader->m_unanchoredFramesets.append( grpMgr );
                            }
                        }
                    }
                    else
                    {
                        kWarning (30508) << "Unexpected value for one of, or all FRAMESET attribute cols, rows: "
                                        << cols << ", " << rows << "!" << endl;
                        AllowNoSubtags (myNode, leader);
                    }
                }
                else
                {
                    kWarning (30508) << "Unset value for one of, or all FRAMESET attributes col, row: "
                                    << col << ", " << row << "!" << endl;
                    AllowNoSubtags (myNode, leader);
                    leader->m_unanchoredFramesets.append( leader->m_currentFramesetName );
                }
            }
            break;

    case 2: // PICTURE
    case 5: // CLIPART: deprecated (up to KOffice 1.2 Beta 2)
        {
#if 0
            kDebug (30508) << "DEBUG: FRAMESET name of picture is " << name << endl;
#endif

            FrameAnchor *frameAnchor = findAnchor ( leader->m_currentFramesetName, *paraList );

            if ( frameAnchor )
            {
                ProcessPictureAnchor( myNode, leader, frameAnchor, frameType );
            }
            else
            {
                // ### TODO: non-inlined picture?
                // No anchor found, so the picture is not inlined
                kDebug (30508) << "ProcessFramesetTag: Couldn't find anchor " << leader->m_currentFramesetName << endl;
                FrameAnchor anchor;
                ProcessPictureAnchor( myNode, leader, &anchor, frameType );
                leader->m_nonInlinedPictureAnchors << anchor;
                leader->m_unanchoredFramesets.append( leader->m_currentFramesetName ); // DEBUG
            }

            break;
        }

    case 4: // KFormula
        {
            kWarning(30508) << "KFormula frameset not supported yet!" << endl; // ### TODO
            break;
        }
    default:
            kWarning (30508) << "Unexpected frametype " << frameType << " (in ProcessFramesetTag)" << endl;
    }

    leader->m_currentFramesetName = oldName;
    
#if 0
    kDebug (30508) << "ProcessFramesetTag () - End" << endl;
#endif
}


static void ProcessFramesetsTag ( QDomNode        myNode,
                                  void            *tagData,
                                  KWEFKWordLeader *leader )
{
    AllowNoAttributes (myNode);
    Q3ValueList<TagProcessing> tagProcessingList;
    tagProcessingList << TagProcessing ( "FRAMESET", ProcessFramesetTag, tagData );
    ProcessSubtags (myNode, tagProcessingList, leader);
}


static void ProcessStyleTag (QDomNode myNode, void *, KWEFKWordLeader *leader )
{
    LayoutData layout;

    ProcessLayoutTag (myNode, &layout, leader);

    leader->doFullDefineStyle (layout);
}


static void ProcessStylesPluralTag (QDomNode myNode, void *, KWEFKWordLeader *leader )
{
    AllowNoAttributes (myNode);

    leader->doOpenStyles ();

    Q3ValueList<TagProcessing> tagProcessingList;
    tagProcessingList << TagProcessing ( "STYLE", ProcessStyleTag, leader );
    ProcessSubtags (myNode, tagProcessingList, leader);

    leader->doCloseStyles ();
}


static void ProcessPaperBordersTag (QDomNode myNode, void*, KWEFKWordLeader* leader)
{

    double left   = 0.0;
    double right  = 0.0;
    double top    = 0.0;
    double bottom = 0.0;

    Q3ValueList<AttrProcessing> attrProcessingList;
    if ( leader->m_oldSyntax )
    {
        attrProcessingList
            << AttrProcessing ( "ptLeft",   left )
            << AttrProcessing ( "ptRight",  right )
            << AttrProcessing ( "ptTop",    top )
            << AttrProcessing ( "ptBottom", bottom )
            << AttrProcessing ( "mmLeft" )
            << AttrProcessing ( "mmRight" )
            << AttrProcessing ( "mmTop" )
            << AttrProcessing ( "mmBottom" )
            << AttrProcessing ( "inchLeft" )
            << AttrProcessing ( "inchRight" )
            << AttrProcessing ( "inchTop" )
            << AttrProcessing ( "inchBottom" )
            ;
    }
    else
    {
        attrProcessingList
            << AttrProcessing ( "left",   left )
            << AttrProcessing ( "right",  right )
            << AttrProcessing ( "top",    top )
            << AttrProcessing ( "bottom", bottom )
            ;
    }
    ProcessAttributes (myNode, attrProcessingList);

    leader->doFullPaperBorders(top, left, bottom, right);

    AllowNoSubtags (myNode, leader);
}

static void ProcessPaperTag (QDomNode myNode, void *, KWEFKWordLeader *leader)
{

    int format      = -1;
    int orientation = -1;
    double width    = -1.0;
    double height   = -1.0;
    int hType       = -1;
    int fType       = -1;
    int columns = 1;
    double columnspacing = 36.0; // Half-inch
    int numPages = -1;

    Q3ValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ( "format",              format      )
                       << AttrProcessing ( "width",               width       )
                       << AttrProcessing ( "height",              height      )
                       << AttrProcessing ( "orientation",         orientation )
                       << AttrProcessing ( "columns",             columns )
                       << AttrProcessing ( "columnspacing",       columnspacing )
                       << AttrProcessing ( "pages",               numPages )
                       << AttrProcessing ( "hType",               hType        )
                       << AttrProcessing ( "fType",               fType        )
                       << AttrProcessing ( "spHeadBody" )
                       << AttrProcessing ( "spFootBody" )
                       << AttrProcessing ( "spFootNoteBody" )
                       << AttrProcessing ( "slFootNotePosition" )
                       << AttrProcessing ( "slFootNoteLength" )
                       << AttrProcessing ( "slFootNoteWidth" )
                       << AttrProcessing ( "slFootNoteType" );

    if ( leader->m_oldSyntax )
    {
        // ### TODO: in syntax 1 hType and fType have other values!
        attrProcessingList
            << AttrProcessing ( "ptWidth", width )
            << AttrProcessing ( "ptHeight", height )
            << AttrProcessing ( "ptColumnspc", columnspacing )
            << AttrProcessing ( "mmWidth" )
            << AttrProcessing ( "mmHeight" )
            << AttrProcessing ( "mmColumnspc" )
            << AttrProcessing ( "inchWidth" )
            << AttrProcessing ( "inchHeight" )
            << AttrProcessing ( "inchColumnspc" )
            ;
    }

    ProcessAttributes (myNode, attrProcessingList);

    leader->setHeaderType( hType );
    leader->setFooterType( fType );

    leader->doPageInfo ( hType, fType );
    leader->doFullPaperFormat (format, width, height, orientation);
    leader->doFullPaperFormatOther( columns, columnspacing, numPages );

    Q3ValueList<TagProcessing> tagProcessingList;
    tagProcessingList
        << TagProcessing ( "PAPERBORDERS", ProcessPaperBordersTag, NULL )
        ;

    ProcessSubtags (myNode, tagProcessingList, leader);
}

static void ProcessVariableSettingsTag (QDomNode myNode, void *, KWEFKWordLeader *leader)
{
    VariableSettingsData vs;
    QString print, creation, modification; // Dates (in ISO 8601 format)
    int creationYear = -1;
    int creationMonth = -1;
    int creationDay = -1;
    int modificationYear = -1;
    int modificationMonth = -1;
    int modificationDay = -1;

    Q3ValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ( "startingPageNumber", vs.startingPageNumber )
                       << AttrProcessing ( "displaylink", vs.displaylink )
                       << AttrProcessing ( "underlinelink", vs.underlinelink )
                       << AttrProcessing ( "displaycomment", vs.displaycomment )
                       << AttrProcessing ( "displayfieldcode", vs.displayfieldcode )
        ;


    // The following 3 attributes are from syntax 3 but at least the RTF import filter generate them with syntax 2.
    attrProcessingList
        << AttrProcessing ( "lastPrintingDate", print )
        << AttrProcessing ( "creationDate", creation )
        << AttrProcessing ( "modificationDate", modification );
    ;

    // Some files have the creation and modification date not in one attribute but in an attribute for each the year, the month, the day
    // ( e.g. syntax 2 file kofficetests/documents/export/kword/text/all.kwd )
    attrProcessingList
        << AttrProcessing( "modifyFileYear", modificationYear )
        << AttrProcessing( "modifyFileMonth", modificationMonth )
        << AttrProcessing( "modifyFileDay", modificationDay )
        << AttrProcessing( "createFileYear", creationYear )
        << AttrProcessing( "createFileMonth", creationMonth )
        << AttrProcessing( "createFileDay", creationDay )
        ;

    ProcessAttributes (myNode, attrProcessingList);

    if ( creation.isEmpty() )
    {
        if ( ( creationYear >= 1970 ) && QDate::isValid( creationYear, creationMonth, creationDay ) )
        {
            vs.creationTime = QDateTime( QDate( creationYear, creationMonth, creationDay ) );
        }
    }
    else
        vs.creationTime=QDateTime::fromString(creation, Qt::ISODate);
    //kDebug(30508) << "Creation time: " << vs.creationTime.toString( Qt::ISODate ) << endl;

    if ( modification.isEmpty() )
    {
        if ( ( modificationYear >= 1970 ) && QDate::isValid( modificationYear, modificationMonth, modificationDay ) )
        {
            vs.modificationTime = QDateTime( QDate( modificationYear, modificationMonth, modificationDay ) );
        }
    }
    else
        vs.modificationTime=QDateTime::fromString(modification, Qt::ISODate);
    //kDebug(30508) << "Modification time: " << vs.modificationTime.toString( Qt::ISODate ) << endl;

    if (!print.isEmpty())
        vs.printTime=QDateTime::fromString(print, Qt::ISODate);
    //kDebug(30508) << "Print time: " << vs.printTime.toString( Qt::ISODate ) << endl;

    leader->doVariableSettings (vs);
}

static void ProcessSpellCheckIgnoreWordTag (QDomNode myNode, void *, KWEFKWordLeader *leader )
{
    QString ignoreword;

    Q3ValueList<AttrProcessing> attrProcessingList;
    attrProcessingList
        << AttrProcessing ( "word", ignoreword )
        ;
    ProcessAttributes (myNode, attrProcessingList);

    leader->doFullSpellCheckIgnoreWord (ignoreword);

    AllowNoSubtags (myNode, leader);
}


static void ProcessSpellCheckIgnoreListTag (QDomNode myNode, void *, KWEFKWordLeader *leader )
{
    AllowNoAttributes (myNode);

    leader->doOpenSpellCheckIgnoreList ();

    Q3ValueList<TagProcessing> tagProcessingList;
    tagProcessingList << TagProcessing ( "SPELLCHECKIGNOREWORD", ProcessSpellCheckIgnoreWordTag, leader );
    ProcessSubtags (myNode, tagProcessingList, leader);

    leader->doCloseSpellCheckIgnoreList ();
}


static void ProcessPixmapsKeyTag ( QDomNode         myNode,
                                   void            *tagData,
                                   KWEFKWordLeader *leader )
{
    Q3ValueList<ParaData> *paraList = (Q3ValueList<ParaData> *) tagData;

    KoPictureKey key;

    // Let KoPictureKey do most of the loading
    key.loadAttributes(myNode.toElement());
    const QString name(myNode.toElement().attribute("name"));

    kDebug(30508) << "search anchors: " << key.toString() << endl;
    bool found = false;

    // NOTE: we must always search in both inlined and non-inlined pictures. A picture can be used in both ways and a few times in each!
    
    // Process inlined pictures
    Q3ValueList<ParaData>::Iterator paraIt;

    for ( paraIt = paraList->begin(); paraIt != paraList->end(); ++paraIt )
    {
        ValueListFormatData::Iterator formattingIt;
        for ( formattingIt = (*paraIt).formattingList.begin();
              formattingIt != (*paraIt).formattingList.end();
              formattingIt++ )
        {
            if ( ( ( (*formattingIt).id == 6 ) || ( (*formattingIt).id == 2 ) )
                 && (*formattingIt).frameAnchor.key == key )
            {
                kDebug(30508) << "Found anchor for inlined picture: " << (*formattingIt).frameAnchor.key.toString() << endl;
                (*formattingIt).frameAnchor.picture.koStoreName = name;
                found = true;
            }
        }
    }
    // Process non-inline pictures
    Q3ValueList<FrameAnchor>::Iterator it;
    for ( it = leader->m_nonInlinedPictureAnchors.begin(); it !=  leader->m_nonInlinedPictureAnchors.end(); ++it )
    {
        if ( (*it).key == key )
        {
            kDebug(30508) << "Found pseudo-anchor for non-inlined picture: " << (*it).key.toString() << endl;
            (*it).picture.koStoreName = name;
            found = true;
        }
    }
    
    if ( !found )
    {
        kWarning (30508) << "Could not find any anchor for picture " << key.toString() << endl;
    }

    AllowNoSubtags (myNode, leader);
}


static void ProcessPixmapsTag ( QDomNode         myNode,
                                void            *tagData,
                                KWEFKWordLeader *leader )
{
    AllowNoAttributes (myNode);

    Q3ValueList<TagProcessing> tagProcessingList;
    tagProcessingList << TagProcessing ( "KEY", ProcessPixmapsKeyTag, tagData );
    ProcessSubtags (myNode, tagProcessingList, leader);
}


static void FreeCellParaLists ( Q3ValueList<ParaData> &paraList )
{
    Q3ValueList<ParaData>::Iterator paraIt;

    for ( paraIt = paraList.begin (); paraIt != paraList.end (); ++paraIt )
    {
        ValueListFormatData::Iterator formattingIt;

        for ( formattingIt = (*paraIt).formattingList.begin ();
              formattingIt != (*paraIt).formattingList.end ();
              formattingIt++ )
        {
            if ( (*formattingIt).id == 6 && (*formattingIt).frameAnchor.type == 6 )
            {
                Q3ValueList<TableCell>::Iterator cellIt;

                for ( cellIt = (*formattingIt).frameAnchor.table.cellList.begin ();
                      cellIt != (*formattingIt).frameAnchor.table.cellList.end ();
                      cellIt++ )
                {
                    FreeCellParaLists ( *(*cellIt).paraList );   // recursion is great
                    delete (*cellIt).paraList;
                }
            }
        }
    }
}

// like ProcessFramesetTag, but only handle footnotes
static void ProcessFootnoteFramesetTag ( QDomNode myNode, void *tagData, KWEFKWordLeader *leader )
{
    QString frameName;
    int frameType = -1, frameInfo = -1;
    bool visible = false;

    Q3ValueList<AttrProcessing> attrProcessingList;
    attrProcessingList
        << AttrProcessing ( "name",        frameName      )
        << AttrProcessing ( "frameType",   frameType )
        << AttrProcessing ( "frameInfo",   frameInfo )
        << AttrProcessing ( "removable" )
        << AttrProcessing ( "visible",     visible )
        << AttrProcessing ( "grpMgr" )
        << AttrProcessing ( "row" )
        << AttrProcessing ( "col" )
        << AttrProcessing ( "rows" )
        << AttrProcessing ( "cols" )
        << AttrProcessing ( "protectSize" )
        ;
    ProcessAttributes (myNode, attrProcessingList);

    // for footnote frame, frameType is 1 and frameInfo is 7
    if( ( frameType == 1 ) && ( frameInfo == 7 ) )
    {
        FootnoteData footnote;
        footnote.frameName = frameName;
        Q3ValueList<TagProcessing> tagProcessingList;
        tagProcessingList.append(TagProcessing ( "FRAME" ));
        tagProcessingList.append(TagProcessing ( "PARAGRAPH", ProcessParagraphTag,  &footnote.para ));
        ProcessSubtags (myNode, tagProcessingList, leader);
        leader->footnoteList.append( footnote );
    }
}

// like ProcessFramesetsTag, but only handle footnotes
static void ProcessFootnoteFramesetsTag ( QDomNode myNode, void *tagData, KWEFKWordLeader *leader )
{
    AllowNoAttributes (myNode);

    Q3ValueList<TagProcessing> tagProcessingList;
    tagProcessingList << TagProcessing ( "FRAMESET", ProcessFootnoteFramesetTag, tagData );
    ProcessSubtags (myNode, tagProcessingList, leader);
}

static void ProcessBookmarkItemTag ( QDomNode myNode, void* tag, KWEFKWordLeader *leader )
{
    Q3ValueList<Bookmark> * bookmarkList = static_cast< Q3ValueList<Bookmark> * > ( tag );

    Bookmark bookmark;

    Q3ValueList<AttrProcessing> attrProcessingList;
    attrProcessingList
        << AttrProcessing ( "name", bookmark.m_name )
        << AttrProcessing ( "cursorIndexStart", bookmark.m_cursorIndexStart )
        << AttrProcessing ( "cursorIndexEnd", bookmark.m_cursorIndexEnd )
        << AttrProcessing ( "frameset", bookmark.m_frameset )
        << AttrProcessing ( "startparag", bookmark.m_startparag )
        << AttrProcessing ( "endparag", bookmark.m_endparag )
        ;

    ProcessAttributes (myNode, attrProcessingList);

    AllowNoSubtags( myNode, leader );
    
    // ### TODO: some verifications

    kDebug(30508) << "Bookmark: " << bookmark.m_name << " in frameset " << bookmark.m_frameset << endl;

    bookmarkList->append( bookmark );
}

static void ProcessBookmarksTag ( QDomNode myNode, void* tag, KWEFKWordLeader *leader )
{
    AllowNoAttributes (myNode);

    Q3ValueList<TagProcessing> tagProcessingList;
    tagProcessingList << TagProcessing ( "BOOKMARKITEM", ProcessBookmarkItemTag, tag );
    ProcessSubtags (myNode, tagProcessingList, leader);
}

void ProcessDocTag ( QDomNode         myNode,
    void* /*tagData*/, KWEFKWordLeader* leader )
{
    //kDebug (30508) << "Entering ProcessDocTag" << endl;

    QString editor, author;
    
    Q3ValueList<AttrProcessing> attrProcessingList;

    attrProcessingList
        << AttrProcessing ( "xmlns" )
        << AttrProcessing ( "editor", editor )
        << AttrProcessing ( "mime" )
        << AttrProcessing ( "syntaxVersion", leader->m_syntaxVersion )
        << AttrProcessing ( "author", author )
        << AttrProcessing ( "email" )
        ;

    ProcessAttributes( myNode, attrProcessingList );

    kDebug(30508) << "Document written by " << editor << endl;
    kDebug(30508) << "Document of syntax version " << leader->m_syntaxVersion << endl;

    if ( leader->m_syntaxVersion == 1 )
    {
        leader->m_oldSyntax = true; // Syntax 1 is old syntax
    }
    else if ( leader->m_syntaxVersion == -1 )
    {
        // We do not know the version, but it still might be an old syntax.
        // However such old documents have still an author attribute, so check its value
        if ( author == "Reginald Stadlbauer and Torben Weis" )
        {
            kDebug(30508) << "No syntax version but author attribute matches => assuming old syntax" << endl;
            leader->m_oldSyntax = true;
        }
        else
        {
            kWarning(30508) << "No syntax version found, author attribute does not match => assuming new syntax" << endl;
        }
    }

    leader->doOpenHead();

    // At first, process <SPELLCHECKIGNORELIST>, even if mostly it will not be needed
    QDomNode nodeIgnoreList=myNode.namedItem("SPELLCHECKIGNORELIST");
    if ( nodeIgnoreList.isNull () )
        kDebug (30508) << "No <SPELLCHECKIGNORELIST>" << endl; // Most files will not have it!
    else
        ProcessSpellCheckIgnoreListTag (nodeIgnoreList, NULL, leader);

    // Process <PAPER> now, even if mostly the output will need to be delayed.
    QDomNode nodePaper=myNode.namedItem("PAPER");
    if ( nodePaper.isNull () )
        kWarning (30508) << "No <PAPER>" << endl;
    else
        ProcessPaperTag (nodePaper, NULL, leader);

    // Process <VARIABLESETTINGS>
    QDomNode nodeVariableSettings=myNode.namedItem("VARIABLESETTINGS");
    if ( nodeVariableSettings.isNull () )
	kWarning (30508) << "No <VARIABLESETTINGS>" << endl;
    else
	ProcessVariableSettingsTag (nodeVariableSettings, NULL, leader);

    // Then we process the styles
    QDomNode nodeStyles=myNode.namedItem("STYLES");
    if ( nodeStyles.isNull () )
        kWarning (30508) << "No <STYLES>" << endl;
    else
        ProcessStylesPluralTag (nodeStyles, NULL, leader);

    // Process framesets, but only to find and extract footnotes (also endnotes)
    Q3ValueList<FootnoteData> footnotes;
    QDomNode nodeFramesets=myNode.namedItem("FRAMESETS");
    if ( !nodeFramesets.isNull() )
        ProcessFootnoteFramesetsTag(nodeFramesets, &footnotes, leader );

    // Process all framesets and pictures
    Q3ValueList<TagProcessing> tagProcessingList;
    Q3ValueList<ParaData> paraList;

    tagProcessingList
        << TagProcessing ( "PAPER" ) // Already done
        << TagProcessing ( "ATTRIBUTES" )
        << TagProcessing ( "FRAMESETS",   ProcessFramesetsTag,    &paraList )
        << TagProcessing ( "STYLES" ) // Already done
        << TagProcessing ( "PICTURES",    ProcessPixmapsTag,      &paraList )
        << TagProcessing ( "PIXMAPS",     ProcessPixmapsTag,      &paraList )
        << TagProcessing ( "CLIPARTS",    ProcessPixmapsTag,      &paraList )
        << TagProcessing ( "EMBEDDED" )
        << TagProcessing ( "BOOKMARKS",   ProcessBookmarksTag,    &leader->m_bookmarkList )
        ;

    // TODO: why are the followings used by KWord 1.2 but are not in its DTD?
    tagProcessingList << TagProcessing ( "SERIALL" );
    tagProcessingList << TagProcessing ( "FOOTNOTEMGR" );

    ProcessSubtags (myNode, tagProcessingList, leader);

    leader->doCloseHead();
    leader->doDeclareNonInlinedFramesets( leader->m_nonInlinedPictureAnchors, leader->m_nonInlinedTableAnchors );
    leader->doOpenBody();

    leader->doFullDocument (paraList);

    kDebug(30508) << "Unachored Framesets : START" << endl;
    QStringList::ConstIterator it;
    for ( it = leader->m_unanchoredFramesets.begin(); it != leader->m_unanchoredFramesets.end(); ++it )
    {
        kDebug(30508) << (*it) << endl;
    }
    kDebug(30508) << "Unachored Framesets : END" << endl;
    
    FreeCellParaLists (paraList);

    leader->doCloseBody();

    //kDebug (30508) << "Exiting ProcessDocTag" << endl;
}


void KWEFKWordLeader::setWorker ( KWEFBaseWorker *newWorker )
{
    m_worker = newWorker;

    if (newWorker)
        newWorker->registerKWordLeader(this);
}


KWEFBaseWorker *KWEFKWordLeader::getWorker(void) const
{
    return m_worker;
}


// Short simple definition for methods with void parameter
#define DO_VOID_DEFINITION(string) \
    bool KWEFKWordLeader::string() \
    {\
        if (m_worker) \
            return m_worker->string(); \
        return false; \
    }


bool KWEFKWordLeader::doOpenFile ( const QString &filenameOut, const QString &to )
{
    if ( m_worker )
        return m_worker->doOpenFile (filenameOut, to);

    // As it would be the first method to be called, warn if worker is NULL
    kError (30508) << "No Worker! (in KWEFKWordLeader::doOpenFile)" << endl;

    return false;
}


DO_VOID_DEFINITION (doCloseFile)
DO_VOID_DEFINITION (doAbortFile)
DO_VOID_DEFINITION (doOpenDocument)
DO_VOID_DEFINITION (doCloseDocument)
DO_VOID_DEFINITION (doOpenStyles)
DO_VOID_DEFINITION (doCloseStyles)
DO_VOID_DEFINITION (doOpenHead)
DO_VOID_DEFINITION (doCloseHead)
DO_VOID_DEFINITION (doOpenBody)
DO_VOID_DEFINITION (doCloseBody)
DO_VOID_DEFINITION (doOpenSpellCheckIgnoreList)
DO_VOID_DEFINITION (doCloseSpellCheckIgnoreList)

bool KWEFKWordLeader::doFullDocumentInfo (const KWEFDocumentInfo &docInfo)
{
    if ( m_worker )
        return m_worker->doFullDocumentInfo (docInfo);

    return false;
}


bool KWEFKWordLeader::doVariableSettings (const VariableSettingsData &varSettings)
{
    if ( m_worker )
        return m_worker->doVariableSettings (varSettings);

    return false;
}


bool KWEFKWordLeader::doFullDocument (const Q3ValueList<ParaData> &paraList)
{
    if ( m_worker )
        return m_worker->doFullDocument (paraList);

    return false;
}

bool KWEFKWordLeader::doPageInfo ( const int headerType, const int footerType )
{
    if ( m_worker )
        return m_worker->doPageInfo ( headerType, footerType );

    return false;
}

bool KWEFKWordLeader::doFullPaperFormat ( const int format, const double width, const double height, const int orientation )
{
    if ( m_worker )
        return m_worker->doFullPaperFormat (format, width, height, orientation);

    return false;
}

bool KWEFKWordLeader::doFullPaperBorders (const double top, const double left, const double bottom, const double right)
{
    if ( m_worker )
        return m_worker->doFullPaperBorders (top, left, bottom, right);

    return false;
}

bool KWEFKWordLeader::doFullPaperFormatOther ( const int columns, const double columnspacing, const int numPages )
{
    if ( m_worker )
        return m_worker->doFullPaperFormatOther ( columns, columnspacing, numPages );
    
    return false;
}

bool KWEFKWordLeader::doFullDefineStyle ( LayoutData &layout )
{
    if ( m_worker )
        return m_worker->doFullDefineStyle (layout);

    return false;
}

bool KWEFKWordLeader::doFullSpellCheckIgnoreWord (const QString& ignoreword)
{
    if ( m_worker )
        return m_worker->doFullSpellCheckIgnoreWord (ignoreword);

    return false;
}

bool KWEFKWordLeader::doHeader ( const HeaderData& header )
{
    if ( m_worker )
        return m_worker->doHeader (header);

    return false;
}

bool KWEFKWordLeader::doFooter ( const FooterData& footer )
{
    if ( m_worker )
        return m_worker->doFooter (footer);

    return false;
}

bool KWEFKWordLeader::doDeclareNonInlinedFramesets( Q3ValueList<FrameAnchor>& pictureAnchors, Q3ValueList<FrameAnchor>& tableAnchors )
{
    if ( m_worker )
        return m_worker->doDeclareNonInlinedFramesets( pictureAnchors, tableAnchors );

    return false;
}

static bool ParseFile ( QIODevice* subFile, QDomDocument& doc)
{
    QString errorMsg;
    int errorLine;
    int errorColumn;

    if ( !doc.setContent (subFile, &errorMsg, &errorLine, &errorColumn) )
    {
        kError (30508) << "Parsing Error! Aborting! (in ParseFile)" << endl
            << "  Line: " << errorLine << " Column: " << errorColumn << endl
            << "  Message: " << errorMsg << endl;
        // ### TODO: the error is in which sub-file?
        KMessageBox::error( 0L, i18n("An error has occurred while parsing the KWord file.\nAt line: %1, column %2\nError message: %3",errorLine ,errorColumn ,i18n( "QXml", errorMsg ) ),
            i18n("KWord Export Filter Library"), 0 );
        return false;
    }
    return true;
}

static bool ProcessStoreFile ( QIODevice* subFile,
    void (*processor) (QDomNode, void *, KWEFKWordLeader *),
    KWEFKWordLeader* leader)
{
    if (!subFile)
    {
        kWarning(30508) << "Could not get a device for the document!" << endl;
    }
    else if ( subFile->open ( QIODevice::ReadOnly ) )
    {
        kDebug (30508) << "Processing Document..." << endl;
        QDomDocument doc;
        if (!ParseFile(subFile, doc))
        {
            subFile->close();
            return false;
        }
        // We must close the subFile before processing,
        //  as the processing could open other sub files.
        //  However, it would crash if two sub files are opened together
        subFile->close();

        QDomNode docNode = doc.documentElement();
        processor (docNode, NULL, leader);
        return true;
    }
    else
    {
        // Note: we do not worry too much if we cannot open the document info!
        kWarning (30508) << "Unable to open document!" << endl;
    }
    return false;
}

QIODevice* KWEFKWordLeader::getSubFileDevice(const QString& fileName)
{
    KoStoreDevice* subFile;

    subFile=m_chain->storageFile(fileName,KoStore::Read);

    if (!subFile)
    {
        kError(30508) << "Could not get a device for sub-file: " << fileName << endl;
        return NULL;
    }
    return subFile;
}


bool KWEFKWordLeader::loadSubFile(const QString& fileName, QByteArray& array)
{
    KoStoreDevice* subFile;

    subFile=m_chain->storageFile(fileName,KoStore::Read);

    if (!subFile)
    {
        kError(30508) << "Could not get a device for sub-file: " << fileName << endl;
        return false;
    }
    else if ( subFile->open ( QIODevice::ReadOnly ) )
    {
        array = subFile->readAll();
        subFile->close ();
    }
    else
    {
        kError(30508) << "Unable to open " << fileName << " sub-file" << endl;
        return false;
    }

    return true;
}

KoFilter::ConversionStatus KWEFKWordLeader::convert( KoFilterChain* chain,
    const QByteArray& from, const QByteArray& to)
{
    if ( from != "application/x-kword" )
    {
        return KoFilter::NotImplemented;
    }

    if (!chain)
    {
        kError(30508) << "'Chain' is NULL! Internal error of the filter system?" << endl;
        return KoFilter::StupidError;
    }

    m_chain=chain;

    if ( !doOpenFile (chain->outputFile(),to) )
    {
        kError (30508) << "Worker could not open export file! Aborting!" << endl;
        return KoFilter::StupidError;
    }

    if ( !doOpenDocument () )
    {
        kError (30508) << "Worker could not open document! Aborting!" << endl;
        doAbortFile ();
        return KoFilter::StupidError;
    }

    KoStoreDevice* subFile;

    subFile=chain->storageFile("documentinfo.xml",KoStore::Read);
    kDebug (30508) << "Processing documentinfo.xml..." << endl;
    // Do not care if we cannot open the document info.
    ProcessStoreFile (subFile, ProcessDocumentInfoTag, this);

    subFile=chain->storageFile("root",KoStore::Read);
    kDebug (30508) << "Processing root..." << endl;
    if (!ProcessStoreFile (subFile, ProcessDocTag, this))
    {
        kWarning(30508) << "Opening root has failed. Trying raw XML file!" << endl;

        const QString filename( chain->inputFile() );
        if (filename.isEmpty() )
        {
            kError(30508) << "Could not open document as raw XML! Aborting!" << endl;
            doAbortFile();
            return KoFilter::StupidError;
        }
        else
        {
            QFile file( filename );
            if ( ! ProcessStoreFile( &file, ProcessDocTag, this ) )
            {
                kError(30508) << "Could not process document! Aborting!" << endl;
                doAbortFile();
                return KoFilter::StupidError;
            }
        }
    }
    
    doCloseDocument ();

    doCloseFile ();

    return KoFilter::OK;
}
