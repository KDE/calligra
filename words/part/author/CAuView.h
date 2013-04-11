/* This file is part of the KDE project
 * Copyright (C) 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 * Copyright (C) 2012 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
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
 * Boston, MA 02110-1301, USA
 */

#ifndef CAUVIEW_H
#define CAUVIEW_H

#include "words_export.h"
#include "../dockers/KWStatisticsWidget.h"
#include "../dockers/KWStatisticsDocker.h"

#include "KWView.h"

class KWDocument;
class KoPart;
class KToggleAction;
class QWidget;

class WORDS_EXPORT CAuView : public KWView
{
    Q_OBJECT

public:
    CAuView(KoPart *part, KWDocument *document, QWidget *parent);
    QPair<QString, QByteArray> getCurrentCoverImage();
    void setCurrentCoverImage(QPair<QString, QByteArray> img);

private slots:
    void selectCoverImage();
    void showStatsInStatusBar(bool toggled);

private:
    void setupActions();
    void buildAssociatedWidget();
    KWStatisticsWidget *stats;
};

#endif
