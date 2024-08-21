/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2011 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "CellEditorWidget.h"
#include "CanvasBase.h"

// Qt
#include <QGridLayout>
#include <QHBoxLayout>
#include <QToolButton>

// KF5
#include <KLocalizedString>

// Calligra
#include <KoToolManager.h>
#include <KoToolProxy.h>

// Calligra Sheets
#include "ui/CellToolBase.h"
#include "ui/ExternalEditor.h"
#include "ui/LocationComboBox.h"

using namespace Calligra::Sheets;

class CellEditorWidget::Private
{
public:
    CanvasBase *canvas;
    LocationComboBox *locationComboBox;
    QToolButton *formulaButton, *applyButton, *cancelButton;
    ExternalEditor *editor;
    QHBoxLayout *layout;
    CellToolBase *cellTool;
    QPointer<KoToolProxy> toolProxy;
    bool canvasResetBugWorkaround;
};

CellEditorWidget::CellEditorWidget(QWidget *parent)
    : QWidget(parent)
    , d(std::make_unique<Private>())
{
    setWindowTitle(i18n("Cell Editor"));

    d->canvas = nullptr;
    d->canvasResetBugWorkaround = false;

    d->locationComboBox = new LocationComboBox(this);
    d->locationComboBox->setMinimumWidth(100);

    d->formulaButton = new QToolButton(this);
    d->formulaButton->setText(i18n("Formula"));

    d->editor = new ExternalEditor(this);
    d->editor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    d->editor->setMaximumHeight(d->locationComboBox->height());

    d->applyButton = new QToolButton(this);
    d->applyButton->setDefaultAction(d->editor->applyAction());

    d->cancelButton = new QToolButton(this);
    d->cancelButton->setDefaultAction(d->editor->cancelAction());

    d->layout = new QHBoxLayout(this);
    d->layout->setObjectName(QLatin1String("CellToolOptionWidget::Layout"));
    d->layout->addWidget(d->locationComboBox);
    d->layout->addWidget(d->formulaButton);
    d->layout->addWidget(d->applyButton);
    d->layout->addWidget(d->cancelButton);
    d->layout->addWidget(d->editor);
}

CellEditorWidget::~CellEditorWidget() = default;

void CellEditorWidget::setCanvas(KoCanvasBase *canvas)
{
    setEnabled(canvas != nullptr);

    d->canvasResetBugWorkaround = !!d->canvas;
    if (d->toolProxy) {
        disconnect(d->toolProxy.data(), &KoToolProxy::toolChanged, this, &CellEditorWidget::toolChanged);
    }
    d->canvas = dynamic_cast<CanvasBase *>(canvas);
    if (d->canvas) {
        d->locationComboBox->setSelection(d->canvas->selection());
        d->toolProxy = d->canvas->toolProxy();
        connect(d->toolProxy.data(), &KoToolProxy::toolChanged, this, &CellEditorWidget::toolChanged);
    }
}

void CellEditorWidget::unsetCanvas()
{
    if (d->canvasResetBugWorkaround)
        return;
    debugSheets << "unsetting canvas";
    if (d->toolProxy) {
        disconnect(d->toolProxy.data(), &KoToolProxy::toolChanged, this, &CellEditorWidget::toolChanged);
    }
    d->canvas = nullptr;
    d->toolProxy = nullptr;
    d->locationComboBox->setSelection(nullptr);
}

void CellEditorWidget::toolChanged(const QString &toolId)
{
    debugSheets << "tool changed to" << toolId;

    const bool isCellTool = toolId == QLatin1String("KSpreadCellToolId");
    setEnabled(isCellTool);

    if (isCellTool) {
        KoToolBase *tool = KoToolManager::instance()->toolById(d->canvas, toolId);
        d->cellTool = qobject_cast<CellToolBase *>(tool);
        Q_ASSERT(d->cellTool);
        d->editor->setCellTool(d->cellTool);
        d->cellTool->setExternalEditor(d->editor);
        d->formulaButton->setDefaultAction(d->cellTool->action("insertFormula"));
        debugSheets << tool << d->cellTool;
    }
}
