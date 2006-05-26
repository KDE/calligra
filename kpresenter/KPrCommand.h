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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef kprcommand_h
#define kprcommand_h

#include <kcommand.h>
#include <q3ptrlist.h>
#include <QPoint>
#include <QColor>
#include <qsize.h>
#include "global.h"
#include <KoPictureCollection.h>
#include <q3valuelist.h>
#include <qpen.h>
#include <qbrush.h>
#include <KoPageLayoutDia.h>
#include <KoParagLayout.h>
#include <KoTextCommand.h>
#include <KoPoint.h>
#include <KoSize.h>
#include <QVariant>
#include <q3valuevector.h>
#include "KPrBackground.h"
#include "KPrVariableCollection.h"
#include <KoPen.h>

class KPrDocument;
class KPrTextObject;
class KPrObject;
class KPr2DObject;
class KPrPixmapObject;
class KPrGroupObject;
class KPrView;
class KoParagLayout;
class KPrPage;
class KoCustomVariable;
class KoLinkVariable;
class KPrPolylineObject;
class KPrFieldVariable;
class KPrTimeVariable;
class KPrDateVariable;
class KPrPgNumVariable;

class KPrShadowCmd : public KNamedCommand
{
public:
    struct ShadowValues
    {
        int shadowDistance;
        ShadowDirection shadowDirection;
        QColor shadowColor;
    };

    KPrShadowCmd( const QString &_name, Q3PtrList<ShadowValues> &_oldShadow, ShadowValues _newShadow,
               Q3PtrList<KPrObject> &_objects, KPrDocument *_doc );
    ~KPrShadowCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    KPrDocument *doc;
    Q3PtrList<ShadowValues> oldShadow;
    Q3PtrList<KPrObject> objects;
    ShadowValues newShadow;
    KPrPage *m_page;
};

class KPrSetOptionsCmd : public KNamedCommand
{
public:
    KPrSetOptionsCmd( const QString &_name, Q3ValueList<KoPoint> &_diffs, Q3PtrList<KPrObject> &_objects,
                   double _rastX, double _rastY, double _orastX, double _orastY,
                   const QColor &_txtBackCol, const QColor &_otxtBackCol, KPrDocument *_doc );
    ~KPrSetOptionsCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    Q3ValueList<KoPoint> diffs;
    Q3PtrList<KPrObject> objects;
    double gridX;
    double gridY;
    double oldGridX;
    double oldGridY;
    KPrDocument *doc;
    QColor txtBackCol;
    QColor otxtBackCol;
};

class KPrSetBackCmd : public KNamedCommand
{
public:
    KPrSetBackCmd( const QString &name, const KPrBackGround::Settings &settings,
                const KPrBackGround::Settings &oldSettings,
                bool useMasterBackground,
                bool takeGlobal, KPrDocument *doc, KPrPage *page );

    virtual void execute();
    virtual void unexecute();

protected:

    KPrBackGround::Settings m_settings;
    KPrBackGround::Settings m_oldSettings;
    bool m_useMasterBackground;
    bool m_oldUseMasterBackground;
    bool m_takeGlobal;
    KPrDocument *m_doc;
    KPrPage *m_page;
};

class KPrRotateCmd : public KNamedCommand
{
public:
    struct RotateValues
    {
        float angle;
    };

    KPrRotateCmd( const QString &_name, float newAngle, Q3PtrList<KPrObject> &objects,
               KPrDocument *doc, bool addAngle = false );
    ~KPrRotateCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    KPrDocument *m_doc;
    Q3PtrList<RotateValues> m_oldAngles;
    Q3PtrList<KPrObject> m_objects;
    float m_newAngle;
    //necessary for duplicate object, we can duplicated and add angle.
    bool m_addAngle;
    KPrPage *m_page;
};

class KPrResizeCmd : public KNamedCommand
{
public:
    KPrResizeCmd( const QString &_name, const KoPoint &_m_diff, const KoSize &_r_diff,
               KPrObject *_object, KPrDocument *_doc );
    ~KPrResizeCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    KoPoint m_diff;
    KoSize r_diff;
    KPrObject *object;
    KPrDocument *doc;
    KPrPage *m_page;
};

