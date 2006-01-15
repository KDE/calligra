// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2004 Thorsten Zachmann <zachmann@kde.orgReginald Stadlbauer <reggie@kde.org>

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

#ifndef PENSTYLEWIDGET_H
#define PENSTYLEWIDGET_H

#include "global.h"

#include <qwidget.h>

#include "KPrCommand.h"
#include <KoPen.h>

class PenStyleUI;


class KPrPenStyleWidget : public QWidget
{
    Q_OBJECT
public:
    KPrPenStyleWidget( QWidget *parent, const char *name, const KoPenCmd::Pen &pen, bool configureLineEnds = true );
    ~KPrPenStyleWidget();

    int getPenConfigChange() const;
    KoPenCmd::Pen getPen() const;

    void setPen( const KoPenCmd::Pen &pen );
    void apply();

private:
    KoPen getKPPen() const;
    LineEnd getLineBegin() const;
    LineEnd getLineEnd() const;

    void setPen( const KoPen &pen );
    void setLineBegin( LineEnd lb );
    void setLineEnd( LineEnd le );

    KoPenCmd::Pen m_pen;

    PenStyleUI *m_ui;

private slots:
    void slotReset();
    void slotPenChanged();
    void slotLineBeginChanged();
    void slotLineEndChanged();
};

#endif /* PENSTYLEWIDGET_H */
