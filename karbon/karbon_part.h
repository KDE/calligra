/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __KARBON_PART_H__
#define __KARBON_PART_H__

#include <qlist.h>
#include <koDocument.h>

#include "vlayer.h"

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

	// insert a command into the undo/redo-history:
	void addCommand( VCommand* cmd );

	// read-only access to layers:
	const QList<VLayer>& layers() const { return m_layers; }
	// sacrifying privatness:
	VLayer* activeLayer() const { return m_activeLayer; }

public slots:
    void repaintAllViews( bool erase = false );

protected:
	virtual KoView* createViewInstance( QWidget* parent, const char* name );

private:
	// each graphical object lies on a layer:
	QList<VLayer> m_layers;
	// the active/current layer:
	VLayer* m_activeLayer;

	// everybody loves undo/redo:
	VCommandHistory* m_commandHistory;
};

#endif
