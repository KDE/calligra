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

#ifndef __kontour_doc_h__
#define __kontour_doc_h__

#include <koDocument.h>
#include <koDocumentChild.h>

#include <kcommand.h>

class KontourDocument;
class KontourView;
class KoDocumentEntry;
class KoStore;
namespace Kontour {
class GDocument;
};
using namespace Kontour;
class QDomDocument;

class KontourChild : public KoDocumentChild
{
public:
  KontourChild(KontourDocument *);
  KontourChild(KontourDocument *kontour, KoDocument *doc, const QRect &geometry);
  ~KontourChild();

  virtual KoDocument *hitTest( const QPoint &, const QWMatrix & );
  KontourDocument *kontourParent() const {return (KontourDocument*)parent(); }
};

class KontourDocument : public KoDocument
{
  Q_OBJECT
public:
  KontourDocument(QWidget *parentWidget = 0, const char *widgetName = 0, QObject* parent = 0, const char* name = 0, bool singleViewMode = false );
  ~KontourDocument();

  GDocument *document() const {return mGDoc; }
  KCommandHistory *history() const {return mHistory; }

  void paintContent(QPainter &painter, const QRect &rect, bool transparent = false, double zoomX = 1.0, double zoomY = 1.0);

  bool initDoc();

  void addShell(KoMainWindow *shell);

  bool loadXML(QIODevice *, const QDomDocument &doc);
  bool loadChildren(KoStore *store);
  bool completeLoading(KoStore *store);

  QDomDocument saveXML();
  bool saveChildren(KoStore *store);
  bool completeSaving(KoStore *store);

  KoView *createViewInstance(QWidget *parent, const char *name);

  void insertChild(KoDocumentChild *child);

  void insertPart(const QRect &rect, KoDocumentEntry &e);
  void changeChildGeometry(KontourChild *child, const QRect &r);

//signals:
//    void partInserted (KontourChild* child, GPart* part);
//    void childGeometryChanged (KontourChild* child);

private slots:
  void modified(bool m);

private:
  GDocument* mGDoc;
  KCommandHistory *mHistory;
};

#endif
