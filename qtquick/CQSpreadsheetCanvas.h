/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Shantanu Tushar <shantanu@kde.org>
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#ifndef CQSPREADSHEETCANVAS_H
#define CQSPREADSHEETCANVAS_H

#include "CQCanvasBase.h"

namespace Calligra
{
namespace Sheets
{
class Map;
}
}

class KoCanvasBase;

class CQSpreadsheetCanvas : public CQCanvasBase
{
    Q_OBJECT
    Q_PROPERTY(int currentSheet READ currentSheet WRITE setCurrentSheet NOTIFY currentSheetChanged)
    Q_PROPERTY(QObjectList linkTargets READ linkTargets NOTIFY linkTargetsChanged)

public:
    explicit CQSpreadsheetCanvas(QQuickItem *parent = nullptr);
    virtual ~CQSpreadsheetCanvas();

    int currentSheet() const;
    Calligra::Sheets::Map *documentMap() const;

    QObjectList linkTargets() const;

    void setCurrentSheet(int sheet);

    void render(QPainter *painter, const QRectF &target) override;

Q_SIGNALS:
    void currentSheetChanged();
    void linkTargetsChanged();

protected:
    virtual void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    void openFile(const QString &file) override;

private Q_SLOTS:
    void updateDocumentSize(const QSize &size);

private:
    void createAndSetCanvasControllerOn(KoCanvasBase *canvas);
    void createAndSetZoomController(KoCanvasBase *canvas);

    class Private;
    Private *const d;
};

#endif // CQSPREADSHEETCANVAS_H
