/* This file is part of the KDE project

   Copyright (C) 2012-2014 Inge Wallin            <inge@lysator.liu.se>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef ODFREADERINTERNALS_H
#define ODFREADERINTERNALS_H

#define DECLARE_READER_FUNCTION(name) \
    void readElement##name(KoXmlStreamReader &reader)


// Backend functions

#define DECLARE_BACKEND_FUNCTION(name)                               \
    virtual void element##name(KoXmlStreamReader &reader, OdfReaderContext *context)

#define IMPLEMENT_BACKEND_FUNCTION(readername, name)                 \
void readername##Backend::element##name(KoXmlStreamReader &reader,   \
				OdfReaderContext *context)	     \
{                                                                    \
    Q_UNUSED(reader);                                                \
    Q_UNUSED(context);						     \
}

#endif // ODFREADERINTERNALS_H
