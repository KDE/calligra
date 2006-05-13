/* This file is part of the KDE project
   Copyright (C) 1999 Stephan Kulow <coolo@kde.org>

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

#ifndef KSPREAD_EVENTS
#define KSPREAD_EVENTS

#include <QEvent>
#include <QRect>
#include <QString>

#include <string.h>

#include <kparts/event.h>
#include <koffice_export.h>

#include "region.h"
namespace KSpread
{

class KSPREAD_EXPORT SelectionChanged : public KParts::Event
{
public:
    SelectionChanged( const Region&, const QString& sheet );
    ~SelectionChanged();

    Region region() const { return m_region; }
    QString sheet() const { return m_sheet; }

    static bool test( const QEvent* e ) { return KParts::Event::test( e, s_strSelectionChanged ); }

private:
    static const char *s_strSelectionChanged;
    Region m_region;
    QString m_sheet;
};

} // namespace KSpread

#endif
