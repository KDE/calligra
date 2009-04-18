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
#ifndef KPRPRESENTATIONTOOL_H
#define KPRPRESENTATIONTOOL_H

#include <KoTool.h>
#include <KDialog>
#include <QtGui/QFrame>
#include <QtGui/QLabel>

#include <QList>
#include "ui/KPrPresentationToolWidget.h"
#include "KPrPresentationHighlightWidget.h"
#include "KPrPresentationDrawWidget.h"

class KoEventAction;
class KPrViewModePresentation;
class KPrPresentationStrategyInterface;

/// The tool used for presentations
class KPrPresentationTool : public KoTool
{
    Q_OBJECT
public:
    explicit KPrPresentationTool( KPrViewModePresentation & viewMode );
    ~KPrPresentationTool();

    bool wantsAutoScroll();

    void paint( QPainter &painter, const KoViewConverter &converter );

    void mousePressEvent( KoPointerEvent *event );
    void mouseDoubleClickEvent( KoPointerEvent *event );
    void mouseMoveEvent( KoPointerEvent *event );
    void mouseReleaseEvent( KoPointerEvent *event );
    void keyPressEvent( QKeyEvent *event );
    void keyReleaseEvent( QKeyEvent *event );
    void wheelEvent( KoPointerEvent * event );

    bool getDrawMode();
    bool getHighlightMode();
    KPrViewModePresentation & getViewMode();

public slots:
    void activate( bool temporary = false );
    void deactivate();
    void highLightPresentation();
    void drawOnPresentation();

private:
    void finishEventActions();
    void switchStrategy( KPrPresentationStrategyInterface * strategy );
    bool eventFilter( QObject * obj, QEvent * event );

    KPrViewModePresentation & m_viewMode;

    QList<KoEventAction *> m_eventActions;

    KPrPresentationToolWidget * m_presentationToolWidget;
    QFrame * m_frame;
    KPrPresentationStrategyInterface * m_strategy;
    friend class KPrPresentationStrategyInterface;
};

#endif /* KPRPRESENTATIONTOOL_H */

