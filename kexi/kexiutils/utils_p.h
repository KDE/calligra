/* This file is part of the KDE project
   Copyright (C) 2004,2006 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIUTILS_P_H
#define KEXIUTILS_P_H

#include <qtimer.h>
#include <qapplication.h>
#include <qdialog.h>

/*! @internal */
class DelayedCursorHandler : public QObject
{
  Q_OBJECT
  public:
    DelayedCursorHandler();
    void start(bool noDelay);
    void stop();
    bool startedOrActive : 1; //! true if ounting started or the cursor is active
  protected slots:
    void show();
  protected:
    QTimer timer;
};

/*! @internal */
class DebugWindowDialog : public QDialog
{
  public:
    explicit DebugWindowDialog( QWidget * parent )
     : QDialog(parent, Qt::Dialog|Qt::WindowMinMaxButtonsHint|Qt::WindowStaysOnTopHint)
    {
      setWindowState( Qt::WindowMinimized );
    }
};

#endif

