/* This file is part of the KDE project
   Copyright (C) 2001-2002 Lennart Kudling <kudling@kde.org>
   Copyright (C) 2001-2005 Rob Buis <buis@kde.org>
   Copyright (C) 2002,2004-2005 Laurent Montel <montel@kde.org>
   Copyright (C) 2002 Benoît Vautrin <benoit.vautrin@free.fr>
   Copyright (C) 2004-2005,2007 David Faure <faure@kde.org>
   Copyright (C) 2004,2006 Peter Simonsson <psn@linux.se>
   Copyright (C) 2004-2005 Fredrik Edemar <f_edemar@linux.se>
   Copyright (C) 2005-2007 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2005-2007 Thomas Zander <zander@kde.org>
   Copyright (C) 2006 Inge Wallin <inge@lysator.liu.se>
   Copyright (C) 2006 Tim Beaulen <tbscope@gmail.com>
   Copyright (C) 2006 Casper Boemann <cbr@boemann.dk>
   Copyright (C) 2006 Thorsten Zachmann <t.zachmann@zagge.de>

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

#ifndef __KARBON_PART_H__
#define __KARBON_PART_H__


#include <q3ptrlist.h>

#include <KoDocument.h>
#include <KoUnit.h>
#include "vdocument.h"
#include "karbon_export.h"

class QRectF;
class QRect;
class VCommand;
class VDocument;

/**
 * Keeps track of visual per document properties.
 * It manages actions performed on this object in a command history.
 * It loads initial settings and applies them to the document and its views.
 * Finally a dcop interface is set up here.
 */

class KARBONCOMMON_EXPORT KarbonPart : public KoDocument, public KoShapeControllerBase
{
	Q_OBJECT
public:
	explicit KarbonPart( QWidget* parentWidget = 0L, const char* widgetName = 0L,
				QObject* parent = 0L, const char* name = 0L, bool singleViewMode = false );
	virtual ~KarbonPart();

	virtual void paintContent( QPainter& painter, const QRect& rect);

	/// file-> open calls this method
	virtual bool loadXML( QIODevice*, const QDomDocument& document );
    virtual bool loadOasis( const KoXmlDocument & doc, KoOasisStyles& oasisStyles,
                            const KoXmlDocument & settings, KoStore* store );

	/// file-> save and file-> save as call this method
	virtual QDomDocument saveXML();
	virtual bool saveOasis( KoStore *store, KoXmlWriter *manifestWriter );

	/// insert a command into the undo/redo-history:
	void addCommand( VCommand* cmd, bool repaint = false );

	// access static document:
	VDocument& document() { return m_doc; }

	bool showStatusBar() const
	{
		return m_bShowStatusBar;
	}

	void setShowStatusBar( bool b );
	/// update attached view(s) on the current doc settings
	/// at this time only the status bar is handled
	void reorganizeGUI();

	void initConfig();
	unsigned int maxRecentFiles() const { return m_maxRecentFiles; }

	void setPageLayout( KoPageLayout& layout, KoUnit _unit );

	bool mergeNativeFormat( const QString & file );

	// implemented from KoShapeController
	virtual void addShape( KoShape* shape );
	virtual void removeShape( KoShape* shape );

public slots:
	/// repaint all views attached to this koDocument
	void repaintAllViews( bool repaint = true );
	void slotDocumentRestored();

protected:
	virtual KoView* createViewInstance( QWidget* parent );
	virtual void removeView( KoView *view );
    void saveOasisSettings( KoStore * store );
    void saveOasisDocumentStyles( KoStore * store, KoGenStyles& mainStyles );
    void saveOasisAutomaticStyles( KoXmlWriter * contentWriter, KoGenStyles& mainStyles );
	void loadOasisSettings( const QDomDocument&settingsDoc );

    void updateDocumentSize();
private:
	VDocument m_doc;					/// store non-visual doc info

	bool m_bShowStatusBar;				/// enable/disable status bar in attached view(s)
	bool m_merge;
	unsigned int m_maxRecentFiles;				/// max. number of files shown in open recent menu item
};

#endif

