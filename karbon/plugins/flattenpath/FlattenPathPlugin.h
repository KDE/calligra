/* This file is part of the KDE project
   Copyright (C) 2002-2003,2005 Rob Buis <buis@kde.org>
   Copyright (C) 2005 Thomas Zander <zander@kde.org>
   Copyright (C) 2006 Tim Beaulen <tbscope@gmail.com>
   Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>

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

#ifndef FLATTENPATHPLUGIN_H
#define FLATTENPATHPLUGIN_H

#include <kparts/plugin.h>
#include <kdialog.h>

class FlattenDlg;

class FlattenPathPlugin : public KParts::Plugin
{
    Q_OBJECT
public:
    FlattenPathPlugin(QWidget *parent, const QStringList &);
    virtual ~FlattenPathPlugin() {}

private slots:
    void slotFlattenPath();

private:
    FlattenDlg *m_flattenPathDlg;
};

class KDoubleNumInput;

class FlattenDlg : public KDialog
{
    Q_OBJECT

public:
    explicit FlattenDlg(QWidget* parent = 0L, const char* name = 0L);

    qreal flatness() const;
    void setFlatness(qreal value);

private:
    KDoubleNumInput* m_flatness;
};

#endif

