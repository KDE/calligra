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
#include "cdr4structs.h"
// Qt
#include <QtGui/QColor>
#include <QtCore/QHash>
#include <QtCore/QVector>
#include <QtCore/QString>


class CdrAbstractTransformation
{
public:
    enum Id { Normal = 0 };
protected:
    explicit CdrAbstractTransformation(Id id) : mId( id ) {}
private:
    CdrAbstractTransformation( const CdrAbstractTransformation& );
    CdrAbstractTransformation& operator=( const CdrAbstractTransformation& );
public:
    virtual ~CdrAbstractTransformation() {}
public:
    Id id() const { return mId; }
private:
    Id mId;
};

class CdrNormalTransformation : public CdrAbstractTransformation
{
public:
    CdrNormalTransformation() : CdrAbstractTransformation(Normal) {}
    void setData( double a, double c, qint32 e, double b, double d, qint32 f )
    { mA = a; mC = c; mE = e; mB = b; mD = d; mF = f; }
    qint32 e() const { return mE; }
    qint32 f() const { return mF; }
    double a() const { return mA; }
    double b() const { return mB; }
    double c() const { return mC; }
    double d() const { return mD; }
private:
    double mA;
    double mC;
    qint32 mE;
    double mB;
    double mD;
    qint32 mF;
};



enum CdrObjectTypeId
{
    PathObjectId,
    RectangleObjectId,
    EllipseObjectId,
    GraphicTextObjectId,
    BlockTextObjectId,
    GroupObjectId
};
typedef quint16 CdrObjectId;
static const CdrObjectId cdrObjectInvalidId = 0;


class CdrAbstractObject
{
protected:
    explicit CdrAbstractObject(CdrObjectTypeId typeId) : mObjectId( cdrObjectInvalidId ), mTypeId( typeId ) {}
private:
    CdrAbstractObject( const CdrAbstractObject& );
    CdrAbstractObject& operator=( const CdrAbstractObject& );
public:
    virtual ~CdrAbstractObject() { qDeleteAll(mTransformations);}
public:
    void setTransformations( const QVector<CdrAbstractTransformation*>& transformations )
    { mTransformations = transformations; }
    void setObjectId( CdrObjectId objectId ) { mObjectId = objectId; }
    CdrObjectId objectId() const { return mObjectId; }
    CdrObjectTypeId typeId() const { return mTypeId; }
    const QVector<CdrAbstractTransformation*>& transformations() const { return mTransformations; }
private:
    CdrObjectId mObjectId;
    CdrObjectTypeId mTypeId;
    QVector<CdrAbstractTransformation*> mTransformations;
};

typedef qint16 CdrCoord;

typedef quint16 CdrPointType;
struct CdrPoint
{
public:
    CdrPoint() {}
    CdrPoint( CdrCoord x, CdrCoord y ) : mX(x), mY(y) {}
public:
    CdrCoord x() const { return mX; }
    CdrCoord y() const { return mY; }
private:
    CdrCoord mX;
    CdrCoord mY;
};


struct CdrPathPoint
{
    CdrPathPoint() : mPoint(0,0) {}
    CdrPathPoint( CdrPoint point, CdrPointType type ) : mPoint(point), mType(type) {}

    CdrPoint mPoint;
    CdrPointType mType;
};


enum CdrTextAlignment
{
    CdrTextAlignmentUnknown = 0,
    CdrTextAlignLeft = 1,
    CdrTextAlignCenter = 2,
    CdrTextAlignRight = 3
};

struct CdrParagraph
{
public:
    void setText( const QString& text ) { mText = text; }
public:
    const QString& text() const { return mText; }
private:
    QString mText;
};

class CdrBlockText
{
public:
    ~CdrBlockText() { qDeleteAll(mParagraphs);}
    void addParagraph( CdrParagraph* paragraph ) { mParagraphs.append(paragraph); }
public:
    const QVector<CdrParagraph*>& paragraphs() const { return mParagraphs; }
private:
    QVector<CdrParagraph*> mParagraphs;
};


class CdrGraphObject : public CdrAbstractObject
{
protected:
    explicit CdrGraphObject(CdrObjectTypeId id)
    : CdrAbstractObject( id ), mStyleId(0), mOutlineId(0), mFillId(0) {}
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
    void setCornerPoint( CdrPoint cornerPoint ) { mCornerPoint = cornerPoint; }
public:
    CdrPoint cornerPoint() const { return mCornerPoint; }
private:
    CdrPoint mCornerPoint;
};

class CdrEllipseObject : public CdrGraphObject
{
public:
    CdrEllipseObject() : CdrGraphObject(EllipseObjectId) {}
public:
    void setCornerPoint( CdrPoint cornerPoint ) { mCornerPoint = cornerPoint; }
    void setStartAngle( quint16 startAngle ) { mStartAngle = startAngle; }
    void setEndAngle( quint16 endAngle ) { mEndAngle = endAngle; }
public:
    CdrPoint cornerPoint() const { return mCornerPoint; }
    quint16 startAngle() const { return mStartAngle; }
    quint16 endAngle() const { return mEndAngle; }
private:
    CdrPoint mCornerPoint;
    quint16 mStartAngle;
    quint16 mEndAngle;
};

