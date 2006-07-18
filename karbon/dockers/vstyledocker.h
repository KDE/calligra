/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __VSTYLEDOCKER_H__
#define __VSTYLEDOCKER_H__

#include <koIconChooser.h>
#include <QWidget>
//Added by qt3to4:
#include <QMouseEvent>
#include <Q3PtrList>

class QTabWidget;
class KarbonView;
class KarbonPart;
class Q3HButtonGroup;
class QToolButton;

class VClipartIconItem;

class ClipartChooser : public KoIconChooser
{
public:
	ClipartChooser( QSize iconSize, QWidget *parent = 0L, const char *name = 0L );
	virtual void startDrag();
};

class ClipartWidget : public QWidget
{
	Q_OBJECT

public:
	ClipartWidget( Q3PtrList<VClipartIconItem>* clipartItems, KarbonPart *part, QWidget* parent = 0L );
	~ClipartWidget();

	VClipartIconItem* selectedClipart();

public slots:
	void addClipart();
	void importClipart();
	void deleteClipart();
	void clipartSelected( QTableWidgetItem* item );

	void slotButtonClicked( int id );

private:
	ClipartChooser* m_clipartChooser;
	Q3HButtonGroup* m_buttonGroup;
	QToolButton* m_importClipartButton;
	QToolButton* m_deleteClipartButton;
	KarbonPart* m_part;
	VClipartIconItem* m_clipartItem;
	VClipartIconItem* m_selectedItem;
};

class VStyleDocker : public QWidget
{
	Q_OBJECT

public:
	 VStyleDocker( KarbonPart* part, KarbonView* parent = 0L, const char* name = 0L );
	 virtual ~VStyleDocker();

public slots:
	void slotItemSelected( QTableWidgetItem * );

private:
	virtual void mouseReleaseEvent( QMouseEvent *e );
	QTabWidget *mTabWidget;
	KarbonPart *m_part;
	KarbonView *m_view;
};

#endif

