/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VLAYER_H__
#define __VLAYER_H__

#include <qlist.h>
#include <qstring.h>
#include "vobject.h"

// Each graphical object lies on a layer.

class VLayer
{
// TODO: add wrapper functions to access objects and remove this friendship
	friend class KarbonPart;	// im lazy for now

public:
	VLayer();
	~VLayer();

	void draw( QPainter& painter, const QRect& rect, const double& zoomFactor );

	const bool& isVisible() const { return m_isVisible; }
	const bool& isReadOnly() const { return m_isReadOnly; }

private:
	QList<VObject> m_objects;
	QString m_name;
	bool m_isVisible;
	bool m_isReadOnly;
};

#endif
