/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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

#ifndef KFORMDESIGNER_MAINWINDOW_H
#define KFORMDESIGNER_MAINWINDOW_H

#include <kparts/mainwindow.h>

class KFDMainWindow : public KParts::MainWindow
{
  Q_OBJECT

  public:
    KFDMainWindow();
    ~KFDMainWindow() {;}

    /** @todo change it to bool */
    void loadUIFile(const QString &filename);
    /** @todo change it to bool */
    void loadUIFile(const KUrl &url);
    virtual bool queryClose();

  private:
    void setupActions();

  private:
    KParts::ReadWritePart  *m_part;
};

#endif
