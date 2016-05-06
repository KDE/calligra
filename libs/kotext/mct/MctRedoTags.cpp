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

#include "MctRedoTags.h"

const QString MctRedoTags::ADDED = "add";
const QString MctRedoTags::REMOVED = "del";
const QString MctRedoTags::MOVED = "move";
const QString MctRedoTags::STRING = "text";
const QString MctRedoTags::PARAGRAPH = "par";
const QString MctRedoTags::STYLE = "style";
const QString MctRedoTags::TEXTFRAME = "textframe";
const QString MctRedoTags::TEXTGRAPHICOBJECT = "graphobj";
const QString MctRedoTags::EMBEDDEDOBJECT = "embobj";
const QString MctRedoTags::ROWCHANGE = "row";
const QString MctRedoTags::COLCHANGE = "col";
const QString MctRedoTags::TABLE = "table";
const QString MctRedoTags::FORMATTAG = "format";
const QString MctRedoTags::PROPERTIESTAG = "properties";
const QString MctRedoTags::TEXTTAG = "text";
const QString MctRedoTags::PARAGRAPHTAG = "paragraph";
const QString MctRedoTags::LIST = "list";
const QString MctRedoTags::UNDEFINEDTAG = "undef";

MctRedoTags::MctRedoTags()
{

}

MctRedoTags::~MctRedoTags()
{

}

