// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __kprhelplinedia__
#define __kprhelplinedia__

#include <kdialogbase.h>
#include <KoRect.h>

class QRadioButton;
class KPrDocument;
class KLineEdit;
class KFloatValidator;
class KoUnitDoubleSpinBox;

class KPrMoveHelpLineDia : public KDialogBase
{
    Q_OBJECT
public:
    KPrMoveHelpLineDia( QWidget *parent, double value, double limitTop, double limitBottom,
                        KPrDocument *_doc, const char *name=0L);

    double newPosition() const;
    bool removeLine() const { return m_bRemoveLine;}

private slots:
    void slotRemoveHelpLine();

protected:
    KoUnitDoubleSpinBox* position;
    KPrDocument *m_doc;
    bool m_bRemoveLine;
};

class KPrInsertHelpLineDia : public KDialogBase
{
    Q_OBJECT
public:
    KPrInsertHelpLineDia( QWidget *parent, const KoRect &r, KPrDocument *_doc, const char *name=0L);

    double newPosition() const;
    //return true if we add a new horizontal line help
    bool addHorizontalHelpLine();

private slots:
    void slotRadioButtonClicked();

protected:
    KoRect limitOfPage;
    KoUnitDoubleSpinBox* position;
    QRadioButton *m_rbHoriz;
    QRadioButton *m_rbVert;
    KPrDocument *m_doc;
};


class KPrInsertHelpPointDia : public KDialogBase
{
    Q_OBJECT
public:
    KPrInsertHelpPointDia( QWidget *parent, const KoRect &r, KPrDocument *_doc, double poxX=0.0,
                           double posY=0.0, const char *name=0L);

    KoPoint newPosition() const;
    bool removePoint() const { return m_bRemovePoint;}
private slots:
    void slotRemoveHelpPoint();

protected:
    KoRect limitOfPage;
    bool m_bRemovePoint;
    KoUnitDoubleSpinBox* positionX;
    KoUnitDoubleSpinBox* positionY;
    KPrDocument *m_doc;
};


#endif
