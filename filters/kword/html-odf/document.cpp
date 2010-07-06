/* This file is part of the KOffice project
   Copyright (C) 2010 Pramod S G <pramod.xyle@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the Library GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.

*/
#include "generated/leinputstream.h"
#include "document.h"
#include "drawstyle.h"

#include <KoUnit.h>
#include <KoPageLayout.h>
#include <kdebug.h>

#include <klocale.h>
#include <KoStore.h>
#include <KoFilterChain.h>
#include <KoFontFace.h>

#include <QBuffer>
#include <QColor>


Document::Document()
        : m_textHandler(0)
        , m_tableHandler(0)
        , m_graphicsHandler(0)
        , m_bodyFound(false)
        , m_footNoteNumber(0)
        , m_endNoteNumber(0)
        , m_bodyWriter(0)
        , m_mainStyles(0)
        , m_metaWriter(0)
        , m_headerWriter(0)
        , m_headerCount(0)
        , m_writingHeader(false)
        , m_evenOpen(false)
        , m_firstOpen(false)
        , m_buffer(0)
        , m_bufferEven(0)
        , m_writeMasterPageName(false)
        , m_omittMasterPage(false)
{

}

Document::~Document()
{

}

void Document::finishDocument()
{
    kDebug(30513);

    Q_ASSERT(m_mainStyles);
    if (m_mainStyles) {

        QString footnoteConfig("<text:notes-configuration "
                               "text:note-class=\"footnote\" "
                               "text:default-style-name=\"Footnote\" "
                               "text:citation-style-name=\"Footnote_20_Symbol\" "
                               "text:citation-body-style-name=\"Footnote_20_anchor\" "
                               "text:master-page-name=\"Footnote\" "
                               "style:num-format=\"%1\" "
                               "text:start-value=\"%2\" "
                               "text:footnotes-position=\"page\" "
                               "text:start-numbering-at=\"%3\" "
                               "/>");


        QString endnoteConfig("<text:notes-configuration "
                              "text:note-class=\"endnote\" "
                              "text:default-style-name=\"Endnote\" "
                              "text:citation-style-name=\"Endnote_20_Symbol\" "
                              "text:citation-body-style-name=\"Endnote_20_anchor\" "
                              "text:master-page-name=\"Endnote\" "
                              "style:num-format=\"%1\" "
                              "text:start-value=\"%2\" "
                              //"text:start-numbering-at=\"%3\" "
                              "/>");


    }

}


void Document::processAssociatedStrings()
{

}

void Document::processStyles()
{


}
void Document::bodyEnd()
{

}

void Document::headersMask(QList<bool> mask)
{
    kDebug(30513) ;
    m_headersMask = mask;
}


void Document::footnoteStart()
{
    kDebug(30513);
}

void Document::footnoteEnd()
{
    kDebug(30513);
}


void Document::annotationStart()
{
}

void Document::annotationEnd()
{
}


#include "document.moc"
