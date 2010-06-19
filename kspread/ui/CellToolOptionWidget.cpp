/* This file is part of the KDE project
   Copyright 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#include "CellToolOptionWidget.h"

#include "CellStorage.h"
#include "CellToolBase.h"
#include "Editors.h"
#include "Sheet.h"
#include "Selection.h"

#include <KoCanvasBase.h>

#include <KAction>
#include <KIcon>
#include <KLocale>

#include <QGridLayout>
#include <QToolButton>

using namespace KSpread;

class CellToolOptionWidget::Private
{
public:
    CellToolBase *cellTool;
    LocationComboBox* locationComboBox;
    QToolButton *formulaButton, *applyButton, *cancelButton;
    ExternalEditor *editor;
    QGridLayout *layout;
};


CellToolOptionWidget::CellToolOptionWidget(CellToolBase *parent)
    : QWidget(parent->canvas()->canvasWidget())
    , d(new Private)
{
    d->cellTool = parent;

    d->locationComboBox = new LocationComboBox(d->cellTool, this);
    d->locationComboBox->setMinimumWidth(100);

    d->formulaButton = new QToolButton(this);
    d->formulaButton->setText(i18n("Formula"));
    d->formulaButton->setDefaultAction(d->cellTool->action("insertFormula"));

    d->applyButton = new QToolButton(this);
    d->applyButton->setText(i18n("Apply"));
    d->applyButton->setToolTip(i18n("Apply changes"));
    d->applyButton->setIcon(KIcon("dialog-ok"));
    d->applyButton->setEnabled(false);

    d->cancelButton = new QToolButton(this);
    d->cancelButton->setText(i18n("Cancel"));
    d->cancelButton->setToolTip(i18n("Discard changes"));
    d->cancelButton->setIcon(KIcon("dialog-cancel"));
    d->cancelButton->setEnabled(false);

    d->editor = new ExternalEditor(this);
    d->editor->setCellTool(d->cellTool);
    d->editor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
//     d->editor->setMinimumHeight(d->locationComboBox->height());

    d->layout = new QGridLayout(this);
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
    connect(d->cellTool->selection(), SIGNAL(changed(const Region&)),
            this, SLOT(updateLocationComboBox()));
}

CellToolOptionWidget::~CellToolOptionWidget()
{
    delete d;
}

LocationComboBox *CellToolOptionWidget::locationComboBox() const
{
    return d->locationComboBox;
}

QToolButton *CellToolOptionWidget::formulaButton() const
{
    return d->formulaButton;
}

QToolButton *CellToolOptionWidget::applyButton() const
{
    return d->applyButton;
}

QToolButton *CellToolOptionWidget::cancelButton() const
{
    return d->cancelButton;
}

ExternalEditor *CellToolOptionWidget::editor() const
{
    return d->editor;
}

void CellToolOptionWidget::resizeEvent(QResizeEvent *event)
{
    const int margin = 2 * d->layout->margin();
    const int newWidth = event->size().width();
    const int minWidth = d->layout->minimumSize().width();
    // The triggering width is the same in both cases, but it is calculated in
    // different ways.
    // After a row got occupied, it does not vanish anymore, even if all items
    // get removed. Hence, check for the existance of the item in the 2nd row.
    if (!d->layout->itemAtPosition(1, 0)) /* one row */ {
        const int column = d->layout->count() - 1;
        QLayoutItem *const item = d->layout->itemAtPosition(0, column);
        const int itemWidth = item->minimumSize().width();
        if (newWidth <= 2 * (minWidth - itemWidth) + margin) {
            d->layout->removeItem(item);
            d->layout->addItem(item, 1, 0, 1, column + 1);
        }
    } else /* two rows */ {
        if (newWidth > 2 * minWidth + margin) {
            QLayoutItem *const item = d->layout->itemAtPosition(1, 0);
            d->layout->removeItem(item);
            d->layout->addItem(item, 0, d->layout->count());
        }
    }
    QWidget::resizeEvent(event);
}

void CellToolOptionWidget::updateLocationComboBox()
{
    if (d->cellTool->selection()->referenceSelectionMode()) {
        return;
    }
    QString address;
    const QList< QPair<QRectF, QString> > names = d->cellTool->selection()->activeSheet()->cellStorage()->namedAreas(*d->cellTool->selection());
    {
        QRect range;
        if (d->cellTool->selection()->isSingular()) {
            range = QRect(d->cellTool->selection()->marker(), QSize(1, 1));
        } else {
            range = d->cellTool->selection()->lastRange();
        }
        for (int i = 0; i < names.size(); i++) {
            if (names[i].first.toRect() == range) {
                address = names[i].second;
            }
        }
    }
    if (address.isEmpty()) {
        Selection *const selection = d->cellTool->selection();
        if (selection->activeSheet()->getLcMode()) {
            if (selection->isSingular()) {
                address = 'L' + QString::number(selection->marker().y()) +
                        'C' + QString::number(selection->marker().x());
            } else {
                const QRect lastRange = selection->lastRange();
                address = QString::number(lastRange.height()) + "Lx";
                address += QString::number(lastRange.width()) + 'C';
            }
        } else {
            address = selection->name();
        }
    }
    d->locationComboBox->lineEdit()->setText(address);
}

#include "CellToolOptionWidget.moc"
