/*
   Copyright (c) 2017 Dag Andersen <danders@get2net.dk>*
   Copyright (c) 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef TEXTSHAPE_DEBUG_H
#define TEXTSHAPE_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>


extern const QLoggingCategory &TEXTSHAPE_LOG();

#define debugTextShape qCDebug(TEXTSHAPE_LOG)<<Q_FUNC_INFO
#define warnTextShape qCWarning(TEXTSHAPE_LOG)
#define errorTextShape qCCritical(TEXTSHAPE_LOG)


#endif
