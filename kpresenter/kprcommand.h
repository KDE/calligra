// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2001 Laurent Montel <lmontel@mandrakesoft.com>
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef kprcommand_h
#define kprcommand_h

#include <kcommand.h>
#include <qptrlist.h>
#include <qpoint.h>
#include <qcolor.h>
#include <qsize.h>
#include "global.h"
#include <koPictureCollection.h>
#include <qvaluelist.h>
#include <qpen.h>
#include <qbrush.h>
#include <koPageLayoutDia.h>
#include <koparaglayout.h>
#include <kocommand.h>
#include <koPoint.h>
#include <koSize.h>
#include <qvariant.h>
#include <qvaluevector.h>
#include "kpbackground.h"
#include "kprvariable.h"

class KPresenterDoc;
class KPTextObject;
class KPObject;
class KP2DObject;
class KPPixmapObject;
class KPGroupObject;
class KPresenterView;
class KoParagLayout;
class KPrPage;
class KoCustomVariable;
class KoLinkVariable;
class KPPolylineObject;
class KPrFieldVariable;
class KPrTimeVariable;
class KPrDateVariable;
class KPrPgNumVariable;

class ShadowCmd : public KNamedCommand
{
public:
    struct ShadowValues
    {
        int shadowDistance;
        ShadowDirection shadowDirection;
        QColor shadowColor;
    };

    ShadowCmd( const QString &_name, QPtrList<ShadowValues> &_oldShadow, ShadowValues _newShadow,
               QPtrList<KPObject> &_objects, KPresenterDoc *_doc );
    ~ShadowCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    KPresenterDoc *doc;
    QPtrList<ShadowValues> oldShadow;
    QPtrList<KPObject> objects;
    ShadowValues newShadow;
    KPrPage *m_page;
};

class SetOptionsCmd : public KNamedCommand
{
public:
    SetOptionsCmd( const QString &_name, QValueList<KoPoint> &_diffs, QPtrList<KPObject> &_objects,
                   double _rastX, double _rastY, double _orastX, double _orastY,
                   const QColor &_txtBackCol, const QColor &_otxtBackCol, KPresenterDoc *_doc );
    ~SetOptionsCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    QValueList<KoPoint> diffs;
    QPtrList<KPObject> objects;
    double gridX;
    double gridY;
    double oldGridX;
    double oldGridY;
    KPresenterDoc *doc;
    QColor txtBackCol;
    QColor otxtBackCol;
};

class SetBackCmd : public KNamedCommand
{
public:
    SetBackCmd( const QString &name, const KPBackGround::Settings &settings,
                const KPBackGround::Settings &oldSettings,
                bool useMasterBackground,
                bool takeGlobal, KPresenterDoc *doc, KPrPage *page );

    virtual void execute();
    virtual void unexecute();

protected:

    KPBackGround::Settings m_settings;
    KPBackGround::Settings m_oldSettings;
    bool m_useMasterBackground;
    bool m_oldUseMasterBackground;
    bool m_takeGlobal;
    KPresenterDoc *m_doc;
    KPrPage *m_page;
};

class RotateCmd : public KNamedCommand
{
public:
    struct RotateValues
    {
        float angle;
    };

    RotateCmd( const QString &_name, float newAngle, QPtrList<KPObject> &objects,
               KPresenterDoc *doc, bool addAngle = false );
    ~RotateCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    KPresenterDoc *m_doc;
    QPtrList<RotateValues> m_oldAngles;
    QPtrList<KPObject> m_objects;
    float m_newAngle;
    //necessary for duplicate object, we can duplicated and add angle.
    bool m_addAngle;
    KPrPage *m_page;
};

class ResizeCmd : public KNamedCommand
{
public:
    ResizeCmd( const QString &_name, const KoPoint &_m_diff, const KoSize &_r_diff,
               KPObject *_object, KPresenterDoc *_doc );
    ~ResizeCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    KoPoint m_diff;
    KoSize r_diff;
    KPObject *object;
    KPresenterDoc *doc;
    KPrPage *m_page;
};

