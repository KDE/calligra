/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001 Igor Janssen (rm@linux.ru.net)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef __LayerPanel_h__
#define __LayerPanel_h__

#include <qlistview.h>
#include <qwidget.h>

class GDocument;
class GPage;
class GLayer;
class QPushButton;
class QGridLayout;

class PageTreeItem : public QListViewItem
{
public:
  PageTreeItem(QListView *parent, GPage *p);
  virtual ~PageTreeItem();

  void paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align);
//  virtual QString key( int, bool ) const;
private:
  GPage *page;
};

class LayerTreeItem : public QListViewItem
{
public:
  LayerTreeItem(QListViewItem *parent, GLayer *l);
  virtual ~LayerTreeItem();

  void paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align);
//  virtual QString key( int, bool ) const;
private:
  GLayer *layer;
};

class LayerView : public QListView
{
  Q_OBJECT
public:
  LayerView(GDocument *aGDoc, QWidget *parent = 0, const char *name = 0);
  virtual ~LayerView();
  void updateView();
private:
  GDocument *mGDoc;
};

class LayerPanel : public QWidget
{
  Q_OBJECT
public:
  LayerPanel(GDocument *aGDoc, QWidget *parent = 0L, const char *name = 0L);
  void stateOfButton();

public slots:
  void updatePanel();

private slots:
  void upPressed();
  void downPressed();
  void newPressed();
  void deletePressed();
  void slotLayerChanged();
private:
  GDocument *mGDoc;
  QGridLayout *mGrid;
  LayerView *mLayerView;
  QPushButton *mRaiseButton;
  QPushButton *mLowerButton;
  QPushButton *mNewButton;
  QPushButton *mDeleteButton;
};

#endif
