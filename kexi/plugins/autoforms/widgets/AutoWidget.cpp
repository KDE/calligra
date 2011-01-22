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
#include <kexidb/field.h>
#include <QHBoxLayout>

AutoWidget::AutoWidget(KexiDB::Field* fld, QWidget* parent): QWidget(parent)
{
    m_field = fld;
    m_fieldLabel = new QLabel(m_field->captionOrName(), this);
    
    m_layout = new QHBoxLayout(this);
    m_layout->addWidget(m_fieldLabel);
    
    setLayout(m_layout);
}

AutoWidget::~AutoWidget()
{

}

void AutoWidget::setValue(QVariant val)
{
m_OriginalValue = val;
}