class ChgPixCmd : public KNamedCommand
{
public:
    ChgPixCmd( const QString &_name, KPPixmapObject *_oldObject, KPPixmapObject *_newObject,
               KPresenterDoc *_doc, KPrPage *_page );
    ~ChgPixCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    KPPixmapObject *oldObject, *newObject;
    KPresenterDoc *doc;
    KPrPage *m_page;
};

class DeleteCmd : public KNamedCommand
{
public:
    DeleteCmd( const QString &_name, QPtrList<KPObject> &_objects, KPresenterDoc *_doc,
               KPrPage *_page);
    ~DeleteCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    QPtrList<KPObject> m_oldObjectList;
    QPtrList<KPObject> m_objectsToDelete;
    KPresenterDoc *m_doc;
    KPrPage *m_page;
};

class EffectCmd : public KNamedCommand
{
public:
    struct EffectStruct {
        int appearStep, disappearStep;
        Effect effect;
        Effect2 effect2;
        Effect3 effect3;
        EffectSpeed m_appearSpeed;
        EffectSpeed m_disappearSpeed;
        bool disappear;
        int appearTimer, disappearTimer;
        bool appearSoundEffect, disappearSoundEffect;
        QString a_fileName, d_fileName;
    };

    EffectCmd( const QString &_name, const QPtrList<KPObject> &_objs,
               const QValueList<EffectStruct> &_oldEffects, EffectStruct _newEffect );
    ~EffectCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    QValueList<EffectStruct> oldEffects;
    EffectStruct newEffect;
    QPtrList<KPObject> objs;
};

class GroupObjCmd : public KNamedCommand
{
public:
    GroupObjCmd( const QString &_name,
                 const QPtrList<KPObject> &_objects,
                 KPresenterDoc *_doc, KPrPage *_page );
    ~GroupObjCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    QPtrList<KPObject> m_objectsToGroup;
    QPtrList<KPObject> m_oldObjectList;
    KPresenterDoc *m_doc;
    KPGroupObject *m_groupObject;
    KPrPage *m_page;

};

class UnGroupObjCmd : public KNamedCommand
{
public:
    UnGroupObjCmd( const QString &_name,
                   KPGroupObject *grpObj_,
                   KPresenterDoc *_doc, KPrPage *_page );
    ~UnGroupObjCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    QPtrList<KPObject> m_groupedObjects;
    KPGroupObject *m_groupObject;
    KPresenterDoc *m_doc;
    KPrPage *m_page;
};


class InsertCmd : public KNamedCommand
{
public:
    InsertCmd( const QString &_name, KPObject *_object, KPresenterDoc *_doc, KPrPage *_page );
    ~InsertCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    KPObject *object;
    KPresenterDoc *doc;
    KPrPage *m_page;
};


class LowerRaiseCmd : public KNamedCommand
{
public:
    LowerRaiseCmd( const QString &_name, QPtrList<KPObject> _oldList, QPtrList<KPObject> _newList,
                   KPresenterDoc *_doc, KPrPage *_page );
    ~LowerRaiseCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    QPtrList<KPObject> oldList, newList;
    KPresenterDoc *doc;
    KPrPage *m_page;
};

class MoveByCmd : public KNamedCommand
{
public:
    MoveByCmd( const QString &_name, const KoPoint &_diff, QPtrList<KPObject> &_objects,
               KPresenterDoc *_doc, KPrPage *_page );
    ~MoveByCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    KoPoint diff;
    QPtrList<KPObject> objects;
    KPresenterDoc *doc;
    KPrPage *m_page;
};

/**
 * Command to align objects
 */
class AlignCmd : public KNamedCommand
{
public:
    AlignCmd( const QString &_name, QPtrList<KPObject> &_objects, AlignType _at, KPresenterDoc *_doc );
    ~AlignCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    QPtrList<KoPoint> diffs;
    QPtrList<KPObject> objects;
    KPresenterDoc *doc;
    KPrPage *m_page;
};

