/* This file is part of the KDE project
 * Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>
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
#ifndef KPRPRESENTATIONHIGHLIGHTWIDGET_H
#define KPRPRESENTATIONHIGHLIGHTWIDGET_H

#include <QWidget>
#include <QtGui/QFrame>
#include <QtGui/QLabel>

#include "KPrViewModePresentation.h"
/*dans le destructeur tu pourras faire un set sur le booléen à False, et dans le constructeur un Set à true*/

class KPrPresentationHighlightWidget : public QWidget {

public :
    KPrPresentationHighlightWidget(KoPACanvas * canvas);
    ~KPrPresentationHighlightWidget();
public :
    bool m_blackBackgroundVisibility;
    QFrame *m_blackBackgroundframe;
};

#endif /* KPRPRESENTATIONHIGHLIGHTWIDGET_H */