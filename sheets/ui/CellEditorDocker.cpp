/* This file is part of the KDE project
   Copyright 2011 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "CellEditorDocker.h"

// Qt
#include <QGridLayout>
#include <QResizeEvent>
#include <QToolButton>

// KF5
#include <KLocalizedString>

// Calligra
#include <KoToolProxy.h>
#include <KoToolManager.h>

// Calligra Sheets
#include "SheetsDebug.h"
#include "CellToolBase.h"
#include "ExternalEditor.h"
#include "LocationComboBox.h"
#include "SheetView.h"
#include "part/CanvasBase.h"

using namespace Calligra::Sheets;

class CellEditorDocker::Private
{
public:
    CanvasBase *canvas;
    LocationComboBox *locationComboBox;
    QToolButton *formulaButton, *applyButton, *cancelButton;
    ExternalEditor *editor;
    QGridLayout *layout;
    CellToolBase *cellTool;
    QPointer<KoToolProxy> toolProxy;
    bool canvasResetBugWorkaround;
};

CellEditorDocker::CellEditorDocker()
    : d(new Private)
{
    setWindowTitle(i18n("Cell Editor"));

    d->canvas = 0;
    d->canvasResetBugWorkaround = false;

    QWidget* w = new QWidget(this);

    d->locationComboBox = new LocationComboBox(w);
    d->locationComboBox->setMinimumWidth(100);

    d->formulaButton = new QToolButton(w);
    d->formulaButton->setText(i18n("Formula"));

    d->editor = new ExternalEditor(w);
    d->editor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
//     d->editor->setMinimumHeight(d->locationComboBox->height());

    d->applyButton = new QToolButton(w);
    d->applyButton->setDefaultAction(d->editor->applyAction());

    d->cancelButton = new QToolButton(w);
    d->cancelButton->setDefaultAction(d->editor->cancelAction());

    d->layout = new QGridLayout(w);
    d->layout->setObjectName(QLatin1String("CellToolOptionWidget::Layout"));
    d->layout->addWidget(d->locationComboBox, 0, 0, Qt::AlignTop);
    d->layout->addWidget(d->formulaButton, 0, 1, Qt::AlignTop);
    d->layout->addWidget(d->applyButton, 0, 2, Qt::AlignTop);
    d->layout->addWidget(d->cancelButton, 0, 3, Qt::AlignTop);
    d->layout->addWidget(d->editor, 0, 4);
    d->layout->setColumnStretch(4, 1);

//     w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    setWidget(w);
}

CellEditorDocker::~CellEditorDocker()
{
    delete d;
}

void CellEditorDocker::setCanvas(KoCanvasBase *canvas)
{
    setEnabled(canvas != 0);

    d->canvasResetBugWorkaround = !!d->canvas;
    if (d->toolProxy) {
        disconnect(d->toolProxy, SIGNAL(toolChanged(QString)), this, SLOT(toolChanged(QString)));
    }
    d->canvas = dynamic_cast<CanvasBase*>(canvas);
    if (d->canvas) {
        d->locationComboBox->setSelection(d->canvas->selection());
        d->toolProxy = d->canvas->toolProxy();
        connect(d->toolProxy, SIGNAL(toolChanged(QString)), this, SLOT(toolChanged(QString)));
    }
}

void CellEditorDocker::unsetCanvas()
{
    if (d->canvasResetBugWorkaround) return;
    debugSheets << "unsetting canvas";
    if (d->toolProxy) {
        disconnect(d->toolProxy, SIGNAL(toolChanged(QString)), this, SLOT(toolChanged(QString)));
    }
    d->canvas = 0;
    d->toolProxy = 0;
    d->locationComboBox->setSelection(0);
}

void CellEditorDocker::resizeEvent(QResizeEvent *event)
{
    const int margin = 2 * d->layout->margin();
    const int newWidth = event->size().width();
    const int minWidth = d->layout->minimumSize().width();
    // The triggering width is the same in both cases, but it is calculated in
    // different ways.
    // After a row got occupied, it does not vanish anymore, even if all items
    // get removed. Hence, check for the existence of the item in the 2nd row.
    if (!d->layout->itemAtPosition(1, 0)) { /* one row */
        const int column = d->layout->count() - 1;
        QLayoutItem *const item = d->layout->itemAtPosition(0, column);
        if (!item) {
            QDockWidget::resizeEvent(event);
            return;
        }
        const int itemWidth = item->minimumSize().width();
        if (newWidth <= 2 *(minWidth - itemWidth) + margin) {
            d->layout->removeItem(item);
            d->layout->addItem(item, 1, 0, 1, column + 1);
            d->layout->setRowStretch(0, 0);
            d->layout->setRowStretch(1, 1);
        }
    } else { /* two rows */
        if (newWidth > 2 * minWidth + margin) {
            QLayoutItem *const item = d->layout->itemAtPosition(1, 0);
            d->layout->removeItem(item);
            d->layout->addItem(item, 0, d->layout->count());
            d->layout->setRowStretch(0, 1);
            d->layout->setRowStretch(1, 0);
        }
    }
    QDockWidget::resizeEvent(event);
}

void CellEditorDocker::toolChanged(const QString &toolId)
{
    debugSheets << "tool changed to" << toolId;

    const bool isCellTool = toolId == QLatin1String("KSpreadCellToolId");
    setEnabled(isCellTool);

    if (isCellTool) {
        KoToolBase* tool = KoToolManager::instance()->toolById(d->canvas, toolId);
        d->cellTool = qobject_cast<CellToolBase*>(tool);
        Q_ASSERT(d->cellTool);
        d->editor->setCellTool(d->cellTool);
        d->cellTool->setExternalEditor(d->editor);
        d->formulaButton->setDefaultAction(d->cellTool->action("insertFormula"));
        debugSheets << tool << d->cellTool;
    }
}

CellEditorDockerFactory::CellEditorDockerFactory()
{
}

QString CellEditorDockerFactory::id() const
{
    return QString::fromLatin1("CalligraSheetsCellEditor");
}

QDockWidget* CellEditorDockerFactory::createDockWidget()
{
    CellEditorDocker* widget = new CellEditorDocker();
    widget->setObjectName(id());

    return widget;
}
