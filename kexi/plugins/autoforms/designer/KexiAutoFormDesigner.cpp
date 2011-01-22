/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2011  Adam Pigg <piggz1@gmail.com>

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


#include "KexiAutoFormDesigner.h"
#include <widget/kexidatasourcecombobox.h>
#include <core/KexiMainWindowIface.h>
#include <QVBoxLayout>

KexiAutoFormDesigner::KexiAutoFormDesigner(QWidget* parent): QWidget(parent)
{
    setupUi();
}

KexiAutoFormDesigner::KexiAutoFormDesigner(QDomElement doc, QWidget* parent) : QWidget(parent)
{
    setupUi();
}

KexiAutoFormDesigner::~KexiAutoFormDesigner()
{
    delete m_dataSource;
}

void KexiAutoFormDesigner::setupUi()
{
    m_layout = new QVBoxLayout(this);
    m_dataSource = new KexiDataSourceComboBox(this);
    m_dataSource->setProject(KexiMainWindowIface::global()->project(), true, false);
    
    m_layout->addWidget(m_dataSource);
    
    setLayout(m_layout);
}
