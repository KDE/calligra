/* This file is part of the KDE project
  Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
  Copyright (C) 2002 - 2007 Dag Andersen <danders@get2net.dk>

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

#ifndef KPLATOWORK_VIEW
#define KPLATOWORK_VIEW

#include <KoView.h>
#include <KoQueryTrader.h>

#include <QMenu>
#include <QDockWidget>
#include <QTreeWidget>


class QProgressBar;
class QStackedWidget;
class QSplitter;

class KPrinter;
class KAction;
class KToggleAction;
class QLabel;

/// The main namespace for KPlato WorkPackage Handler
namespace KPlatoWork
{

class Part;

class View : public KoView
{
    Q_OBJECT
public:
    explicit View( Part* part, QWidget* parent = 0 );
    ~View();
    /**
     * Support zooming.
     */
    virtual void setZoom( double zoom ) {}

    virtual void updateReadWrite( bool ) {}
    
    Part *part() const;

    QMenu *popupMenu( const QString& name ) {}
    
public slots:
    void slotPopupMenu( const QString& menuname, const QPoint &pos );


private:
    
    // ------ Edit
    QAction *actionCut;
    QAction *actionCopy;
    QAction *actionPaste;

};

} //KplatoWork namespace

#endif
