/* This file is part of the KDE project
   Copyright (C) 2001 Laurent Montel <lmontel@mandrakesoft.com>

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
#include <global.h>
#include <kpimage.h>
#include <kpclipartcollection.h>
#include <global.h>
#include <qvaluelist.h>
#include <qpen.h>
#include <qbrush.h>
#include <qmap.h>
#include <koPageLayoutDia.h>
#include <koparaglayout.h>
#include <qrichtext_p.h>
#include <kocommand.h>
class KPresenterDoc;

class KPresenterDoc;
class KPTextObject;
class KPObject;
class KPPixmapObject;
class KPGroupObject;
class KPresenterView;
class KPClipartObject;
class KoParagLayout;
class KPrPage;

/******************************************************************/
/* Class: ShadowCmd                                               */
/******************************************************************/

class ShadowCmd : public KCommand
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

};

/******************************************************************/
/* Class: SetOptionsCmd                                           */
/******************************************************************/

class SetOptionsCmd : public KCommand
{
public:
    SetOptionsCmd( const QString &_name, QPtrList<QPoint> &_diffs, QPtrList<KPObject> &_objects,
                   int _rastX, int _rastY, int _orastX, int _orastY,
                   const QColor &_txtBackCol, const QColor &_otxtBackCol, KPresenterDoc *_doc );
    ~SetOptionsCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    QPtrList<QPoint> diffs;
    QPtrList<KPObject> objects;
    int rastX, rastY;
    int orastX, orastY;
    KPresenterDoc *doc;
    QColor txtBackCol;
    QColor otxtBackCol;

};

/******************************************************************/
/* Class: SetBackCmd						  */
/******************************************************************/

class SetBackCmd : public KCommand
{
public:
    SetBackCmd( const QString &_name, const QColor &_backColor1, const QColor &_backColor2, BCType _bcType,
		bool _backUnbalanced, int _backXFactor, int _backYFactor,
		const KPImageKey & _backPix, const KPClipartKey & _backClip,
                BackView _backView, BackType _backType,
		const QColor &_oldBackColor1, const QColor &_oldBackColor2, BCType _oldBcType,
		bool _oldBackUnbalanced, int _oldBackXFactor, int _oldBackYFactor,
		const KPImageKey & _oldBackPix, const KPClipartKey & _oldBackClip,
                BackView _oldBackView, BackType _oldBackType,
		bool _takeGlobal, KPresenterDoc *_doc, KPrPage *_page );

    virtual void execute();
    virtual void unexecute();

protected:

    QColor backColor1, backColor2;
    bool unbalanced;
    int xfactor, yfactor;
    KPImageKey backPix;
    KPClipartKey backClip;
    BCType bcType;
    BackView backView;
    BackType backType;
    QColor oldBackColor1, oldBackColor2;
    bool oldUnbalanced;
    int oldXFactor, oldYFactor;
    KPImageKey oldBackPix;
    KPClipartKey oldBackClip;
    BCType oldBcType;
    BackView oldBackView;
    BackType oldBackType;
    bool takeGlobal;
    KPresenterDoc *doc;
    KPrPage *m_page;

};

/******************************************************************/
/* Class: RotateCmd                                               */
/******************************************************************/

class RotateCmd : public KCommand
{
public:
    struct RotateValues
    {
        float angle;
    };

    RotateCmd( const QString &_name, QPtrList<RotateValues> &_oldRotate, float _newAngle,
               QPtrList<KPObject> &_objects, KPresenterDoc *_doc );
    ~RotateCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    KPresenterDoc *doc;
    QPtrList<RotateValues> oldRotate;
    QPtrList<KPObject> objects;
    float newAngle;

};

/******************************************************************/
/* Class: ResizeCmd                                               */
/******************************************************************/

class ResizeCmd : public KCommand
{
public:
    ResizeCmd( const QString &_name, const KoPoint &_m_diff, const KoSize &_r_diff, KPObject *_object, KPresenterDoc *_doc );
    ~ResizeCmd();

    virtual void execute();
    virtual void unexecute();
    virtual void unexecute( bool _repaint );

protected:

    KoPoint m_diff;
    KoSize r_diff;
    KPObject *object;
    KPresenterDoc *doc;

};

/******************************************************************/
/* Class: ChgClipCmd                                              */
/******************************************************************/

class ChgClipCmd : public KCommand
{
public:
    ChgClipCmd( const QString &_name, KPClipartObject *_object, KPClipartCollection::Key _oldName,
                KPClipartCollection::Key _newName, KPresenterDoc *_doc );
    ~ChgClipCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    KPClipartObject *object;
    KPresenterDoc *doc;
    KPClipartCollection::Key oldKey, newKey;
};

/******************************************************************/
/* Class: ChgPixCmd                                               */
/******************************************************************/

