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

KIllustratorChild::KIllustratorChild (KIllustratorDocument* killu, 
				      const QRect& rect, 
				      OPParts::Document_ptr doc) {
  m_pKilluDoc = killu;
  m_rDoc = OPParts::Document::_duplicate (doc);
  m_geometry = rect;
}

KIllustratorChild::~KIllustratorChild () {
  m_rDoc = 0L;
}


KIllustratorDocument::KIllustratorDocument () {
  m_lstViews.setAutoDelete (true);
  m_lstChildren.setAutoDelete (true);
}

KIllustratorDocument::~KIllustratorDocument () {
  cleanUp ();
}

bool KIllustratorDocument::save (ostream& os) {
  return false;
}

void KIllustratorDocument::cleanUp () {
  if (m_bIsClean)
    return;
  
  m_lstChildren.clear ();
  
  Document_impl::cleanUp ();
}

bool KIllustratorDocument::loadChildren (OPParts::MimeMultipartDict_ptr dict) {
  return false;
}

bool KIllustratorDocument::hasToWriteMultipart () {
  return true;
}

CORBA::Boolean KIllustratorDocument::init () {
  return true;
}

OPParts::View_ptr KIllustratorDocument::createView () {
  cout << "KIllustratorDocument::createView ()" << endl;
  KIllustratorView *view = new KIllustratorView (0L, 0L, this);
  view->setGeometry (5000, 5000, 100, 100);
  view->QWidget::show ();
  view->setDocument (this);
  m_lstViews.append (view);
  return OPParts::View::_duplicate (view);
}

void KIllustratorDocument::removeView (KIllustratorView* view) {
  m_lstViews.removeRef (view);
}

void KIllustratorDocument::viewList (OPParts::Document::ViewList*& list_ptr) {
  list_ptr->length (m_lstViews.count ());
  int i = 0;
  QListIterator<KIllustratorView> it (m_lstViews);
  for (; it.current (); ++it)
    (*list_ptr)[i++] = OPParts::View::_duplicate (it.current ());
}

CORBA::Boolean KIllustratorDocument::isModified () {
  return false;
}

