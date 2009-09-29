/*
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
m_layout = new QGridLayout();
m_table = new QTableView(this);
m_columnType = new QComboBox(this);
m_columnPKey = new QCheckBox(this);

m_columnTypeLabel = new QLabel(this, i18n("Column Type"));
m_columnPKeyLabel = new QLabel(this, i18n("Primary Key"));

m_layout->addWidget(m_columnTypeLabel, 0, 0);
m_layout->addWidget(m_columnPKeyLabel, 0, 1);
m_layout->addWidget(m_columnType, 1, 0);
m_layout->addWidget(m_columnPKey, 1, 1);
m_layout->addWidget(m_table, 2, 0, 1, 2);

setLayout(m_layout);

connect(m_table, SIGNAL(clicked(const QModelIndex&)), this, SLOT(tableClicked(const QModelIndex&)));

}

AlterSchemaWidget::~AlterSchemaWidget()
{

}

void AlterSchemaWidget::setTableSchema(KexiDB::TableSchema* ts)
{
    m_originalSchema = ts;
}

void AlterSchemaWidget::tableClicked(const QModelIndex& idx)
{
    kDebug();
    
}