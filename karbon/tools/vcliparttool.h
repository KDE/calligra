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

#ifndef __VCLIPARTTOOL_H__
#define __VCLIPARTTOOL_H__

#include <qstring.h>
#include <qpixmap.h>

#include <koPoint.h>
#include <koIconChooser.h>

#include "vtool.h"
#include "vcommand.h"


class VObject;
class QToolButton;
class QHButtonGroup;

class VClipartIconItem : public KoIconItem
{
	public:
		VClipartIconItem( const VObject* clipart, QString filename );
		VClipartIconItem( const VClipartIconItem& item );
		~VClipartIconItem();
		
		virtual QPixmap& thumbPixmap() const { return (QPixmap&)m_thumbPixmap; }
		virtual QPixmap& pixmap() const { return (QPixmap&)m_pixmap; }
		const VObject* clipart() const { return m_clipart; }
		QString filename() const { return m_filename; }
		bool canDelete() const { return m_delete; }
		VClipartIconItem* clone();

	private:
		QPixmap  m_pixmap;
		QPixmap  m_thumbPixmap;
		VObject* m_clipart;
		QString  m_filename;
		bool     m_delete;
}; // VClipartIconItem

class VClipartWidget : public QFrame
{
	Q_OBJECT

	public:
		VClipartWidget( QPtrList<VClipartIconItem>* clipartItems, KarbonView* view, QWidget* parent = 0L );
		~VClipartWidget();

		VClipartIconItem* selectedClipart();

	public slots:
		void addClipart();
		void deleteClipart();
		void clipartSelected( KoIconItem* item );
		
		void slotButtonClicked( int id );

	private:
		KoIconChooser*        m_clipartChooser;
		QHButtonGroup*        m_buttonGroup;
		QToolButton*          m_importClipartButton;
		QToolButton*          m_deleteClipartButton;
		KarbonView*           m_view;
		VClipartIconItem*     m_clipartItem;
}; // VClipartWidget

class VClipartTool : public VTool
{
	public:
		VClipartTool( KarbonView* view );
		~VClipartTool();

		virtual void activate();

		virtual QString name() { return "Clipart tool"; }
		virtual QString contextHelp();
		virtual QWidget* optionsWidget() { return m_optionsWidget; }

	protected:
		virtual void draw();
	
		virtual void mouseButtonPress();
		virtual void mouseButtonRelease();
		virtual void mouseDrag();
		virtual void mouseDragShiftPressed();
		virtual void mouseDragShiftReleased();
		virtual void mouseDragRelease();
		virtual void cancel();

	private:
		class VClipartCmd : public VCommand
		{
			public:
				VClipartCmd( VDocument* doc, const QString& name, VObject* clipart );
				virtual ~VClipartCmd() {}

				virtual void execute();
				virtual void unexecute();
				virtual bool isExecuted() { return m_executed; }

			private:
				VObject* m_clipart;
				bool     m_executed;
		}; // VClipartCmd

		VObject*        m_clipart;
		KoPoint         m_last;
		VClipartWidget* m_optionsWidget;
		bool            m_keepRatio;
}; // VClipartTool

#endif /* __VCLIPARTTOOL_H__ */
