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

#ifndef __KARBON_PART_H__
#define __KARBON_PART_H__


#include <qptrlist.h>

#include <koDocument.h>
#include <koUnit.h>

#include "vcolor.h"
#include "vdocument.h"
#include "vlayer.h"

class DCOPObject;
class KoRect;
class QRect;
class VCommand;
class VCommandHistory;


/**
 * Keeps track of visual per document properties.
 * It manages actions performed on this object in a command history.
 * It loads initial settings and applies them to the document and its views.
 * Finally a dcop interface is set up here.
 */

class KarbonPart : public KoDocument
{
	Q_OBJECT
        Q_PROPERTY( int maxRecentFiles READ maxRecentFiles )
public:
	KarbonPart( QWidget* parentWidget = 0L, const char* widgetName = 0L,
				QObject* parent = 0L, const char* name = 0L, bool singleViewMode = false );
	virtual ~KarbonPart();

	virtual void paintContent( QPainter& painter, const QRect& rect,
							   bool transparent = false, double zoomX = 1.0, double zoomY = 1.0 );

	/// standard koDocument inherited methods
	virtual bool initDoc();

	/// file-> open calls this method
	virtual bool loadXML( QIODevice*, const QDomDocument& document );
	/// file-> save and file-> save as call this method
	virtual QDomDocument saveXML();

	virtual DCOPObject* dcopObject();

	// access static document:
	VDocument& document()
	{
		return m_doc;
	}

	/// insert a new vobject
	void insertObject( VObject* object );

	/// insert a command into the undo/redo-history:
	void addCommand( VCommand* cmd, bool repaint = false );

	// Clear history.
	void clearHistory();

	/// access the command history:
	VCommandHistory* commandHistory()
	{
		return m_commandHistory;
	}

	bool showStatusBar() const
	{
		return m_bShowStatusBar;
	}

	void setShowStatusBar( bool b );
	/// update attached view(s) on the current doc settings
	/// at this time only the status bar is handled
	void reorganizeGUI();

	void setUndoRedoLimit( int undos );

	void initConfig();
	int maxRecentFiles() const
	{
		return m_maxRecentFiles;
	}

	QString getUnitName() const
	{
		return KoUnit::unitName( m_unit );
	}

	KoUnit::Unit getUnit() const
	{
		return m_unit;
	}

	void setUnit( KoUnit::Unit _unit );
	void initUnit();

public slots:
	/// repaint all views attached to this koDocument
	void repaintAllViews( bool repaint = true );
	void repaintAllViews( const KoRect& );
	void slotDocumentRestored();
	void slotCommandExecuted();

protected:
	virtual KoView* createViewInstance( QWidget* parent, const char* name );
	virtual void removeView( KoView *view );

private:
	VDocument m_doc;					/// store non-visual doc info

	VCommandHistory* m_commandHistory;	/// maintain a command history for undo/redo

	bool m_bShowStatusBar;				/// enable/disable status bar in attached view(s)
	int m_maxRecentFiles;				/// max. number of files shown in open recent menu item
	DCOPObject *dcop;
	KoUnit::Unit m_unit;
};

#endif

