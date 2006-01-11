/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef KIVIOLAYERPANEL_H
#define KIVIOLAYERPANEL_H

class KAction;

class KivioView;
class KivioLayer;

#include <qlistview.h>

#include "kivio_layer_panel_base.h"

class KivioLayerItem: public QListViewItem
{
  friend class KivioLayerPanel;
  public:
    KivioLayerItem(QListView* parent, KivioLayer*, int id);
    ~KivioLayerItem();

    KivioLayer* layer() const { return data; }

  protected:
    void update();

  private:
    KivioLayer* data;
};


/*********************************************************/
class KivioLayerPanel : public KivioLayerPanelBase
{
  Q_OBJECT
  public:
    KivioLayerPanel(KivioView* view, QWidget* parent=0, const char* name=0);
    ~KivioLayerPanel();

    bool eventFilter(QObject*, QEvent*);

  public slots:
    void reset();

  protected slots:
    void addItem();
    void removeItem();
    void renameItem();
    void upItem();
    void downItem();

    void itemClicked(QListViewItem*, const QPoint&, int);
    void itemActivated(QListViewItem*);
    void updateButtons(QListViewItem*);

  private:
    KivioView* m_pView;

    KAction* actNew;
    KAction* actDel;
    KAction* actRename;
    KAction* actUp;
    KAction* actDown;

    int id;
};

#endif
