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

#ifndef QUICKRECORDSET_H
#define QUICKRECORDSET_H

#include <QObject>

namespace KexiDB {
  class Connection;
}

class QuickRecordSet : public QObject
{
    Q_OBJECT

public:
    QuickRecordSet(const QString &source, KexiDB::Connection *pConnection, QObject* parent = 0);
    ~QuickRecordSet();

    Q_PROPERTY(qint64 at READ at NOTIFY positionChanged)
    Q_INVOKABLE bool moveFirst();
    Q_INVOKABLE bool movePrevious();
    Q_INVOKABLE bool moveNext();
    Q_INVOKABLE bool moveLast();
    Q_INVOKABLE qint64 at() const;
    Q_INVOKABLE qint64 recordCount() const;
    
signals:
    void positionChanged(qint64);
    
private:
    class Private;
    Private * const d;
    
    bool getSchema();
    bool open();
    bool close();
};

#endif // QUICKRECORDSET_H