class PenCmd : public KNamedCommand
{
public:
    struct Pen {
        Pen( QPen p, LineEnd lb, LineEnd le )
        : pen( p )
        , lineBegin( lb )
        , lineEnd( le )
        {}

        QPen pen;
        LineEnd lineBegin, lineEnd;

        Pen &operator=( const Pen &_pen ) {
            pen  = _pen.pen;
            lineBegin = _pen.lineBegin;
            lineEnd = _pen.lineEnd;
            return *this;
        }
    };

    // the flags indicate what has changed
    enum PenConfigChange {
        LineBegin = 1,
        LineEnd = 2,
        Color = 4,
        Width = 8,
        Style = 16,
        All = LineBegin | LineEnd | Color | Width | Style
    };

    PenCmd( const QString &_name, QPtrList<KPObject> &_objects, Pen _newPen,
            KPresenterDoc *_doc, KPrPage *_page, int _flags = All);
    ~PenCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    void addObjects( const QPtrList<KPObject> &_objects );
    void applyPen( KPObject *object, Pen *tmpPen );

    KPresenterDoc *doc;
    KPrPage *m_page;
    QPtrList<Pen> oldPen;
    QPtrList<KPObject> objects;
    Pen newPen;
    int flags;
};

class BrushCmd : public KNamedCommand
{
public:
    struct Brush {
        Brush() {}
        Brush( QBrush _brush, QColor _gColor1, QColor _gColor2,
               BCType _gType, FillType _fillType, bool _unbalanced, 
               int _xfactor, int _yfactor )
            : brush( _brush )
            , gColor1( _gColor1 )
            , gColor2( _gColor2 )
            , gType( _gType )
            , fillType( _fillType )
            , unbalanced( _unbalanced )
            , xfactor( _xfactor )
            , yfactor( _yfactor )
            {}

        QBrush brush;
        QColor gColor1;
        QColor gColor2;
        BCType gType;
        FillType fillType;
        bool unbalanced;
        int xfactor, yfactor;

        Brush &operator=( const Brush &_brush ) {
            brush = _brush.brush;
            gColor1 = _brush.gColor1;
            gColor2 = _brush.gColor2;
            gType = _brush.gType;
            fillType = _brush.fillType;
            unbalanced = _brush.unbalanced;
            xfactor = _brush.xfactor;
            yfactor = _brush.yfactor;
            return *this;
        }
    };

    // the flags indicate what has changed
    enum BrushConfigChange {
        BrushColor = 1,
        BrushStyle = 2,
        BrushGradientSelect = 4,
        GradientColor1 = 8,
        GradientColor2 = 16,
        GradientType = 32,
        GradientBalanced = 64,
        GradientXFactor = 128,
        GradientYFactor = 256,
        All = BrushColor | BrushStyle | BrushGradientSelect | GradientColor1 | GradientColor2 | GradientType | GradientBalanced | GradientXFactor | GradientYFactor
    };

    BrushCmd( const QString &_name, QPtrList<KPObject> &_objects, Brush _newBrush,
              KPresenterDoc *_doc, KPrPage *_page, int _flags = All );
    ~BrushCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    void addObjects( const QPtrList<KPObject> &_objects );
    void applyBrush( KP2DObject *object, Brush *tmpBrush );

    KPresenterDoc *doc;
    QPtrList<Brush> oldBrush;
    QPtrList<KP2DObject> objects;
    Brush newBrush;
    KPrPage *m_page;
    int flags;
};

class PgConfCmd : public KNamedCommand
{
public:
    PgConfCmd( const QString &_name, bool _manualSwitch, bool _infiniteLoop,
               bool _showPresentationDuration, QPen _pen,
               QValueList<bool> _selectedSlides, const QString & _presentationName,
               bool _oldManualSwitch, bool _oldInfiniteLoop,
               bool _oldShowPresentationDuration, QPen _oldPen,
               QValueList<bool> _oldSelectedSlides, const QString & _oldPresentationName,
               KPresenterDoc *_doc );

