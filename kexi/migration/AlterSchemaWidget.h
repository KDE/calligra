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

#ifndef ALTERSCHEMAWIDGET_H
#define ALTERSCHEMAWIDGET_H

#include <QWidget>
#include <QTableView>
#include <AlterSchemaTableModel.h>

class QGridLayout;
class QTableView;
class QComboBox;
class QCheckBox;
class QLabel;

namespace KexiDB {
class TableSchema;
}

namespace KexiMigration {
    
class AlterSchemaWidget : public QWidget {
Q_OBJECT
    public:
        AlterSchemaWidget ( QWidget* parent = 0 );
        ~AlterSchemaWidget ( );

        void setTableSchema(KexiDB::TableSchema *schema);
        void setData(QList< QList<QVariant> >);

    private:
        
        QGridLayout *m_layout;
        QTableView *m_table;
        QComboBox *m_columnType;
        QCheckBox *m_columnPKey;

        AlterSchemaTableModel *m_model;

        QLabel *m_columnNumLabel;
        QLabel *m_columnTypeLabel;
        QLabel *m_columnPKeyLabel;

        KexiDB::TableSchema *m_originalSchema;
        KexiDB::TableSchema *m_newSchema;

        int m_selectedColumn;

    private slots:
        void tableClicked(const QModelIndex& idx);
};
}
#endif // ALTERSCHEMAWIDGET_H
