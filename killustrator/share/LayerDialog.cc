/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

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

#include <qlistview.h>

#include <iostream.h>

#include "LayerDialog.h"
#include "LayerDialog.moc"

#include <klocale.h>
#include <kapp.h>
#include <kbuttonbox.h>
#include <kseparator.h>

#include <qpushbt.h>
#include <qlayout.h>
#include <qheader.h>

#include "GLayer.h"

/*
class LayerItem : public QListBoxItem {
public:
  LayerItem (GLayer* l);

protected:
  virtual void paint (QPainter *);
  virtual int height (const QListBox *) const;
  virtual int width (const QListBox *) const;

private:
  GLayer* layer;
};

LayerItem::LayerItem (GLayer *l) : QListBoxItem (), layer (l) {
  setText (l->name ());
}

void LayerItem::paint (QPainter *p) {
  QFontMetrics fm = p->fontMetrics ();
  int y;
  if (15 < fm.height ())
    y = fm.ascent () + fm.leading () / 2;
  else
    y = 15 / 2 - fm.height () / 2 + fm.ascent ();
  p->drawText (4 * 25 + 5, y, text ());
}

int LayerItem::width (const QListBox *box) const {
  return box->fontMetrics ().width (text ()) + 6 + 4 * 25;
}

int LayerItem::height (const QListBox *box) const {
  return QMAX (15, box->fontMetrics ().lineSpacing () + 1);
}

*/
LayerDialog::LayerDialog (QWidget* parent, const char* name) : 
    QDialog (parent, name, false) {
  QPushButton* button;

  document = 0L;
  setCaption (i18n ("Layers"));

  QVBoxLayout *vl = new QVBoxLayout (this, 10);

  listView = new QListView (this);
  listView->setAllColumnsShowFocus (true);
  listView->addColumn ("v", 15);
  listView->addColumn ("p", 15);
  listView->addColumn ("e", 15);
  listView->addColumn ("a", 15);
  listView->addColumn ("Name", 100);
  QHeader* header = listView->header ();
  header->setFixedHeight (1);
  header->hide ();
  vl->addWidget (listView, 1);

  KButtonBox *bbox = new KButtonBox (this);
  button = bbox->addButton ("Up");
  connect (button, SIGNAL (clicked ()), SLOT (upPressed ()));
  button = bbox->addButton ("Down");
  connect (button, SIGNAL (clicked ()), SLOT (downPressed ()));
  button = bbox->addButton ("New");
  connect (button, SIGNAL (clicked ()), SLOT (newPressed ()));
  button = bbox->addButton ("Delete");
  connect (button, SIGNAL (clicked ()), SLOT (deletePressed ()));

//  bbox->setMaximumHeight (bbox->sizeHint ().height ());
  vl->addWidget (bbox);

  // a separator
  KSeparator* sep = new KSeparator (this);
  sep->setMaximumHeight (5);
  vl->addWidget (sep);

  // the standard buttons
  bbox = new KButtonBox (this);
  bbox->addStretch (1);
  button = bbox->addButton (i18n ("Close"));
  connect (button, SIGNAL (clicked ()), SLOT (closePressed ()));
  button = bbox->addButton (i18n ("Help"));
  connect (button, SIGNAL (clicked ()), SLOT (helpPressed ()));
//  bbox->setMaximumHeight (bbox->sizeHint ().height ());
  vl->addWidget (bbox);

  vl->activate ();
  adjustSize ();
 }

void LayerDialog::manageDocument (GDocument* doc) {
  document = doc;
  QCheckListItem* item = new QCheckListItem (listView, "Layer #1");
  item->setText (0, "1");
  item->setText (1, "2");
  item->setText (2, "3");
  item->setText (3, "4");
  item->setText (4, "Layer #1");
  item = new QCheckListItem (listView, "Layer #2");
  item->setText (0, "1");
  item->setText (1, "2");
  item->setText (2, "3");
  item->setText (3, "4");
  item->setText (4, "Layer #2");
}

void LayerDialog::upPressed () {
}

void LayerDialog::downPressed () {
}

void LayerDialog::newPressed () {
}

void LayerDialog::deletePressed () {
}

void LayerDialog::closePressed () {
}

void LayerDialog::helpPressed () {
}
