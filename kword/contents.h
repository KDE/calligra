/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef contents_h
#define contents_h

#include <qvaluelist.h>

class KWDocument;
class KWStyle;

/**
 * This class holds information about the table of contents of a given
 * document.
 * In particular, the list of paragraphs that form the TOC is kept here
 * (to remove them before regenerating).
 */
class KWContents
{
public:
    KWContents( KWDocument *doc );

    /**
     * Regenerate table of contents
     */
    void createContents();

    // Well, actually we don't need that. Too dangerous.
    // Better remove parags that have the style "Contents Head X",
    // than remove parags based on their id (in case of deleting a parag etc.!)
    // Load/save support
    //void restoreParagList( QValueList<int> paragIds );
    //QValueList<int> saveParagList() const;

    /* bool hasContents() const {
	return !m_paragIds.isEmpty();
    }*/

protected:

    KWStyle * findOrCreateTOCStyle( int depth );

    KWDocument *m_doc;
};

#endif
