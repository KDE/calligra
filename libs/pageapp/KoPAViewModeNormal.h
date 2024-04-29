/* This file is part of the KDE project
 * Copyright ( C ) 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPAVIEWMODENORMAL_H
#define KOPAVIEWMODENORMAL_H

#include "KoPAViewMode.h"

class KoPAPage;
class KoPACanvasBase;
class QPainter;
class QRectF;
class KoViewConverter;

class KOPAGEAPP_EXPORT KoPAViewModeNormal : public KoPAViewMode
{
public:
    KoPAViewModeNormal(KoPAViewBase *view, KoPACanvasBase *m_canvas);
    ~KoPAViewModeNormal() override;

    void paint(KoPACanvasBase *canvas, QPainter &painter, const QRectF &paintRect) override;
    // void paintEvent( KoPACanvas * canvas, QPaintEvent* event );
    void tabletEvent(QTabletEvent *event, const QPointF &point) override;
    void mousePressEvent(QMouseEvent *event, const QPointF &point) override;
    void mouseDoubleClickEvent(QMouseEvent *event, const QPointF &point) override;
    void mouseMoveEvent(QMouseEvent *event, const QPointF &point) override;
    void mouseReleaseEvent(QMouseEvent *event, const QPointF &point) override;
    void shortcutOverrideEvent(QKeyEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event, const QPointF &point) override;

    /**
     * @brief Switch the active view mode to work on master/normal pages
     *
     * When it is switched to master mode the master page of the current active page
     * is selected. If it switches back the page which was shown before going into
     * the master mode is shown. If the mode is the same nothing happens.
     *
     * @param master if true work on master pages, if false work on normal pages
     */
    void setMasterMode(bool master) override;

    bool masterMode() override;

    void addShape(KoShape *shape) override;

    void removeShape(KoShape *shape) override;

    void changePageLayout(const KoPageLayout &pageLayout, bool applyToDocument, KUndo2Command *parent = nullptr) override;

private:
    void paintMargins(QPainter &painter, const KoViewConverter &converter);

    /// if true it works on master pages, if false on normal pages
    bool m_masterMode;
    /// the page which was active before entering the master mode
    KoPAPage *m_savedPage;
};

#endif /* KOPAVIEWMODENORMAL_H */
