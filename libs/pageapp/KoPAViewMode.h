/* This file is part of the KDE project
 * Copyright ( C ) 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPAVIEWMODE_H
#define KOPAVIEWMODE_H

#include "kopageapp_export.h"

#include <QObject>
#include <QPointF>

struct KoPageLayout;
class KoPAViewBase;
class KoPACanvas;
class KoPACanvasBase;
class KoPAPageBase;
class KoToolProxy;
class KoShape;
class KoViewConverter;
class QPainter;
class QPaintEvent;
class QTabletEvent;
class QMouseEvent;
class QKeyEvent;
class QWheelEvent;
class QCloseEvent;
class QRectF;
class KUndo2Command;

class KOPAGEAPP_EXPORT KoPAViewMode : public QObject
{
    Q_OBJECT
public:
    KoPAViewMode(KoPAViewBase *view, KoPACanvasBase *canvas, const QString &name = QString());
    ~KoPAViewMode() override;

    virtual void paint(KoPACanvasBase *canvas, QPainter &painter, const QRectF &paintRect) = 0;
    // virtual void paintEvent( KoPACanvas * canvas, QPaintEvent* event ) = 0;
    virtual void tabletEvent(QTabletEvent *event, const QPointF &point) = 0;
    virtual void mousePressEvent(QMouseEvent *event, const QPointF &point) = 0;
    virtual void mouseDoubleClickEvent(QMouseEvent *event, const QPointF &point) = 0;
    virtual void mouseMoveEvent(QMouseEvent *event, const QPointF &point) = 0;
    virtual void mouseReleaseEvent(QMouseEvent *event, const QPointF &point) = 0;
    virtual void shortcutOverrideEvent(QKeyEvent *event) = 0;
    virtual void keyPressEvent(QKeyEvent *event) = 0;
    virtual void keyReleaseEvent(QKeyEvent *event) = 0;
    virtual void wheelEvent(QWheelEvent *event, const QPointF &point) = 0;
    /**
     * The default implementation ignores this event
     */
    virtual void closeEvent(QCloseEvent *event);

    /**
     * @brief Switch the active view mode to work on master/normal pages
     *
     * The default implementation does not change anything. If it is needed in the
     * view mode you have to implement it.
     *
     * @param master if true work on master pages, if false work on normal pages
     */
    virtual void setMasterMode(bool master);

    /**
     * @brief Check if the active view mode works on master/normal pages
     *
     * The default implementation always returns false
     *
     * @return false
     */
    virtual bool masterMode();

    /**
     * @brief This method is called when the view mode is activated
     *
     * The default implementation does nothing.
     *
     * @param previousViewMode the view mode which was active before the
     *        activation of this view mode;
     */
    virtual void activate(KoPAViewMode *previousViewMode);

    /**
     * @brief This method is called when the view mode is deactivated
     *
     * The default implementation does nothing.
     */
    virtual void deactivate();

    /**
     * @brief Get the canvas
     *
     * @return canvas canvas used by the view mode
     */
    KoPACanvasBase *canvas() const;

    /**
     * @brief Get the view
     *
     * @return view view used by the view mode
     */
    KoPAViewBase *view() const;

    /**
     * @brief Get the view mode's implementation of view converter
     *
     * The default implementation returns the KoPAView's view converter
     *
     * @return the view converter used in the view mode
     */
    virtual KoViewConverter *viewConverter(KoPACanvasBase *canvas);

    virtual const KoPageLayout &activePageLayout() const;

    virtual void changePageLayout(const KoPageLayout &pageLayout, bool applyToDocument, KUndo2Command *parent = nullptr);

    QPointF origin();

    void setOrigin(const QPointF &origin);
    void setName(const QString &name);
    QString name() const;

public Q_SLOTS:

    /**
     * @brief Update the view when a new shape is added to the document
     *
     * The default implementation does nothing. The derived class' implementation
     * should check whether the new shape is added to currently active page.
     *
     * @param shape the new shape added to the document
     */
    virtual void addShape(KoShape *shape);

    /**
     * @brief Update the view when a shape is removed from the document
     *
     * The default implementation does nothing. The derived class' implementation
     * should check whether the shape is removed from currently active page.
     *
     * @param shape the shape removed from the document
     */
    virtual void removeShape(KoShape *shape);

    /**
     * @brief Update the view based on the active page
     *
     * The default implementation calls the KoPAView's updateActivePage(). If
     * other behavior is intended when updating active page, the derived class
     * should reimplement this function.
     *
     * @see KoPAView::updateActivePage()
     *
     * @param page the new page to be updated on the view mode
     */
    virtual void updateActivePage(KoPAPageBase *page);

protected:
    KoPACanvasBase *m_canvas;
    KoToolProxy *m_toolProxy;
    KoPAViewBase *m_view;
    QPointF m_origin;
    QString m_name;
};

#endif /* KOPAVIEWMODE_H */
