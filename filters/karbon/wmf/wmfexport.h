/* This file is part of the KDE project
 * Copyright (c) 2003 thierry lorthiois (lorthioist@wanadoo.fr)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef WMFEXPORT_H
#define WMFEXPORT_H

#include <qpen.h>
#include <qbrush.h>
#include <q3ptrlist.h>
#include <q3pointarray.h>
//Added by qt3to4:
#include <Q3CString>
#include <KoFilter.h>
#include "vvisitor.h"

class KoWmfWrite;
class VPath;
class VDocument;
class VSubpath;
class VText;

class WmfExport : public KoFilter, private VVisitor
{
    Q_OBJECT

public:
    WmfExport( KoFilter *parent, const char *name, const QStringList&);
    virtual ~WmfExport();

    virtual KoFilter::ConversionStatus convert( const QByteArray& from, const QByteArray& to );

private:
    void visitVPath( VPath& composite );
    void visitVDocument( VDocument& document );
    void visitVSubpath( VSubpath& path );
    void visitVText( VText& text );
    void getBrush( QBrush& brush, const VFill *fill );
    void getPen( QPen& pen, const VStroke *stroke );
    
    // coordinate transformation
    // translate origin from (left,bottom) to (left,top) -> scale to wmf size 
    // Wmf origin is (left,top) corner
    // Karbon origin is (left,bottom) corner
    int coordX( double left ) 
            { return (int)(left * mScaleX); }
    int coordY( double top ) 
            { return (int)((mDoc->height() - top) * mScaleY); }
    
private:
    KoWmfWrite *mWmf;
    VDocument *mDoc;
    int       mDpi;
    double    mScaleX;
    double    mScaleY;
    Q3PtrList<Q3PointArray> mListPa;
};

#endif
