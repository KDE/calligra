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

#include <koDocument.h>
#include <koDocumentChild.h>

#include <qcstring.h>

class KIllustratorDocument;
class KIllustratorView;
class KoDocumentEntry;
class KoStore;
class GDocument;
class GPart;

class KIllustratorChild : public KoDocumentChild
{
public:
    KIllustratorChild( KIllustratorDocument* );
    KIllustratorChild( KIllustratorDocument* killu, KoDocument* doc, const QRect& geometry );
    ~KIllustratorChild();

    KIllustratorDocument* killuParent () { return (KIllustratorDocument*)parent(); }
};

class KIllustratorDocument : public KoDocument
{
    Q_OBJECT
public:
    KIllustratorDocument( QObject* parent = 0, const char* name = 0, bool singleViewMode = false );
    ~KIllustratorDocument ();

    // Overloaded methods from KoDocument

    virtual bool saveChildren (KoStore* _store, const char *_path);
    bool save (std::ostream& os, const char *fmt);
    bool completeSaving (KoStore* store);

    bool load (std::istream& is, KoStore* store);
    bool loadChildren (KoStore* store);

    /**
     * Overloaded @ref Part::createView
     */
    KoView* createView( QWidget* parent, const char* name );

    /**
     * Overloaded @ref Part::createShell
     */
    KoMainWindow* createShell();

    /**
     * Overloaded @ref KoDocument::initDoc.
     */
    bool initDoc ();

    /**
     * Overloaded @ref KoDocument::mimeType.
     */
    QCString mimeType() const { return "application/x-killustrator"; }

    /**
     * Overloaded @ref ContainerPart::insertChild.
     */
    void insertChild( KoDocumentChild* child );
	
    /**
     * Overloaded @ref Part::paintContent
     */
    void paintContent( QPainter& painter, const QRect& rect, bool transparent );
	
    // Killustrator stuff
    GDocument* gdoc();

    void insertPart (const QRect& rect, KoDocumentEntry& e);
    void changeChildGeometry (KIllustratorChild* child, const QRect& r);

signals:
    void partInserted (KIllustratorChild* child, GPart* part);
    void childGeometryChanged (KIllustratorChild* child);

private:
    GDocument* m_gdocument;
};

#endif
