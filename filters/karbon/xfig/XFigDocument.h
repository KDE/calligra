/*  This file is part of the Calligra project, made within the KDE community.

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

#ifndef XFIGDOCUMENT_H
#define XFIGDOCUMENT_H

// Qt
#include <QHash>
#include <QVector>
#include <QColor>
#include <QString>
#include <QFont>

typedef qint32 XFigCoord;


enum XFigCapType {
    XFigCapButt,
    XFigCapRound,
    XFigCapProjecting
};

enum XFigJoinType {
    XFigJoinMiter,
    XFigJoinRound,
    XFigJoinBevel
};

struct XFigPoint
{
public:
    XFigPoint() : mX(0), mY(0) {}
    XFigPoint( XFigCoord x, XFigCoord y ) : mX(x), mY(y) {}
public:
    XFigCoord x() const { return mX; }
    XFigCoord y() const { return mY; }
private:
    XFigCoord mX;
    XFigCoord mY;
};

enum XFigArrowHeadType
{
    XFigArrowHeadStick, ///>  -->
    XFigArrowHeadClosedTriangle, ///>  --|>
    XFigArrowHeadClosedIndentedButt, ///>  -->>
    XFigArrowHeadClosedPointedButt ///> --<>
};

class XFigArrowHead
{
public:
    XFigArrowHead()
    : mType(XFigArrowHeadStick), mIsHollow(false), mThickness(0.0), mWidth(0.0), mHeight(0.0)
    {}
public:
    void setType(XFigArrowHeadType type) { mType = type; }
    void setIsHollow(bool isHollow) { mIsHollow = isHollow; }
    void setThickness(double thickness) { mThickness = thickness; }
    void setSize(double width, double height) { mWidth = width; mHeight = height; }
public:
    XFigArrowHeadType type() const { return mType; }
    bool isHollow() const { return mIsHollow; }
    double thickness() const { return mThickness; }
    double width() const { return mWidth; }
    double height() const { return mHeight; }
private:
    XFigArrowHeadType mType;
    bool mIsHollow;
    double mThickness;
    double mWidth;
    double mHeight;
};

class XFigAbstractObject
{
public:
    enum TypeId
    {
        EllipseId,
        PolylineId,
        PolygonId,
        BoxId,
        PictureBoxId,
        SplineId,
        ArcId,
        TextId,
        CompoundId
    };

protected:
    explicit XFigAbstractObject(TypeId typeId) : mTypeId( typeId ) {}
private:
    XFigAbstractObject( const XFigAbstractObject& );
    XFigAbstractObject& operator=( const XFigAbstractObject& );
public:
    virtual ~XFigAbstractObject() {}
public:
    void setComment(const QString& comment) { m_Comment = comment; }

    TypeId typeId() const { return mTypeId; }
    const QString& comment() const { return m_Comment; }
private:
    TypeId mTypeId;
    QString m_Comment;
};


class XFigFillable
{
protected:
    XFigFillable() {}
public:
    void setFill( qint32 styleId, qint32 colorId ) { mFillStyleId = styleId; mFillColorId = colorId; }
public:
    qint32 fillStyleId() const { return mFillStyleId; }
    qint32 fillColorId() const { return mFillColorId; }
private:
    qint32 mFillStyleId;
    qint32 mFillColorId;
};

class XFigAbstractGraphObject : public XFigAbstractObject
{
protected:
    explicit XFigAbstractGraphObject(TypeId typeId)
    : XFigAbstractObject( typeId ) {}
public:
    void setDepth( qint32 depth ) { mDepth = depth; }
    void setPen( qint32 styleId, qint32 colorId ) { mPenStyleId = styleId; mPenColorId = colorId; }
public:
    qint32 depth() const { return mDepth; }
    qint32 penStyleId() const { return mPenStyleId; }
    qint32 penColorId() const { return mPenColorId; }
private:
    qint32 mDepth;
    qint32 mPenStyleId;
    qint32 mPenColorId;
};

enum XFigLineType
{
    XFigLineDefault = -1,
    XFigLineSolid = 0,
    XFigLineDashed = 1,
    XFigLineDotted = 2,
    XFigLineDashDotted = 3,
    XFigLineDashDoubleDotted = 4,
    XFigLineDashTrippleDotted = 5
};

class XFigLineable
{
protected:
    XFigLineable() {}
public:
    void setLine( XFigLineType type, qint32 thickness, float styleValue, qint32 colorId  )
    { mType = type; mThickness = thickness; mStyleValue = styleValue; mColorId = colorId; }
public:
    XFigLineType lineType() const { return mType; }
    qint32 lineThickness() const { return mThickness; }
    float lineStyleValue() const { return mStyleValue; }
    qint32 lineColorId() const { return mColorId; }
private:
    XFigLineType mType;
    qint32 mThickness;
    float mStyleValue;
    qint32 mColorId;
};

class XFigEllipseObject : public XFigAbstractGraphObject, public XFigFillable, public XFigLineable
{
public:
    enum Subtype { EllipseByRadii, EllipseByDiameter, CircleByRadius, CircleByDiameter };
public:
    XFigEllipseObject() : XFigAbstractGraphObject(EllipseId), mSubtype(EllipseByRadii)  {}
public:
    void setSubtype( Subtype subtype ) { mSubtype = subtype; }
    void setCenterPoint( XFigPoint centerPoint ) { mCenterPoint = centerPoint; }
    void setStartEnd( XFigPoint startPoint, XFigPoint endPoint ) { mStartPoint = startPoint; mEndPoint = endPoint; }
    void setRadii( qint32 xRadius, qint32 yRadius ) { mXRadius = xRadius; mYRadius = yRadius; }
    void setXAxisAngle( double xAxisAngle ) { mXAxisAngle = xAxisAngle; }
public:
    Subtype subtype() const { return mSubtype; }
    XFigPoint centerPoint() const { return mCenterPoint; }
    XFigPoint startPoint() const { return mStartPoint; }
    XFigPoint endPoint() const { return mEndPoint; }
    qint32 xRadius() const { return mXRadius; }
    qint32 yRadius() const { return mYRadius; }
    double xAxisAngle() const { return mXAxisAngle; }
private:
    Subtype mSubtype;
    XFigPoint mCenterPoint;
    XFigPoint mStartPoint;
    XFigPoint mEndPoint;
    qint32 mXRadius;
    qint32 mYRadius;
    double mXAxisAngle;
};

class XFigAbstractPolylineObject : public XFigAbstractGraphObject, public XFigFillable, public XFigLineable
{
protected:
    explicit XFigAbstractPolylineObject(TypeId typeId)
    : XFigAbstractGraphObject( typeId ), mJoinType(XFigJoinMiter)
    {}
public: // API to be implemented
    virtual void setPoints(const QVector<XFigPoint>& points) = 0;
public:
    void setJoinType(XFigJoinType joinType) { mJoinType = joinType; }
public:
    XFigJoinType joinType() const { return mJoinType; }
private:
    XFigJoinType mJoinType;
};

class XFigPolylineObject : public XFigAbstractPolylineObject
{
public:
    XFigPolylineObject()
    : XFigAbstractPolylineObject(PolylineId), mCapType(XFigCapButt), mForwardArrow(0), mBackwardArrow(0)
    {}
    ~XFigPolylineObject() { delete mForwardArrow; delete mBackwardArrow; }
public: // XFigAbstractPolylineObject API
    virtual void setPoints(const QVector<XFigPoint>& points) { mPoints = points; }
public:
    void setCapType(XFigCapType capType) { mCapType = capType; }
    void setForwardArrow( XFigArrowHead* forwardArrow ) { delete mForwardArrow; mForwardArrow = forwardArrow; }
    void setBackwardArrow( XFigArrowHead* backwardArrow ) { delete mBackwardArrow; mBackwardArrow = backwardArrow; }
public:
    const QVector<XFigPoint>& points() const { return mPoints; }
    XFigCapType capType() const { return mCapType; }
    const XFigArrowHead* forwardArrow() const { return mForwardArrow; }
    const XFigArrowHead* backwardArrow() const { return mBackwardArrow; }
private:
    QVector<XFigPoint> mPoints;
    XFigCapType mCapType;
    XFigArrowHead* mForwardArrow;
    XFigArrowHead* mBackwardArrow;
};

class XFigPolygonObject : public XFigAbstractPolylineObject
{
public:
    XFigPolygonObject() : XFigAbstractPolylineObject(PolygonId) {}
public: // XFigAbstractPolylineObject API
    virtual void setPoints(const QVector<XFigPoint>& points) { mPoints = points; }
public:
    const QVector<XFigPoint>& points() const { return mPoints; }
private:
    QVector<XFigPoint> mPoints;
};

class XFigBoxObject : public XFigAbstractPolylineObject
{
public:
    XFigBoxObject() : XFigAbstractPolylineObject(BoxId), mWidth(0), mHeight(0), mRadius(0) {}
protected:
    explicit XFigBoxObject(TypeId typeId)
    : XFigAbstractPolylineObject( typeId ), mWidth(0), mHeight(0), mRadius(0) {}
public: // XFigAbstractPolylineObject API
    virtual void setPoints(const QVector<XFigPoint>& points);
public:
    void setRadius( qint32 radius ) { mRadius = radius; }
public:
    XFigPoint upperLeft() const { return mUpperLeftCorner; }
    qint32 width() const { return mWidth; }
    qint32 height() const { return mHeight; }
    qint32 radius() const { return mRadius; }
private:
    XFigPoint mUpperLeftCorner;
    qint32 mWidth;
    qint32 mHeight;
    qint32 mRadius;
};

class XFigPictureBoxObject : public XFigBoxObject
{
public:
    XFigPictureBoxObject() : XFigBoxObject(PictureBoxId) {}
public:
    void setIsFlipped( bool isFlipped ) { mIsFlipped = isFlipped; }
    void setFileName( const QString& fileName ) { mFileName = fileName; }
public:
    bool isFlipped() const { return mIsFlipped; }
    const QString& fileName() const { return mFileName; }
private:
    bool mIsFlipped;
    QString mFileName;
};

class XFigSplineObject : public XFigAbstractGraphObject, public XFigFillable, public XFigLineable
{
public:
    enum Subtype { OpenApproximated, ClosedApproximated, OpenInterpolated, ClosedInterpolated, OpenX, ClosedX };
public:
    XFigSplineObject() : XFigAbstractGraphObject(SplineId), mSubtype(OpenApproximated) {}
public:
    void setSubtype( Subtype subtype ) { mSubtype = subtype; }
//     void addPathPoint( const XFigPathPoint& pathPoint ) { mPathPoints.append(pathPoint); }
public:
    Subtype subtype() const { return mSubtype; }
//     const QVector<XFigPathPoint>& pathPoints() const { return mPathPoints; }
private:
    Subtype mSubtype;
//     QVector<XFigPathPoint> mPathPoints;
};

class XFigArcObject : public XFigAbstractGraphObject, public XFigFillable, public XFigLineable
{
public:
    XFigArcObject() : XFigAbstractGraphObject(ArcId) {}
public:
//     void addPathPoint( const XFigPathPoint& pathPoint ) { mPathPoints.append(pathPoint); }
public:
//     const QVector<XFigPathPoint>& pathPoints() const { return mPathPoints; }
private:
//     QVector<XFigPathPoint> mPathPoints;
};


enum XFigTextAlignment {
    XFigTextLeftAligned,
    XFigTextCenterAligned,
    XFigTextRightAligned
};

struct XFigFontData
{
    QString mFamily;
    QFont::Weight mWeight;
    QFont::Style mStyle;
    float mSize; ///> in points
};

class XFigTextObject : public XFigAbstractGraphObject, public XFigFillable
{
public:
    XFigTextObject()
    : XFigAbstractGraphObject(TextId), mTextAlignment(XFigTextLeftAligned), mLength(0), mHeight(0) {}
public:
    void setText(const QString& text) { mText = text; }
    void setTextAlignment(XFigTextAlignment textAlignment) { mTextAlignment = textAlignment; }
    void setBaselineStartPoint(XFigPoint baselineStartPoint) { mBaselineStartPoint = baselineStartPoint; }
    void setSize(double length, double height) { mLength = length; mHeight = height; }
    void setXAxisAngle(double xAxisAngle) { mXAxisAngle = xAxisAngle; }
    void setColorId(qint32 colorId) { mColorId = colorId; }
    void setFontData(const XFigFontData& fontData) { mFontData = fontData; }
    void setIsHidden(bool isHidden) { mIsHidden = isHidden; }
public:
    const QString& text() const { return mText; }
    XFigTextAlignment textAlignment() const { return mTextAlignment; }
    XFigPoint baselineStartPoint() const { return mBaselineStartPoint; }
    double height() const { return mHeight; }
    double length() const { return mLength; }
    double xAxisAngle() const { return mXAxisAngle; }
    qint32 colorId() const { return mColorId; }
    const XFigFontData& fontData() const { return mFontData; }
    bool isHidden() const { return mIsHidden; }
private:
    QString mText;
    XFigTextAlignment mTextAlignment;
    XFigPoint mBaselineStartPoint;
    double mLength;
    double mHeight;
    double mXAxisAngle;
    qint32 mColorId;
    XFigFontData mFontData;
    bool mIsHidden :1;
};


class XFigBoundingBox
{
public:
    XFigBoundingBox() {}
    XFigBoundingBox( XFigPoint upperLeft, XFigPoint lowerRight)
    : mUpperLeft(upperLeft), mLowerRight(lowerRight)
    {}
public:
    void setUpperLeft( XFigPoint upperLeft ) { mUpperLeft = upperLeft; }
    void setLowerRight( XFigPoint lowerRight ) { mLowerRight = lowerRight; }
public:
    XFigPoint upperLeft() const { return mUpperLeft; }
    XFigPoint lowerRight() const { return mLowerRight; }
private:
    XFigPoint mUpperLeft;
    XFigPoint mLowerRight;
};

class XFigCompoundObject : public XFigAbstractObject
{
public:
    XFigCompoundObject() : XFigAbstractObject(CompoundId) {}
public:
    virtual ~XFigCompoundObject() { qDeleteAll( mObjects );}
public:
    void addObject( XFigAbstractObject* object ) { mObjects.append(object); }
    void setBoundingBox( XFigBoundingBox boundingBox ) { mBoundingBox = boundingBox; }
public:
    const QVector<XFigAbstractObject*>& objects() const { return mObjects; }
    XFigBoundingBox boundingBox() const { return mBoundingBox; }
private:
    QVector<XFigAbstractObject*> mObjects;
    XFigBoundingBox mBoundingBox;
};

class XFigPage
{
public:
    XFigPage() {}
    ~XFigPage() { qDeleteAll( mObjects ); }
public:
    void addObject( XFigAbstractObject* object ) { mObjects.append(object); }
public:
    const QVector<XFigAbstractObject*>& objects() const { return mObjects; }
private:
    QVector<XFigAbstractObject*> mObjects;
};

enum XFigUnitType
{
    XFigUnitTypeUnknown,
    XFigUnitMetric,
    XFigUnitInches
};

enum XFigCoordSystemOriginType
{
    XFigCoordSystemOriginTypeUnknown,
    XFigCoordSystemOriginUpperLeft,
    XFigCoordSystemOriginLowerLeft
};

enum XFigPageOrientation
{
    XFigPageOrientationUnknown,
    XFigPagePortrait,
    XFigPageLandscape
};

enum XFigPageSizeType
{
    XFigPageSizeUnknown,
    XFigPageSizeLetter,
    XFigPageSizeLegal,
    XFigPageSizeLedger,
    XFigPageSizeTabloid,
    XFigPageSizeA,
    XFigPageSizeB,
    XFigPageSizeC,
    XFigPageSizeD,
    XFigPageSizeE,
    XFigPageSizeA4,
    XFigPageSizeA3,
    XFigPageSizeA2,
    XFigPageSizeA1,
    XFigPageSizeA0,
    XFigPageSizeB5
};

class XFigDocument
{
public:
    XFigDocument();
    ~XFigDocument() { qDeleteAll( mPages); }
public:
    void setPageOrientation( XFigPageOrientation pageOrientation ) { mPageOrientation = pageOrientation; }
    void setCoordSystemOriginType( XFigCoordSystemOriginType coordSystemOriginType )
    { mCoordSystemOriginType = coordSystemOriginType; }
    void setUnitType( XFigUnitType unitType ) { mUnitType = unitType; }
    void setPageSizeType( XFigPageSizeType pageSizeType ) { mPageSizeType = pageSizeType; }
    void setResolution( qint32 resolution ) { mResolution = resolution; }
    void setComment(const QString& comment) { m_Comment = comment; }
    void addPage( XFigPage* page ) { mPages.append(page); }
    void setUserColor( int id, const QColor& color )
    { if ((32<=id) && (id<=543)) mColorTable.insert(id, color); }
public:
    XFigPageOrientation pageOrientation() const { return mPageOrientation; }
    XFigCoordSystemOriginType coordSystemOriginType() const { return mCoordSystemOriginType; }
    XFigUnitType unitType() const { return mUnitType; }
    XFigPageSizeType pageSizeType() const { return mPageSizeType; }
    qint32 resolution() const { return mResolution; }
    const QString& comment() const { return m_Comment; }
    const QVector<XFigPage*>& pages() const { return mPages; }
    const QColor* color( int id ) const;
private:
    XFigPageOrientation mPageOrientation;
    XFigCoordSystemOriginType mCoordSystemOriginType;
    XFigUnitType mUnitType;
    XFigPageSizeType mPageSizeType;
    qint32 mResolution;
    QString m_Comment;

    QHash<int, QColor> mColorTable;

    QVector<XFigPage*> mPages;
};

#endif
