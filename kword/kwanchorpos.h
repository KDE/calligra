/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>

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

#ifndef kwanchorpos_h
#define kwanchorpos_h

class KWTextFrameSet;
class KWTextParag;
/**
 * Definition of an anchor's position:
 * - A text frameset
 * - A paragraph in this frameset
 * - An index in the paragraph
 * This is used in KWFrameSet for floating framesets
 * and in KWDocument for making framesets floating upon loading.
 */
struct KWAnchorPosition
{
    KWAnchorPosition() { makeInvalid(); }
    void makeInvalid() { textfs = 0L; parag = 0L; index = 0; }

    KWTextFrameSet* textfs;
    KWTextParag* parag;
    int index;
};

#endif
