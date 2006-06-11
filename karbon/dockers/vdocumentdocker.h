/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers

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

#ifndef __VDOCUMENTDOCKER_H__
#define __VDOCUMENTDOCKER_H__

#include <q3listview.h>
#include <q3ptrdict.h>
#include <QPaintEvent>
#include <QPixmap>
#include <QLabel>
#include <QEvent>

class Q3HButtonGroup;
class QPoint;
class QPointF;
class QLabel;
class QPixmap;
class QCheckBox;

class VDocument;
class KoShape;
class KoShapeContainer;
class KarbonView;

/*************************************************************************
 *  Document Tab                                                         *
 *************************************************************************/

class VDocumentPreview : public QWidget
{
	Q_OBJECT

	public:
		VDocumentPreview( KarbonView* view, QWidget* parent = 0 );
		~VDocumentPreview();

		void reset();

	protected:
		void paintEvent( QPaintEvent* e );
		virtual bool eventFilter( QObject* object, QEvent* event );

	private:
		VDocument	*m_document;
		KarbonView	*m_view;
		QPointF		m_firstPoint;
		QPointF		m_lastPoint;
		bool		m_dragging;
		QPixmap		*m_docpixmap;
}; // VDocumentPreview

class VDocumentTab : public QWidget
{
	Q_OBJECT

	public:
		VDocumentTab( KarbonView* view, QWidget* parent );
		~VDocumentTab();

	public slots:
		void updateDocumentInfo();
		void slotCommandAdded( VCommand* command );
		void slotZoomChanged( double );
		void slotViewportChanged();
		void slotCommandExecuted();

	private:
		VDocumentPreview*	m_documentPreview;
		QLabel*				m_height;
		QLabel*				m_width;
		QLabel*				m_layers;
		QLabel*				m_format;

		KarbonView*			m_view;
}; // VDocumentTab

/*************************************************************************
 *  Layers Tab                                                           *
 *************************************************************************/

class VLayerListViewItem : public Q3CheckListItem
{
public:
	VLayerListViewItem( Q3ListView* parent, KoLayerShape* layer, VDocument *doc, Q3PtrDict<VLayerListViewItem> *map );
	virtual ~VLayerListViewItem();

	KoLayerShape* layer() { return m_layer; }
	int pos();
	void update();
	virtual QString key( int column, bool ascending ) const;
	virtual int compare( Q3ListViewItem *i, int col, bool ascending ) const;
	void setKey( uint key ) { m_key = key; }

protected:
	virtual void stateChange( bool on );

private:
	KoLayerShape *m_layer;
	VDocument	 *m_document;
	uint		  m_key;
	Q3PtrDict<VLayerListViewItem> *m_map;
}; // VLayerListViewItem

class VObjectListViewItem : public Q3ListViewItem
{
public:
	VObjectListViewItem( Q3ListViewItem* parent, KoShape* object, VDocument *doc, uint key, Q3PtrDict<VObjectListViewItem> *map );
	virtual ~VObjectListViewItem();

	KoShape* object() { return m_object; }
	void update();
	virtual QString key( int column, bool ascending ) const;
	virtual int compare( Q3ListViewItem *i, int col, bool ascending ) const;
	void setKey( uint key ) { m_key = key; }
private:
	KoShape		*m_object;
	VDocument	*m_document;
	uint		 m_key;
	Q3PtrDict<VObjectListViewItem> *m_map;
};

class VLayersTab : public QWidget
{
Q_OBJECT

public:
	VLayersTab( KarbonView* view, QWidget* parent = 0 );
	~VLayersTab();

public slots:
	void updatePreviews();
	void updateLayers();

	void itemClicked( Q3ListViewItem* item, const QPoint&, int col );
	void selectionChangedFromList();
	void selectionChangedFromTool();
	void renameItem( Q3ListViewItem* item, const QPoint&, int col );
	void addLayer();
	void raiseItem();
	void lowerItem();
	void deleteItem();
	void slotCommandExecuted( VCommand* command );

private slots:
	void slotButtonClicked( int ID );
	void removeDeletedObjectsFromList();
	void updateChildItems( Q3ListViewItem *item );
	void toggleState( KoShape *obj, int col );

protected:
	VLayerListViewItem* listItem( int pos );
	void updateObjects( KoShapeContainer *object, Q3ListViewItem *item );
	void resetSelection();
	void selectActiveLayer();

private:
	Q3ListView						*m_layersListView;
	Q3HButtonGroup					*m_buttonGroup;
	KarbonView						*m_view;
	VDocument						*m_document;
	Q3PtrDict<VLayerListViewItem>	m_layers;
	Q3PtrDict<VObjectListViewItem>	m_objects;
}; // VLayersTab

/*************************************************************************
 *  History Tab                                                          *
 *************************************************************************/

class VHistoryItem;
 
class VHistoryGroupItem : public Q3ListViewItem
{
	public:
		VHistoryGroupItem( VHistoryItem* item, Q3ListView* parent, Q3ListViewItem* after );
		~VHistoryGroupItem();

		void paintCell( QPainter* p, const QColorGroup& cg, int column, int width, int align );
		void paintFocus( QPainter*, const QColorGroup&, const QRect& );

		virtual QString key( int, bool ) const { return m_key; }
		virtual int rtti() const { return 1001; }

	private:
		QString	m_key;
}; // VHistoryGroupItem
 
class VHistoryItem : public Q3ListViewItem
{
	public:
		VHistoryItem( VCommand* command, Q3ListView* parent, Q3ListViewItem* after );
		VHistoryItem( VCommand* command, VHistoryGroupItem* parent, Q3ListViewItem* after );
		~VHistoryItem();

		VCommand* command() { return m_command; }

		void paintCell( QPainter* p, const QColorGroup& cg, int column, int width, int align );
		void paintFocus( QPainter*, const QColorGroup&, const QRect& );

		virtual QString key( int, bool ) const { return m_key; }
		virtual int rtti() const { return 1002; }

	private:
		void init();

		QString     m_key;
		VCommand*   m_command;
}; // VHistoryItem

class VHistoryTab : public QWidget
{
	Q_OBJECT

	public:
		VHistoryTab( KarbonPart* part, QWidget* parent );
		~VHistoryTab();

		bool groupingEnabled();

	public slots:
		void historyCleared();
		void commandExecuted( VCommand* command );
		void slotCommandAdded( VCommand* command );
		void removeFirstCommand();
		void removeLastCommand();

		void commandClicked( int button, Q3ListViewItem* item, const QPoint& point, int col );
		void groupingChanged( int );

	signals:
		void undoCommand( VCommand* command );
		void redoCommand( VCommand* command );
		void undoCommandsTo( VCommand* command );
		void redoCommandsTo( VCommand* command );

	private:
		Q3ListView*      m_history;
		Q3ListViewItem*  m_lastCommand;
		QCheckBox*      m_groupCommands;
		long            m_lastCommandIndex;

		KarbonPart*     m_part;
}; // VHistoryTab

#endif

