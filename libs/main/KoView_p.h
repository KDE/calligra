/* This file is part of the Calligra project, made within the KDE community.
 * SPDX-FileCopyrightText: 2012 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOVIEW_P_H
#define KOVIEW_P_H

// Calligra
#include "KoDocument.h"
#include "KoUnit.h"
// Qt
#include <QAction>
#include <QActionGroup>

// Action group which keeps the actions in sync with the document's unit property
class UnitActionGroup : public QActionGroup
{
    Q_OBJECT
public:
    explicit UnitActionGroup(KoDocument *document, bool addPixelUnit, QObject *parent = nullptr)
        : QActionGroup(parent)
        , m_document(document)
        , m_listOptions(addPixelUnit ? KoUnit::ListAll : KoUnit::HidePixel)
    {
        setExclusive(true);
        connect(this, &QActionGroup::triggered, this, &UnitActionGroup::onTriggered);
        connect(document, &KoDocument::unitChanged, this, &UnitActionGroup::onUnitChanged);

        const QStringList unitNames = KoUnit::listOfUnitNameForUi(m_listOptions);
        const int currentUnitIndex = m_document->unit().indexInListForUi(m_listOptions);

        for (int i = 0; i < unitNames.count(); ++i) {
            QAction *action = new QAction(unitNames.at(i), this);
            action->setData(i);
            action->setCheckable(true);

            if (currentUnitIndex == i) {
                action->setChecked(true);
            }
        }
    }

private Q_SLOTS:
    void onTriggered(QAction *action)
    {
        m_document->setUnit(KoUnit::fromListForUi(action->data().toInt(), m_listOptions));
    }

    void onUnitChanged(const KoUnit &unit)
    {
        const int indexInList = unit.indexInListForUi(m_listOptions);

        foreach (QAction *action, actions()) {
            if (action->data().toInt() == indexInList) {
                action->setChecked(true);
                break;
            }
            // in case the new unit is not in the list of actions
            // this ensures that the action currently checked is unchecked
            // once the end of actions has been reached
            if (action->isChecked()) {
                action->setChecked(false);
            }
        }
    }

private:
    KoDocument *m_document;
    KoUnit::ListOptions m_listOptions;
};

#endif
