/* This file is part of the KDE project
   Copyright (C) 2004-2005 David Faure <faure@kde.org>

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

#ifndef KWORD_PARAGVISITORS_H
#define KWORD_PARAGVISITORS_H

#include <KoTextDocument.h>
class KWFrameSet;
template <class T> class QValueList;

// This paragraph visitor collects the inline framesets (found via KWAnchor)
// found in a text selection.
class KWCollectFramesetsVisitor : public KoParagVisitor // see kotextdocument.h
{
public:
    KWCollectFramesetsVisitor() : KoParagVisitor() {}
    virtual bool visit( KoTextParag *parag, int start, int end );

    const QValueList<KWFrameSet *>& frameSets() const { return m_framesets; }

private:
    QValueList<KWFrameSet *> m_framesets;
};

#endif /* KWORD_PARAGVISITORS_H */
