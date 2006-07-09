/* This file is part of the KDE project
   Copyright (C) 2006 Fredrik Edemar <f_edemar@linux.se>
             (C) 1999 Stephan Kulow <coolo@kde.org>

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

#ifndef KSPREAD_HANDLER_H
#define KSPREAD_HANDLER_H

#include <KoContainerHandler.h>

#include <QPoint>
//Added by qt3to4:
#include <QEvent>

#include <KoQueryTrader.h>

class QWidget;

enum HandlerType
{
  Part,
  Chart,
  Picture
};

namespace KSpread
{
class View;

/**
 * This event handler is used to insert a new part. The event handler
 * takes care of selecting the rectangle at which the new
 * part will appear.
 */
class InsertHandler : public KoEventHandler
{
    Q_OBJECT
public:
    InsertHandler( View* view, QWidget* widget );
    ~InsertHandler();

    virtual HandlerType getType() = 0;

protected:
    bool eventFilter( QObject*, QEvent* );
    virtual void insertObject( QRect ) = 0;

    View* m_view;
    QPoint m_geometryStart;
    QPoint m_geometryEnd;
    bool m_started;
    bool m_clicked;
};

class InsertPartHandler : public InsertHandler
{
  public:
    InsertPartHandler( View* view, QWidget* widget, const KoDocumentEntry& entry );
    virtual ~InsertPartHandler();
    HandlerType getType() { return Part; }

  private:
    void insertObject( QRect );

    KoDocumentEntry m_entry;
};


class InsertChartHandler : public InsertHandler
{
  public:
    InsertChartHandler( View* view, QWidget* widget, const KoDocumentEntry& entry );
    virtual ~InsertChartHandler();
    HandlerType getType() { return Chart; }

  private:
    void insertObject( QRect );

    KoDocumentEntry m_entry;
};

class InsertPictureHandler : public InsertHandler
{
  public:
    InsertPictureHandler( View* view, QWidget* widget, const KUrl &);
    virtual ~InsertPictureHandler();
    HandlerType getType() { return Picture; }

  private:
    void insertObject( QRect );

    KUrl m_file;
};

} // namespace KSpread

#endif
