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

#ifndef GLayer_h_
#define GLayer_h_

#include <qobject.h>
#include <qlist.h>

class GPage;
class GObject;
class QString;

class GLayer : public QObject {
  Q_OBJECT
public:
  GLayer (GPage* doc, const QString &text=QString::null);
  ~GLayer ();

  /*
   * Layer properties
   */
  QString name () const;
  void setName(const QString &text);

  bool isVisible () const { return visibleFlag; }
  bool isPrintable () const { return printableFlag; }
  bool isEditable () const { return editableFlag; }
  bool isInternal () const { return internalFlag; }

  void setVisible (bool flag);
  void setPrintable (bool flag);
  void setEditable (bool flag);
  void setInternal ();

  /*
   * Content management
   */
  const QList<GObject> &objects () { return contents; }
  unsigned int objectCount () const { return contents.count(); }
  void insertObject (GObject* obj);
  void deleteObject (GObject* obj);
  GObject* findContainingObject (int x, int y);

  int findIndexOfObject (GObject *obj);
  void insertObjectAtIndex (GObject* obj, unsigned int idx);
  void moveObjectToIndex (GObject* obj, unsigned int idx);
  GObject *objectAtIndex (unsigned int idx);

signals:
  void propertyChanged ();
  void contentChanged ();

private:
  QString ident;    // layer identifier
  bool visibleFlag, // layer is visible
    printableFlag,  // layer is printable
    editableFlag,   // layer is editable
    wasEditable,    // layer was editable before the change to unvisible
    internalFlag;   // layer used for helplines

  QList<GObject> contents; // the list of objects
  GPage* document;

  static int lastID;
};

#endif
