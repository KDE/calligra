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

#ifndef _VLAYERSDOCKER_H_
#define _VLAYERSDOCKER_H_

#include <qlistview.h>
#include "vdocker.h"
#include "vdocument.h"

class QPushButton;
class VLayer;
class KarbonView;

class VLayerListViewItem : public QCheckListItem
{
	public:
		VLayerListViewItem( QListView* parent, KarbonView* view, VLayer* layer );

		VLayer* layer() { return m_layer; }
		int pos();
		void update();
	
	protected:
		virtual void stateChange( bool on );

	private:
		KarbonView*  m_view;
		VLayer*      m_layer;
}; // VLayerListViewItem

class VLayersDocker : public VDocker
{
	Q_OBJECT

	public:
		VLayersDocker( KarbonView* view );

		void updatePreviews();
		void updateLayers();

	public slots:
		void selectionChanged( QListViewItem* item, const QPoint& p, int col );
		void addLayer();
		void raiseLayer();
		void lowerLayer();
		void deleteLayer();

	protected:
		VLayerListViewItem* listItem( int pos );

	private:	
		QListView*       m_layersListView;
		QPushButton*     m_addButton;
		QPushButton*     m_raiseButton;
		QPushButton*     m_lowerButton;
		QPushButton*     m_deleteButton;

		KarbonView*      m_view;
}; // VLayersDocker

#endif /* _VLAYERSDOCKER_H_ */
