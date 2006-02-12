// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>

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
#ifndef GENERALPROPERTY_H
#define GENERALPROPERTY_H

#include <qwidget.h>
#include "kspread_global.h"

#include <KoRect.h>
#include <KoUnit.h>

class KoGeneralPropertyUI;

namespace KSpread
{

class GeneralProperty : public QWidget
{
    Q_OBJECT
public:
    struct GeneralValue
    {
        GeneralValue()
        {}
        QString m_name;
        PropValue m_keepRatio;
        PropValue m_protect;
        KoRect m_rect;
    };

    enum GeneralConfigChange
    {
        Name = 1,
        Protect = 2,
        KeepRatio = 4,
        Left = 8,
        Top = 16,
        Width = 32,
        Height = 64
    };

    GeneralProperty( QWidget *parent, const char *name, GeneralValue &generalValue, KoUnit::Unit unit );
    ~GeneralProperty();

    int getGeneralPropertyChange() const;
    GeneralValue getGeneralValue() const;

    void apply();

protected:
    KoRect getRect() const;
    void setRect( KoRect &rect );

    KoGeneralPropertyUI *m_ui;
    double m_ratio;

    GeneralValue m_generalValue;
    KoUnit::Unit m_unit;

protected slots:
    void slotReset();
    void slotProtectToggled( bool state );
    void slotKeepRatioToggled( bool state );
    void slotWidthChanged( double value );
    void slotHeightChanged( double value );
};

}

#endif /* GENERALPROPERTY_H */
