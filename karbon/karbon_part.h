/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __KARBON_PART_H__
#define __KARBON_PART_H__

#include <qptrlist.h>
#include <koDocument.h>

#include "vlayer.h"

class VHandle;
class VCommand;
class VCommandHistory;

class KarbonPart : public KoDocument
{
	Q_OBJECT
public:
	KarbonPart( QWidget* parentWidget = 0, const char* widgetName = 0,
		QObject* parent = 0, const char* name = 0, bool singleViewMode = false );
	virtual ~KarbonPart();

	virtual void paintContent( QPainter& painter, const QRect& rect,
		bool transparent = false, double zoomX = 1.0, double zoomY = 1.0 );

	virtual bool initDoc();

	virtual bool loadXML( QIODevice*, const QDomDocument& );
	virtual QDomDocument saveXML();

	// insert a new vobject:
	void insertObject( const VObject* object );

	// select all vobjects period:
	void selectAllObjects();

	// select vobjects within the rect:
	void selectObjects( const QRect &rect );

	// unselect all vobjects from all vlayers:
	void unselectObjects();

	// delete selected vobjects from all vlayers:
	void deleteObjects( QPtrList<VObject> &list );

	// insert a command into the undo/redo-history:
	void addCommand( VCommand* cmd );

	// read-only access to layers:
	const QPtrList<VLayer>& layers() const { return m_layers; }
	// sacrifying privatness:
	VLayer* activeLayer() const { return m_activeLayer; }
	// draw handle
	void drawHandle( QPainter &p ) const;

public slots:
    void repaintAllViews( bool erase = false );

protected:
	virtual KoView* createViewInstance( QWidget* parent, const char* name );

private:
	// each graphical object lies on a layer:
	QPtrList<VLayer> m_layers;
	// the active/current layer:
	VLayer* m_activeLayer;
	// each part has one handle:
	VHandle *m_handle;

	// everybody loves undo/redo:
	VCommandHistory* m_commandHistory;
};

#endif
