/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __KARBON_PART_H__
#define __KARBON_PART_H__

#include <qptrlist.h>
#include <koDocument.h>

#include "vdocument.h"
#include "vlayer.h"
#include "vcolor.h"

class QRect;

class VCommand;
class VCommandHistory;
class DCOPObject;

/**
 * Keeps track of visual per document properties.
 * It manages actions performed on this object in a command history.
 * It loads initial settings and applies them to the document and its views.
 * Finally a dcop interface is set up here.
 */
class KarbonPart : public KoDocument
{
	Q_OBJECT
public:
	KarbonPart( QWidget* parentWidget = 0, const char* widgetName = 0,
		QObject* parent = 0, const char* name = 0, bool singleViewMode = false );
	virtual ~KarbonPart();

	virtual void paintContent( QPainter& painter, const QRect& rect,
		bool transparent = false, double zoomX = 1.0, double zoomY = 1.0 );

	/// standard koDocument inherited methods
	virtual bool initDoc();

	/// file -> open calls this method
	virtual bool loadXML( QIODevice*, const QDomDocument& document );
	/// file -> save and file -> save as call this method
	virtual QDomDocument saveXML();

    virtual DCOPObject* dcopObject();

	// access static document:
	VDocument& document() { return m_doc; }

	/// insert a new vobject
	void insertObject( VShape* object );

	/// insert a command into the undo/redo-history:
	void addCommand( VCommand* cmd, bool repaint = false );

	// remove all vobjects which are marked "deleted" and clear command-history:
	void purgeHistory();

	bool showStatusBar() const { return m_bShowStatusBar; }
	void setShowStatusBar( bool b );
	/// update attached view(s) on the current doc settings
	/// at this time only the status bar is handled
	void reorganizeGUI();

	void setUndoRedoLimit( int undos );

	void initConfig();
	int maxRecentFiles() const { return m_maxRecentFiles; }

public slots:
	/// repaint all views attached to this koDocument
	void repaintAllViews( bool erase = false );
	void slotDocumentRestored();
	void slotCommandExecuted();

protected:
	virtual KoView* createViewInstance( QWidget* parent, const char* name );

private:
	VDocument m_doc;					/// store non-visual doc info

	VCommandHistory* m_commandHistory;	/// maintain a command history for undo/redo

	bool m_bShowStatusBar;				/// enable/disable status bar in attached view(s)
	int m_maxRecentFiles;				/// max. number of files shown in open recent menu item
	DCOPObject *dcop;
};

#endif
