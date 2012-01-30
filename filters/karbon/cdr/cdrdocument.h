/* This file is part of the Calligra project, made within the KDE community.

   Copyright 2012 Friedrich W. H. Kossebau <kossebau@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef CDRDOCUMENT_H
#define CDRDOCUMENT_H

// filter
#include "cdrstructs.h"
// Qt
#include <QtCore/QVector>
#include <QtCore/QString>
#include <QtCore/QByteArray>


enum CdrObjectId
{
    PathObjectId,
    RectangleObjectId,
    EllipseObjectId,
    TextObjectId,
    GroupObjectId
};

class CdrObject
{
protected:
    CdrObject(CdrObjectId id) : mId( id ) {}
public:
    virtual ~CdrObject() {}
public:
    CdrObjectId id() const { return mId; }
private:
    CdrObjectId mId;
};


struct Cdr4PathPoint
{
    Cdr4PathPoint() {}
    Cdr4PathPoint( Cdr4Point point, PointType type ) : mPoint(point), mType(type) {}

    Cdr4Point mPoint;
    PointType mType;
};

class CdrRectangleObject : public CdrObject
{
public:
    CdrRectangleObject() : CdrObject(RectangleObjectId) {}
public:
    void setSize( quint16 width, quint16 height ) { mWidth = width; mHeight = height; }
public:
    quint16 width() const { return mWidth; }
    quint16 height() const { return mHeight; }
private:
    quint16 mWidth;
    quint16 mHeight;
};

class CdrEllipseObject : public CdrObject
{
public:
    CdrEllipseObject() : CdrObject(EllipseObjectId) {}
public:
    void setCenterPoint( Cdr4Point centerPoint ) { mCenterPoint = centerPoint; }
    void setXRadius( quint16 xRadius ) { mXRadius = xRadius; }
    void setYRadius( quint16 yRadius ) { mYRadius = yRadius; }
public:
    Cdr4Point centerPoint() const { return mCenterPoint; }
    quint16 xRadius() const { return mXRadius; }
    quint16 yRadius() const { return mYRadius; }
private:
    Cdr4Point mCenterPoint;
    quint16 mXRadius;
    quint16 mYRadius;
};

class CdrPathObject : public CdrObject
{
public:
    CdrPathObject() : CdrObject(PathObjectId) {}
public:
    void addPathPoint( const Cdr4PathPoint& pathPoint ) { mPathPoints.append(pathPoint); }
public:
    const QVector<Cdr4PathPoint>& pathPoints() const { return mPathPoints; }
private:
    QVector<Cdr4PathPoint> mPathPoints;
};

class CdrTextObject : public CdrObject
{
public:
    CdrTextObject() : CdrObject(TextObjectId) {}
public:
    void setText( const QString& text ) { mText = text; }
public:
    const QString& text() const { return mText; }
private:
    Cdr4Point mPoint;
    QString mText;
};

class CdrGroupObject : public CdrObject
{
public:
    CdrGroupObject() : CdrObject(GroupObjectId) {}
public:
    virtual ~CdrGroupObject() { qDeleteAll( mObjects );}
public:
    void addObject( CdrObject* object ) { mObjects.append(object); }
public:
    const QVector<CdrObject*>& objects() const { return mObjects; }
private:
    QVector<CdrObject*> mObjects;
};

class CdrLinkGroupObject : public CdrGroupObject //tmp for now
{
};

class CdrLayer
{
public:
    ~CdrLayer() { qDeleteAll( mObjects );}
public:
    void addObject( CdrObject* object ) { mObjects.append(object); }
public:
    const QVector<CdrObject*>& objects() const { return mObjects; }
private:
    QVector<CdrObject*> mObjects;
};

class CdrPage
{
public:
    ~CdrPage() { qDeleteAll( mLayers );}
public:
    void addLayer( CdrLayer* layer ) { mLayers.append(layer); }
public:
    const QVector<CdrLayer*>& layers() const { return mLayers; }
private:
    QVector<CdrLayer*> mLayers;
};

class CdrDocument
{
public:
    ~CdrDocument() { qDeleteAll( mPages );}
public:
    void addPage( CdrPage* page ) { mPages.append(page); }
    void setFullVersion( quint16 fullVersion ) { mFullVersion = fullVersion; }
    void setSize( quint16 width, quint16 height ) { mWidth = width; mHeight = height; }
    void setStyleSheetFileName( const QByteArray& styleSheetFileName ) { mStyleSheetFileName = styleSheetFileName; }
public:
    quint16 fullVersion() const { return mFullVersion; }
    quint16 width() const { return mWidth; }
    quint16 height() const { return mHeight; }
    const QVector<CdrPage*>& pages() const { return mPages; }
    const QByteArray& styleSheetFileName() const { return mStyleSheetFileName; }
private:
    quint16 mFullVersion;
    quint16 mWidth;
    quint16 mHeight;
    QByteArray mStyleSheetFileName;

    QVector<CdrPage*> mPages;
};

#endif