    virtual void execute();
    virtual void unexecute();

protected:
    bool manualSwitch, oldManualSwitch;
    bool infiniteLoop, oldInfiniteLoop;
    bool showPresentationDuration, oldShowPresentationDuration;
    QPen pen, oldPen;
    QValueList<bool> selectedSlides, oldSelectedSlides;
    QString oldPresentationName, presentationName;

    KPresenterDoc *doc;
};

class TransEffectCmd : public KCommand
{
public:
    struct PageEffectSettings {
        PageEffect pageEffect;
        EffectSpeed effectSpeed;
        bool soundEffect;
        QString soundFileName;
        bool autoAdvance;
        int slideTime;
        void applyTo( KPrPage * );
    };
    // TODO also pass a flag to know which settings need to be applied (especially for "all pages")
    TransEffectCmd( QValueVector<PageEffectSettings> oldSettings,
                    PageEffectSettings newSettings,
                    KPrPage* page, KPresenterDoc* doc );

    virtual void execute();
    virtual void unexecute();
    virtual QString name() const;

private:
    void exec( KPrPage* page );
    void unexec( KPrPage* page );

    QValueVector<PageEffectSettings> m_oldSettings;
    PageEffectSettings m_newSettings;
    KPrPage *m_page;
    KPresenterDoc *m_doc;
};

class PgLayoutCmd : public KNamedCommand
{
public:
    PgLayoutCmd( const QString &_name, KoPageLayout _layout, KoPageLayout _oldLayout,
                 KoUnit::Unit _oldUnit, KoUnit::Unit _unit,KPresenterDoc *_doc );

    virtual void execute();
    virtual void unexecute();

protected:
    KPresenterDoc *m_doc;
    KoPageLayout layout, oldLayout;
    KoUnit::Unit unit, oldUnit;
};

class PieValueCmd : public KNamedCommand
{
public:
    struct PieValues
    {
        PieType pieType;
        int pieAngle, pieLength;
    };

    // the flags indicate what has changed
    enum PieConfigChange {
        Type = 1,
        Angle = 2,
        Length = 4,
        All = Type | Angle | Length
    };

    PieValueCmd( const QString &_name, QPtrList<PieValues> &_oldValues, PieValues _newValues,
                 QPtrList<KPObject> &_objects, KPresenterDoc *_doc, KPrPage *_page, int _flags = All );
    ~PieValueCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    KPresenterDoc *doc;
    KPrPage *m_page;
    QPtrList<PieValues> oldValues;
    QPtrList<KPObject> objects;
    PieValues newValues;
    int flags;
};

class PolygonSettingCmd : public KNamedCommand
{
public:
    struct PolygonSettings
    {
        bool checkConcavePolygon;
        int cornersValue;
        int sharpnessValue;
    };

    // the flags indicate what has changed
    enum PolygonConfigChange {
        ConcaveConvex = 1,
        Corners = 2,
        Sharpness = 4,
        All = ConcaveConvex | Corners | Sharpness
    };

    PolygonSettingCmd( const QString &_name, QPtrList<PolygonSettings> &_oldSettings,
                       PolygonSettings _newSettings, QPtrList<KPObject> &_objects,
                       KPresenterDoc *_doc,  KPrPage *_page, int _flags = All );
    ~PolygonSettingCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    KPresenterDoc *doc;
    KPrPage *m_page;
    QPtrList<PolygonSettings> oldSettings;
    QPtrList<KPObject> objects;
    PolygonSettings newSettings;
    int flags;
};

class PictureSettingCmd : public KNamedCommand
{
public:
    struct PictureSettings
    {
        PictureMirrorType mirrorType;
        int depth;
        bool swapRGB;
        bool grayscal;
        int bright;
    };

    PictureSettingCmd( const QString &_name, QPtrList<PictureSettings> &_oldSettings,
                       PictureSettings _newSettings, QPtrList<KPObject> &_objects,
                       KPresenterDoc *_doc );
    ~PictureSettingCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    KPresenterDoc *doc;
    QPtrList<PictureSettings> oldSettings;
    QPtrList<KPObject> objects;
    PictureSettings newSettings;
    KPrPage *m_page;
};

