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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __CUSTOM_SLIDE_SHOW__
#define __CUSTOM_SLIDE_SHOW__

#include <kdialogbase.h>

class QListBox;
class QListBoxItem;
class QLineEdit;
class QMultiLineEdit;
class QPushButton;

class CustomSlideShowDia : public KDialogBase
{
  Q_OBJECT

public:
  CustomSlideShowDia( QWidget* parent, const char* name );

public slots:
  virtual void slotOk();
  void slotDoubleClicked(QListBoxItem *);
  void slotTextClicked(QListBoxItem*);
  void slotRemove();
  void slotAdd();
  void slotModify();
  void slotCopy();

protected:
  QListBox * list;
  QPushButton* m_pRemove;
  QPushButton* m_pAdd;
  QPushButton* m_pModify;
  QPushButton* m_pCopy;
  bool m_bChanged;
};

class DefineCustomSlideShow : public KDialogBase
{
    Q_OBJECT
public:
    DefineCustomSlideShow( QWidget* parent, const char *name );

protected slots:
void slotMoveUpSlide();
    void slotMoveDownSlide();
    void slotMoveRemoveSlide();
    void slotMoveInsertSlide();


protected:
    QListBox *listSlide;
    QListBox *listSlideShow;
    QPushButton *m_insertSlide;
    QPushButton *m_removeSlide;
    QPushButton *m_moveUpSlide;
    QPushButton *m_moveDownSlide;
    QLineEdit *m_name;

};

#endif
