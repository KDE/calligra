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

#ifndef docstruct_h
#define docstruct_h

#include <qlistview.h>
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
/* Class: KWDocStructParagItem                                    */
/******************************************************************/

class KWDocStructParagItem : public QObject,
                             virtual public QListViewItem
{
    Q_OBJECT

public:
    KWDocStructParagItem( QListViewItem *_parent, QString _text, KWTextParag *_parag, KWGUI *__parent );
    KWDocStructParagItem( QListViewItem *_parent, QListViewItem *_after, QString _text, KWTextParag *_parag, KWGUI*__parent );

public slots:
    void slotDoubleClicked( QListViewItem *_item );

protected:
    KWTextParag *parag;
    KWGUI *gui;

};

/******************************************************************/
/* Class: KWDocStructFrameItem                                    */
/******************************************************************/

class KWDocStructFrameItem : public QObject,
                             virtual public QListViewItem
{
    Q_OBJECT

public:
    KWDocStructFrameItem( QListViewItem *_parent, QString _text, KWFrameSet *_frameset, KWFrame *_frame, KWGUI *__parent );

public slots:
    void slotDoubleClicked( QListViewItem *_item );

protected:
    KWFrame *frame;
    KWFrameSet *frameset;
    KWGUI *gui;

};

/******************************************************************/
/* Class: KWDocStructTableItem                                    */
/******************************************************************/

class KWDocStructTableItem : public QObject,
                             virtual public QListViewItem
{
    Q_OBJECT

public:
    KWDocStructTableItem( QListViewItem *_parent, QString _text, KWTableFrameSet *_table, KWGUI*__parent );

public slots:
    void slotDoubleClicked( QListViewItem *_item );

protected:
    KWTableFrameSet *table;
    KWGUI *gui;

};

/******************************************************************/
/* Class: KWDocStructFormulaItem                                  */
/******************************************************************/

class KWDocStructFormulaItem : public QObject,
                               virtual public QListViewItem
{
    Q_OBJECT

public:
    KWDocStructFormulaItem( QListViewItem *_parent, QString _text, KWFormulaFrameSet *_form, KWGUI*__parent );

public slots:
    void slotDoubleClicked( QListViewItem *_item );

protected:
    KWFormulaFrameSet *form;
    KWGUI *gui;

};


/******************************************************************/
/* Class: KWDocStructPictureItem                                  */
/******************************************************************/

class KWDocStructPictureItem : public QObject,
                               virtual public QListViewItem
{
    Q_OBJECT

public:
    KWDocStructPictureItem( QListViewItem *_parent, QString _text, KWPictureFrameSet *_pic, KWGUI*__parent );

public slots:
    void slotDoubleClicked( QListViewItem *_item );

protected:
    KWPictureFrameSet *pic;
    KWGUI *gui;

};

/******************************************************************/
/* Class: KWDocStructPartItem                                     */
/******************************************************************/

class KWDocStructPartItem : public QObject,
                            virtual public QListViewItem
{
    Q_OBJECT

public:
    KWDocStructPartItem( QListViewItem *_parent, QString _text, KWPartFrameSet *_part, KWGUI*__parent );

public slots:
    void slotDoubleClicked( QListViewItem *_item );

protected:
    KWPartFrameSet *part;
    KWGUI *gui;

};

/******************************************************************/
/* Class: KWDocStructRootItem                                     */
/******************************************************************/

class KWDocStructRootItem : public QListViewItem
{
public:

    KWDocStructRootItem( QListView *_parent, KWDocument *_doc, TypeStructDocItem _type, KWGUI*__parent );

    void setupArrangement();
    void setupTextFrames();
    void setupTables();
    void setupPictures();
    void setupCliparts();
    void setupEmbedded();
    void setupFormulaFrames();
    virtual void setOpen( bool o );

protected:
    KWDocument *doc;
    TypeStructDocItem type;
    KWGUI *gui;

};

/******************************************************************/
/* Class: KWDocStructTree                                         */
/******************************************************************/

class KWDocStructTree : public QListView
{
    Q_OBJECT

public:
    KWDocStructTree( QWidget *_parent, KWDocument *_doc, KWGUI*__parent );

    void setup();
    void refreshTree(int _type);
    QSize	minimumSizeHint() const {
	return QSize( 0, 0 );
    }

protected:
    KWDocument *doc;
    KWGUI *gui;

    KWDocStructRootItem *arrangement, *tables, *pictures, *cliparts, *textfrms, *embedded, *formulafrms;

};

/******************************************************************/
/* Class: KWDocStruct                                             */
/******************************************************************/

class KWDocStruct : public QWidget
{
    Q_OBJECT

public:
    KWDocStruct( QWidget *_parent, KWDocument *_doc, KWGUI*__parent );
    KWDocStructTree * getDocStructTree() {return tree;}

protected:
    KWDocStructTree *tree;
    QGridLayout *layout;

    KWDocument *doc;
    KWGUI *parent;

};

#endif
