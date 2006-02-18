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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef VDOCUMENT_H
#define VDOCUMENT_H

#include <KoUnit.h>

#include <qstring.h>
#include <qptrlist.h>
#include <qptrdict.h>

#include "vobject.h"

#include "karbon_grid_data.h"
#include <koffice_export.h>

class QDomDocument;
class QDomElement;
class VSelection;
class VLayer;
class KoPageLayout;

typedef QPtrList<VLayer> VLayerList;
typedef QPtrListIterator<VLayer> VLayerListIterator;


/**
 * All non-visual, static doc info is in here.
 * The karbon part uses this class.
 * Filters can use this class as well instead of
 * the visually oriented karbon part.
 */

class KARBONBASE_EXPORT VDocument : public VObject
{
public:
	/** The different selection modes */
	enum VSelectionMode {
		ActiveLayer,	/**< selection within the active layer */
		VisibleLayers,	/**< selection within all visible layers */
		SelectedLayers,	/**< selection within all selected layers */
		AllLayers		/**< selection within all layers */
	};

	/**
	 * Constructs a new document.
	 */
	VDocument();

	/**
	 * Copy constructor.
	 *
	 * @param document the document to copy properties from
	 */
	VDocument( const VDocument& document );

	/** 
	 * Destroys the document and all of the layers.
	 */
	virtual ~VDocument();

	virtual void draw( VPainter* painter, const KoRect* rect ) const;
	
	/**
	 * Draw the document frame to a painting device.
	 *
	 * @param painter abstraction that is used to render to a painting device.
	 * @param pl layout describing the page to draw on (restricting the painter)
	 * @param drawPageMargins if @c true, also draw the crop marks for the page margins,
	 *        otherwise, don't draw them.
	 */
	void drawPage( VPainter *painter, const KoPageLayout &pl, bool drawPageMargins ) const;

	/**
	 * Returns document width.
	 *
	 * @return the document's width
	 */
	double width() const { return m_width; }

	/**
	 * Returns document height.
	 *
	 * @return the document's height
	 */
	double height() const { return m_height; }

	/**
	 * Sets document width.
	 *
	 * @param width the new document width
	 */
	void setWidth( double width ) { m_width = width; m_boundingBox.setWidth( width ); }

	/**
	 * Sets document height.
	 *
	 * @param height the new document height
	 */
	void setHeight( double height ) { m_height = height; m_boundingBox.setHeight( height ); }

	/**
	 * Returns document unit.
	 *
	 * @return the document's unit
	 */
	KoUnit::Unit unit() const
		{ return m_unit; }

	/**
	 * Sets document unit.
	 *
	 * @param unit the new document unit
	 */
	void setUnit( KoUnit::Unit unit )
		{ m_unit = unit; }

	/**
	 * Checks if specified layer can be raised. 
	 *
	 * A layer can be raised if there is more than one layer and the specified layer
	 * is not already at the top.
	 *
	 * @param layer the layer to check
	 * @return true if layer can be raised, else false
	 */
	bool canRaiseLayer( VLayer* layer );

	/**
	 * Checks if specified layer can be lowered. 
	 *
	 * A layer can be lowered if there is more than one layer and the specified layer
	 * is not already at the bottom.
	 *
	 * @param layer the layer to check
	 * @return true if layer can be lowered, else false
	 */
	bool canLowerLayer( VLayer* layer );

	/**
	 * Raises the layer.
	 * 
	 * @param layer the layer to raise
	 */
	void raiseLayer( VLayer* layer );

	/**
	 * Lowers the layer.
	 * 
	 * @param layer the layer to lower
	 */
	void lowerLayer( VLayer* layer );

	/**
	 * Returns the position of the specified layer.
	 *
	 * @param layer the layer to retrieve the position for
	 * @return the layer position
	 */
	int layerPos( VLayer* layer );

	/**
	 * Inserts a new layer.
	 * 
	 * The layer is appended at the end, on top of all other layers, and is activated.
	 *
	 * @param layer the layer to insert
	 */
	void insertLayer( VLayer* layer );

	/**
	 * Removes the layer.
	 *
	 * If there is no layer left, a new layer is created, inserted and activated.
	 *
	 * @param layer the layer to remove
	 */
	void removeLayer( VLayer* layer );

	/**
	 * Sets the active layer.
	 *
	 * The specified layer is set active, if it is found in the layer list.
	 *
	 * @param layer the layer to set active
	 */
	void setActiveLayer( VLayer* layer );

	/**
	 * Returns a pointer to the active layer.
	 *
	 * @return the currently active layer
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
	virtual bool loadOasis( const QDomElement &element, KoOasisLoadingContext &context );
	virtual void save( QDomElement& element ) const;
	virtual void load( const QDomElement& element );
	void loadDocumentContent( const QDomElement& doc );

	virtual VDocument* clone() const;

	virtual void accept( VVisitor& visitor );


	/**
	 * Returns a pointer to the selection.
	 *
	 * @return the document's selection
	 */
	VSelection* selection() const
		{ return m_selection; }

	/**
	 * Returns the selection mode.
	 * 
	 * @return the actual selection mode
	 */
	VSelectionMode selectionMode() { return m_selectionMode; }

	/**
	 * Sets the selection mode.
	 *
	 * @param mode the new selection mode
	 */
	void setSelectionMode( VSelectionMode mode ) { m_selectionMode = mode; }

	/**
	 * Appends a new object to the active layer.
	 *
	 * @param object the object to append
	 */
	void append( VObject* object );

	/**
	 * Returns custom name of specified object.
	 *
	 * @param obj the object to retrieve name for
	 * @return the custom name of the object or an empty string if no custom name is set
	 */
	QString objectName( const VObject *obj ) const;

	/**
	 * Sets custom name of specified object.
	 *
	 * By default all object have generic names like path, rectangle or text that
	 * is defined within the object's class.
	 * 
	 * @param obj the object to set custom name for
	 * @param name the the custom name to set
	 */
	void setObjectName( const VObject *obj, const QString name ) { m_objectNames.insert( obj, name ); }

	bool saveAsPath() const { return m_saveAsPath; }
	void saveAsPath( bool b ) { m_saveAsPath = b; }

	/**
	 * Returns the document's grid.
	 *
	 * @return the doument's grid 
	 */
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

	QMap<const VObject *, QString>	m_objectNames;

	// TODO this flag is used nowhere, can we remove it?
	bool m_saveAsPath;

	KarbonGridData m_gridData;
};

#endif

