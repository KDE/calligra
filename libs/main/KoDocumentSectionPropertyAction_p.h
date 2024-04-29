/*
  SPDX-FileCopyrightText: 2006 Gábor Lehel <illissius@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KO_DOCUMENT_SECTION_PROPERTY_ACTION_P_H
#define KO_DOCUMENT_SECTION_PROPERTY_ACTION_P_H

#include "KoDocumentSectionModel.h"
#include "KoDocumentSectionView.h"

#include <QAction>
#include <QPersistentModelIndex>

/**
 * Internal class for the KoDocumentSectionView widget. Provides a
 * toggle action for a particular property associated with a document
 * section, such as visible, selected, locked. Property actions have
 * associated on/off icons to show their state in the
 * KoDocumentSectionView.
 */
class KoDocumentSectionView::PropertyAction : public QAction
{
    typedef QAction super;
    Q_OBJECT
    Model::Property m_property;
    int m_num;
    QPersistentModelIndex m_index;

Q_SIGNALS:
    void toggled(bool on, const QPersistentModelIndex &index, int property);

public:
    PropertyAction(int num, const Model::Property &p, const QPersistentModelIndex &index, QObject *parent = nullptr)
        : QAction(parent)
        , m_property(p)
        , m_num(num)
        , m_index(index)
    {
        connect(this, &QAction::triggered, this, &PropertyAction::slotTriggered);
        setText(m_property.name);
        setIcon(m_property.state.toBool() ? m_property.onIcon : m_property.offIcon);
    }

private Q_SLOTS:
    void slotTriggered()
    {
        m_property.state = !m_property.state.toBool();
        setIcon(m_property.state.toBool() ? m_property.onIcon : m_property.offIcon);
        Q_EMIT toggled(m_property.state.toBool(), m_index, m_num);
    }
};

#endif
