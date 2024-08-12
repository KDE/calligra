/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2011 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <KoCanvasObserverBase.h>
#include <KoDockFactoryBase.h>
#include <QWidget>

namespace Calligra
{
namespace Sheets
{
class Sheet;

class CellEditorWidget : public QWidget, public KoCanvasObserverBase
{
    Q_OBJECT
public:
    explicit CellEditorWidget(QWidget *parent = nullptr);
    ~CellEditorWidget() override;

    /// reimplemented from KoCanvasObserver
    void setCanvas(KoCanvasBase *canvas) override;
    void unsetCanvas() override;

private Q_SLOTS:
    void toolChanged(const QString &toolId);

private:
    class Private;
    std::unique_ptr<Private> const d;
};

} // namespace Sheets
} // namespace Calligra
