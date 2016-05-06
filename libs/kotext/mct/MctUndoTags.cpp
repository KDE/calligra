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

#include "MctUndoTags.h"

const QString MctUndoTags::ADDED = "del";
const QString MctUndoTags::REMOVED = "add";
const QString MctUndoTags::MOVED = "move";
const QString MctUndoTags::STRING = "text";
const QString MctUndoTags::PARAGRAPH = "par";
const QString MctUndoTags::STYLE = "style";
const QString MctUndoTags::TEXTFRAME = "textframe";
const QString MctUndoTags::TEXTGRAPHICOBJECT = "graphobj";
const QString MctUndoTags::EMBEDDEDOBJECT = "embobj";
const QString MctUndoTags::ROWCHANGE = "row";
const QString MctUndoTags::COLCHANGE = "col";
const QString MctUndoTags::TABLE = "table";
const QString MctUndoTags::FORMATTAG = "format";
const QString MctUndoTags::PROPERTIESTAG = "properties";
const QString MctUndoTags::TEXTTAG = "text";
const QString MctUndoTags::PARAGRAPHTAG = "paragraph";
const QString MctUndoTags::LIST = "list";
const QString MctUndoTags::UNDEFINEDTAG = "undef";

MctUndoTags::MctUndoTags()
{

}

MctUndoTags::~MctUndoTags()
{

}