class ChgPixCmd : public KCommand
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

/******************************************************************/
/* Class: DeleteCmd                                               */
/******************************************************************/

class DeleteCmd : public KCommand
{
public:
    DeleteCmd( const QString &_name, QPtrList<KPObject> &_objects, KPresenterDoc *_doc , KPrPage *_page);
    ~DeleteCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    QPtrList<KPObject> objects;
    KPresenterDoc *doc;
    KPrPage *m_page;

};

/******************************************************************/
/* Class: EffectCmd                                               */
/******************************************************************/

class EffectCmd : public KCommand
{
public:
    struct EffectStruct {
	int presNum, disappearNum;
	Effect effect;
	Effect2 effect2;
	Effect3 effect3;
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

/******************************************************************/
/* Class: GroupObjCmd						  */
/******************************************************************/

class GroupObjCmd : public KCommand
{
public:
    GroupObjCmd( const QString &_name,
		 const QPtrList<KPObject> &_objects,
		 KPresenterDoc *_doc, KPrPage *_page );
    ~GroupObjCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    QPtrList<KPObject> objects;
    KPresenterDoc *doc;
    KPGroupObject *grpObj;
    KPrPage *m_page;

};

/******************************************************************/
/* Class: UnGroupObjCmd						  */
/******************************************************************/

class UnGroupObjCmd : public KCommand
{
public:
    UnGroupObjCmd( const QString &_name,
		 KPGroupObject *grpObj_,
		 KPresenterDoc *_doc, KPrPage *_page );
    ~UnGroupObjCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    QPtrList<KPObject> objects;
    KPresenterDoc *doc;
    KPGroupObject *grpObj;
    KPrPage *m_page;

};


/******************************************************************/
/* Class: InsertCmd                                               */
/******************************************************************/

class InsertCmd : public KCommand
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

/******************************************************************/
/* Class: LowerRaiseCmd                                           */
/******************************************************************/

class LowerRaiseCmd : public KCommand
{
public:
    LowerRaiseCmd( const QString &_name, QPtrList<KPObject> _oldList, QPtrList<KPObject> _newList, KPresenterDoc *_doc, KPrPage *_page );
    ~LowerRaiseCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    QPtrList<KPObject> oldList, newList;
    KPresenterDoc *doc;
    bool m_executed;
    KPrPage *m_page;

};

/******************************************************************/
/* Class: MoveByCmd                                               */
/******************************************************************/

class MoveByCmd : public KCommand
{
public:
    MoveByCmd( const QString &_name, const KoPoint &_diff, QPtrList<KPObject> &_objects, KPresenterDoc *_doc, KPrPage *_page );
    ~MoveByCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    KoPoint diff;
    QPtrList<KPObject> objects;
    KPresenterDoc *doc;
    KPrPage *m_page;

};

/******************************************************************/
/* Class: MoveByCmd2                                              */
/******************************************************************/

class MoveByCmd2 : public KCommand
{
public:
    MoveByCmd2( const QString &_name, QPtrList<KoPoint> &_diffs, QPtrList<KPObject> &_objects, KPresenterDoc *_doc );
    ~MoveByCmd2();

    virtual void execute();
    virtual void unexecute();

protected:

    QPtrList<KoPoint> diffs;
    QPtrList<KPObject> objects;
    KPresenterDoc *doc;

};

/******************************************************************/
/* Class: PenBrushCmd						  */
/******************************************************************/

class PenBrushCmd : public KCommand
{
public:
    struct Pen {
	QPen pen;
	LineEnd lineBegin, lineEnd;

	Pen &operator=( const Pen &_pen ) {
	    pen	 = _pen.pen;
	    lineBegin = _pen.lineBegin;
	    lineEnd = _pen.lineEnd;
	    return *this;
	}
    };

    struct Brush {
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

    static const int LB_ONLY = 1;
    static const int LE_ONLY = 2;
    static const int PEN_ONLY = 4;
    static const int BRUSH_ONLY = 8;

    PenBrushCmd( const QString &_name, QPtrList<Pen> &_oldPen, QPtrList<Brush> &_oldBrush,
		 Pen _newPen, Brush _newBrush, QPtrList<KPObject> &_objects, KPresenterDoc *_doc, int _flags = 0 );
    ~PenBrushCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    KPresenterDoc *doc;
    QPtrList<Pen> oldPen;
    QPtrList<Brush> oldBrush;
    QPtrList<KPObject> objects;
    Pen newPen;
    Brush newBrush;
    int flags;

};

/******************************************************************/
/* Class: PgConfCmd                                               */
/******************************************************************/

class PgConfCmd : public KCommand
{
public:
    PgConfCmd( const QString &_name, bool _manualSwitch, bool _infinitLoop,
               PageEffect _pageEffect, PresSpeed _presSpeed, int _pageTimer,
               bool _soundEffect, const QString &_fileName,
               bool _oldManualSwitch, bool _oldInfinitLoop,
               PageEffect _oldPageEffect, PresSpeed _oldPresSpeed, int _oldPageTimer,
               bool _oldSoundEffect, const QString &_oldFileName,
               KPresenterDoc *_doc, KPrPage *_page );

