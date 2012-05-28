/* This file is part of the KDE project
 * Copyright (C) 2011 Smit Patel <smitpatel24@gmail.com>
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
#include "BibliographyDb.h"

#include <KComboBox>
#include <KLineEdit>
#include <KDialogButtonBox>
#include <KPushButton>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>

const QStringList EditFiltersDialog::filterConditions = QStringList() << "=" << "<>" << "<" << "<="
                                                                      << ">" << ">=" << "LIKE" << "NOT LIKE"
                                                                      << "NULL" << "NOT NULL";

BibDbFilter::BibDbFilter(QString leftOp, QString rightOp, QString comparison) :
    m_leftOp(leftOp),
    m_rightOp(rightOp),
    m_comparison(comparison),
    m_field(new KComboBox),
    m_cond(new KComboBox),
    m_value(new KLineEdit),
    m_layout(new QHBoxLayout)
{
    setLayout(m_layout);

    m_layout->addWidget(m_field);
    m_layout->addWidget(m_cond);
    m_layout->addWidget(m_value);

    m_field->addItems(BibliographyDb::dbFields);
    m_cond->addItems(EditFiltersDialog::filterConditions);

    m_field->setCurrentIndex(m_field->findText(m_leftOp, Qt::MatchFixedString));
    m_cond->setCurrentIndex(m_cond->findText(m_comparison, Qt::MatchFixedString));

    connect(m_field, SIGNAL(currentIndexChanged(QString)), this, SLOT(setLeftOperand(QString)));
    connect(m_value, SIGNAL(textEdited(QString)), this, SLOT(setRightOperand(QString)));
    connect(m_cond, SIGNAL(currentIndexChanged(QString)), this, SLOT(setCondition(QString)));
    show();
}

void BibDbFilter::setLeftOperand(QString op)
{
    m_leftOp = op;
}

void BibDbFilter::setRightOperand(QString op)
{
    m_rightOp = op;
}

void BibDbFilter::setCondition(QString cond)
{
    if (cond == "NULL" || cond == "NOT NULL") {
        m_value->clear();
        m_value->setDisabled(true);
        m_rightOp = "";
    } else m_value->setEnabled(true);

    m_comparison = cond;
}

QString BibDbFilter::filterString() const
{
    QString filter;
    if (!m_leftOp.isEmpty()) {
        filter.append(m_leftOp);
    } else filter.append("identifier");

    if (!m_comparison.isEmpty()) {
        filter.append(" ").append(m_comparison).append(" ");
    } else filter.append(" <>");

    if (!m_rightOp.isEmpty()) {
        filter.append(QString(" '%1' ").arg(m_rightOp));
    } else if (m_comparison != "NULL" && m_comparison != "NOT NULL") {
        filter.append(" '' ");
    }

    return filter;
}

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
    m_buttonBox->addButton("Cancel", KDialogButtonBox::RejectRole, this, SLOT(reject()));
    m_buttonBox->addButton("Clear filters", KDialogButtonBox::ActionRole, this, SLOT(clearFilters()));
    m_buttonLayout->addWidget(m_addFilter);
    m_buttonLayout->addWidget(m_buttonBox);

    m_layout->addLayout(m_buttonLayout);
    show();
}

void EditFiltersDialog::addFilter()
{
    BibDbFilter *filter = new BibDbFilter;
    m_filters->append(filter);
    m_filterLayout->addWidget(filter);
}

void EditFiltersDialog::applyFilters()
{
    emit accept();
}

void EditFiltersDialog::clearFilters()
{
}

