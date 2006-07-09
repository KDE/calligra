/* This file is part of the KDE project
   Original file (mailmerge.h): Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2001 Joseph Wenninger <jowenn@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef _SERIALLETTER_INTERFACE_H_
#define _SERIALLETTER_INTERFACE_H_
#include <qcstring.h>
#include <qdom.h>
#include <kinstance.h>
#include <qmap.h>
#include <qobject.h>
#include <dcopobject.h>
#include <koffice_export.h>
typedef QMap<QString,QString> DbRecord;
#define KWSLUnspecified 0
#define KWSLEdit 1
#define KWSLCreate 2
#define KWSLOpen 3
#define KWSLMergePreview 4
#define KWSLMergeDocument 5

#define KWSLCreate_text "create"
#define KWSLOpen_text "open"
/******************************************************************
 *
 * Class: KWMailMergeDataSource
 *
 ******************************************************************/

class KWORD_EXPORT KWMailMergeDataSource: public QObject, public DCOPObject
{
    Q_OBJECT
    K_DCOP
    public:
    KWMailMergeDataSource(KInstance* inst,QObject *parent);
    virtual ~KWMailMergeDataSource(){;}
    virtual class QString getValue( const class QString &name, int record = -1 ) const=0;

    const QMap< QString, QString > &getRecordEntries() const;

    KInstance *KWInstance(){return m_instance;}
    virtual  bool showConfigDialog(class QWidget*,int) =0;

    virtual void save(QDomDocument&, QDomElement&)=0;
    virtual void load( class QDomElement& elem )=0;

    virtual void refresh(bool force)=0;
    QByteArray info;
    protected:
    DbRecord sampleRecord;
    private:
    KInstance *m_instance;
k_dcop:
    virtual  int getNumRecords() const =0;
};




#endif
