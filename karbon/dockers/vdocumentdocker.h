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

#include <qlistview.h>
#include <qptrdict.h>

class QHButtonGroup;
class QPoint;
class QLabel;
class QPixmap;
class QCheckBox;

class VDocument;
class VLayer;
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
		KoPoint		m_firstPoint;
		KoPoint		m_lastPoint;
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

class VLayerListViewItem : public QCheckListItem
{
public:
	VLayerListViewItem( QListView* parent, VLayer* layer, VDocument *doc, QPtrDict<VLayerListViewItem> *map );
	virtual ~VLayerListViewItem();

	VLayer* layer() { return m_layer; }
	int pos();
	void update();
	virtual QString key( int column, bool ascending ) const;
	virtual int compare( QListViewItem *i, int col, bool ascending ) const;
	void setKey( uint key ) { m_key = key; }

protected:
	virtual void stateChange( bool on );

private:
	VLayer		*m_layer;
	VDocument	*m_document;
	uint		 m_key;
	QPtrDict<VLayerListViewItem> *m_map;
}; // VLayerListViewItem

class VObjectListViewItem : public QListViewItem
{
public:
	VObjectListViewItem( QListViewItem* parent, VObject* object, VDocument *doc, uint key, QPtrDict<VObjectListViewItem> *map );
	virtual ~VObjectListViewItem();

	VObject* object() { return m_object; }
	void update();
	virtual QString key( int column, bool ascending ) const;
	virtual int compare( QListViewItem *i, int col, bool ascending ) const;
	void setKey( uint key ) { m_key = key; }
private:
	VObject		*m_object;
	VDocument	*m_document;
	uint		 m_key;
	QPtrDict<VObjectListViewItem> *m_map;
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

	void itemClicked( QListViewItem* item, const QPoint&, int col );
	void selectionChangedFromList();
	void selectionChangedFromTool();
	void renameItem( QListViewItem* item, const QPoint&, int col );
	void addLayer();
	void raiseItem();
	void lowerItem();
	void deleteItem();
	void slotCommandExecuted( VCommand* command );

private slots:
	void slotButtonClicked( int ID );
	void removeDeletedObjectsFromList();
	void updateChildItems( QListViewItem *item );
	void toggleState( VObject *obj, int col );

protected:
	VLayerListViewItem* listItem( int pos );
	void updateObjects( VObject *object, QListViewItem *item );
	void resetSelection();
	void selectActiveLayer();

private:
	QListView						*m_layersListView;
	QHButtonGroup					*m_buttonGroup;
	KarbonView						*m_view;
	VDocument						*m_document;
	QPtrDict<VLayerListViewItem>	m_layers;
	QPtrDict<VObjectListViewItem>	m_objects;
}; // VLayersTab

/*************************************************************************
 *  History Tab                                                          *
 *************************************************************************/

class VHistoryItem;
 
class VHistoryGroupItem : public QListViewItem
{
	public:
		VHistoryGroupItem( VHistoryItem* item, QListView* parent, QListViewItem* after );
		~VHistoryGroupItem();

		void paintCell( QPainter* p, const QColorGroup& cg, int column, int width, int align );
		void paintFocus( QPainter*, const QColorGroup&, const QRect& );

		virtual QString key( int, bool ) const { return m_key; }
		virtual int rtti() const { return 1001; }

	private:
		QString	m_key;
}; // VHistoryGroupItem
 
class VHistoryItem : public QListViewItem
{
	public:
		VHistoryItem( VCommand* command, QListView* parent, QListViewItem* after );
		VHistoryItem( VCommand* command, VHistoryGroupItem* parent, QListViewItem* after );
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

		void commandClicked( int button, QListViewItem* item, const QPoint& point, int col );
		void groupingChanged( int );

	signals:
		void undoCommand( VCommand* command );
		void redoCommand( VCommand* command );
		void undoCommandsTo( VCommand* command );
		void redoCommandsTo( VCommand* command );

	private:
		QListView*      m_history;
		QListViewItem*  m_lastCommand;
		QCheckBox*      m_groupCommands;
		long            m_lastCommandIndex;

		KarbonPart*     m_part;
}; // VHistoryTab

#endif

