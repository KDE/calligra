/*
* Kexi Report Plugin
* Copyright (C) 2007-2009 by Adam Pigg (adam@piggz.co.uk)
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef KEXISOURCESELECTOR_H
#define KEXISOURCESELECTOR_H

#include <QWidget>
#include <QVBoxLayout>
#include <QComboBox>
#include <KLineEdit>
#include <KPushButton>

#include <kexidb/connection.h>
#include <KoReportData.h>
#include "kexidbreportdata.h"
#include "keximigratereportdata.h"

//! @todo rename to KexiReportDataSourcePage
//! @todo use KexiPropertyPaneViewBase
class KexiSourceSelector : public QWidget
{
    Q_OBJECT
public:
    KexiSourceSelector(QWidget* parent, KexiDB::Connection* conn);
    ~KexiSourceSelector();

    KoReportData* sourceData();
    void setConnectionData(QDomElement);
    QDomElement connectionData();

private:
    KexiDB::Connection *m_conn;
    QStringList queryList();
    QVBoxLayout *m_layout;
    QComboBox *m_sourceType;
    QComboBox *m_internalSource;
    KLineEdit *m_externalSource;
    KPushButton *m_setData;

    KexiDBReportData *m_kexiDBData;

#ifndef KEXI_MOBILE
    KexiMigrateReportData *m_kexiMigrateData;
#endif

private slots:
    void setDataClicked();

signals:
    void setData(KoReportData*);
};

#endif // KEXISOURCESELECTOR_H