class KPrChgPixCmd : public KNamedCommand
{
public:
    KPrChgPixCmd( const QString &_name, KPrPixmapObject *_oldObject, KPrPixmapObject *_newObject,
               KPrDocument *_doc, KPrPage *_page );
    ~KPrChgPixCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    KPrPixmapObject *oldObject, *newObject;
    KPrDocument *doc;
    KPrPage *m_page;
};

class KPrDeleteCmd : public KNamedCommand
{
public:
    KPrDeleteCmd( const QString &_name, Q3PtrList<KPrObject> &_objects, KPrDocument *_doc,
               KPrPage *_page);
    ~KPrDeleteCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    Q3PtrList<KPrObject> m_oldObjectList;
    Q3PtrList<KPrObject> m_objectsToDelete;
    KPrDocument *m_doc;
    KPrPage *m_page;
};

class KPrEffectCmd : public KNamedCommand
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

    KPrEffectCmd( const QString &_name, const Q3PtrList<KPrObject> &_objs,
               const Q3ValueList<EffectStruct> &_oldEffects, EffectStruct _newEffect );
    ~KPrEffectCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    Q3ValueList<EffectStruct> oldEffects;
    EffectStruct newEffect;
    Q3PtrList<KPrObject> objs;
};

class KPrGroupObjCmd : public KNamedCommand
{
public:
    KPrGroupObjCmd( const QString &_name,
                 const Q3PtrList<KPrObject> &_objects,
                 KPrDocument *_doc, KPrPage *_page );
    ~KPrGroupObjCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    Q3PtrList<KPrObject> m_objectsToGroup;
    Q3PtrList<KPrObject> m_oldObjectList;
    KPrDocument *m_doc;
    KPrGroupObject *m_groupObject;
    KPrPage *m_page;

};

class UnGroupObjCmd : public KNamedCommand
{
public:
    UnGroupObjCmd( const QString &_name,
                   KPrGroupObject *grpObj_,
                   KPrDocument *_doc, KPrPage *_page );
    ~UnGroupObjCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    Q3PtrList<KPrObject> m_groupedObjects;
    KPrGroupObject *m_groupObject;
    KPrDocument *m_doc;
    KPrPage *m_page;
};


class KPrInsertCmd : public KNamedCommand
{
public:
    KPrInsertCmd( const QString &name, const Q3ValueList<KPrObject *> objects, KPrDocument *doc, KPrPage *page );
    KPrInsertCmd( const QString &name, KPrObject *object, KPrDocument *doc, KPrPage *page );
    ~KPrInsertCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    Q3ValueList<KPrObject *> m_objects;
    KPrObject *m_object;
    KPrDocument *m_doc;
    KPrPage *m_page;
};


class KPrLowerRaiseCmd : public KNamedCommand
{
public:
    KPrLowerRaiseCmd( const QString &_name, const Q3PtrList<KPrObject>& _oldList,
                   const Q3PtrList<KPrObject>& _newList,
                   KPrDocument *_doc, KPrPage *_page );
    ~KPrLowerRaiseCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    Q3PtrList<KPrObject> oldList, newList;
    KPrDocument *doc;
    KPrPage *m_page;
};

class KPrMoveByCmd : public KNamedCommand
{
public:
    KPrMoveByCmd( const QString &_name, const KoPoint &_diff, Q3PtrList<KPrObject> &_objects,
               KPrDocument *_doc, KPrPage *_page );
    ~KPrMoveByCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    KoPoint diff;
    Q3PtrList<KPrObject> objects;
    KPrDocument *doc;
    KPrPage *m_page;
};

/**
 * Command to align objects
 */
class KPrAlignCmd : public KNamedCommand
{
public:
    KPrAlignCmd( const QString &_name, Q3PtrList<KPrObject> &_objects, AlignType _at, KPrDocument *_doc );
    ~KPrAlignCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    Q3PtrList<KoPoint> diffs;
    Q3PtrList<KPrObject> objects;
    KPrDocument *doc;
    KPrPage *m_page;
};

class KoPenCmd : public KNamedCommand
{
public:
    struct Pen {
        Pen( KoPen p, LineEnd lb, LineEnd le )
        : pen( p )
        , lineBegin( lb )
        , lineEnd( le )
        {}

        KoPen pen;
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

