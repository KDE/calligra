/* This file is part of the KDE project
 * Copyright (C) 2007-2009 Thorsten Zachmann <zachmann@kde.org>
 * Copyright (C) 2008 Jim Courtiau <jeremy.courtiau@gmail.com>
 * Copyright (C) 2009 Alexia Allanic <alexia_allanic@yahoo.fr>
 * Copyright (C) 2009 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 * Copyright (C) 2009 Jérémy Lugagne <jejewindsurf@hotmail.com>
 * Copyright (C) 2009 Johann Hingue <yoan1703@hotmail.fr>
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

#include <KoToolBase.h>

#include <QSet>

class QFrame;
class KoEventAction;
class KPrViewModePresentation;
class KPrPresentationToolWidget;
class KPrPresentationStrategyBase;
class KPrPresentationToolAdaptor;

/// The tool used for presentations
class KPrPresentationTool : public KoToolBase
{
    Q_OBJECT
public:
    explicit KPrPresentationTool( KPrViewModePresentation &viewMode );
    ~KPrPresentationTool();

    bool wantsAutoScroll() const;

    void paint( QPainter &painter, const KoViewConverter &converter );

    void mousePressEvent( KoPointerEvent *event );
    void mouseDoubleClickEvent( KoPointerEvent *event );
    void mouseMoveEvent( KoPointerEvent *event );
    void mouseReleaseEvent( KoPointerEvent *event );
    void keyPressEvent( QKeyEvent *event );
    void keyReleaseEvent( QKeyEvent *event );
    void wheelEvent( KoPointerEvent * event );

    KPrPresentationStrategyBase *strategy();
    KPrViewModePresentation & viewModePresentation();

public slots:
    virtual void activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes);
    void deactivate();
    void highlightPresentation();
    void drawOnPresentation();
    void blackPresentation();
    void normalPresentation();

private:
    void finishEventActions();
    void switchStrategy( KPrPresentationStrategyBase * strategy );
    bool eventFilter( QObject * obj, QEvent * event );

    KPrViewModePresentation & m_viewMode;
    QSet<KoEventAction *> m_eventActions;

    KPrPresentationToolWidget * m_presentationToolWidget;
    QFrame * m_frame;
    KPrPresentationStrategyBase * m_strategy;
    KPrPresentationToolAdaptor *m_bus;
    friend class KPrPresentationStrategyBase;
};

#endif /* KPRPRESENTATIONTOOL_H */
