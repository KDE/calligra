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

#include "KIllustrator_doc.h"
#include "KIllustrator_doc.moc"
#include "KIllustrator_view.h"

#include "GPart.h"

#include <qmessagebox.h>

KIllustratorChild::KIllustratorChild (KIllustratorDocument* killu, 
				      const QRect& rect, 
				      KOffice::Document_ptr doc) {
  m_pKilluDoc = killu;
  m_rDoc = KOffice::Document::_duplicate (doc);
  m_geometry = rect;
}

KIllustratorChild::~KIllustratorChild () {
  m_rDoc = 0L;
}


KIllustratorDocument::KIllustratorDocument () {
  ADD_INTERFACE("IDL:OPParts/Print:1.0")
  cout << "create new KIllustratorDocument ..." << endl;
  m_lstViews.setAutoDelete (true);
  m_lstChildren.setAutoDelete (true);
  m_bEmpty = true;
}

KIllustratorDocument::~KIllustratorDocument () {
  cleanUp ();
}

bool KIllustratorDocument::save (ostream& os, const char* fmt) {
  cout << "save KIllu to stream !!!!!!!!!!!!!!!" << endl;
  return GDocument::saveToXml (os);
}

void KIllustratorDocument::cleanUp () {
  if (m_bIsClean)
    return;
  
  m_lstChildren.clear ();
  
  KoDocument::cleanUp ();
}


bool KIllustratorDocument::load (istream& in, KOStore::Store_ptr store) {
  cout << "load KIllu from stream !!!!!!!!!" << endl;
  return GDocument::readFromXml (in);
}

bool KIllustratorDocument::loadChildren (KOStore::Store_ptr store) {
  return true;
}

bool KIllustratorDocument::hasToWriteMultipart () {
  return false;
}

void KIllustratorDocument::insertPart (const QRect& rect, 
				       KoDocumentEntry& e) {
  KOffice::Document_var doc = imr_createDoc (e);
  if (CORBA::is_nil (doc))
    return;

  if (! doc->init ()) {
    QMessageBox::critical ((QWidget *) 0L, i18n ("KIllustrator Error"), 
			   i18n ("Could not insert document"), i18n ("OK"));
    return;
  }

  KIllustratorChild *child =
    new KIllustratorChild (this, rect, doc);
  insertChild (child);

  GPart* part = new GPart (child);
  GDocument::insertObject (part);
}


void KIllustratorDocument::insertChild (KIllustratorChild* child) {
  m_lstChildren.append (child);
  setModified (true);
}

void KIllustratorDocument::changeChildGeometry (KIllustratorChild* child, 
						const QRect& r) {
  child->setGeometry (r);
  setModified (true);
  emit childGeometryChanged (child);
}

CORBA::Boolean KIllustratorDocument::init () {
  return true;
}

KIllustratorView* KIllustratorDocument::createKIllustratorView () {
  KIllustratorView *view = new KIllustratorView (0L, 0L, this);
  view->QWidget::show ();
  m_lstViews.append (view);
  return view;
}

OpenParts::View_ptr KIllustratorDocument::createView () {
  return OpenParts::View::_duplicate (createKIllustratorView ());
}

void KIllustratorDocument::removeView (KIllustratorView* view) {
  m_lstViews.setAutoDelete (false);
  m_lstViews.removeRef (view);
  m_lstViews.setAutoDelete (true);
}

int KIllustratorDocument::viewCount () {
  return m_lstViews.count ();
}

void KIllustratorDocument::viewList (OpenParts::Document::ViewList*& list_ptr) {
  list_ptr->length (m_lstViews.count ());
  int i = 0;
  QListIterator<KIllustratorView> it (m_lstViews);
  for (; it.current (); ++it)
    (*list_ptr)[i++] = OpenParts::View::_duplicate (it.current ());
}

CORBA::Boolean KIllustratorDocument::isModified () {
  return GDocument::isModified ();
}

void KIllustratorDocument::setModified (bool f) {
  GDocument::setModified (f);
  if (f)
    m_bEmpty = false;
}

void KIllustratorDocument::draw (QPaintDevice* dev, 
				 CORBA::Long w, CORBA::Long h) {
  Painter painter;
  painter.begin (dev);
  GDocument::drawContents (painter);
  painter.end ();
}


