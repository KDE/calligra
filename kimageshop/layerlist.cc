//   layer list widget to be incorporated into a layer control widget
//
//   Copyright 1999 Andrew Richards <A.Richards@phys.canterbury.ac.nz>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For more information see at the file COPYING in this package

#include "layerlist.h"
#include <qpainter.h>
#include <qpixmap.h>
#include <qmessagebox.h>
#include "misc.h"

#include <kstddirs.h>
#include <kglobal.h>

QPixmap *layerList::eyeIcon, *layerList::linkIcon;
QRect layerList::eyeRect, layerList::linkRect;

layerList::layerList(QWidget * parent, const char * name, WFlags f)
	: QTableView(parent, name, f)
{
	init(0);
}

layerList::layerList(Canvas *c, QWidget *parent, const char *name,
										 WFlags f=0)
	: QTableView(parent, name, f)
{
	init(c);
}

void
layerList::init(Canvas *c)
{
	setTableFlags(Tbl_autoVScrollBar);
	can=c;
	updateTable();

	setCellWidth(150);
	setCellHeight(40);
	selected=c->layerList().count()-1;
	if (!eyeIcon)
	  {
	    QString _icon = locate("data", "kimageshop/pics/eye.xpm");
	    eyeIcon=new QPixmap;
	    if (!eyeIcon->load(_icon))
	      QMessageBox::critical( this, "Canvas","Can't find eye.xpm");
	    eyeRect=QRect(QPoint(5,(cellHeight()-eyeIcon->height())/2),eyeIcon->size());
	  }
	if (!linkIcon)
	  {
	    QString _icon = locate("data", "kimageshop/pics/link.xpm");
	    linkIcon=new QPixmap;
	    if (!linkIcon->load(_icon))
	      QMessageBox::critical( this, "Canvas","Can't find link.xpm");
	    linkRect=QRect(QPoint(25,(cellHeight()-linkIcon->height())/2), linkIcon->size());
	}
}

void
layerList::paintCell(QPainter *p, int row, int )
{
	if (row==selected) {
		p->fillRect(0,0, cellWidth()-1, cellHeight()-1, QColor(15,175,50));
	}
	if (can->layerList().at(row)->isVisible())
		p->drawPixmap(eyeRect.topLeft(), *eyeIcon);
	if (can->layerList().at(row)->isLinked())
		p->drawPixmap(linkRect.topLeft(), *linkIcon);

	p->drawRect(0,0, cellWidth()-1, cellHeight()-1);
	p->drawText(80,20, can->layerList().at(row)->name());
}

void
layerList::updateTable()
{
	if (can) {
		items=can->layerList().count();
		setNumRows(items);
		setNumCols(1);
	} else {
		items=0;
		setNumRows(0);
		setNumCols(0);
	}
}

void 
layerList::mousePressEvent(QMouseEvent *e)
{
	QPoint localPoint(e->pos().x()%cellWidth(),
										e->pos().y()%cellHeight());

	SHOW_POINT(localPoint);

	int r=findRow(e->pos().y());

	if (eyeRect.contains(localPoint)) {
		can->layerList().at(r)->setVisible(!can->layerList().at(r)->isVisible());
		updateCell(r,0);
		can->compositeImage(can->layerList().at(r)->imageExtents());
		can->repaint(can->layerList().at(r)->imageExtents(), false);
		return;
	}
	if (linkRect.contains(localPoint)) {
		can->layerList().at(r)->setLinked(!can->layerList().at(r)->isLinked());
		updateCell(r,0);
		return;
	}
	if (r!=-1) {
		int currentSel=selected;
		selected=-1;
		updateCell(currentSel,0);
		selected=r;
		can->setCurrentLayer(selected);
		updateCell(selected,0);
	}
}
