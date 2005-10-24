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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef docstruct_h
#define docstruct_h

#include <klistview.h>
#include <qwidget.h>

class KWDocument;
class KWFormulaFrameSet;
class KWPictureFrameSet;
class KWPartFrameSet;
class KWGUI;
class QWidget;
class KWFrame;
class KWFrameSet;
class KWTextParag;

/******************************************************************/
/* Class: KWOrderedFrameSet                                       */
/******************************************************************/

/** Provides a way to sort framesets using a QValueList based on their top left corner. */
class KWOrderedFrameSet
{
public:
    KWOrderedFrameSet(KWFrameSet* fs);
    KWOrderedFrameSet();    // default constructor
    bool operator<( KWOrderedFrameSet ofs);
    KWFrameSet* frameSet() { return m_frameset; }

private:
    KWFrameSet* m_frameset;
};

class KWDocListViewItem : public QObject,public KListViewItem
{
    Q_OBJECT
public:
    KWDocListViewItem(QListViewItem *_parent, const QString &_text);
    KWDocListViewItem( QListViewItem *_parent, QListViewItem *_after, const QString &_text );
    virtual void selectFrameSet() {}
    virtual void editFrameSet() {}
    virtual void deleteFrameSet() {}
    virtual void editProperties() {}

public slots:
    virtual void slotRightButtonClicked( QListViewItem *, const QPoint &, int ) {}
    virtual void slotDoubleClicked( QListViewItem * ) {}
};

/******************************************************************/
/* Class: KWDocStructParagItem                                    */
/******************************************************************/

class KWDocStructParagItem : public KWDocListViewItem
{
    Q_OBJECT
public:
    KWDocStructParagItem( QListViewItem *_parent, const QString &_text, KWTextParag *_parag, KWGUI *__parent );
    KWDocStructParagItem( QListViewItem *_parent, QListViewItem *_after, const QString &_text, KWTextParag *_parag, KWGUI*__parent );
    virtual void selectFrameSet();
    virtual void editFrameSet();
    virtual void deleteFrameSet();

public slots:
    void slotDoubleClicked( QListViewItem *_item );
    void slotRightButtonClicked( QListViewItem *, const QPoint &, int );

protected:
    KWTextParag *parag;
    KWGUI *gui;

};

/******************************************************************/
/* Class: KWDocStructFrameSetItem                                 */
/******************************************************************/

class KWDocStructFrameSetItem : public KWDocListViewItem
{
    Q_OBJECT
public:
    KWDocStructFrameSetItem( QListViewItem *_parent, const QString &_text,
        KWFrameSet *_frameset, KWGUI *__parent );
    KWDocStructFrameSetItem( QListViewItem *_parent, QListViewItem *_after, const QString &_text,
        KWFrameSet *_frameset, KWGUI*__parent );
    virtual void selectFrameSet();
    virtual void editFrameSet();
    virtual void deleteFrameSet();
    virtual void editProperties();

public slots:
    void slotDoubleClicked( QListViewItem *_item );
    void slotRightButtonClicked( QListViewItem *, const QPoint &, int );

protected:
    KWFrameSet *frameset;
    KWGUI *gui;

};

/******************************************************************/
/* Class: KWDocStructFrameItem                                    */
/******************************************************************/

class KWDocStructFrameItem : public KWDocListViewItem
{
    Q_OBJECT

public:
    KWDocStructFrameItem( QListViewItem *_parent, const QString &_text, KWFrameSet *_frameset, KWFrame *_frame, KWGUI *__parent );
    virtual void selectFrameSet();
    virtual void editFrameSet();
    virtual void deleteFrameSet();
    virtual void editProperties();
public slots:
    void slotDoubleClicked( QListViewItem *_item );
    void slotRightButtonClicked( QListViewItem *, const QPoint &, int );
protected:
    KWFrame *frame;
    KWFrameSet *frameset;
    KWGUI *gui;

};

/******************************************************************/
/* Class: KWDocStructTableItem                                    */
/******************************************************************/

class KWDocStructTableItem : public KWDocListViewItem
{
    Q_OBJECT

public:
    KWDocStructTableItem( QListViewItem *_parent, const QString &_text, KWTableFrameSet *_table, KWGUI*__parent );
    virtual void selectFrameSet();
    virtual void editFrameSet();
    virtual void deleteFrameSet();
    virtual void editProperties();
public slots:
    void slotDoubleClicked( QListViewItem *_item );
    void slotRightButtonClicked( QListViewItem *, const QPoint &, int );

protected:
    KWTableFrameSet *table;
    KWGUI *gui;

};

