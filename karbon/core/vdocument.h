/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
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

	~VDocument();

	virtual void draw( VPainter* painter, const KoRect& rect ) const;

	// TODO
	virtual void transform( const QWMatrix& ) {}

	const QString& mime() { return m_mime; }
	void setMime( const QString& mime ) { m_mime = mime; }

	const QString& version() { return m_version; }
	void setVersion( const QString& version ) { m_version = version; }

	const QString& editor() { return m_editor; }
	void setEditor( const QString& editor ) { m_editor = editor; }

	const QString& syntaxVersion() { return m_syntaxVersion; }
	void setSyntaxVersion( const QString& syntaxVersion ) { m_syntaxVersion = syntaxVersion; }

	void insertLayer( VLayer* layer );

	const VLayerList& layers() const { return m_layers; }

	void saveXML( QDomDocument& doc ) const;
	bool loadXML( const QDomElement& element );
	virtual void save( QDomElement& ) const {}
	virtual void load( const QDomElement& element ) { loadXML( element ); }

	virtual VDocument* clone() const;

	virtual void accept( VVisitor& visitor );


	// manipulate selection:
	VSelection* selection() const { return m_selection; }

	void select() const;	// select all vobjects period.
	void deselect() const;	// unselect all vobjects from all vlayers.

	void select( VObject& object, bool exclusive = false ) const;
	void deselect( VObject& object ) const;

	/// Select all objects within rect.
	void select( const KoRect& rect, bool exclusive = false ) const;


	// move up/down within layer
// TODO: move these functions into a command:
	void moveSelectionToTop();
	void moveSelectionToBottom();
	void moveSelectionDown();
	void moveSelectionUp();

	VStroke defaultStroke() { return m_defaultStroke; }
	VFill defaultFill() { return m_defaultFill; }
	void setDefaultStrokeColor( const VColor &color ) { m_defaultStroke.setColor( color ); }
	void setDefaultFillColor( const VColor &color ) { m_defaultFill.setColor( color ); }
	/// all newly created shapes in this document get the default color by using this method
	void applyDefaultColors( VObject & ) const;

	void append( VObject* object ); // insert a new vobject

	VLayer* activeLayer() const { return m_activeLayer; }   // active layer.

private:
	VLayerList m_layers;			// all layers in this document
	VSelection* m_selection;		// selected objects.
	VLayer* m_activeLayer;			// the active/current layer.

	VStroke m_defaultStroke;      /// keep track of a default stroke color for created shapes
	VFill m_defaultFill;          /// keep track of a default fill color for created shapes

	QString m_mime;
	QString m_version;
	QString m_editor;
	QString m_syntaxVersion;
};

#endif