    KoPenCmd( const QString &_name, Q3PtrList<KPrObject> &_objects, Pen _newPen,
            KPrDocument *_doc, KPrPage *_page, int _flags = All);
    ~KoPenCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    void addObjects( const Q3PtrList<KPrObject> &_objects );
    void applyPen( KPrObject *object, Pen *tmpPen );

    KPrDocument *doc;
    KPrPage *m_page;
    Q3PtrList<Pen> oldPen;
    Q3PtrList<KPrObject> objects;
    Pen newPen;
    int flags;
};

class KPrBrushCmd : public KNamedCommand
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

    KPrBrushCmd( const QString &_name, Q3PtrList<KPrObject> &_objects, Brush _newBrush,
              KPrDocument *_doc, KPrPage *_page, int _flags = All );
    ~KPrBrushCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    void addObjects( const Q3PtrList<KPrObject> &_objects );
    void applyBrush( KPr2DObject *object, Brush *tmpBrush );

    KPrDocument *doc;
    Q3PtrList<Brush> oldBrush;
    Q3PtrList<KPr2DObject> objects;
    Brush newBrush;
    KPrPage *m_page;
    int flags;
};

class KPrPgConfCmd : public KNamedCommand
{
public:
    KPrPgConfCmd( const QString &_name, bool _manualSwitch, bool _infiniteLoop,
               bool _showPresentationDuration, QPen _pen,
               Q3ValueList<bool> _selectedSlides, const QString & _presentationName,
               bool _oldManualSwitch, bool _oldInfiniteLoop,
               bool _oldShowPresentationDuration, QPen _oldPen,
               Q3ValueList<bool> _oldSelectedSlides, const QString & _oldPresentationName,
               KPrDocument *_doc );

    virtual void execute();
    virtual void unexecute();

protected:
    bool manualSwitch, oldManualSwitch;
    bool infiniteLoop, oldInfiniteLoop;
    bool showPresentationDuration, oldShowPresentationDuration;
    QPen pen, oldPen;
    Q3ValueList<bool> selectedSlides, oldSelectedSlides;
    QString oldPresentationName, presentationName;

    KPrDocument *doc;
};

class KPrTransEffectCmd : public KCommand
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
    KPrTransEffectCmd( Q3ValueVector<PageEffectSettings> oldSettings,
                    PageEffectSettings newSettings,
                    KPrPage* page, KPrDocument* doc );

    virtual void execute();
    virtual void unexecute();
    virtual QString name() const;

private:
    void exec( KPrPage* page );
    void unexec( KPrPage* page );

    Q3ValueVector<PageEffectSettings> m_oldSettings;
    PageEffectSettings m_newSettings;
    KPrPage *m_page;
    KPrDocument *m_doc;
};

class KPrPgLayoutCmd : public KNamedCommand
{
public:
    KPrPgLayoutCmd( const QString &_name, KoPageLayout _layout, KoPageLayout _oldLayout,
                 KoUnit::Unit _oldUnit, KoUnit::Unit _unit,KPrDocument *_doc );

    virtual void execute();
    virtual void unexecute();

protected:
    KPrDocument *m_doc;
    KoPageLayout layout, oldLayout;
    KoUnit::Unit unit, oldUnit;
};

class KPrPieValueCmd : public KNamedCommand
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

    KPrPieValueCmd( const QString &name, PieValues newValues,
                 Q3PtrList<KPrObject> &objects, KPrDocument *doc,
                 KPrPage *page, int flags = All );
    KPrPieValueCmd( const QString &_name, Q3PtrList<PieValues> &_oldValues, PieValues _newValues,
                 Q3PtrList<KPrObject> &_objects, KPrDocument *_doc, KPrPage *_page, int _flags = All );
    ~KPrPieValueCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    void addObjects( const Q3PtrList<KPrObject> &objects );

    KPrDocument *m_doc;
    KPrPage *m_page;
    Q3PtrList<PieValues> m_oldValues;
    Q3PtrList<KPrObject> m_objects;
    PieValues m_newValues;
    int m_flags;
};

class KPrPolygonSettingCmd : public KNamedCommand
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

    KPrPolygonSettingCmd( const QString &name, PolygonSettings newSettings,
                          Q3PtrList<KPrObject> &objects, KPrDocument *doc,
                          KPrPage *page, int flags = All );
    ~KPrPolygonSettingCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    void addObjects( const Q3PtrList<KPrObject> &objects );

    KPrDocument *m_doc;
    KPrPage *m_page;
    Q3PtrList<PolygonSettings> m_oldSettings;
    Q3PtrList<KPrObject> m_objects;
    PolygonSettings m_newSettings;
    int m_flags;
};

