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

#ifndef MCTUNDOTAGS_H
#define MCTUNDOTAGS_H

#include <QString>

class MctUndoTags
{    
public:

    static const QString ADDED;
    static const QString REMOVED;
    static const QString MOVED;
    static const QString STRING;
    static const QString PARAGRAPH;
    static const QString STYLE;
    static const QString TEXTFRAME;
    static const QString TEXTGRAPHICOBJECT;
    static const QString EMBEDDEDOBJECT;
    static const QString ROWCHANGE;
    static const QString COLCHANGE;
    static const QString TABLE;
    static const QString FORMATTAG;
    static const QString PROPERTIESTAG;
    static const QString TEXTTAG;
    static const QString PARAGRAPHTAG;
    static const QString LIST;
    static const QString UNDEFINEDTAG;

    MctUndoTags();
    ~MctUndoTags();
};

#endif // MCTUNDOTAGS_H
