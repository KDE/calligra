/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001-2002 Igor Janssen (rm@kde.org)

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

#ifndef __GLayer_h__
#define __GLayer_h__

#include <qobject.h>
#include <qptrlist.h>

class QDomDocument;
class QDomElement;
class QString;

namespace Kontour
{
class GPage;
class GObject;

class GLayer : public QObject
{
  Q_OBJECT
public:
  GLayer(GPage *aPage);
  ~GLayer();

  GPage *page() const {return mPage; }

  /**
   * Layer properties.
   */
  QString name() const {return mName; }
  void name(const QString &aName);

  bool isVisible() const {return visibleFlag; }
  void setVisible(bool flag);
  
  bool isPrintable() const {return printableFlag; }
  void setPrintable(bool flag);
  
  bool isEditable() const {return editableFlag; }
  void setEditable(bool flag);

  QDomElement saveToXml(QDomDocument &document);
  bool readFromXml(const QDomElement &layer);

  /**
   * Content management
   */
  
  /**
   * Read only objects manipulation.
   */
  const QPtrList<GObject> &objects() const {return contents; }
  
  /**
   * 
   */
  unsigned int objectCount() const {return contents.count(); }
  
  /**
   * 
   */
  void insertObject(GObject *obj);
  
  /**
   * 
   */
  void deleteObject(GObject *obj);
  
  
  /**
   * 
   */
  GObject *findContainingObject(double x, double y);
  
  int findIndexOfObject(GObject *obj);
  void insertObjectAtIndex(GObject *obj, unsigned int idx);
  void moveObjectToIndex(GObject *obj, unsigned int idx);
  GObject *objectAtIndex(unsigned int idx);

signals:
  void propertyChanged();
//  void contentChanged();

private:
  QString mName;               // layer identifier
  
  bool visibleFlag:1;          // layer is visible
  bool printableFlag:1;        // layer is printable
  bool editableFlag:1;         // layer is editable
  bool wasEditable:1;          // layer was editable before the change to unvisible

  QPtrList<GObject> contents;  // the list of objects
  GPage *mPage;
};
};

using namespace Kontour;

#endif
