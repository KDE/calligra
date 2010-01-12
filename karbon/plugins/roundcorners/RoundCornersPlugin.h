/* This file is part of the KDE project
 * Copyright (C) 2002-2003,2005 Rob Buis <buis@kde.org>
 * Copyright (C) 2005 Laurent Montel <montel@kde.org>
 * Copyright (C) 2005 Thomas Zander <zander@kde.org>
 * Copyright (C) 2006 Tim Beaulen <tbscope@gmail.com>
 * Copyright (C) 2007 David Faure <faure@kde.org>
 * Copyright (C) 2008 Jan Hambrecht <jaham@gmx.net>
 * Copyright (C) 2008 Patrick Spendrin <ps_ml@gmx.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef ROUNDCORNERSPLUGIN_H
#define ROUNDCORNERSPLUGIN_H

#include <kparts/plugin.h>
#include <kdialog.h>

#include <QtGui/QUndoCommand>

class KarbonView;
class RoundCornersDlg;

class RoundCornersPlugin : public KParts::Plugin
{
    Q_OBJECT
public:
    RoundCornersPlugin(QWidget * parent, const QStringList &);
    virtual ~RoundCornersPlugin();

private slots:
    void slotRoundCorners();

private:
    RoundCornersDlg * m_roundCornersDlg;
};

class KoUnitDoubleSpinBox;
class KoUnit;

class RoundCornersDlg : public KDialog
{
    Q_OBJECT

public:
    explicit RoundCornersDlg(QWidget* parent = 0L, const char* name = 0L);

    qreal radius() const;
    void setRadius(qreal value);
    void setUnit(const KoUnit &unit);

private:
    KoUnitDoubleSpinBox * m_radius;
};

#endif // ROUNDCORNERSPLUGIN_H