class KPrPictureSettingCmd : public KNamedCommand
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

    enum PictureConfigChange {
        MirrorType = 1,
        Depth = 2,
        SwapRGB = 4,
        Grayscal = 8,
        Bright = 16,
        All = MirrorType | Depth | SwapRGB | Grayscal | Bright
    };

    KPrPictureSettingCmd( const QString &name, PictureSettings newSettings,
                       Q3PtrList<KPrObject> &objects, KPrDocument *doc,
                       KPrPage *page, int flags = All );
    KPrPictureSettingCmd( const QString &_name, Q3PtrList<PictureSettings> &_oldSettings,
                       PictureSettings _newSettings, Q3PtrList<KPrObject> &_objects,
                       KPrDocument *_doc, int flags = All );
    ~KPrPictureSettingCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    void addObjects( const Q3PtrList<KPrObject> &_objects );

    KPrDocument *m_doc;
    Q3PtrList<PictureSettings> m_oldValues;
    Q3PtrList<KPrObject> m_objects;
    PictureSettings m_newSettings;
    KPrPage *m_page;
    int m_flags;
};

class KPrImageEffectCmd : public KNamedCommand
{
public:
    struct ImageEffectSettings
    {
        ImageEffect effect;
        QVariant param1;
        QVariant param2;
        QVariant param3;
    };

    KPrImageEffectCmd(const QString &_name, Q3PtrList<ImageEffectSettings> &_oldSettings,
                   ImageEffectSettings _newSettings, Q3PtrList<KPrObject> &_objects,
                   KPrDocument *_doc );
    ~KPrImageEffectCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    KPrDocument *doc;
    Q3PtrList<ImageEffectSettings> oldSettings;
    Q3PtrList<KPrObject> objects;
    ImageEffectSettings newSettings;
    KPrPage *m_page;

};

class KPrRectValueCmd : public KNamedCommand
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

    KPrRectValueCmd( const QString &_name, Q3PtrList<RectValues> &_oldValues, RectValues _newValues,
                  Q3PtrList<KPrObject> &_objects, KPrDocument *_doc, KPrPage *_page, int _flags = All );
    KPrRectValueCmd( const QString &_name, Q3PtrList<KPrObject> &_objects, RectValues _newValues,
                  KPrDocument *_doc, KPrPage *_page, int _flags = All );
    ~KPrRectValueCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    void addObjects( const Q3PtrList<KPrObject> &_objects );

    KPrDocument *m_doc;
    KPrPage *m_page;
    Q3PtrList<RectValues> m_oldValues;
    Q3PtrList<KPrObject> m_objects;
    RectValues m_newValues;
    int m_flags;
};

class KPrDeletePageCmd : public KNamedCommand
{
public:
    KPrDeletePageCmd( const QString &name, int pageNum, KPrDocument *doc );
    ~KPrDeletePageCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    KPrDocument *m_doc;
    KPrPage *m_page;
    int m_pageNum;
};

class KPrInsertPageCmd : public KNamedCommand
{
public:
    KPrInsertPageCmd( const QString &name, int currentPageNum, InsertPos pos,
                      KPrPage *page, KPrDocument *doc );
    ~KPrInsertPageCmd();

    virtual void execute();
    virtual void unexecute();
protected:
    KPrDocument *m_doc;
    KPrPage *m_page;
    int m_currentPageNum;
    int m_insertPageNum;
};

class KPrMovePageCmd : public KNamedCommand
{
public:
    KPrMovePageCmd( const QString &_name, int from,int to, KPrDocument *_doc );
    ~KPrMovePageCmd();

    virtual void execute();
    virtual void unexecute();
protected:
    KPrDocument *m_doc;
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
                             const QByteArray &data );
    KoTextCursor *execute( KoTextCursor *c );
    KoTextCursor *unexecute( KoTextCursor *c );
protected:
    int m_parag;
    int m_idx;
    QByteArray m_data;
    // filled in by execute(), for unexecute()
    int m_lastParag;
    int m_lastIndex;
    KoParagLayout* m_oldParagLayout;
};

/**
 * Command to change variable setting
 */
