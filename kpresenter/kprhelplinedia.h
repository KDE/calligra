/* This file is part of the KDE project
   Copyright (C)  2002 Montel Laurent <lmontel@mandrakesoft.com>

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

#ifndef __kprhelplinedia__
#define __kprhelplinedia__

#include <kdialogbase.h>
#include <koRect.h>

class QRadioButton;
class KPresenterDoc;
class KLineEdit;
class KFloatValidator;

class KPrMoveHelpLineDia : public KDialogBase
{
    Q_OBJECT
public:
    KPrMoveHelpLineDia( QWidget *parent, double value, double limitTop, double limitBottom , KPresenterDoc *_doc, const char *name=0L);

    double newPosition();

private slots:
    void slotRemoveHelpLine();

protected:
    KLineEdit* position;
    KPresenterDoc *m_doc;
};

class KPrInsertHelpLineDia : public KDialogBase
{
    Q_OBJECT
public:
    KPrInsertHelpLineDia( QWidget *parent, const KoRect &r, KPresenterDoc *_doc, const char *name=0L);

    double newPosition();
    //return true if we add a new horizontal line help
    bool addHorizontalHelpLine();

private slots:
    void slotRadioButtonClicked();

protected:
    KoRect limitOfPage;
    KLineEdit* position;
    QRadioButton *m_rbHoriz;
    QRadioButton *m_rbVert;
    KPresenterDoc *m_doc;
    KFloatValidator *floatValidator;
};


class KPrInsertHelpPointDia : public KDialogBase
{
    Q_OBJECT
public:
    KPrInsertHelpPointDia( QWidget *parent, const KoRect &r, KPresenterDoc *_doc, const char *name=0L);

    KoPoint newPosition();
protected:
    KoRect limitOfPage;
    KLineEdit* positionX;
    KLineEdit* positionY;
    KPresenterDoc *m_doc;
};


#endif
