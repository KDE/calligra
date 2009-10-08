/* This file is part of the KDE project
   Copyright (C) 2009 Adam Pigg <adam@piggz.co.uk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "AlterSchemaWidget.h"

#include <QGridLayout>
#include <QTableView>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <kexidb/tableschema.h>
#include <klocale.h>
#include <kdebug.h>

using namespace KexiMigration;

AlterSchemaWidget::AlterSchemaWidget(QWidget *parent) : QWidget(parent)
{
    m_originalSchema = 0;
    m_newSchema = 0;

    m_layout = new QGridLayout();
    m_table = new QTableView(this);
    m_columnType = new QComboBox(this);
    m_columnPKey = new QCheckBox(this);

    m_columnNumLabel = new QLabel(i18n("Column %1").arg(1), this);
    m_columnTypeLabel = new QLabel(i18n("Type"), this);
    m_columnPKeyLabel = new QLabel(i18n("Primary Key"), this);

    m_types = KexiDB::Field::typeNames();
    m_types.removeFirst(); //Remove InvalidTypes

    for (unsigned int i = KexiDB::Field::FirstType; i <= KexiDB::Field::LastType; ++i) {
        m_columnType->addItem(KexiDB::Field::typeName(i), i);
    }

    m_layout->addWidget(m_columnNumLabel, 0, 0, 1, 2);
    m_layout->addWidget(m_columnTypeLabel, 1, 0);
    m_layout->addWidget(m_columnPKeyLabel, 1, 1);
    m_layout->addWidget(m_columnType, 2, 0);
    m_layout->addWidget(m_columnPKey, 2, 1);
    m_layout->addWidget(m_table, 3, 0, 1, 2);

    setLayout(m_layout);

    connect(m_table, SIGNAL(clicked(const QModelIndex&)), this, SLOT(tableClicked(const QModelIndex&)));
    connect(m_columnType, SIGNAL(activated(int)), this, SLOT(typeActivated(int)));
    connect(m_columnPKey, SIGNAL(clicked(bool)), this, SLOT(pkeyClicked(bool)));

    m_model = new AlterSchemaTableModel();
    m_table->setModel(m_model);
}

AlterSchemaWidget::~AlterSchemaWidget()
{

}

void AlterSchemaWidget::setTableSchema(KexiDB::TableSchema* ts)
{
    m_originalSchema = ts;
    m_newSchema = new KexiDB::TableSchema(*ts, false);

    m_model->setSchema(m_newSchema);
    tableClicked(m_model->index(0,0));
}

void AlterSchemaWidget::setData(const QList<KexiDB::RecordData>& data)
{
    m_model->setData(data);
}

void AlterSchemaWidget::tableClicked(const QModelIndex& idx)
{
    m_selectedColumn = idx.column();
    m_columnNumLabel->setText(QString("Column %1").arg(m_selectedColumn + 1));
    if (m_newSchema) {
        kDebug() << m_newSchema->field(m_selectedColumn)->typeName() << m_types.indexOf(m_newSchema->field(m_selectedColumn)->typeName());
        m_columnType->setCurrentIndex(m_types.indexOf(m_newSchema->field(m_selectedColumn)->typeName()));

        //Only set the pkey check enabled if the field type is integer
        m_columnPKey->setEnabled(KexiDB::Field::isIntegerType(KexiDB::Field::Type(m_columnType->itemData(m_types.indexOf(m_newSchema->field(m_selectedColumn)->typeName())).toInt())));
        
        m_columnPKey->setChecked(m_newSchema->field(m_selectedColumn)->isPrimaryKey());
    }
}

void AlterSchemaWidget::typeActivated(int typ)
{
    m_newSchema->field(m_selectedColumn)->setType(KexiDB::Field::Type(m_columnType->itemData(typ).toInt()));

    //Only set the pkey check enabled if the field type is integer
    m_columnPKey->setEnabled(KexiDB::Field::isIntegerType(KexiDB::Field::Type(m_columnType->itemData(typ).toInt())));

    //If the field type is not integer, then the field cannot be a pkey
    if (!KexiDB::Field::isIntegerType(KexiDB::Field::Type(m_columnType->itemData(typ).toInt()))) {
        m_newSchema->field(m_selectedColumn)->setPrimaryKey(false);
    }
}

void AlterSchemaWidget::pkeyClicked(bool pkey){
    m_newSchema->field(m_selectedColumn)->setPrimaryKey(pkey);
}

KexiDB::TableSchema* AlterSchemaWidget::newSchema()
{
    return m_newSchema;
}