class KPrChangeStartingPageCommand : public KNamedCommand
{
public:
    KPrChangeStartingPageCommand( const QString &name, KPrDocument *_doc,
                                  int _oldStartingPage, int _newStartingPage);
    ~KPrChangeStartingPageCommand(){}

    void execute();
    void unexecute();
protected:
    KPrDocument *m_doc;
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
    KPrChangeVariableSettingsCommand( const QString &name, KPrDocument *_doc, bool _oldValue,
                                      bool _newValue, VariableProperties _type);
    ~KPrChangeVariableSettingsCommand(){}

    void execute();
    void unexecute();
protected:
    void changeValue( bool b );
    KPrDocument *m_doc;
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
    KPrChangeTitlePageNameCommand( const QString &name, KPrDocument *_doc, const QString &_oldPageName,
                                   const QString &_newPageName, KPrPage *_page);
    ~KPrChangeTitlePageNameCommand(){}

    void execute();
    void unexecute();
protected:
    KPrDocument *m_doc;
    QString oldPageName;
    QString newPageName;
    KPrPage *m_page;
};

class KPrChangeCustomVariableValue : public KNamedCommand
{
public:
    KPrChangeCustomVariableValue( const QString &name, KPrDocument *_doc,const QString & _oldValue,
                                  const QString & _newValue, KoCustomVariable *var);

    void execute();
    void unexecute();
protected:
    KPrDocument *m_doc;
    QString newValue;
    QString oldValue;
    KoCustomVariable *m_var;
};

class KPrChangeLinkVariable : public KNamedCommand
{
public:
    KPrChangeLinkVariable( const QString &name, KPrDocument *_doc,const QString & _oldHref,
                           const QString & _newHref, const QString & _oldLink,const QString &_newLink, KoLinkVariable *var);
    ~KPrChangeLinkVariable(){};
    void execute();
    void unexecute();
protected:
    KPrDocument *m_doc;
    QString oldHref;
    QString newHref;
    QString oldLink;
    QString newLink;
    KoLinkVariable *m_var;
};


class KPrNameObjectCommand : public KNamedCommand
{
public:
    KPrNameObjectCommand( const QString &_name, const QString &_objectName, KPrObject *_obj, KPrDocument *_doc );
    ~KPrNameObjectCommand();
    void execute();
    void unexecute();
protected:
    QString oldObjectName, newObjectName;
    KPrObject *object;
    KPrDocument *doc;
    KPrPage *m_page;
};

class KPrDisplayObjectFromMasterPage : public KNamedCommand
{
public:
    KPrDisplayObjectFromMasterPage( const QString &name, KPrDocument *_doc, KPrPage *_page, bool _newValue);
    ~KPrDisplayObjectFromMasterPage(){};
    void execute();
    void unexecute();
protected:
    KPrDocument *m_doc;
    KPrPage *m_page;
    bool newValue;
};

class KPrDisplayBackgroundPage : public KNamedCommand
{
public:
    KPrDisplayBackgroundPage( const QString &name, KPrDocument *_doc, KPrPage *_page, bool _newValue);
    ~KPrDisplayBackgroundPage(){};
    void execute();
    void unexecute();
protected:
    KPrDocument *m_doc;
    KPrPage *m_page;
    bool newValue;
};

class KPrHideShowHeaderFooter : public KNamedCommand
{
public:
    KPrHideShowHeaderFooter( const QString &name, KPrDocument *_doc, KPrPage *_page, bool _newValue,KPrTextObject *_textObject);
    ~KPrHideShowHeaderFooter(){};
    void execute();
    void unexecute();
protected:
    KPrDocument *m_doc;
    KPrPage *m_page;
    KPrTextObject *m_textObject;
    bool newValue;
};

class KPrFlipObjectCommand : public KNamedCommand
{
public:
    KPrFlipObjectCommand( const QString &name, KPrDocument *_doc, bool _horizontal ,
                          Q3PtrList<KPrObject> &_objects );
    ~KPrFlipObjectCommand();
    void execute();
    void unexecute();
protected:
    void flipObjects();
    KPrDocument *m_doc;
    Q3PtrList<KPrObject> objects;
    bool horizontal;
    KPrPage *m_page;
};

