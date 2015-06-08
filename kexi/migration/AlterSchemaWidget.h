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
class KDbTableSchema;
class KexiNameWidget;

namespace KexiMigration {

class AlterSchemaWidget : public QWidget
{
    Q_OBJECT
    public:
        explicit AlterSchemaWidget(QWidget* parent = 0);
        ~AlterSchemaWidget();

        void setTableSchema(KDbTableSchema *schema, const QString &suggestedCaption = QString());
        void setData(const QList<KDbRecordData>& data);

        KDbTableSchema* newSchema();
        KDbTableSchema* takeTableSchema();

        KexiNameWidget* nameWidget();
        bool nameExists(const QString &name) const;

        AlterSchemaTableModel* model();
    private:

        QGridLayout *m_layout;
        QTableView *m_table;
        QComboBox *m_columnType;
        QCheckBox *m_columnPKey;
        KexiNameWidget *m_tableNameWidget;

        QStringList m_types;

        AlterSchemaTableModel *m_model;

        QLabel *m_columnNumLabel;
        QLabel *m_columnTypeLabel;
        QLabel *m_columnPKeyLabel;

        KDbTableSchema *m_schema;

        int m_selectedColumn;

        //! @todo Something like this could go in kexi utils/project?
        QString suggestedItemCaption(const QString& baseCaption);

    private Q_SLOTS:
        void tableClicked(const QModelIndex& idx);
        void typeActivated(int typ);
        void pkeyClicked(bool pkey);
};
}
#endif // ALTERSCHEMAWIDGET_H
