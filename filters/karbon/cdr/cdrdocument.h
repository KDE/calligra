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
#include <QtGui/QColor>
#include <QtCore/QHash>
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
    explicit CdrObject(CdrObjectId id) : mId( id ) {}
private:
    CdrObject( const CdrObject& );
    CdrObject& operator=( const CdrObject& );
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

class CdrGraphObject : public CdrObject
{
protected:
    explicit CdrGraphObject(CdrObjectId id) : CdrObject( id ), mStyleId(0), mOutlineId(0), mFillId(0) {}
public:
    void setStyleId( quint32 styleId ) { mStyleId = styleId; }
    void setOutlineId( quint32 outlineId ) { mOutlineId = outlineId; }
    void setFillId( quint32 fillId ) { mFillId = fillId; }
public:
    quint16 styleId() const { return mStyleId; }
    quint32 outlineId() const { return mOutlineId; }
    quint32 fillId() const { return mFillId; }
private:
    quint16 mStyleId; // TODO: make sure that 0 is never an id
    quint32 mOutlineId; // TODO: make sure that 0 is never an id
    quint32 mFillId; // TODO: make sure that 0 is never an id
};

class CdrRectangleObject : public CdrGraphObject
{
public:
    CdrRectangleObject() : CdrGraphObject(RectangleObjectId) {}
public:
    void setSize( quint16 width, quint16 height ) { mWidth = width; mHeight = height; }
public:
    quint16 width() const { return mWidth; }
    quint16 height() const { return mHeight; }
private:
    quint16 mWidth;
    quint16 mHeight;
};

class CdrEllipseObject : public CdrGraphObject
{
public:
    CdrEllipseObject() : CdrGraphObject(EllipseObjectId) {}
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

class CdrPathObject : public CdrGraphObject
{
public:
    CdrPathObject() : CdrGraphObject(PathObjectId) {}
public:
    void addPathPoint( const Cdr4PathPoint& pathPoint ) { mPathPoints.append(pathPoint); }
public:
    const QVector<Cdr4PathPoint>& pathPoints() const { return mPathPoints; }
private:
    QVector<Cdr4PathPoint> mPathPoints;
};

class CdrTextObject : public CdrGraphObject
{
public:
    CdrTextObject() : CdrGraphObject(TextObjectId) {}
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


class CdrStyle
{
public:
    CdrStyle() : mFontId(-1), mFontSize(18) {}
public:
    void setTitle( const QString& title ) { mTitle = title; }
    void setFontId( quint16 fontId ) { mFontId = fontId; }
    void setFontSize( quint16 fontSize ) { mFontSize = fontSize; }
public:
    const QString& title() const { return mTitle; }
    quint16 fontId() const { return mFontId; }
    quint16 fontSize() const { return mFontSize; }
private:
    quint16 mFontId;
    quint16 mFontSize;
    QString mTitle;
};

class CdrOutline
{
public:
    CdrOutline() : mLineWidth(0) {}
public:
    void setType( quint32 type ) { mType = type; }
    void setLineWidth( quint16 lineWidth ) { mLineWidth = lineWidth; }
    void setColor( const QColor& color ) { mColor = color; }
public:
    quint32 type() const { return mType; }
    quint16 lineWidth() const { return mLineWidth; }
    const QColor& color() const { return mColor; }
private:
    quint32 mType;
    quint16 mLineWidth;
    QColor mColor;
};

class CdrAbstractFill
{
public:
    enum Id { Transparent = 0, Solid = 1, Gradient = 2 };
protected:
    explicit CdrAbstractFill(Id id) : mId( id ) {}
private:
    CdrAbstractFill( const CdrAbstractFill& );
    CdrAbstractFill& operator=( const CdrAbstractFill& );
public:
    virtual ~CdrAbstractFill() {}
public:
    Id id() const { return mId; }
private:
    Id mId;
};

class CdrTransparentFill : public CdrAbstractFill
{
public:
    CdrTransparentFill() : CdrAbstractFill(Transparent) {}
};

class CdrSolidFill : public CdrAbstractFill
{
public:
    CdrSolidFill() : CdrAbstractFill(Solid) {}
public:
    void setColor( const QColor& color ) { mColor = color; }
public:
    const QColor& color() const { return mColor; }
private:
    QColor mColor;
};

class CdrFont
{
public:
    void setName( const QString& name ) { mName = name; }
public:
    const QString& name() const { return mName; }
private:
    QString mName;
};

class CdrDocument
{
public:
    ~CdrDocument();
public:
    void insertStyle( quint16 id, CdrStyle* style ) { mStyleTable.insert(id, style); }
    void insertOutline( quint32 id, CdrOutline* outline ) { mOutlineTable.insert(id, outline); }
    void insertFill( quint32 id, CdrAbstractFill* fill ) { mFillTable.insert(id, fill); }
    void insertFont( quint16 id, CdrFont* font ) { mFontTable.insert(id, font); }
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
    CdrStyle* style( quint16 id ) { return mStyleTable.value(id); }
    CdrOutline* outline( quint32 id ) { return mOutlineTable.value(id); }
    CdrAbstractFill* fill( quint32 id ) { return mFillTable.value(id); }
    CdrFont* font( quint16 id ) { return mFontTable.value(id); }
private:
    quint16 mFullVersion;
    quint16 mWidth;
    quint16 mHeight;
    QByteArray mStyleSheetFileName;

    QHash<quint16, CdrStyle*> mStyleTable;
    QHash<quint32, CdrOutline*> mOutlineTable;
    QHash<quint32, CdrAbstractFill*> mFillTable;
    QHash<quint16, CdrFont*> mFontTable;
    QVector<CdrPage*> mPages;
};

#endif
