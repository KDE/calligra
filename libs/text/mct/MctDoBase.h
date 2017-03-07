/* This file is part of the KDE project
 * Copyright (C) 2015-2016 MultiRacio Ltd. <multiracio@multiracio.com> (S.Schliszka, F.Novak, P.Rakyta)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef MCTDOBASE_H
#define MCTDOBASE_H

#include "MctChange.h"
#include "MctStaticData.h"

class KoTextDocument;

/**
 * Base class of Undo and Redo classes
 *
 * This class is responsible for the connection with Calligra via containing the KoDocument pointer.
 */
class MctDoBase
{
public:
    /// constructor
    MctDoBase();
    virtual ~MctDoBase();

    /**
     * create textcursor from a change node and a position
     *
     * Wrapper function which calls static method to get cursor
     * at the given position, in the current document (m_doc).
     * @param change @todo remove this param
     * @param pos where to set the new cursor
     * @return new cursor
     */
    QTextCursor *createcursor(MctChange *change, MctPosition *pos);

protected:

    KoTextDocument *m_doc;    ///< pointer access to the current document which is handled by Calligra
};

#endif // MCTDOBASE_H
