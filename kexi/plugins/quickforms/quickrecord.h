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

#ifndef QUICKRECORD_H
#define QUICKRECORD_H

#include <QObject>
#include <db/RecordData.h>

namespace KexiDB {
class Cursor;
class QuerySchema;
}

class QuickRecordSet;

class QuickRecord : public QObject
{
    Q_OBJECT

public:
    QuickRecord(QuickRecordSet *set);
    ~QuickRecord();
    
    Q_INVOKABLE int fieldCount() const;
    Q_INVOKABLE QString fieldName(int) const;
    Q_INVOKABLE QVariant value(int);
    Q_INVOKABLE QVariant value(const QString&);
    

private:
  QuickRecordSet *m_recordSet;
  KexiDB::RecordData *m_data;
  
  int fieldNumber(const QString& name) const;

private slots:
    void recordChanged(qint64 r);
    
};

#endif // QUICKRECORD_H
