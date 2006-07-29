/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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

#ifndef __main_h__
#define __main_h__

#include <qobject.h>
#include <kdatatool.h>
#include <klibloader.h>

class SpellChecker : public KDataTool
{
    Q_OBJECT

public:
    SpellChecker( QObject* parent, const char* name, const QStringList & );
    virtual bool run( const QString& command, void* data, const QString& datatype, const QString& mimetype);
};

#endif
