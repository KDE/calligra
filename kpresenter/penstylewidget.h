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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef PENSTYLEWIDGET_H
#define PENSTYLEWIDGET_H

#include "global.h"

#include <qwidget.h>
#include <qpen.h>

#include "kprcommand.h"

class PenStyleUI;


class PenStyleWidget : public QWidget
{
    Q_OBJECT
public:
    PenStyleWidget( QWidget *parent, const char *name, const PenCmd::Pen &pen, bool configureLineEnds = true );
    ~PenStyleWidget();

    QPen getPen() const;
    LineEnd getLineBegin() const;
    LineEnd getLineEnd() const;
    int getPenConfigChange() const;

    void setPen( const PenCmd::Pen &pen );
    
private:
    void setPen( const QPen &pen );
    void setLineBegin( LineEnd lb );
    void setLineEnd( LineEnd le );

    PenCmd::Pen m_pen;

    PenStyleUI *m_ui;

private slots:
    void slotReset();
    void slotPenChanged();
    void slotLineBeginChanged();
    void slotLineEndChanged();
};

#endif /* PENSTYLEWIDGET_H */