class KPrGeometryPropertiesCommand : public KNamedCommand
{
public:
    enum KgpType { ProtectSize, KeepRatio};
    KPrGeometryPropertiesCommand( const QString &name, Q3PtrList<KPrObject> &objects,
                                  bool newValue, KgpType type, KPrDocument *_doc );
    KPrGeometryPropertiesCommand( const QString &name, Q3ValueList<bool> &lst, Q3PtrList<KPrObject> &objects,
                                  bool newValue, KgpType type, KPrDocument *_doc );
    ~KPrGeometryPropertiesCommand();

    virtual void execute();
    virtual void unexecute();

protected:
    Q3ValueList<bool> m_oldValue;
    Q3PtrList<KPrObject> m_objects;
    bool m_newValue;
    KgpType m_type;
    KPrDocument *m_doc;
};

class KPrProtectContentCommand : public KNamedCommand
{
public:
    KPrProtectContentCommand( const QString &name, Q3PtrList<KPrObject> &objects,
                              bool protectContent, KPrDocument *doc );
    KPrProtectContentCommand( const QString &name, bool protectContent,
                              KPrTextObject *obj, KPrDocument *doc );

    ~KPrProtectContentCommand();
    virtual void execute();
    virtual void unexecute();

protected:
    void addObjects( const Q3PtrList<KPrObject> &objects );

    Q3PtrList<KPrTextObject> m_objects;
    bool m_protectContent;
    Q3ValueList<bool> m_oldValues;
    KPrDocument *m_doc;
};

class KPrCloseObjectCommand : public KNamedCommand
{
public:
    KPrCloseObjectCommand( const QString &name, Q3PtrList<KPrObject> objects, KPrDocument *doc );

    ~KPrCloseObjectCommand();
    virtual void execute();
    virtual void unexecute();

protected:
    void closeObject(bool close);

    Q3PtrList<KPrObject> m_openObjects;
    Q3PtrList<KPrObject> m_closedObjects;
    KPrDocument * m_doc;
    KPrPage *m_page;
};

struct MarginsStruct {
    MarginsStruct() {}
    MarginsStruct( KPrTextObject *obj );
    MarginsStruct( double _left, double top, double right, double bottom );
    double topMargin;
    double bottomMargin;
    double leftMargin;
    double rightMargin;
};

class KPrChangeMarginCommand : public KNamedCommand
{
public:
    KPrChangeMarginCommand( const QString &name, Q3PtrList<KPrObject> &objects, MarginsStruct newMargins,
                            KPrDocument *doc, KPrPage *page );
    ~KPrChangeMarginCommand();

    virtual void execute();
    virtual void unexecute();
protected:
    void addObjects( const Q3PtrList<KPrObject> &objects );
    Q3PtrList<MarginsStruct> m_oldMargins;
    Q3PtrList<KPrTextObject> m_objects;
    MarginsStruct m_newMargins;
    KPrPage *m_page;
    KPrDocument *m_doc;
};


class KPrChangeVerticalAlignmentCommand : public KNamedCommand
{
public:
    KPrChangeVerticalAlignmentCommand( const QString &name, KPrTextObject *_obj, VerticalAlignmentType _oldAlign,
                                       VerticalAlignmentType _newAlign, KPrDocument *_doc);
    ~KPrChangeVerticalAlignmentCommand() {}

    virtual void execute();
    virtual void unexecute();
protected:
    KPrTextObject *m_obj;
    VerticalAlignmentType m_oldAlign;
    VerticalAlignmentType m_newAlign;
    KPrPage *m_page;
    KPrDocument *m_doc;
};


class KPrChangeTabStopValueCommand : public KNamedCommand
{
public:
    KPrChangeTabStopValueCommand( const QString &name, double _oldValue, double _newValue, KPrDocument *_doc);

    ~KPrChangeTabStopValueCommand() {}

    virtual void execute();
    virtual void unexecute();
protected:
    KPrDocument *m_doc;
    double m_oldValue;
    double m_newValue;
};

class KPrChangeVariableNoteText : public KNamedCommand
{
 public:
    KPrChangeVariableNoteText( const QString &name, KPrDocument *_doc, const QString &_oldValue,const QString &_newValue, KoNoteVariable *var);
    ~KPrChangeVariableNoteText();
    void execute();
    void unexecute();
 protected:
    KPrDocument *m_doc;
    QString newValue;
    QString oldValue;
    KoNoteVariable *m_var;
};


#endif
