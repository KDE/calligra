/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __KARBON_PART_H__
#define __KARBON_PART_H__

#include <qptrlist.h>
#include <koDocument.h>

#include "vlayer.h"

class QRect;

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

	virtual bool loadXML( QIODevice*, const QDomDocument& document );
	virtual QDomDocument saveXML();

	// manipulate selection:
	const VObjectList& selection() const { return m_selection; }
	void selectObject( VObject& object, bool exclusive = false );
	void deselectObject( VObject& object );
	void selectObjectsWithinRect( const QRect& rect,
		const double zoomFactor, bool exclusive = false );
	void selectAllObjects();	// select all vobjects period.
	void deselectAllObjects();	// unselect all vobjects from all vlayers.

	// inserting, deleting objects:
	void insertObject( const VObject* object );	// insert a new vobject:

	// insert a command into the undo/redo-history:
	void addCommand( VCommand* cmd, bool repaint = false );

	// remove all vobjects which are marked "deleted" and clear command-history:
	void purgeHistory();

	// layers:
	const QPtrList<VLayer>& layers() const { return m_layers; }	// r/o access.
	// TODO: still needed?
	VLayer* activeLayer() const { return m_activeLayer; }	// active layer.
	void moveSelectionToTop();
	void moveSelectionToBottom();

public slots:
    void repaintAllViews( bool erase = false );
    void slotDocumentRestored();
    void slotCommandExecuted();

protected:
	virtual KoView* createViewInstance( QWidget* parent, const char* name );

private:
	VLayerList m_layers;			// all objects exist inside a layer.
	VLayer* m_activeLayer;				// the active/current layer.

	VObjectList m_selection;		// a list of selected objects.

	VCommandHistory* m_commandHistory;	// everybody loves undo/redo.
};

#endif
