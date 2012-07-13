/*
    Kexi Auto Form Plugin
    Copyright (C) 2011  Adam Pigg <adam@piggz.co.uk>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "AutoWidget.h"
#include <QLabel>
#include <db/field.h>
#include <QHBoxLayout>
#include <QEvent>

#include "AutoForm.h"

AutoWidget::AutoWidget(AutoForm* parent): QWidget(parent), m_widget(0)
{
    m_fieldLabel = new QLabel(this);
    
    m_layout = new QHBoxLayout(this);
    m_layout->addWidget(m_fieldLabel, 1);
    
    setLayout(m_layout);
    m_parent = parent;
    
    setFocusPolicy(Qt::StrongFocus);
}

AutoWidget::~AutoWidget()
{

}

void AutoWidget::setLabel(const QString& label)
{
    m_fieldLabel->setText(label);
}

void AutoWidget::setWidget(QWidget* widget)
{
    if (!m_widget) {
        m_widget = widget;
        setFocusProxy(m_widget);
        m_layout->addWidget(m_widget, 1);
        m_widget->installEventFilter(this);
    }
}

void AutoWidget::gotFocus()
{
    kDebug() << m_parent->itemIndex(this);
    m_parent->setCursorPosition(m_parent->currentRow(), m_parent->itemIndex(this));
}

bool AutoWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_widget) {
        //Get the focus event for the editing widget
        if (event->type() == QEvent::FocusIn) {
            gotFocus();
            return true;
        } else {
            return false;
        }
    } else {
        // pass the event on to the parent class
        return QWidget::eventFilter(obj, event);
    }
}