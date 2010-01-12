/* This file is part of the KDE project
   Copyright (C) 2002-2003,2005 Rob Buis <buis@kde.org>
   Copyright (C) 2005 Thomas Zander <zander@kde.org>
   Copyright (C) 2006 Tim Beaulen <tbscope@gmail.com>
   Copyright (C) 2006-2007 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2006 Peter Simonsson <psn@linux.se>

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

#ifndef WHIRLPINCHPLUGIN_H
#define WHIRLPINCHPLUGIN_H

#include <kdialog.h>
#include <kparts/plugin.h>

#include <QtCore/QPointF>

class KarbonView;
class WhirlPinchDlg;

class WhirlPinchPlugin : public KParts::Plugin
{
    Q_OBJECT
public:
    WhirlPinchPlugin(QWidget *parent, const QStringList &);
    virtual ~WhirlPinchPlugin() {}

private slots:
    void slotWhirlPinch();

private:
    WhirlPinchDlg *m_whirlPinchDlg;
};

class KDoubleNumInput;
class KoUnitDoubleSpinBox;
class KoUnit;

class WhirlPinchDlg : public KDialog
{
    Q_OBJECT

public:
    explicit WhirlPinchDlg(QWidget* parent = 0L, const char* name = 0L);

    qreal angle() const;
    qreal pinch() const;
    qreal radius() const;
    void setAngle(qreal value);
    void setPinch(qreal value);
    void setRadius(qreal value);
    void setUnit(const KoUnit &unit);

private:
    KDoubleNumInput* m_angle;
    KDoubleNumInput* m_pinch;
    KoUnitDoubleSpinBox* m_radius;
};

#endif // WHIRLPINCHPLUGIN_H

