#ifndef KWTABLEFRAMESET_H
#define KWTABLEFRAMESET_H
/*
    Copyright (C) 2001, S.R.Haque (srhaque@iee.org).
    This file is part of the KDE project

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

DESCRIPTION

    This file defines KWord tables. A table is a KWFrameSet, thus allowing its
    internal structure to be invisible to the rest of kword. This is useful
    since the internal structure of a table is itself a series of KWFrameSets
    (one for each cell).
*/

#include "KWFrame.h"
#include "KWTextFrameSet.h"

#include <q3ptrlist.h>
#include <q3valuevector.h>
#include <QObject>
#include <QString>
//Added by qt3to4:
#include <QDragLeaveEvent>
#include <Q3MemArray>
#include <QKeyEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <Q3ValueList>
#include <QDragEnterEvent>
#include <QMouseEvent>
#include <KoRichText.h>

class KWDocument;
class KWTableFrameSetEdit;
class QPainter;
class KWAnchor;
class KWordFrameSetIface;
class KWFrameViewManager;

class RemovedRow;
class RemovedColumn;

/**
 * @brief This class implements tables by acting as the manager for
 * the frame(set)s which make up the table cells.
 *
 * We have a cell structure which contains one frameset, because
 * of the nature of the table this frameset will always hold
 * exactly one frame. Therefore the terms cell, frameSet and frame
 * can be used to describe the same thing: one table-cell
 */
class KWTableFrameSet : public KWFrameSet
{
};

/**
 * @brief The object created to edit this table
 *
 * In fact at a given moment,
 * it edits one cell (frameset) of the table, the one in which the cursor is.
 */
class KWTableFrameSetEdit : public KWFrameSetEdit
{
};
#endif
