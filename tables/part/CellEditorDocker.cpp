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

// KDE
#include <KIcon>
#include <KLocale>

#include <kdebug.h>

// Calligra

// Calligra Tables
#include "CanvasBase.h"
#include "ui/ExternalEditor.h"
#include "ui/LocationComboBox.h"

using namespace Calligra::Tables;

class CellEditorDocker::Private
{
public:
    CanvasBase* canvas;
    LocationComboBox* locationComboBox;
    QToolButton *formulaButton, *applyButton, *cancelButton;
    ExternalEditor *editor;
    QGridLayout *layout;
};

CellEditorDocker::CellEditorDocker()
    : d(new Private)
{
    setWindowTitle(i18n("Cell Editor"));

    d->canvas = 0;

    QWidget* w = widget();

    d->locationComboBox = new LocationComboBox(w);
    d->locationComboBox->setMinimumWidth(100);
    //connect(d->locationComboBox, SIGNAL(scrollToCell(QPoint)), d->cellTool, SLOT(scrollToCell(QPoint)));

    d->formulaButton = new QToolButton(w);
    d->formulaButton->setText(i18n("Formula"));
    //d->formulaButton->setDefaultAction(d->cellTool->action("insertFormula"));

    d->applyButton = new QToolButton(w);
    d->applyButton->setText(i18n("Apply"));
    d->applyButton->setToolTip(i18n("Apply changes"));
    d->applyButton->setIcon(KIcon("dialog-ok"));
    d->applyButton->setEnabled(false);

    d->cancelButton = new QToolButton(w);
    d->cancelButton->setText(i18n("Cancel"));
    d->cancelButton->setToolTip(i18n("Discard changes"));
    d->cancelButton->setIcon(KIcon("dialog-cancel"));
    d->cancelButton->setEnabled(false);

    d->editor = new ExternalEditor(w);
    //d->editor->setCellTool(d->cellTool);
    d->editor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
//     d->editor->setMinimumHeight(d->locationComboBox->height());

    d->layout = new QGridLayout(w);
    d->layout->setObjectName(QLatin1String("CellToolOptionWidget::Layout"));
    d->layout->addWidget(d->locationComboBox, 0, 0);
    d->layout->addWidget(d->formulaButton, 0, 1);
    d->layout->addWidget(d->applyButton, 0, 2);
    d->layout->addWidget(d->cancelButton, 0, 3);
    d->layout->addWidget(d->editor, 0, 4);
    d->layout->setColumnStretch(4, 1);

//     setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    connect(d->applyButton, SIGNAL(clicked(bool)),
            d->editor, SLOT(applyChanges()));
    connect(d->cancelButton, SIGNAL(clicked(bool)),
            d->editor, SLOT(discardChanges()));
}

CellEditorDocker::~CellEditorDocker()
{
    delete d;
}

void CellEditorDocker::setCanvas(KoCanvasBase *canvas)
{
    kDebug() << "setting canvas to" << canvas;
    d->canvas = dynamic_cast<CanvasBase*>(canvas);
    if (d->canvas) {
        d->locationComboBox->setSelection(d->canvas->selection());
    }
}

void CellEditorDocker::unsetCanvas()
{
    kDebug() << "unsetting canvas";
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
    // get removed. Hence, check for the existance of the item in the 2nd row.
    if (!d->layout->itemAtPosition(1, 0)) { /* one row */
        const int column = d->layout->count() - 1;
        QLayoutItem *const item = d->layout->itemAtPosition(0, column);
        if (!item) {
            QWidget::resizeEvent(event);
            return;
        }
        const int itemWidth = item->minimumSize().width();
        if (newWidth <= 2 *(minWidth - itemWidth) + margin) {
            d->layout->removeItem(item);
            d->layout->addItem(item, 1, 0, 1, column + 1);
            d->layout->setRowStretch(1, 1);
        }
    } else { /* two rows */
        if (newWidth > 2 * minWidth + margin) {
            QLayoutItem *const item = d->layout->itemAtPosition(1, 0);
            d->layout->removeItem(item);
            d->layout->addItem(item, 0, d->layout->count());
        }
    }
    QWidget::resizeEvent(event);
}

CellEditorDockerFactory::CellEditorDockerFactory()
{
}

QString CellEditorDockerFactory::id() const
{
    return QString::fromLatin1("CalligraTablesCellEditor");
}

QDockWidget* CellEditorDockerFactory::createDockWidget()
{
    CellEditorDocker* widget = new CellEditorDocker();
    widget->setObjectName(id());

    return widget;
}
