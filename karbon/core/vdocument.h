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

#include <koUnit.h>

#include <qstring.h>
#include <qptrlist.h>
#include <qptrdict.h>

#include "vobject.h"

#include "karbon_grid_data.h"

class QDomDocument;
class QDomElement;
class VSelection;
class VLayer;

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
	enum VSelectionMode {
		ActiveLayer,
		VisibleLayers,
		SelectedLayers,
		AllLayers
	};

	VDocument();
	VDocument( const VDocument& document );

	virtual ~VDocument();

	virtual void draw( VPainter* painter, const KoRect* rect ) const;
	void drawPage( VPainter *painter ) const;

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


	/**
	 * Returns document width.
	 */
	double width() const { return m_width; }

	/**
	 * Returns document height.
	 */
	double height() const { return m_height; }

	/**
	 * Sets document width.
	 */
	void setWidth( double width ) { m_width = width; m_boundingBox.setWidth( width ); }

	/**
	 * Sets document height.
	 */
	void setHeight( double height ) { m_height = height; m_boundingBox.setHeight( height ); }


	KoUnit::Unit unit() const
		{ return m_unit; }
	void setUnit( KoUnit::Unit unit )
		{ m_unit = unit; }

	bool canRaiseLayer( VLayer* layer );
	bool canLowerLayer( VLayer* layer );

	/**
	 * Lifts the layer.
	 */
	void raiseLayer( VLayer* layer );

	/**
	 * Lowers the layer.
	 */
	void lowerLayer( VLayer* layer );

	/**
	 * Returns the position of the layer.
	 */
	int layerPos( VLayer* layer );

	/**
	 * Inserts a new layer.
	 * if pos is -1, appends it to the list.
	 */
	void insertLayer( VLayer* layer );

	/**
	 * Removes the layer.
	 */
	void removeLayer( VLayer* layer );

	/**
	 * Sets the active layer.
	 */
	void setActiveLayer( VLayer* layer );

	/**
	 * Returns a pointer to the active layer.
	 */
	VLayer* activeLayer() const { return m_activeLayer; }

	/**
	 * Returns the list of layers.
	 */
	const VLayerList& layers() const { return m_layers; }

	QDomDocument saveXML() const;
	virtual void saveOasis( KoStore *store, KoXmlWriter *docWriter, KoGenStyles &mainStyles ) const;
	enum { STYLE_GRAPHICAUTO = 20, STYLE_LINEAR_GRADIENT, STYLE_RADIAL_GRADIENT, STYLE_STROKE };
	bool loadXML( const QDomElement& doc );
	virtual bool loadOasis( const QDomElement &element, KoOasisContext &context );
	virtual void save( QDomElement& me ) const;
	virtual void load( const QDomElement& me );
	void loadDocumentContent( const QDomElement& doc );

	virtual VDocument* clone() const;

	virtual void accept( VVisitor& visitor );


	/**
	 * Returns a pointer to the selection.
	 */
	VSelection* selection() const
		{ return m_selection; }

	/**
	 * Returns the selection mode.
	 */
	VSelectionMode selectionMode() { return m_selectionMode; }

	/**
	 * Sets the selection mode.
	 */
	void setSelectionMode( VSelectionMode mode ) { m_selectionMode = mode; }

	/**
	 * Append a new object.
	 */
	void append( VObject* object );

	QString objectName( const VObject *obj ) const;
	void setObjectName( const VObject *obj, const QString name ) { m_objectNames.insert( obj, name ); }

	bool saveAsPath() const { return m_saveAsPath; }
	void saveAsPath( bool b ) { m_saveAsPath = b; }

	KarbonGridData &grid() { return m_gridData; }

private:
	/**
	 * Document width.
	 */
	double m_width;

	/**
	 * Document height.
	 */
	double m_height;


	/// The layers in this document.
	VLayerList m_layers;
	/// The active layer.
	VLayer* m_activeLayer;

	/// The selection. A list of selected objects.
	VSelection* m_selection;
	/// The selectionMode
	VSelectionMode m_selectionMode;

	/**
	 * The unit.
	 */
	KoUnit::Unit m_unit;

	/**
	 * The mime type.
	 */
	QString m_mime;

	/// The version.
	QString m_version;
	/// The editor name.
	QString m_editor;
	/// The syntax version.
	QString m_syntaxVersion;

	QMap<const VObject *, QString>	m_objectNames;

	bool m_saveAsPath;

	KarbonGridData m_gridData;
};

#endif

