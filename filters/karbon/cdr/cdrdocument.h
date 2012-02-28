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
    CdrPoint() : mX(0), mY(0) {}
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


enum CdrFontWeight
{
    CdrFontWeightUnknown,
    CdrFontNormal,
    CdrFontBold
};

enum CdrTextAlignment
{
    CdrTextAlignmentUnknown = 0,
    CdrTextAlignLeft = 1,
    CdrTextAlignCenter = 2,
    CdrTextAlignRight = 3,
    CdrTextAlignBlock = 4
};

class CdrAbstractTextSpan
{
public:
    enum Id { Normal = 0, Styled };
protected:
    explicit CdrAbstractTextSpan(Id id) : mId( id ) {}
private:
    CdrAbstractTextSpan( const CdrAbstractTextSpan& );
    CdrAbstractTextSpan& operator=( const CdrAbstractTextSpan& );
public:
    virtual ~CdrAbstractTextSpan() {}
    void setText( const QString& text ) { mText = text; }
public:
    Id id() const { return mId; }
    const QString& text() const { return mText; }
private:
    Id mId;
protected:
    QString mText;
};

class CdrNormalTextSpan : public CdrAbstractTextSpan
{
public:
    CdrNormalTextSpan() : CdrAbstractTextSpan(Normal) {}
};

class CdrStyledTextSpan : public CdrAbstractTextSpan
{
public:
    CdrStyledTextSpan() : CdrAbstractTextSpan(Styled),
    mFontId(-1), mFontSize(0), mFontWeight(CdrFontWeightUnknown)
    {}
    void appendText( const QString& text ) { mText.append(text); }
    void setFontId( quint16 fontId ) { mFontId = fontId; }
    void setFontSize( quint16 fontSize ) { mFontSize = fontSize; }
    void setFontWeight( CdrFontWeight fontWeight ) { mFontWeight = fontWeight; }
public:
    bool isFontDataEqual( const CdrStyledTextSpan& other ) const
    { return (mFontId == other.mFontId) && (mFontSize == other.mFontSize) && (mFontWeight == other.mFontWeight); }
    qint32 fontId() const { return mFontId; }
    quint16 fontSize() const { return mFontSize; }
    CdrFontWeight fontWeight() const { return mFontWeight; }
private:
    qint32 mFontId;
    quint16 mFontSize;
    CdrFontWeight mFontWeight;
};

class CdrParagraphLine
{
public:
    ~CdrParagraphLine() { qDeleteAll(mTextSpans);}
    void addTextSpan( CdrAbstractTextSpan* textSpan );
    void setNextLineOffset( CdrPoint nextLineOffset ) { mNextLineOffset = nextLineOffset; }
public:
    const QVector<CdrAbstractTextSpan*>& textSpans() const { return mTextSpans; }
    CdrPoint nextLineOffset() const { return mNextLineOffset; }
private:
    QVector<CdrAbstractTextSpan*> mTextSpans;
    CdrPoint mNextLineOffset;
};

class CdrParagraph
{
public:
    CdrParagraph() : mStyleId(-1) {}
    ~CdrParagraph() { qDeleteAll(mParagraphLines);}
    void setStyleId( qint32 styleId ) { mStyleId = styleId; }
    void addParagraphLine( CdrParagraphLine* paragraphLine ) { mParagraphLines.append(paragraphLine); }
public:
    qint16 styleId() const { return mStyleId; }
    const QVector<CdrParagraphLine*>& paragraphLines() const { return mParagraphLines; }
private:
    qint16 mStyleId;
    QVector<CdrParagraphLine*> mParagraphLines;
};

struct CdrParagraphLineIndex
{
    CdrParagraphLineIndex() : mParagraphIndex(0), mLineIndex(0) {} // TODO: set maxint
    CdrParagraphLineIndex( quint16 paragraphIndex, quint16 lineIndex)
    : mParagraphIndex(paragraphIndex), mLineIndex(lineIndex) {}

    quint16 mParagraphIndex;
    quint16 mLineIndex;
};

struct CdrBlockTextPartSpan
{
    CdrBlockTextPartSpan() {}
    CdrBlockTextPartSpan( CdrParagraphLineIndex begin, CdrParagraphLineIndex end)
    : mBegin(begin), mEnd(end) {}

    CdrParagraphLineIndex mBegin;
    CdrParagraphLineIndex mEnd;
};

class CdrBlockText
{
public:
    ~CdrBlockText() { qDeleteAll(mParagraphs);}
    void addParagraph( CdrParagraph* paragraph );
public:
    const QVector<CdrParagraph*>& paragraphs() const { return mParagraphs; }
    CdrBlockTextPartSpan partSpan( quint16 id ) const;
private:
    QVector<CdrParagraph*> mParagraphs;
    QVector<CdrParagraphLineIndex> mParagraphPartStarts;
};


