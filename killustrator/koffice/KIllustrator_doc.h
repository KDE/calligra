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

#ifndef KIllustrator_doc_h_
#define KIllustrator_doc_h_

#include <koFrame.h>
#include <koDocument.h>
#include <koView.h>
#include <koPrintExt.h>
#include <koStore.h>
#include <koIMR.h>

#include "KIllustrator.h"
#include "GDocument.h"
#include "GPart.h"

#define MIME_TYPE "application/x-killustrator"
#define EDITOR "IDL:KIllustrator/Document:1.0"

class KIllustratorDocument;
class KIllustratorView;

class KIllustratorChild : public KoDocumentChild {
public:
  KIllustratorChild () {}
  KIllustratorChild (KIllustratorDocument* killu, const QRect& rect, 
		     KOffice::Document_ptr doc);
  ~KIllustratorChild ();

  KIllustratorDocument* parent () { return m_pKilluDoc; }

  void setURL (const char* url);
  void setMimeType (const char *mime);

  const char* urlForSave ();

protected:
  KIllustratorDocument *m_pKilluDoc; 
};

class KIllustratorDocument : public GDocument,
			virtual public KoDocument, 
			virtual public KoPrintExt, 
			virtual public KIllustrator::Document_skel {
  Q_OBJECT
public:
  KIllustratorDocument ();
  ~KIllustratorDocument ();

  // --- C++ ---
  // Overloaded methods from KoDocument

  virtual void makeChildListIntern (KOffice::Document_ptr _root, 
				    const char *_path);
  bool save (ostream& os, const char *fmt);
  bool load (istream& is, KOStore::Store_ptr store);
  bool loadChildren (KOStore::Store_ptr store);
  bool hasToWriteMultipart ();

  void cleanUp ();

  void insertPart (const QRect& rect, KoDocumentEntry& e);
  void changeChildGeometry (KIllustratorChild* child, const QRect& r);

  // --- IDL ---
  virtual CORBA::Boolean initDoc ();

  virtual KOffice::MainWindow_ptr createMainWindow();

  // create a view
  virtual OpenParts::View_ptr createView ();

  // get list of views
  virtual void viewList (OpenParts::Document::ViewList*& list_ptr);

  void removeView (KIllustratorView* view);

  // get mime type
  virtual char* mimeType () { return CORBA::string_dup (MIME_TYPE); }

  // ask, if document is modified
  virtual CORBA::Boolean isModified ();
  virtual void setModified (bool f);

  virtual void draw (QPaintDevice* dev, CORBA::Long w, CORBA::Long h,
		     CORBA::Float _scale );

  virtual int viewCount ();
  virtual KIllustratorView* createKIllustratorView ( QWidget* _parent = 0 );

  virtual bool isEmpty () { return m_bEmpty; }

  virtual CORBA::Boolean checkForSelection ();
  virtual KIllustrator::GfxObjectSeq* getSelection ();
  virtual void addToSelection (KIllustrator::GfxObject_ptr obj);
  virtual void removeFromSelection (KIllustrator::GfxObject_ptr obj);
  virtual void groupSelection ();

signals:
  void partInserted (KIllustratorChild* child, GPart* part);
  void childGeometryChanged (KIllustratorChild* child);
 
protected:
  void insertChild (KIllustratorChild* child);

  bool m_bEmpty;
  QList<KIllustratorView> m_lstViews;
  QList<KIllustratorChild> m_lstChildren;
  KIllustratorView *viewPtr;
};

#endif
