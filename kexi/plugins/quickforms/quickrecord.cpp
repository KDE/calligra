/*
 * Copyright 2014 Adam Pigg <email>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "quickrecord.h"
#include "quickrecordset.h"
#include <kdebug.h>
#include <db/tableviewdata.h>
#include <db/cursor.h>

QuickRecord::QuickRecord(QuickRecordSet* set): QObject(), m_data(0)
{
  m_recordSet = set;
  
  connect(m_recordSet, SIGNAL(positionChanged(qint64)), this, SLOT(recordChanged(qint64)));
  recordChanged(0);
}

QuickRecord::~QuickRecord()
{
}

int QuickRecord::fieldCount() const
{
  if (m_recordSet->data()) {
        return m_recordSet->data()->columnsCount();
    }
    return 0;
   
}

QString QuickRecord::fieldName(int) const
{
  return QString();
}

QVariant QuickRecord::value(int i)
{
  kDebug() << "Getting value for field" << i;
  if (i < fieldCount() && m_data && i < m_data->size()) {  
    return m_data->at(i);
  }
  return QVariant();
}

QVariant QuickRecord::value(const QString &f)
{
  kDebug() << "Getting value for field" << f;
  return value(fieldNumber(f));
}

void QuickRecord::recordChanged(qint64 r)
{
  //Update the record data
  kDebug() << "Getting record data:" << r;
  m_data = m_recordSet->data()->at(m_recordSet->at());
  kDebug() << m_data;
}

int QuickRecord::fieldNumber ( const QString &fld ) const
{
    if (!m_recordSet->data()->cursor() || !m_recordSet->data()->cursor()->query()) {
        return -1;
    }
    const KexiDB::QueryColumnInfo::Vector fieldsExpanded(
        m_recordSet->data()->cursor()->query()->fieldsExpanded(KexiDB::QuerySchema::Unique));
    for (int i = 0; i < fieldsExpanded.size() ; ++i) {
        if (0 == QString::compare(fld, fieldsExpanded[i]->aliasOrName(), Qt::CaseInsensitive)) {
            return i;
        }
    }
    return -1;
}
#include "quickrecord.moc"