class CdrGraphObject : public CdrAbstractObject
{
protected:
    explicit CdrGraphObject(CdrObjectTypeId id)
    : CdrAbstractObject( id ), mStyleId(-1), mOutlineId(-1), mFillId(-1) {}
public:
    void setStyleId( qint32 styleId ) { mStyleId = styleId; }
    void setOutlineId( qint32 outlineId ) { mOutlineId = outlineId; }
    void setFillId( qint32 fillId ) { mFillId = fillId; }
public:
    qint16 styleId() const { return mStyleId; }
    qint32 outlineId() const { return mOutlineId; }
    qint32 fillId() const { return mFillId; }
private:
    qint16 mStyleId;
    qint32 mOutlineId;
    qint32 mFillId;
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
    CdrBlockTextObject() : CdrGraphObject(BlockTextObjectId), mWidth(0), mHeight(0) {}
public:
    void setWidth( quint16 width ) { mWidth = width; }
    void setHeight( quint16 height ) { mHeight = height; }
public:
    quint16 width() const { return mWidth; }
    quint16 height() const { return mHeight; }
private:
    quint16 mWidth;
    quint16 mHeight;
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

class CdrFontData
{
public:
    CdrFontData()
    : mFontId(-1), mFontSize(0), mFontWeight(CdrFontWeightUnknown)
    {}
public:
    void setFontId( quint16 fontId ) { mFontId = fontId; }
    void setFontSize( quint16 fontSize ) { mFontSize = fontSize; }
    void setFontWeight( CdrFontWeight fontWeight ) { mFontWeight = fontWeight; }
public:
    qint32 fontId() const { return mFontId; }
    quint16 fontSize() const { return mFontSize; }
    CdrFontWeight fontWeight() const { return mFontWeight; }
private:
    qint32 mFontId;
    quint16 mFontSize;
    CdrFontWeight mFontWeight;
};

class CdrTextMargins
{
public:
    CdrTextMargins()
    : mLeftMargin(0), mRightMargin(0), mTopMargin(0), mBottomMargin(0)
    {}
public:
    void setLeftMargin( qint16 leftMargin ) { mLeftMargin = leftMargin; }
    void setRightMargin( qint16 rightMargin ) { mRightMargin = rightMargin; }
    void setTopMargin( qint16 topMargin ) { mTopMargin = topMargin; }
    void setBottomMargin( qint16 bottomMargin ) { mBottomMargin = bottomMargin; }
public:
    qint32 leftMargin() const { return mLeftMargin; }
    qint32 rightMargin() const { return mRightMargin; }
    qint32 topMargin() const { return mTopMargin; }
    qint32 bottomMargin() const { return mBottomMargin; }
private:
    qint32 mLeftMargin;
    qint32 mRightMargin;
    qint32 mTopMargin;
    qint32 mBottomMargin;
};

class CdrStyle
{
public:
    CdrStyle()
    : mBaseStyle(0), mFontData(0), mTextMargins(0), mTextAlignment(CdrTextAlignmentUnknown)
    {}
    ~CdrStyle() { delete mFontData; delete mTextMargins; }
public:
    void setBaseStyle( const CdrStyle* baseStyle ) { mBaseStyle = baseStyle; }
    void setTitle( const QString& title ) { mTitle = title; }
    void setFontData( CdrFontData* fontData ) { delete mFontData; mFontData = fontData; }
    void setTextMargins( CdrTextMargins* textMargins ) { delete mTextMargins; mTextMargins = textMargins; }
    void setTextAlignment( CdrTextAlignment alignment ) { mTextAlignment = alignment; }
public:
    const CdrStyle* baseStyle() const { return mBaseStyle; }
    const QString& title() const { return mTitle; }
    const CdrFontData* fontData() const
    { return mFontData ? mFontData : mBaseStyle ? mBaseStyle->fontData() : 0; }
    const CdrTextMargins* textMargins() const
    { return mTextMargins ? mTextMargins : mBaseStyle ? mBaseStyle->textMargins() : 0; }
    CdrTextAlignment textAlignment() const
    {
        return (mTextAlignment!=CdrTextAlignmentUnknown) ? mTextAlignment :
               mBaseStyle ?                                mBaseStyle->textAlignment() :
                                                           CdrTextAlignmentUnknown;
    }
private:
    const CdrStyle* mBaseStyle;
    CdrFontData* mFontData;
    CdrTextMargins* mTextMargins;
    CdrTextAlignment mTextAlignment;
    QString mTitle;
};

enum CdrStrokeCapType
{
    CdrStrokeButtCap,
    CdrStrokeRoundCap,
    CdrStrokeSquareCap
};

enum CdrStrokeJoinType
{
    CdrStrokeMiterJoin,
    CdrStrokeRoundJoin,
    CdrStrokeBevelJoin
};

class CdrOutline
{
public:
    CdrOutline() : mCapType(CdrStrokeButtCap), mJoinType(CdrStrokeMiterJoin), mStrokeWidth(0) {}
public:
    void setStrokeType( quint32 strokeType ) { mStrokeType = strokeType; }
    void setStrokeCapType( CdrStrokeCapType capType ) { mCapType = capType; }
    void setStrokeJoinType( CdrStrokeJoinType joinType ) { mJoinType = joinType; }
    void setStrokeWidth( quint16 strokeWidth ) { mStrokeWidth = strokeWidth; }
    void setColor( const QColor& color ) { mColor = color; }
public:
    quint32 strokeType() const { return mStrokeType; }
    CdrStrokeCapType strokeCapType() const { return mCapType; }
    CdrStrokeJoinType strokeJoinType() const { return mJoinType; }
    quint16 strokeWidth() const { return mStrokeWidth; }
    const QColor& color() const { return mColor; }
private:
    quint32 mStrokeType;
    CdrStrokeCapType mCapType;
    CdrStrokeJoinType mJoinType;
    quint16 mStrokeWidth;
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

struct CdrBlockTextPartIndex
{
    CdrBlockTextPartIndex() {}
    CdrBlockTextPartIndex( quint16 blockTextId, quint16 partId )
    : mBlockTextId(blockTextId), mPartId(partId) {}

