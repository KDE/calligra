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

#ifndef GDocument_h_
#define GDocument_h_

#include <set>
#include <string>

#include <qobject.h>
#include <qlist.h>
#include <qfile.h>

#include "Handle.h"
#include "GObject.h"

#include "koPageLayoutDia.h"

#define XML_DOCTYPE "kiml"
#define XML_DTD     "kiml1.0.dtd"
#define UNNAMED_FILE "<unnamed>"

class GDocument : public QObject {
  Q_OBJECT
public:
  GDocument ();
  ~GDocument ();

  void initialize ();

  const QString& fileName () const { return filename; }

  void setPaperSize (int width, int height);
  int getPaperWidth () const;
  int getPaperHeight () const;

  void setModified (bool flag = true);
  bool isModified () const { return modifyFlag; }

  void insertObject (GObject* obj);
  void selectObject (GObject* obj);
  void unselectObject (GObject* obj);
  void unselectAllObjects ();
  void selectAllObjects ();
  
  GObject* lastObject () { return last; }
  void setLastObject (GObject* obj);
  
  QListIterator<GObject> getObjects () { 
    return QListIterator<GObject> (objects); }
  QListIterator<GObject> getSelection () { 
    return QListIterator<GObject> (selection); }
  
  bool selectionIsEmpty () const { return selection.isEmpty (); }
  unsigned int selectionCount () const { return selection.count (); }

  unsigned int objectCount () const { return objects.count (); }

  Rect boundingBoxForSelection ();
  Rect boundingBoxForAllObjects ();
  void deleteSelectedObjects ();
  void deleteObject (GObject* obj);

  GObject* findContainingObject (int x, int y);
  GObject* findNextObject (int x, int y, const char* type);

  bool findContainingObjects (int x, int y, QList<GObject>& olist);
  bool findObjectsContainedIn (const Rect& r, QList<GObject>& olist);
	
  bool saveToXml (const char* fname);
  bool readFromXml (const char* fname);
  
  Handle& handle () { return selHandle; }
  
  unsigned int findIndexOfObject (GObject *obj);
  void insertObjectAtIndex (GObject* obj, unsigned int idx);
  void moveObjectToIndex (GObject* obj, unsigned int idx);

  KoPageLayout pageLayout ();
  void setPageLayout (const KoPageLayout& layout);

  bool requiredFonts (set<string>& fonts);

protected:
  void updateHandle ();
  
public slots:
  void objectChanged ();
  
signals:
  void changed ();
  void selectionChanged ();
  void sizeChanged ();

private:
  bool modifyFlag;
  QString filename;
  int paperWidth, paperHeight; // pt
  QList<GObject> objects;
  QList<GObject> selection;
  GObject *last;
  Handle selHandle;
  Rect selBox;
  bool selBoxIsValid;
  KoPageLayout pLayout;
};

#endif 