class ImageEffectCmd : public KNamedCommand
{
public:
    struct ImageEffectSettings
    {
        ImageEffect effect;
        QVariant param1;
        QVariant param2;
        QVariant param3;
    };

    ImageEffectCmd(const QString &_name, QPtrList<ImageEffectSettings> &_oldSettings,
                   ImageEffectSettings _newSettings, QPtrList<KPObject> &_objects,
                   KPresenterDoc *_doc );
    ~ImageEffectCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    KPresenterDoc *doc;
    QPtrList<ImageEffectSettings> oldSettings;
    QPtrList<KPObject> objects;
    ImageEffectSettings newSettings;
    KPrPage *m_page;

};

class RectValueCmd : public KNamedCommand
{
public:
    struct RectValues
    {
        int xRnd, yRnd;
    };

    // the flags indicate what has changed
    enum RectangleConfigChange {
        XRnd = 1,
        YRnd = 2,
        All = XRnd | YRnd
    };

    RectValueCmd( const QString &_name, QPtrList<RectValues> &_oldValues, RectValues _newValues,
                  QPtrList<KPObject> &_objects, KPresenterDoc *_doc, KPrPage *_page, int _flags = All );
    RectValueCmd( const QString &_name, QPtrList<KPObject> &_objects, RectValues _newValues,
                  KPresenterDoc *_doc, KPrPage *_page, int _flags = All );
    ~RectValueCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    void addObjects( const QPtrList<KPObject> &_objects );

    KPresenterDoc *m_doc;
    KPrPage *m_page;
    QPtrList<RectValues> m_oldValues;
    QPtrList<KPObject> m_objects;
    RectValues m_newValues;
    int m_flags;
};

class KPrDeletePageCmd : public KNamedCommand
{
public:
    KPrDeletePageCmd( const QString &name, int pageNum, KPresenterDoc *doc );
    ~KPrDeletePageCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    KPresenterDoc *m_doc;
    KPrPage *m_page;
    int m_pageNum;
};

class KPrInsertPageCmd : public KNamedCommand
{
public:
    KPrInsertPageCmd( const QString &name, int currentPageNum, InsertPos pos,
                      KPrPage *page, KPresenterDoc *doc );
    ~KPrInsertPageCmd();

    virtual void execute();
    virtual void unexecute();
protected:
    KPresenterDoc *m_doc;
    KPrPage *m_page;
    int m_currentPageNum;
    int m_insertPageNum;
};

class KPrMovePageCmd : public KNamedCommand
{
public:
    KPrMovePageCmd( const QString &_name, int from,int to, KPresenterDoc *_doc );
    ~KPrMovePageCmd();

    virtual void execute();
    virtual void unexecute();
protected:
    KPresenterDoc *m_doc;
    int m_oldPosition;
    int m_newPosition;
};

/**
 * Command created when pasting oasis-formatted text
 */
class KPrOasisPasteTextCommand : public KoTextDocCommand
{
public:
    KPrOasisPasteTextCommand( KoTextDocument *d, int parag, int idx,
                             const QCString & data );
    KoTextCursor *execute( KoTextCursor *c );
    KoTextCursor *unexecute( KoTextCursor *c );
protected:
    int m_parag;
    int m_idx;
    QCString m_data;
    // filled in by execute(), for unexecute()
    int m_lastParag;
    int m_lastIndex;
    KoParagLayout* m_oldParagLayout;
};

struct ParagBookmark {
    QString m_bookName;
    int m_startParagIndex;
    int m_endParagIndex;
};

/**
 * Command to change variable setting
 */
class KPrChangeStartingPageCommand : public KNamedCommand
{
public:
    KPrChangeStartingPageCommand( const QString &name, KPresenterDoc *_doc,
                                  int _oldStartingPage, int _newStartingPage);
    ~KPrChangeStartingPageCommand(){}

    void execute();
    void unexecute();
protected:
    KPresenterDoc *m_doc;
    int oldStartingPage;
    int newStartingPage;
};

