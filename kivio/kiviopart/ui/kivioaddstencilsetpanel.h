/* This file is part of the KDE project
   Copyright (C) 2004 Peter Simonsson <psn@linux.se>,

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
#ifndef KIVIOKIVIOADDSTENCILSETPANEL_H
#define KIVIOKIVIOADDSTENCILSETPANEL_H

#include "kiviostencilsetwidget.h"

#include <qpixmap.h>

class QListViewItem;

namespace Kivio {
  class AddStencilSetPanel : public KivioStencilSetWidget
  {
    Q_OBJECT
    public:
      AddStencilSetPanel(QWidget *parent = 0, const char *name = 0);
      ~AddStencilSetPanel();
    
    public slots:
      void updateList();
      
    signals:
      void addStencilSet(const QString&);
      
    protected slots:
      void changeStencilSet(QListViewItem* li);
      void addToDocument();
    
    protected:
      void loadCollections(const QString& dir);
      void loadStencilSet(QListViewItem* li, const QString& dir);
      QPixmap loadIcon(const QString& name, const QString& dir);
    
    private:
      QString m_currentDir;
  };
}

#endif
