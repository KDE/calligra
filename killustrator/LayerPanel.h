/* -*- C++ -*-


  This file is part of KIllustrator.
  Copyright (C) 2000 Igor Janssen (rm@linux.ru.net)

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

#ifndef LayerPanel_h_
#define LayerPanel_h_

#include <qwidget.h>

class GDocument;
class LayerView;
class QPushButton;
class QGridLayout;

class LayerPanel : public QWidget {
  Q_OBJECT
public:
  LayerPanel (QWidget* parent = 0L, const char* name = 0L);

  void manageDocument (GDocument* doc);
    void stateOfButton();
    void upDownButton();
private slots:
  void upPressed ();
  void downPressed ();
  void newPressed ();
  void deletePressed ();

private:
  GDocument *document;
  QGridLayout *mGrid;
  LayerView *layerView;
  QPushButton *btn_rl;
  QPushButton *btn_ll;
  QPushButton *btn_nl;
  QPushButton *btn_dl;
};

#endif
