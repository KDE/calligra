//   brush class - contains the imformation needed to describe a brush
//     inherits the layer class
//
//   Copyright 1999 Andrew Richards <A.Richards@phys.canterbury.ac.nz>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For more information see at the file COPYING in this package

#include "brush.h"
#include <qregexp.h>
#include <stdio.h>

brush::brush(QString file)
	: layer(3, true)
{
	validVal=true;
	loadBrush(file);
	spacingVal=5;
	hotSpotVal=QPoint(0,0);
}


void
brush::loadBrush(QString file)
{
	printf("brush::loadBrush: %s\n",file.latin1());
	QImage img(file);
	if (img.isNull()) {
		printf("Unable to load image: %s\n",file.latin1());
		validVal=false;
		return;
	}

	// XXX currently assumes the alpha image IS a greyscale and the same size as
	// the other channels
	QString alphaName=file;
	alphaName.replace(QRegExp("\\.jpg$"),"-alpha.jpg");
	printf("brush::loadBrushAlpha: %s\n",alphaName.latin1());
	QImage alpha(alphaName);
	if (!alpha.isNull() && (img.size()!=alpha.size())) {
		puts("Incorrect sized alpha channel - not loaded");
		alpha=QImage();
		validVal=false;
		return;
	}
	//loadGrayImage(img, alpha);
	loadRGBImage(img, alpha);
	sizeVal=img.size();
}
