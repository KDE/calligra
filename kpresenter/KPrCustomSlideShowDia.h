/* This file is part of the KDE project
   Copyright (C) 2004 Laurent Montel <montel@kde.org>

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

#ifndef __CUSTOM_SLIDE_SHOW__
#define __CUSTOM_SLIDE_SHOW__

#include <kdialogbase.h>
#include "global.h"

class QListBox;
class QListBoxItem;
class QLineEdit;
class QPushButton;
class KPrDocument;
class QToolButton;
class KPrView;

class KPrCustomSlideShowDia : public KDialogBase
{
    Q_OBJECT

public:
    KPrCustomSlideShowDia( KPrView* _view, KPrDocument *_doc, const char* name );
    ~KPrCustomSlideShowDia();
public slots:
    virtual void slotOk();
    void slotDoubleClicked(QListBoxItem *);
    void slotTextClicked(QListBoxItem*);
    void slotRemove();
    void slotAdd();
    void slotModify();
    void slotCopy();
    void slotTest();
    void slotPresentationFinished();

protected:
    void hideEvent( QHideEvent* );

    void init();
    void updateButton();
    bool uniqueName( int val, const QString & name ) const;

    QListBox * list;
    QPushButton* m_pRemove;
    QPushButton* m_pAdd;
    QPushButton* m_pModify;
    QPushButton* m_pCopy;
    QPushButton* m_pTest;
    bool m_bChanged;
    KPrDocument *m_doc;
    KPrView *m_view;
    CustomListMap m_customListMap;
    QStringList listPageName;
};

class KPrDefineCustomSlideShow : public KDialogBase
{
    Q_OBJECT
public:
    KPrDefineCustomSlideShow( QWidget* parent, QStringList &_listNameSlideShow, QStringList & _listPage, const char *name = 0L);
    KPrDefineCustomSlideShow( QWidget* parent, const QString &_customName, QStringList &_listNameSlideShow, QStringList& _listPage, QStringList &_customListPage, const char* name = 0L );

    QString customSlideShowName() const;
    QStringList customListSlideShow();

protected slots:
    void slotMoveUpSlide();
    void slotMoveDownSlide();
    void slotMoveRemoveSlide();
    void slotMoveInsertSlide();
    void slideNameChanged( const QString & _name);
    void updateButton();
    virtual void slotOk();

protected:
    void init();
    QStringList listNameCustomSlideShow;
    QListBox *listSlide;
    QListBox *listSlideShow;
    QToolButton *m_insertSlide;
    QToolButton *m_removeSlide;
    QToolButton *m_moveUpSlide;
    QToolButton *m_moveDownSlide;
    QLineEdit *m_name;

};

#endif
