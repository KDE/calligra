/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers
*/

#ifndef __VIMAGE_H__
#define __VIMAGE_H__

#include "vobject.h"
#include <koffice_export.h>
class QImage;

// all vobjects exist inside a layer.

class KARBONBASE_EXPORT VImage : public VObject
{
public:
	VImage( VObject *parent, const QString &fname = "" );
	VImage( const VImage & );
	virtual ~VImage();

	virtual void draw( VPainter *painter, const KoRect *rect ) const;

    virtual void transform( const QWMatrix& m );
	virtual VObject* clone() const;

	virtual void save( QDomElement& element ) const;
	virtual void load( const QDomElement& element );

	virtual void accept( VVisitor& visitor );

private:
	QImage *m_image;
	QString m_fname;
	QWMatrix m_matrix;
};

#endif
