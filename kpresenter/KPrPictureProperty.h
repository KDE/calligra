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

#ifndef PICTUREPROPERTY_H
#define PICTUREPROPERTY_H

#include <QWidget>

#include "KPrCommand.h"

class PicturePropertyUI;
class QPixmap;

class KPrPictureProperty : public QWidget
{
    Q_OBJECT
public:
    KPrPictureProperty( QWidget *parent, const char *name, const QPixmap &pixmap,
                     KPrPictureSettingCmd::PictureSettings pictureSettings );
    ~KPrPictureProperty();

    int getPicturePropertyChange() const;
    KPrPictureSettingCmd::PictureSettings getPictureSettings() const;

    void apply();

private:
    PicturePropertyUI *m_ui;

    KPrPictureSettingCmd::PictureSettings m_pictureSettings;

protected slots:
    void slotReset();
};

#endif /* PICTUREPROPERTY_H */
