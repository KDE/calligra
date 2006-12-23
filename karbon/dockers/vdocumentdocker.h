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

#include <QPaintEvent>
#include <QPixmap>
#include <QLabel>
#include <QEvent>

class QPointF;
class QLabel;
class QPixmap;

class VDocument;
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

#endif

