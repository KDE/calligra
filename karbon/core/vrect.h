/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VRECT_H__
#define __VRECT_H__

class QRect;

// This class is mostly used for bounding-boxes and intersection calculations.

class VRect
{
public:
	VRect();

	const QRect& getQRect();

private:
};

#endif
