/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOPACANVASBASE_H
#define KOPACANVASBASE_H

#include <KoCanvasBase.h>

#include "kopageapp_export.h"

class KoPAViewBase;
class KoPADocument;

class QRect;
class QPainter;

/// Widget that shows a KoPAPage
class KOPAGEAPP_EXPORT KoPACanvasBase : public KoCanvasBase
{
public:
    explicit KoPACanvasBase(KoPADocument *doc);
    ~KoPACanvasBase() override;

    /// set the viewbase on the canvas; this needs to be called before the canvas can be used.
    void setView(KoPAViewBase *view);

    /// Update the canvas
    virtual void repaint() = 0;

    /// Returns pointer to the KoPADocument
    KoPADocument *document() const;

    /// reimplemented method
    void gridSize(qreal *horizontal, qreal *vertical) const override;
    /// reimplemented method
    bool snapToGrid() const override;
    /// reimplemented method
    void addCommand(KUndo2Command *command) override;
    /// reimplemented method
    KoShapeManager *shapeManager() const override;
    KoShapeManager *masterShapeManager() const;
    /// reimplemented from KoCanvasBase
    KoGuidesData *guidesData() override;

    KoToolProxy *toolProxy() const override;
    KoViewConverter *viewConverter() const override;
    KoUnit unit() const override;

    /// XXX
    void setDocumentOffset(const QPoint &offset);

    /// XXX
    const QPoint &documentOffset() const;

    /// reimplemented in view coordinates
    QPoint documentOrigin() const override;
    /// Set the origin of the page inside the canvas in document coordinates
    void setDocumentOrigin(const QPointF &origin);

    KoPAViewBase *koPAView() const;

    /// translate widget coordinates to view coordinates
    QPoint widgetToView(const QPoint &p) const;
    QRect widgetToView(const QRect &r) const;
    QPoint viewToWidget(const QPoint &p) const;
    QRect viewToWidget(const QRect &r) const;

    /// Recalculates the size of the canvas (needed when zooming or changing pagelayout)
    virtual void updateSize() = 0;

    /// Show/hide page margins
    void setShowPageMargins(bool state);
    /// Returns true if page margins shall be shown
    bool showPageMargins() const;

protected:
    void paint(QPainter &painter, const QRectF &paintRect);

private:
    class Private;
    Private *const d;
};

#endif /* KOPACANVASBASE_H */