/******************************************************************/
/* Class: KWDocStructFormulaItem                                  */
/******************************************************************/

class KWDocStructFormulaItem : public KWDocListViewItem
{
    Q_OBJECT

public:
    KWDocStructFormulaItem( QListViewItem *_parent, const QString &_text, KWFormulaFrameSet *_form, KWGUI*__parent );
    virtual void selectFrameSet();
    virtual void editFrameSet();
    virtual void deleteFrameSet();
    virtual void editProperties();
public slots:
    void slotDoubleClicked( QListViewItem *_item );
    void slotRightButtonClicked( QListViewItem *, const QPoint &, int );

protected:
    KWFormulaFrameSet *form;
    KWGUI *gui;

};


/******************************************************************/
/* Class: KWDocStructPictureItem                                  */
/******************************************************************/

class KWDocStructPictureItem : public KWDocListViewItem
{
    Q_OBJECT

public:
    KWDocStructPictureItem( QListViewItem *_parent, const QString &_text, KWPictureFrameSet *_pic, KWGUI*__parent );
    virtual void selectFrameSet();
    virtual void deleteFrameSet();
    virtual void editProperties();
public slots:
    void slotDoubleClicked( QListViewItem *_item );
    void slotRightButtonClicked( QListViewItem *, const QPoint &, int );

protected:
    KWPictureFrameSet *pic;
    KWGUI *gui;

};

/******************************************************************/
/* Class: KWDocStructPartItem                                     */
/******************************************************************/

class KWDocStructPartItem : public KWDocListViewItem
{
    Q_OBJECT

public:
    KWDocStructPartItem( QListViewItem *_parent, const QString &_text, KWPartFrameSet *_part, KWGUI*__parent );
    virtual void selectFrameSet();
    virtual void editFrameSet();
    virtual void deleteFrameSet();
    virtual void editProperties();
public slots:
    void slotDoubleClicked( QListViewItem *_item );
    void slotRightButtonClicked( QListViewItem *, const QPoint &, int );

protected:
    KWPartFrameSet *part;
    KWGUI *gui;

};

/******************************************************************/
/* Class: KWDocStructRootItem                                     */
/******************************************************************/

class KWDocStructRootItem : public KListViewItem
{
public:

    KWDocStructRootItem( QListView *_parent, KWDocument *_doc, TypeStructDocItem _type, KWGUI*__parent );

    void setupArrangement();
    void setupTextFrames();
    void setupTables();
    void setupPictures();
    void setupEmbedded();
    void setupFormulaFrames();
    virtual void setOpen( bool o );

protected:
    void deleteAllChildren();

    KWDocument *doc;
    TypeStructDocItem type;
    KWGUI *gui;

};

/******************************************************************/
/* Class: KWDocStructTree                                         */
/******************************************************************/

class KWDocStructTree : public KListView
{
    Q_OBJECT

public:
    KWDocStructTree( QWidget *_parent, KWDocument *_doc, KWGUI*__parent );
    ~KWDocStructTree();

    void setup();
    void refreshTree(int _type);
    QSize minimumSizeHint() const {
        return QSize( 0, 0 );
    }
    void selectFrameSet();
    void editFrameSet();
    void deleteFrameSet();
    void editProperties();
public slots:
    void slotContextMenu(KListView *l, QListViewItem *i, const QPoint &p);
protected:
    KWDocument *doc;
    KWGUI *gui;

    KWDocStructRootItem *arrangement, *tables, *pictures, *textfrms, *embedded, *formulafrms;

};

/******************************************************************/
/* Class: KWDocStruct                                             */
/******************************************************************/

class KWDocStruct : public QWidget
{
    Q_OBJECT

public:
    KWDocStruct( QWidget *_parent, KWDocument *_doc, KWGUI*__parent );
    void selectFrameSet();
    void editFrameSet();
    void deleteFrameSet();
    void editProperties();
    /** Refresh the specified branches of the tree.  Actual refreshing is deferred
        until the paint event. */
    void refreshTree(int _type);
    /** Set focus to the tree, if visible. */
    void setFocusHere();

protected:
    KWDocStructTree *tree;
    QGridLayout *layout;
    virtual void paintEvent( QPaintEvent* ev );

    KWDocument *doc;
    KWGUI *parent;
    int dirtyTreeTypes;
};

#endif
