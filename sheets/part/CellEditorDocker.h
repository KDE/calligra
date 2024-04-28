/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2011 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_CELL_EDITOR_DOCKER
#define CALLIGRA_SHEETS_CELL_EDITOR_DOCKER

#include <QDockWidget>

#include <KoCanvasObserverBase.h>
#include <KoDockFactoryBase.h>

namespace Calligra
{
namespace Sheets
{
class Sheet;

class CellEditorDocker : public QDockWidget, public KoCanvasObserverBase
{
    Q_OBJECT
public:
    explicit CellEditorDocker();
    ~CellEditorDocker() override;

    /// reimplemented from KoCanvasObserver
    void setCanvas(KoCanvasBase *canvas) override;
    void unsetCanvas() override;

protected: // reimplementations
    void resizeEvent(QResizeEvent *event) override;

private Q_SLOTS:
    void toolChanged(const QString &toolId);

private:
    class Private;
    Private *const d;
};

class CellEditorDockerFactory : public KoDockFactoryBase
{
public:
    CellEditorDockerFactory();

    QString id() const override;
    QDockWidget *createDockWidget() override;
    DockPosition defaultDockPosition() const override
    {
        return DockTop;
    }
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_CELL_EDITOR_DOCKER
