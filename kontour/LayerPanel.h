/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001-2002 Igor Jansen (rm@kde.org)

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
#include <qdockwindow.h>

class QPushButton;
class QGridLayout;

namespace Kontour
{
class GDocument;
class GPage;
class GLayer;

class TreeItem : public QListViewItem
{
public:
  TreeItem(QListView *parent);
  TreeItem(QListViewItem *parent);

  virtual void select() = 0;
};

class PageTreeItem : public TreeItem
{
public:
  PageTreeItem(QListView *parent, GPage *aGPage);

  GPage *page() const {return mGPage; }

  void select();
  void paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align);
//  virtual QString key( int, bool ) const;
protected:
  void activate();

private:
  GPage *mGPage;
};

class LayerTreeItem : public TreeItem
{
public:
  LayerTreeItem(QListViewItem *parent, GLayer *aGLayer);

  GLayer *layer() const {return mGLayer; }

  void select();
  void paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align);
//  virtual QString key( int, bool ) const;
private:
  GLayer *mGLayer;
};

class LayerView : public QListView
{
  Q_OBJECT
public:
  LayerView(GDocument *aGDoc, QWidget *parent = 0, const char *name = 0);
  virtual ~LayerView();

  GDocument *document() const {return mGDoc; }

  void updateView();

private slots:
  void slotDoubleClicked(QListViewItem *item);  

private:
  GDocument *mGDoc;
};

class LayerPanel : public QDockWindow
{
  Q_OBJECT
public:
  LayerPanel(GDocument *aGDoc, QWidget *parent = 0L, const char *name = 0L);
  ~LayerPanel();

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
};
using namespace Kontour;

#endif
