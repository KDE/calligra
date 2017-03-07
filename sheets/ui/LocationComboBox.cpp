/* This file is part of the KDE project
   Copyright 1999-2006 The KSpread Team <calligra-devel@kde.org>

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

#include "LocationComboBox.h"

// Sheets
#include "CellStorage.h"
#include "Map.h"
#include "NamedAreaManager.h"
#include "Selection.h"
#include "Sheet.h"
#include "SheetsDebug.h"

#include "commands/NamedAreaCommand.h"

// Qt
#include <QKeyEvent>
#include <QLineEdit>

using namespace Calligra::Sheets;

LocationComboBox::LocationComboBox(QWidget *_parent)
        : KComboBox(true, _parent)
        , m_selection(0)
{
    setCompletionObject(&completionList, true);
    setCompletionMode(KCompletion::CompletionAuto);

    connect(this, SIGNAL(activated(QString)),
            this, SLOT(slotActivateItem()));
}

void LocationComboBox::setSelection(Selection *selection)
{
    if (m_selection == selection) return;

    completionList.clear();
    clear();
    if (m_selection) {
        if (m_selection->activeSheet()) {
            Map *const oldMap = m_selection->activeSheet()->map();
            disconnect(oldMap->namedAreaManager(), SIGNAL(namedAreaAdded(QString)), this, SLOT(slotAddAreaName(QString)));
            disconnect(oldMap->namedAreaManager(), SIGNAL(namedAreaRemoved(QString)), this, SLOT(slotRemoveAreaName(QString)));
        }
        disconnect(m_selection, SIGNAL(activeSheetChanged(Sheet*)), this, SLOT(slotActiveSheetChanged(Sheet*)));
        disconnect(m_selection, SIGNAL(changed(Region)), this, SLOT(slotSelectionChanged()));
    }

    m_selection = selection;

    if (m_selection) {
        insertItem(0, QString());
        updateAddress();
        Sheet* sheet = m_selection->activeSheet();
        if (sheet) {
            slotActiveSheetChanged(sheet);
        } else {
            connect(m_selection, SIGNAL(activeSheetChanged(Sheet*)), this, SLOT(slotActiveSheetChanged(Sheet*)));
        }
        connect(m_selection, SIGNAL(changed(Region)), this, SLOT(slotSelectionChanged()));
    }
}

void LocationComboBox::slotActiveSheetChanged(Sheet *sheet)
{
    if (!sheet) return;
    disconnect(this, SLOT(slotActiveSheetChanged(Sheet*)));

    Map *const map = sheet->map();
    const QList<QString> areaNames = map->namedAreaManager()->areaNames();
    for (int i = 0; i < areaNames.count(); ++i)
        slotAddAreaName(areaNames[i]);

    connect(map->namedAreaManager(), SIGNAL(namedAreaAdded(QString)), this, SLOT(slotAddAreaName(QString)));
    connect(map->namedAreaManager(), SIGNAL(namedAreaRemoved(QString)), this, SLOT(slotRemoveAreaName(QString)));
}

void LocationComboBox::updateAddress()
{
    if (!m_selection) return;

    QString address;
    Selection *const selection = m_selection;
    Sheet *const sheet = m_selection->activeSheet();
    if (sheet) {
        const QList< QPair<QRectF, QString> > names = sheet->cellStorage()->namedAreas(*selection);
        {
            QRect range;
            if (selection->isSingular()) range = QRect(selection->marker(), QSize(1, 1));
            else range = selection->lastRange();
            for (int i = 0; i < names.size(); i++) {
                if (names[i].first.toRect() == range) {
                    address = names[i].second;
                }
            }
        }
    }
    if (sheet && sheet->getLcMode()) {
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
    setItemText(0, address);
    setCurrentItem(0);
    lineEdit()->setText(address);
}

void LocationComboBox::slotAddAreaName(const QString &_name)
{
    insertItem(count(), _name);
    addCompletionItem(_name);
}

void LocationComboBox::slotRemoveAreaName(const QString &_name)
{
    for (int i = 0; i < count(); i++) {
        if (itemText(i) == _name) {
            removeItem(i);
            break;
        }
    }
    removeCompletionItem(_name);
}

void LocationComboBox::addCompletionItem(const QString &_item)
{
    if (completionList.items().contains(_item) == 0) {
        completionList.addItem(_item);
        debugSheetsUI << _item;
    }
}

void LocationComboBox::removeCompletionItem(const QString &_item)
{
    completionList.removeItem(_item);
}

void LocationComboBox::slotActivateItem()
{
    if (!m_selection) return;

    if (activateItem()) {
        m_selection->scrollToCursor();
    }
}

bool LocationComboBox::activateItem()
{
    if (!m_selection) return false;

    Selection *const selection = m_selection;

    // Set the focus back on the canvas.
    parentWidget()->setFocus();

    const QString text = lineEdit()->text();
    // check whether an already existing named area was entered
    Region region = selection->activeSheet()->map()->namedAreaManager()->namedArea(text);
    if (region.isValid()) {
        // TODO Stefan: Merge the sheet change into Selection.
        if (region.firstSheet() != selection->activeSheet()) {
            selection->emitVisibleSheetRequested(region.firstSheet());
        }
        selection->initialize(region);
        return true;
    }

    // check whether a valid cell region was entered
    region = Region(text, selection->activeSheet()->map(), selection->activeSheet());
    if (region.isValid()) {
        // TODO Stefan: Merge the sheet change into Selection.
        if (region.firstSheet() != selection->activeSheet()) {
            selection->emitVisibleSheetRequested(region.firstSheet());
        }
        selection->initialize(region);
        return true;
    }

    // A name for an area entered?
    // FIXME Stefan: allow all characters
    bool validName = true;
    for (int i = 0; i < text.length(); ++i) {
        if (!text[i].isLetter()) {
            validName = false;
            break;
        }
    }
    if (validName) {
        NamedAreaCommand* command = new NamedAreaCommand();
        command->setSheet(selection->activeSheet());
        command->setAreaName(text);
        command->add(Region(selection->lastRange(), selection->activeSheet()));
        if (command->execute())
            return true;
        else
            delete command;
    }
    return false;
}


void LocationComboBox::keyPressEvent(QKeyEvent * _ev)
{
    if (!m_selection) return;

    Selection *const selection = m_selection;

    // Do not handle special keys and accelerators. This is
    // done by KComboBox.
    if (_ev->modifiers() & (Qt::AltModifier | Qt::ControlModifier)) {
        KComboBox::keyPressEvent(_ev);
        // Never allow that keys are passed on to the parent.
        _ev->accept(); // QKeyEvent

        return;
    }

    // Handle some special keys here. Eve
    switch (_ev->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter: {
        if (activateItem()) {
            selection->scrollToCursor();
            return;
        }
        _ev->accept(); // QKeyEvent
    }
    break;
    // Escape pressed, restore original value
    case Qt::Key_Escape:
        updateAddress();
        parentWidget()->setFocus();
        _ev->accept(); // QKeyEvent
        break;
    default:
        KComboBox::keyPressEvent(_ev);
        // Never allow that keys are passed on to the parent.
        _ev->accept(); // QKeyEvent
    }
}

void LocationComboBox::slotSelectionChanged()
{
    if (!m_selection->referenceSelectionMode()) {
        updateAddress();
    }
}
