/* This file is part of the KDE project
   Copyright (C) 2009 Dag Andersen <danders@get2net.dk>

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

#ifndef ICALENDAREXPORT_H
#define ICALENDAREXPORT_H


#include <KoFilter.h>


#include <kcal/calendarlocal.h>

#include <QObject>

class QFile;
class QByteArray;
class QStringList;

namespace KCal
{
class Todo;
}
namespace KPlato
{
class Project;
class Node;
}

class ICalendarExport : public KoFilter
{

    Q_OBJECT

public:
    ICalendarExport(QObject* parent, const QStringList &);
    virtual ~ICalendarExport() {}

    virtual KoFilter::ConversionStatus convert(const QByteArray& from, const QByteArray& to);

protected:
    KoFilter::ConversionStatus convert(const KPlato::Project &project, QFile &file);
    void createTodos(KCal::CalendarLocal &cal, const KPlato::Node *node, long id, KCal::Todo *parent = 0);
};

#endif // ICALENDAREXPORT_H