    quint16 mBlockTextId;
    quint16 mPartId;
};

typedef QHash<quint16,CdrBlockTextPartIndex> CdrBlockTextLinkTable;

enum CdrPageOrientation
{
    CdrPageOrientationUnknown,
    CdrPagePortrait,
    CdrPageLandscape
};

enum CdrPageSizeType
{
    CdrPageSizeTypeUnknown,
    CdrPageSizeDinA4,
    CdrPageSizeDinA5
};

class CdrDocument
{
public:
    CdrDocument()
    : mWidth(0), mHeight(0),
      mPageOrientation(CdrPageOrientationUnknown), mPageSizeType(CdrPageSizeTypeUnknown),
      mMasterPage(0)
    {}
    ~CdrDocument();
public:
    void insertStyle( quint16 id, CdrStyle* style ) { mStyleTable.insert(id, style); }
    void insertOutline( quint32 id, CdrOutline* outline ) { mOutlineTable.insert(id, outline); }
    void insertFill( quint32 id, CdrAbstractFill* fill ) { mFillTable.insert(id, fill); }
    void insertFont( quint16 id, CdrFont* font ) { mFontTable.insert(id, font); }
    void insertBlockText( quint16 id, CdrBlockText* blockText ) { mBlockTextTable.insert(id, blockText); }
    void setMasterPage( CdrPage* page ) { mMasterPage = page; }
    void addPage( CdrPage* page ) { mPages.append(page); }
    void setFullVersion( quint16 fullVersion ) { mFullVersion = fullVersion; }
    void setSize( quint16 width, quint16 height ) { mWidth = width; mHeight = height; }
    void setStyleSheetFileName( const QString& styleSheetFileName ) { mStyleSheetFileName = styleSheetFileName; }
    void setPageOrientation( CdrPageOrientation pageOrientation ) { mPageOrientation = pageOrientation; }
    void setPageSizeType( CdrPageSizeType pageSizeType ) { mPageSizeType = pageSizeType; }
    void setBlockTextLinkTable( const CdrBlockTextLinkTable& blockTextLinkTable ) { mBlockTextLinkTable = blockTextLinkTable; }
public:
    quint16 fullVersion() const { return mFullVersion; }
    quint16 width() const { return mWidth; }
    quint16 height() const { return mHeight; }
    const CdrPage* masterPage() const { return mMasterPage; }
    const QVector<CdrPage*>& pages() const { return mPages; }
    const QString& styleSheetFileName() const { return mStyleSheetFileName; }
    CdrPageOrientation pageOrientation() const { return mPageOrientation; }
    CdrPageSizeType pageSizeType() const { return mPageSizeType; }
    const CdrStyle* style( qint32 id ) { return mStyleTable.value(id); }
    const CdrOutline* outline( qint32 id ) { return mOutlineTable.value(id); }
    const CdrAbstractFill* fill( qint32 id ) { return mFillTable.value(id); }
    const CdrFont* font( quint16 id ) { return mFontTable.value(id); }
    const CdrBlockText* blockText( quint16 id ) { return mBlockTextTable.value(id); }
    CdrBlockTextPartIndex blockTextPartIndex( CdrObjectId id ) { return mBlockTextLinkTable.value(id); }
private:
    quint16 mFullVersion;
    quint16 mWidth;
    quint16 mHeight;
    QString mStyleSheetFileName;
    CdrPageOrientation mPageOrientation;
    CdrPageSizeType mPageSizeType;

    QHash<qint32, CdrStyle*> mStyleTable;
    QHash<qint32, CdrOutline*> mOutlineTable;
    QHash<qint32, CdrAbstractFill*> mFillTable;
    QHash<quint16, CdrFont*> mFontTable;
    QHash<quint16, CdrBlockText*> mBlockTextTable;
    QVector<CdrPage*> mPages;
    CdrPage* mMasterPage;
    CdrBlockTextLinkTable mBlockTextLinkTable;
};

#endif
