// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>

   The code is mostly a copy from kword/framedia.h

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
#ifndef KPMARGINWIDGET_H
#define KPMARGINWIDGET_H

#include <QWidget>

#include <KoUnit.h>

class MarginUI;

class KPrMarginWidget : public QWidget
{
    Q_OBJECT
public:
    KPrMarginWidget( QWidget *parent, const char *name, const KoUnit::Unit unit );
    ~KPrMarginWidget();

    bool changed() const { return m_changed; }
    void resetChanged() { m_changed = false; }

    void setValues( double left, double right, double top, double bottom ); // in pt
    double leftValue() const; // in pt
    double rightValue() const; // in pt
    double topValue() const; // in pt
    double bottomValue() const; // in pt

private:
    MarginUI *m_ui;
    KoUnit::Unit m_unit;
    bool m_changed;
    bool m_noSignal;

protected slots:
    void slotValueChanged( double );
};

#endif /* KPMARGINWIDGET_H */