    virtual void execute();
    virtual void unexecute();

protected:
    bool manualSwitch, oldManualSwitch;
    bool infinitLoop, oldInfinitLoop;
    PageEffect pageEffect, oldPageEffect;
    PresSpeed presSpeed, oldPresSpeed;
    int pageTimer, oldPageTimer;
    bool soundEffect, oldSoundEffect;
    QString fileName, oldFileName;
    KPrPage *m_page;
    KPresenterDoc *doc;

};

/******************************************************************/
/* Class: PgLayoutCmd                                             */
/******************************************************************/

class PgLayoutCmd : public KCommand
{
public:
    PgLayoutCmd( const QString &_name, KoPageLayout _layout, KoPageLayout _oldLayout,
                 KoUnit::Unit _oldUnit, KoUnit::Unit _unit,
                 KPresenterView *_view );

    virtual void execute();
    virtual void unexecute();

protected:

    KoPageLayout layout, oldLayout;
    KoUnit::Unit unit, oldUnit;
    KPresenterView *view;

};

/******************************************************************/
/* Class: PieValueCmd                                             */
/******************************************************************/

class PieValueCmd : public KCommand
{
public:
    struct PieValues
    {
        PieType pieType;
        int pieAngle, pieLength;
    };

    PieValueCmd( const QString &_name, QPtrList<PieValues> &_oldValues, PieValues _newValues,
                 QPtrList<KPObject> &_objects, KPresenterDoc *_doc );
    ~PieValueCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    KPresenterDoc *doc;
    QPtrList<PieValues> oldValues;
    QPtrList<KPObject> objects;
    PieValues newValues;

};

/******************************************************************/
/* Class: PolygonSettingCmd                                       */
/******************************************************************/

class PolygonSettingCmd : public KCommand
{
public:
    struct PolygonSettings
    {
        bool checkConcavePolygon;
        int cornersValue;
        int sharpnessValue;
    };

    PolygonSettingCmd( const QString &_name, QPtrList<PolygonSettings> &_oldSettings,
                       PolygonSettings _newSettings, QPtrList<KPObject> &_objects, KPresenterDoc *_doc );
    ~PolygonSettingCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    KPresenterDoc *doc;
    QPtrList<PolygonSettings> oldSettings;
    QPtrList<KPObject> objects;
    PolygonSettings newSettings;

};

/******************************************************************/
/* Class: RectValueCmd                                            */
/******************************************************************/

class RectValueCmd : public KCommand
{
public:
    struct RectValues
    {
        int xRnd, yRnd;
    };

    RectValueCmd( const QString &_name, QPtrList<RectValues> &_oldValues, RectValues _newValues,
                  QPtrList<KPObject> &_objects, KPresenterDoc *_doc );
    ~RectValueCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    KPresenterDoc *doc;
    QPtrList<RectValues> oldValues;
    QPtrList<KPObject> objects;
    RectValues newValues;

};

/******************************************************************/
/* Class: DeletePageCmd                                           */
/******************************************************************/

class KPrDeletePageCmd : public KCommand
{
public:
    KPrDeletePageCmd( const QString &_name,int _pos, KPrPage *page, KPresenterDoc *_doc );
    ~KPrDeletePageCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    KPresenterDoc *doc;
    KPrPage *m_page;
    int position;
};

/******************************************************************/
/* Class: KPrPasteTextCommand                                     */
/******************************************************************/
class KPrPasteTextCommand : public Qt3::QTextCommand
{
public:
    KPrPasteTextCommand( KoTextDocument *d, int parag, int idx,
                    const QCString & data );
    ~KPrPasteTextCommand() {}
    Qt3::QTextCursor *execute( Qt3::QTextCursor *c );
    Qt3::QTextCursor *unexecute( Qt3::QTextCursor *c );
protected:
    int m_parag;
    int m_idx;
    QCString m_data;
    // filled in by execute(), for unexecute()
    int m_lastParag;
    int m_lastIndex;
    KoParagLayout m_oldParagLayout;
};

/**
 * Command to change variable setting
 */
class KPrChangeVariableSettingCommand : public KCommand
{
public:
    KPrChangeVariableSettingCommand( const QString &name, KPresenterDoc *_doc, int _oldVarOffset, int _newVarOffset);
    ~KPrChangeVariableSettingCommand(){}

    void execute();
    void unexecute();
protected:
    KPresenterDoc *m_doc;
    int oldVarOffset;
    int newVarOffset;
};


#endif
