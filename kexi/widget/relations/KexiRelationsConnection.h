/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004-2007 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KexiRelationsConnection_H
#define KexiRelationsConnection_H

#include <QString>
#include <kexi_export.h>

class QPainter;
class KexiRelationsTableContainer;
class KexiRelationsScrollArea;
struct SourceConnection;

//! A model for a single connection (relationship) between tables in the relations view
class KEXIRELATIONSVIEW_EXPORT KexiRelationsConnection
{
  public:
    KexiRelationsConnection(
      KexiRelationsTableContainer *masterTbl, 
      KexiRelationsTableContainer *detailsTbl, 
      SourceConnection &s,
      KexiRelationsScrollArea *scrollArea);

    ~KexiRelationsConnection();

    void drawConnection(QPainter *p);

    bool selected() const;
    
    void setSelected(bool s);

    QRect connectionRect();

    QRect oldRect() const;

    KexiRelationsTableContainer *masterTable() const;

    KexiRelationsTableContainer *detailsTable() const;

    QString masterField() const;

    QString detailsField() const;

    bool matchesPoint(const QPoint &p, int tolerance = 3);

    QString toString() const;

  private:
    class Private;
    Private* const d;
};

#endif
