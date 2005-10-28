/* This file is part of the KDE project
   Copyright (C) 2005 Inge Wallin <inge@lysator.liu.se>

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


#include "WUDocument.h"


/******************************************************************/
/* Class: WUDocument                                              */
/******************************************************************/

void WUDocument::clearUndoRedoInfos()
{
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWTextFrameSet *fs = dynamic_cast<KWTextFrameSet *>( fit.current() );
        if ( fs )
            fs->clearUndoRedoInfo();
    }
}

/**
 * Temporary storage for the initial edition info
 * (activeFrameset, cursorParagraph and cursorIndex attributes of the XML)
 */
class WUDocument::InitialEditing {
public:
    QString m_initialFrameSet;
    int m_initialCursorParag;
    int m_initialCursorIndex;
};

const int WUDocument::CURRENT_SYNTAX_VERSION = 3;

WUDocument::WUDocument(QWidget *parentWidget, const char *widname, QObject* parent, const char* name, bool singleViewMode )
    : KWDocument( parentWidget, widname, parent, name, singleViewMode ),
      m_urlIntern()
{
}


WUDocument::~WUDocument()
{
}


#include "WUDocument.moc"
