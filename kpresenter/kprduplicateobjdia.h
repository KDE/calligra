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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __kprduplicateobjdia__
#define __kprduplicateobjdia__

#include <kdialogbase.h>
#include <koRect.h>

class KIntNumInput;
class KDoubleNumInput;
class KPresenterDoc;
class KoUnitDoubleSpinBox;

class KPrDuplicatObjDia : public KDialogBase
{
    Q_OBJECT
public:
    KPrDuplicatObjDia( QWidget *parent, KPresenterDoc * _doc, const char *name=0L);
    int nbCopy() const;
    double angle() const;
    double increaseX() const;
    double increaseY() const;
    double moveX() const;
    double moveY() const;

protected:
    KIntNumInput *m_nbCopy;
    KDoubleNumInput *m_rotation;
    KoUnitDoubleSpinBox *m_increaseX, *m_increaseY;
    KoUnitDoubleSpinBox *m_moveX, *m_moveY;
    KPresenterDoc *m_doc;

};


#endif
