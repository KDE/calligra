/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2009 Casper Boemann <cbo@boemann.dk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "FormattingButton.h"

#include <QMenu>
#include <QFrame>
#include <QLabel>
#include <QGridLayout>
#include <QWidgetAction>

#include <kdebug.h>

//This class is a helper to add a label
class LabelAction : public QWidgetAction
{
public:
    LabelAction(QString label);
    QLabel *m_label;
};

LabelAction::LabelAction(QString label)
 : QWidgetAction(0)
{
    m_label = new QLabel(label);
    setDefaultWidget(m_label);
}

//This class is the main place where the expanding grid is done
class ItemChooserAction : public QWidgetAction
{
public:
    ItemChooserAction(int columns);
    QWidget *m_widget;
    QGridLayout *m_containerLayout;
    int m_cnt;
    int m_columns;
    QToolButton *addItem(QPixmap pm);
    void addBlanks(int n);
};

ItemChooserAction::ItemChooserAction(int columns)
 : QWidgetAction(0)
 , m_cnt(0)
 , m_columns(columns)
{
    m_widget = new QWidget;
    m_containerLayout = new QGridLayout();
    m_containerLayout->setSpacing(4);
    m_widget->setLayout(m_containerLayout);
    setDefaultWidget(m_widget);
    m_widget->setBackgroundRole(QPalette::Base);
}

QToolButton *ItemChooserAction::addItem(QPixmap pm)
{
    QToolButton *b = new QToolButton();
    b->setIcon(QIcon(pm));
    b->setIconSize(pm.size());
    b->setAutoRaise(true);
    m_containerLayout->addWidget(b, m_cnt / m_columns, m_cnt % m_columns);
    ++m_cnt;
    return b;
}

void ItemChooserAction::addBlanks(int n)
{
    m_cnt += n;
}


FormattingButton::FormattingButton(QWidget *parent)
    : QToolButton(parent)
    , m_lastId(0)
    , m_styleAction(0)
    , m_columns(1)
    , m_menuShownFirstTime(true)
{
    m_menu = new QMenu();
    setPopupMode(MenuButtonPopup);
    setMenu(m_menu);
    connect(this, SIGNAL(released()), this, SLOT(itemSelected()));
    connect(m_menu, SIGNAL(aboutToHide()), this, SIGNAL(doneWithFocus()));
    connect(m_menu, SIGNAL(aboutToShow()), this, SIGNAL(aboutToShowMenu()));
    connect(m_menu, SIGNAL(aboutToHide()), this, SLOT(menuShown()));
}

void FormattingButton::setNumColumns(int columns)
{
    m_styleAction = 0;
    m_columns = columns;
}

void FormattingButton::addItem(QPixmap pm, int id, QString toolTip)
{
    //Note: Do not use 0 as the item id, because that will break the m_lastId functionality
    Q_ASSERT(id != 0);

    if (m_styleMap.contains(id)) {
        QToolButton *button = dynamic_cast<QToolButton *> (m_styleMap.value(id));
        if (button) {
            button->setIcon(QIcon(pm));
            button->setIconSize(pm.size());
        }
    } else {
        if(m_styleAction == 0) {
            m_styleAction = new ItemChooserAction(m_columns);
            m_menu->addAction(m_styleAction);
        }

        QToolButton *b = m_styleAction->addItem(pm);
        b->setToolTip(toolTip);
        m_styleMap.insert(id, b);
        connect(b, SIGNAL(released()), this, SLOT(itemSelected()));
    }
    if (!m_lastId) {
        m_lastId = id;
    }
}

void FormattingButton::addBlanks(int n)
{
    if(m_styleAction) {
        m_styleAction->addBlanks(n);
    }
}

void FormattingButton::addAction(QAction *action)
{
    m_styleAction = 0;
    m_menu->addAction(action);
}

void FormattingButton::addSeparator()
{
    m_styleAction = 0;
    m_menu->addSeparator();
}

void FormattingButton::itemSelected()
{
    if (sender() != this && m_styleMap.key(sender()) == 0) {
        // this means that the sender() is not in the m_styleMap. Have you missed something?
        return;
    }

    if (sender() == this && m_lastId == 0) {
        //menu not yet populated
        return;
    }

    if(sender() != this) {
        m_lastId = m_styleMap.key(sender());
    }
    m_menu->hide();
    emit itemTriggered(m_lastId);
}

bool FormattingButton::hasItemId(int id)
{
    return m_styleMap.contains(id);
}

void FormattingButton::menuShown()
{
    m_menuShownFirstTime = false;
}

bool FormattingButton::isFirstTimeMenuShown()
{
    return m_menuShownFirstTime;
}

#include <FormattingButton.moc>
