//   Copyright 1999 Andrew Richards <A.Richards@phys.canterbury.ac.nz>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For more information see at the file COPYING in this package

#include <qtableview.h>
#include <qrect.h>
#include "canvas.h"

class layerList : public QTableView
{
 public:
	layerList(QWidget * parent=0, const char * name=0, WFlags f=0);
	layerList(Canvas *c, QWidget *parent=0, const char *name=0, WFlags f=0);

	void updateTable();

 protected:
	virtual void paintCell( QPainter *, int row, int col );
	virtual void mousePressEvent(QMouseEvent *e);

 private:
	void init(Canvas *c);

	Canvas *can;
	int items, selected;
	static QPixmap *eyeIcon, *linkIcon;
	static QRect eyeRect, linkRect;
};
