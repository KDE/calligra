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

#include <part_frame_impl.h>
#include <document_impl.h>
#include <koDocument.h>

#include "KIllustrator.h"
#include "GDocument.h"

#define MIME_TYPE "application/x-killustrator"
#define EDITOR "IDL:KIllustrator/Document:1.0"

class KIllustratorDocument;
class KIllustratorView;

class KIllustratorChild : public KoDocumentChild {
public:
  KIllustratorChild (KIllustratorDocument* killu, const QRect& rect, 
		     OPParts::Document_ptr doc);
  ~KIllustratorChild ();

  KIllustratorDocument* parent () { return m_pKilluDoc; }
protected:
  KIllustratorDocument *m_pKilluDoc; 
};

class KIllustratorDocument : public GDocument,
			virtual public KoDocument, 
			virtual public KIllustrator::Document_skel {
  Q_OBJECT
public:
  KIllustratorDocument ();
  ~KIllustratorDocument ();

  // --- C++ ---
  // Overloaded methods from KoDocument

  bool save (ostream& os);
  bool load (istream &, bool _randomaccess = false);
  bool loadChildren (OPParts::MimeMultipartDict_ptr dict);
  bool hasToWriteMultipart ();

  void cleanUp ();

  // --- IDL ---
  // Overloaded methods from OPParts::Document
  //
  virtual CORBA::Boolean init ();

  // create a view
  virtual OPParts::View_ptr createView ();

  // get list of views
  virtual void viewList (OPParts::Document::ViewList*& list_ptr);

  void removeView (KIllustratorView* view);

  // get mime type
  virtual char* mimeType () { return CORBA::string_dup (MIME_TYPE); }

  // ask, if document is modified
  virtual CORBA::Boolean isModified ();

protected:
  QList<KIllustratorView> m_lstViews;
  QList<KIllustratorChild> m_lstChildren;
  KIllustratorView *viewPtr;
};

#endif