class CdrPathObject : public CdrGraphObject
{
public:
    CdrPathObject() : CdrGraphObject(PathObjectId) {}
public:
    void addPathPoint( const CdrPathPoint& pathPoint ) { mPathPoints.append(pathPoint); }
public:
    const QVector<CdrPathPoint>& pathPoints() const { return mPathPoints; }
private:
    QVector<CdrPathPoint> mPathPoints;
};

class CdrGraphicTextObject : public CdrGraphObject
{
public:
    CdrGraphicTextObject() : CdrGraphObject(GraphicTextObjectId) {}
public:
    void setText( const QString& text ) { mText = text; }
public:
    const QString& text() const { return mText; }
private:
    QString mText;
};

class CdrBlockTextObject : public CdrGraphObject
{
public:
    CdrBlockTextObject() : CdrGraphObject(BlockTextObjectId) {}
};

class CdrGroupObject : public CdrAbstractObject
{
public:
    CdrGroupObject() : CdrAbstractObject(GroupObjectId) {}
public:
    virtual ~CdrGroupObject() { qDeleteAll( mObjects );}
public:
    // TODO: prepend instead of append here is just workaround for inversed order needed for odf (or not done z-index)
    void addObject( CdrAbstractObject* object ) { mObjects.prepend(object); }
public:
    const QVector<CdrAbstractObject*>& objects() const { return mObjects; }
private:
    QVector<CdrAbstractObject*> mObjects;
};

class CdrLinkGroupObject : public CdrGroupObject //tmp for now
{
};

class CdrLayer
{
public:
    ~CdrLayer() { qDeleteAll( mObjects );}
public:
    // TODO: prepend instead of append here is just workaround for inversed order needed for odf (or not done z-index)
    void addObject( CdrAbstractObject* object ) { mObjects.prepend(object); }
public:
    const QVector<CdrAbstractObject*>& objects() const { return mObjects; }
private:
    QVector<CdrAbstractObject*> mObjects;
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
    CdrStyle() : mBaseStyle(0), mFontId(0), mFontSize(0), mTextAlignment(CdrTextAlignmentUnknown) {}
public:
    void setBaseStyle( const CdrStyle* baseStyle ) { mBaseStyle = baseStyle; }
    void setTitle( const QString& title ) { mTitle = title; }
    void setFontId( quint16 fontId ) { mFontId = fontId; }
    void setFontSize( quint16 fontSize ) { mFontSize = fontSize; }
    void setTextAlignment( CdrTextAlignment alignment ) { mTextAlignment = alignment; }
public:
    const CdrStyle* baseStyle() const { return mBaseStyle; }

    const QString& title() const { return mTitle; }

    quint16 fontId() const
    { return (mFontId!=0) ? mFontId : mBaseStyle ? mBaseStyle->fontId() : 0; }
    quint16 fontSize() const
    { return (mFontSize!=0) ? mFontSize : mBaseStyle ? mBaseStyle->fontSize() : 18; }
    CdrTextAlignment textAlignment() const
    {
        return (mTextAlignment!=CdrTextAlignmentUnknown) ? mTextAlignment :
               mBaseStyle ?                                mBaseStyle->textAlignment() :
                                                           CdrTextAlignmentUnknown;
    }
private:
    const CdrStyle* mBaseStyle;
    quint16 mFontId;
    quint16 mFontSize;
    CdrTextAlignment mTextAlignment;
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
    void insertBlockText( quint16 id, CdrBlockText* blockText ) { mBlockTextTable.insert(id, blockText); }
    void addPage( CdrPage* page ) { mPages.append(page); }
    void setFullVersion( quint16 fullVersion ) { mFullVersion = fullVersion; }
    void setSize( quint16 width, quint16 height ) { mWidth = width; mHeight = height; }
    void setStyleSheetFileName( const QString& styleSheetFileName ) { mStyleSheetFileName = styleSheetFileName; }
public:
    quint16 fullVersion() const { return mFullVersion; }
    quint16 width() const { return mWidth; }
    quint16 height() const { return mHeight; }
    const QVector<CdrPage*>& pages() const { return mPages; }
    const QString& styleSheetFileName() const { return mStyleSheetFileName; }
    CdrStyle* style( quint16 id ) { return mStyleTable.value(id); }
    CdrOutline* outline( quint32 id ) { return mOutlineTable.value(id); }
    CdrAbstractFill* fill( quint32 id ) { return mFillTable.value(id); }
    CdrFont* font( quint16 id ) { return mFontTable.value(id); }
    CdrBlockText* blockText( quint16 id ) { return mBlockTextTable.value(id); }
private:
    quint16 mFullVersion;
    quint16 mWidth;
    quint16 mHeight;
    QString mStyleSheetFileName;

    QHash<quint16, CdrStyle*> mStyleTable;
    QHash<quint32, CdrOutline*> mOutlineTable;
    QHash<quint32, CdrAbstractFill*> mFillTable;
    QHash<quint16, CdrFont*> mFontTable;
    QHash<quint16, CdrBlockText*> mBlockTextTable;
    QVector<CdrPage*> mPages;
};

#endif
