/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef VDOCUMENT_H
#define VDOCUMENT_H

#include <qstring.h>
#include <qptrlist.h>

#include "vcolor.h"
#include "vlayer.h"
#include "vobject.h"
#include "vstroke.h"
#include "vfill.h"

class QDomDocument;
class QDomElement;
class VSelection;


typedef QPtrList<VLayer> VLayerList;
typedef QPtrListIterator<VLayer> VLayerListIterator;


/**
 * All non-visual, static doc info is in here.
 * The karbon part uses this class.
 * Filters can use this class as well instead of
 * the visually oriented karbon part.
 */

class VDocument : public VObject
{
public:
	VDocument();
	VDocument( const VDocument& document );

	virtual ~VDocument();

	virtual void draw( VPainter* painter, const KoRect* rect ) const;

	// TODO
	virtual void transform( const QWMatrix& )
		{}

	const QString& mime()
		{ return m_mime; }
	void setMime( const QString& mime )
		{ m_mime = mime; }

	const QString& version()
		{ return m_version; }
	void setVersion( const QString& version )
		{ m_version = version; }

	const QString& editor()
		{ return m_editor; }
	void setEditor( const QString& editor )
		{ m_editor = editor; }

	const QString& syntaxVersion()
		{ return m_syntaxVersion; }
	void setSyntaxVersion( const QString& syntaxVersion )
		{ m_syntaxVersion = syntaxVersion; }

	void insertLayer( VLayer* layer );

	const VLayerList& layers() const { return m_layers; }

	void saveXML( QDomDocument& doc ) const;
	bool loadXML( const QDomElement& doc );
	virtual void save( QDomElement& ) const;
	virtual void load( const QDomElement& element );

	virtual VDocument* clone() const;

	virtual void accept( VVisitor& visitor );


	/**
	 * Returns a pointer to the selection.
	 */
	VSelection* selection() const
		{ return m_selection; }

	/**
	 * Append a new object.
	 */
	void append( VObject* object );

	/**
	 * Returns a pointer to the active layer.
	 */
	VLayer* activeLayer() const
		{ return m_activeLayer; }

private:
	/// The layers in this document.
	VLayerList m_layers;
	/// The active layer.
	VLayer* m_activeLayer;

	/// The selection. A list of selected objects.
	VSelection* m_selection;

	/// The mime type.
	QString m_mime;
	/// The version.
	QString m_version;
	/// The editor name.
	QString m_editor;
	/// The snytax version.
	QString m_syntaxVersion;
};

#endif

