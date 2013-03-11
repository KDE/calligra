/* This file is part of the KDE project
 * Copyright (C) 2012 Smit Patel <smitpatel24@gmail.com>
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

#include "EditFiltersDialog.h"

#include <BibDbFilter.h>
#include <BibliographyDb.h>

#include <KComboBox>
#include <KLineEdit>
#include <KDialogButtonBox>
#include <KPushButton>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPair>
#include <QList>
#include <QDebug>

EditFiltersDialog::EditFiltersDialog(QList<BibDbFilter*> *filters, QWidget *parent) :
    QDialog(parent),
    m_layout(new QVBoxLayout),
    m_filterLayout(new QVBoxLayout),
    m_buttonLayout(new QHBoxLayout),
    m_buttonBox(new KDialogButtonBox(this, Qt::Horizontal)),
    m_group(new QGroupBox("Filters", this)),
    m_addFilter(new KPushButton("Add filter", this)),
    m_filters(filters)
{
    setLayout(m_layout);
    setWindowTitle("Edit filters");
    connect(m_addFilter, SIGNAL(clicked()), this, SLOT(addFilter()));

    foreach(BibDbFilter *filter, *m_filters) {
        m_filterLayout->addWidget(filter);
    }

    m_group->setLayout(m_filterLayout);
    m_layout->addWidget(m_group);

    m_buttonBox->addButton("Apply filters", KDialogButtonBox::AcceptRole, this, SLOT(applyFilters()));
    //m_buttonBox->addButton("Clear filters", KDialogButtonBox::RejectRole, this, SLOT(clearFilters()));
    m_buttonBox->addButton("Cancel", KDialogButtonBox::RejectRole, this, SLOT(reject()));

    m_buttonLayout->addWidget(m_addFilter);
    m_buttonLayout->addWidget(m_buttonBox);

    m_layout->addLayout(m_buttonLayout);
    show();
}

void EditFiltersDialog::addFilter()
{
    BibDbFilter *filter;
    if (m_filters->size() == 0) {
        filter = new BibDbFilter(false);
    } else {
        filter = new BibDbFilter(true);
    }
    m_filters->append(filter);
    m_filterLayout->addWidget(filter);
}

void EditFiltersDialog::applyFilters()
{
    if (m_filters->size() != 0) {
        emit accept();
    }
}
