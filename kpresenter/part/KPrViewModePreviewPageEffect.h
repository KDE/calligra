/* This file is part of the KDE project
 * Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>
 * Copyright (C) 2008 Casper Boemann <cbr@boemann.dk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
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

#ifndef KPRVIEWMODEPREVIEWPAGEEFFECT_H
#define KPRVIEWMODEPREVIEWPAGEEFFECT_H

#include "kpresenter_export.h"

#include <KoPAViewMode.h>
#include "KPrAnimationDirector.h"
#include <QTimeLine>
#include <QPixmap>

class KPrPage;
class KPrPageEffect;
class KPrPageEffectRunner;

class KPRESENTER_EXPORT KPrViewModePreviewPageEffect : public KoPAViewMode
{

    Q_OBJECT
public:
    KPrViewModePreviewPageEffect( KoPAViewBase * view, KoPACanvasBase * m_canvas );
    ~KPrViewModePreviewPageEffect();

    void paint(KoPACanvasBase* canvas, QPainter& painter, const QRectF &paintRect);
    void tabletEvent( QTabletEvent *event, const QPointF &point );
    void mousePressEvent( QMouseEvent *event, const QPointF &point );
    void mouseDoubleClickEvent( QMouseEvent *event, const QPointF &point );
    void mouseMoveEvent( QMouseEvent *event, const QPointF &point );
    void mouseReleaseEvent( QMouseEvent *event, const QPointF &point );
    void keyPressEvent( QKeyEvent *event );
    void keyReleaseEvent( QKeyEvent *event );
    void wheelEvent( QWheelEvent * event, const QPointF &point );

    void activate( KoPAViewMode * previousViewMode );
    void deactivate();

    /// reimplemented
    virtual void updateActivePage( KoPAPageBase *page );


    /**
     * Set a page effect
     *
     * @param pageEffect The effect which will be previewed.
     * @param page The current page used in the preview. If 0 the preview will be x
     * @param prevpage The page coming before @p page
     */
    void setPageEffect( KPrPageEffect* pageEffect, KPrPage* page, KPrPage* prevpage );

public slots:
    /**
     * @brief Activate the saved view mode
     *
     * This ends the presentation mode. The view mode that was active before the
     * presentation will be restored.
     */
    void activateSavedViewMode();

protected slots:
    void animate();

private:
    void updatePixmaps();

    KoPAViewMode * m_savedViewMode;
    QTimeLine m_timeLine;

    KPrPageEffect* m_pageEffect;
    KPrPageEffectRunner* m_pageEffectRunner;
    KPrPage* m_page;
    KPrPage* m_prevpage;

    QPixmap m_oldPage;
    QPixmap m_newPage;
};

#endif /* KPRVIEWMODEPREVIEWPAGEEFFECT_H */
