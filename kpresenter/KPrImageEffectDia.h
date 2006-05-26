// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2002 Lukas Tinkl <lukas@kde.org>

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

#ifndef IMAGEEFFECTDIA_H
#define IMAGEEFFECTDIA_H

#include <qpixmap.h>
//Added by qt3to4:
#include <QShowEvent>

#include <kdialogbase.h>

#include "imageEffectBase.h"
#include "global.h"

class KPrImageEffectDia: public KDialogBase {
    Q_OBJECT

public:
    KPrImageEffectDia(QWidget * parent = 0, const char * name = 0);
    ~KPrImageEffectDia();

    /**
     * Set the initial preview pixmap
     */
    void setPixmap(QPixmap pix);

    /**
     * Returns the selected effect
     */
    ImageEffect getEffect() const {return m_effect;}
    /**
     * Returns the first param of the selected effect
     */
    QVariant getParam1() const {return m_param1;}
    /**
     * Returns the second param of the the selected effect
     */
    QVariant getParam2() const {return m_param2;}
    /**
     * Returns the third param of the the selected effect
     */
    QVariant getParam3() const {return m_param3;}

    /**
     * Set the initial effect and its params before showing the dialog
     */
    void setEffect(ImageEffect eff, QVariant p1, QVariant p2, QVariant p3);

protected:
    virtual void showEvent(QShowEvent * e);

private slots:
    void effectChanged(int);
    void okClicked();
    void effectParamChanged();

private:
    void updatePreview();
    void setupSignals();
    QPixmap m_pix;
    QPixmap m_origpix;
    ImageEffect m_effect;
    QVariant m_param1, m_param2, m_param3;
    EffectBrowserBase * base;
};

#endif //IMAGEEFFECTDIA_H