/**
 * Command to display link setting
 */
class KPrChangeVariableSettingsCommand : public KNamedCommand
{
public:
    enum VariableProperties { VS_DISPLAYLINK, VS_UNDERLINELINK, VS_DISPLAYCOMMENT, VS_DISPLAYFIELDCODE};
    KPrChangeVariableSettingsCommand( const QString &name, KPresenterDoc *_doc, bool _oldValue,
                                      bool _newValue, VariableProperties _type);
    ~KPrChangeVariableSettingsCommand(){}

    void execute();
    void unexecute();
protected:
    void changeValue( bool b );
    KPresenterDoc *m_doc;
    VariableProperties type;
    bool m_bOldValue;
    bool m_bNewValue;
};

/**
 * Command to change title page name
 */
class KPrChangeTitlePageNameCommand : public KNamedCommand
{
public:
    KPrChangeTitlePageNameCommand( const QString &name, KPresenterDoc *_doc, const QString &_oldPageName,
                                   const QString &_newPageName, KPrPage *_page);
    ~KPrChangeTitlePageNameCommand(){}

    void execute();
    void unexecute();
protected:
    KPresenterDoc *m_doc;
    QString oldPageName;
    QString newPageName;
    KPrPage *m_page;
};

class KPrChangeCustomVariableValue : public KNamedCommand
{
public:
    KPrChangeCustomVariableValue( const QString &name, KPresenterDoc *_doc,const QString & _oldValue,
                                  const QString & _newValue, KoCustomVariable *var);

    void execute();
    void unexecute();
protected:
    KPresenterDoc *m_doc;
    QString newValue;
    QString oldValue;
    KoCustomVariable *m_var;
};

class KPrChangeLinkVariable : public KNamedCommand
{
public:
    KPrChangeLinkVariable( const QString &name, KPresenterDoc *_doc,const QString & _oldHref,
                           const QString & _newHref, const QString & _oldLink,const QString &_newLink, KoLinkVariable *var);
    ~KPrChangeLinkVariable(){};
    void execute();
    void unexecute();
protected:
    KPresenterDoc *m_doc;
    QString oldHref;
    QString newHref;
    QString oldLink;
    QString newLink;
    KoLinkVariable *m_var;
};

class KPrStickyObjCommand : public KNamedCommand
{
public:
    KPrStickyObjCommand( const QString &_name, QPtrList<KPObject> &_objects, bool sticky,
                         KPrPage*_page,KPresenterDoc *_doc );

    ~KPrStickyObjCommand();
    void execute();
    void unexecute();
    void stickObj(KPObject *_obj);
    void unstickObj(KPObject *_obj);
protected:
    QPtrList<KPObject> objects;
    bool m_bSticky;
    KPresenterDoc *m_doc;
    KPrPage*m_page;
};

class KPrNameObjectCommand : public KNamedCommand
{
public:
    KPrNameObjectCommand( const QString &_name, const QString &_objectName, KPObject *_obj, KPresenterDoc *_doc );
    ~KPrNameObjectCommand();
    void execute();
    void unexecute();
protected:
    QString oldObjectName, newObjectName;
    KPObject *object;
    KPresenterDoc *doc;
    KPrPage *m_page;
};

class KPrHideShowHeaderFooter : public KNamedCommand
{
public:
    KPrHideShowHeaderFooter( const QString &name, KPresenterDoc *_doc, KPrPage *_page, bool _newValue,KPTextObject *_textObject);
    ~KPrHideShowHeaderFooter(){};
    void execute();
    void unexecute();
protected:
    KPresenterDoc *m_doc;
    KPrPage *m_page;
    KPTextObject *m_textObject;
    bool newValue;
};

class KPrFlipObjectCommand : public KNamedCommand
{
public:
    KPrFlipObjectCommand( const QString &name, KPresenterDoc *_doc, bool _horizontal ,
                          QPtrList<KPObject> &_objects );
    ~KPrFlipObjectCommand();
    void execute();
    void unexecute();
protected:
    void flipObjects();
    KPresenterDoc *m_doc;
    QPtrList<KPObject> objects;
    bool horizontal;
    KPrPage *m_page;
};

