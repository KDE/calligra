/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VTEXT_H__
#define __VTEXT_H__

#include <qstring.h>

#include "vobject.h"

class VText : public VObject
{
public:
	VText();
	VText( const VText& text );
	virtual ~VText();

	virtual void draw( VPainter *painter, const QRect& rect,
		const double zoomFactor = 1.0 );

	virtual VObject& transform( const QWMatrix& m );

	virtual QRect boundingBox( const double zoomFactor ) const;
	virtual bool intersects( const QRect& rect, const double zoomFactor ) const;

	virtual VObject* clone();

	virtual void save( QDomElement& element ) const;
	virtual void load( const QDomElement& element );

private:
	QString m_text;
};

#endif
