/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef kptextobject_h
#define kptextobject_h

#include <kpobject.h>
#include "qrichtext_p.h"
#include <kotextview.h>


class KPresenterView;
class KPresenterDoc;
class QDomElement;
class TextCmd;
class KPGradient;

class KoTextView;
class KoTextObject;
class KPTextView;

/******************************************************************/
/* Class: KPTextObject                                            */
/******************************************************************/

class KPTextObject : public KP2DObject
{
    friend class TextCmd;
public:
    KPTextObject( KPresenterDoc *doc );
    virtual ~KPTextObject() {}

    virtual void setSize( int _width, int _height );
    virtual void resizeBy( int _dx, int _dy );

    virtual void setDrawEditRect( bool b )
    { drawEditRect = b; }
    virtual void setDrawEmpty( bool b )
    { drawEmpty = b; }

    virtual ObjType getType() const
    { return OT_TEXT; }
#if 0
    virtual int getSubPresSteps() const
    { return ktextobject.paragraphs() - 1; }
#endif
    virtual bool getDrawEditRect() const
    { return drawEditRect; }
    virtual bool getDrawEmpty() const
    { return drawEmpty; }

#if 0
    virtual KTextEdit* getKTextObject()
    { return &ktextobject; }
#endif

    virtual QDomDocumentFragment save( QDomDocument& doc );
    virtual void load(const QDomElement &element);

    virtual void draw( QPainter *_painter, int _diffx, int _diffy );

    virtual void activate( QWidget *_widget, int diffx, int diffy );
    virtual void deactivate( KPresenterDoc *doc );

    virtual void zoom( float _fakt );
    virtual void zoomOrig();

    virtual void extendObject2Contents( KPresenterView *view = 0 );

    void recalcPageNum( KPresenterDoc *doc );



    /** Return the contained text object */
    KoTextObject * textObject() const { return m_textobj; }

    KPTextView * textObjectView() const {return m_textobjview;}

    KoTextDocument *textDocument() const;

    KPresenterDoc* kPresenterDocument() const { return m_doc; }



protected:
    virtual QDomElement saveKTextObject( QDomDocument& doc );
    QDomElement saveHelper(const QString &tmpText, const QString &tmpFamily, const QString &tmpColor,
                           int tmpPointSize, unsigned int tmpBold, unsigned int tmpItalic,
                           unsigned int tmpUnderline, QDomDocument &doc);

    virtual void loadKTextObject( const QDomElement &e, int type );
    void drawParags( QPainter *p, int from, int to );
#if 0
    KTextEdit ktextobject;
#endif
    bool drawEditRect, drawEmpty;

private:
    KPTextObject &operator=( const KPTextObject & );

    static const QString &tagTEXTOBJ, &attrLineSpacing, &attrParagSpacing,
                           &attrMargin, &attrBulletType1, &attrBulletType2,
                           &attrBulletType3, &attrBulletType4, &attrBulletColor1,
                           &attrBulletColor2, &attrBulletColor3, &attrBulletColor4,
                           &attrObjType, &tagP, &attrAlign, &attrType,
                           &attrDepth, &tagTEXT, &attrFamily, &attrPointSize,
                           &attrBold, &attrItalic, & attrUnderline, &attrColor, &attrWhitespace;

    /** The contained text object */
    KoTextObject * m_textobj;
    KPresenterDoc *m_doc;
    KPTextView *m_textobjview;
};


class KPTextView : public KoTextView
{
Q_OBJECT
public:
    KPTextView( KPTextObject * txtObj );
    virtual ~KPTextView(){};
    KoTextView * textView() { return this; }
public slots:
    void cut();
    void copy();
    void paste();
    // Reimplemented from KoTextView
    virtual void updateUI( bool updateFormat, bool force = false );
    virtual void ensureCursorVisible();

protected slots:
    virtual void startDrag();

protected:
    // Reimplemented from KoTextView
    virtual void doAutoFormat( QTextCursor* cursor, KoTextParag *parag, int index, QChar ch );
    virtual void showFormat( KoTextFormat *format ) ;

    virtual void pgUpKeyPressed();

    virtual void pgDownKeyPressed();
};


#endif