class KPrGeometryPropertiesCommand : public KNamedCommand
{
public:
    enum KgpType { ProtectSize, KeepRatio};
    KPrGeometryPropertiesCommand( const QString &name, QPtrList<KPObject> &objects,
                                  bool newValue, KgpType type );
    KPrGeometryPropertiesCommand( const QString &name, QValueList<bool> &lst, QPtrList<KPObject> &objects,
                                  bool newValue, KgpType type );
    ~KPrGeometryPropertiesCommand();

    virtual void execute();
    virtual void unexecute();

protected:
    QValueList<bool> m_oldValue;
    QPtrList<KPObject> m_objects;
    bool m_newValue;
    KgpType m_type;
};

class KPrProtectContentCommand : public KNamedCommand
{
public:
    KPrProtectContentCommand( const QString &_name, bool _protectContent, KPTextObject *_obj, KPresenterDoc *_doc );

    ~KPrProtectContentCommand();
    virtual void execute();
    virtual void unexecute();

protected:
    bool protectContent;
    KPTextObject *objects;
    KPresenterDoc * doc;
};

class KPrCloseObjectCommand : public KNamedCommand
{
public:
    KPrCloseObjectCommand( const QString &_name, KPObject *_obj, KPresenterDoc *_doc );

    ~KPrCloseObjectCommand();
    virtual void execute();
    virtual void unexecute();

protected:
    void closeObject(bool close);

    KPObject *objects;
    KPresenterDoc * doc;
    KPrPage *m_page;
};

struct MarginsStruct {
    MarginsStruct() {}
    MarginsStruct( KPTextObject *obj );
    MarginsStruct( double _left, double top, double right, double bottom );
    double topMargin;
    double bottomMargin;
    double leftMargin;
    double rightMargin;
};

class KPrChangeMarginCommand : public KNamedCommand
{
public:
    KPrChangeMarginCommand( const QString &name, KPTextObject *_obj, MarginsStruct _MarginsBegin,
                            MarginsStruct _MarginsEnd, KPresenterDoc *_doc );
    ~KPrChangeMarginCommand() {}

    virtual void execute();
    virtual void unexecute();
protected:
    KPTextObject *m_obj;
    MarginsStruct m_marginsBegin;
    MarginsStruct m_marginsEnd;
    KPrPage *m_page;
    KPresenterDoc *m_doc;
};


class KPrChangeVerticalAlignmentCommand : public KNamedCommand
{
public:
    KPrChangeVerticalAlignmentCommand( const QString &name, KPTextObject *_obj, VerticalAlignmentType _oldAlign,
                                       VerticalAlignmentType _newAlign, KPresenterDoc *_doc);
    ~KPrChangeVerticalAlignmentCommand() {}

    virtual void execute();
    virtual void unexecute();
protected:
    KPTextObject *m_obj;
    VerticalAlignmentType m_oldAlign;
    VerticalAlignmentType m_newAlign;
    KPrPage *m_page;
    KPresenterDoc *m_doc;
};


class KPrChangeTabStopValueCommand : public KNamedCommand
{
public:
    KPrChangeTabStopValueCommand( const QString &name, double _oldValue, double _newValue, KPresenterDoc *_doc);

    ~KPrChangeTabStopValueCommand() {}

    virtual void execute();
    virtual void unexecute();
protected:
    KPresenterDoc *m_doc;
    double m_oldValue;
    double m_newValue;
};

class KPrChangeVariableNoteText : public KNamedCommand
{
 public:
    KPrChangeVariableNoteText( const QString &name, KPresenterDoc *_doc, const QString &_oldValue,const QString &_newValue, KoNoteVariable *var);
    ~KPrChangeVariableNoteText();
    void execute();
    void unexecute();
 protected:
    KPresenterDoc *m_doc;
    QString newValue;
    QString oldValue;
    KoNoteVariable *m_var;
};


#endif
