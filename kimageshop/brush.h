//   Copyright 1999 Andrew Richards <A.Richards@phys.canterbury.ac.nz>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For more information see at the file COPYING in this package

#ifndef BRUSH_H
#define BRUSH_H

#include "layer.h"
#include "qsize.h"
#include "qpoint.h"

class Brush : public Layer
{
public:
	Brush(QString file);
	
	int    spacing()   { return spacingVal; };
	QSize  brushSize() { return sizeVal;    };
	bool   isValid()   { return validVal;   };
	void   setHotSpot(QPoint pt) { hotSpotVal=pt; }; // XXX check in brush
	QPoint hotSpot()   { return hotSpotVal; };

private:
	void loadBrush(QString file);

	bool validVal;;
	int spacingVal;
	QSize sizeVal;
	QPoint hotSpotVal;
};

#endif

