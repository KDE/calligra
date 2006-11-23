/***************************************************************************
 * kexidbschema.cpp
 * This file is part of the KDE project
 * copyright (C)2004-2005 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/


#include "kexidbschema.h"
#include "kexidbfieldlist.h"

#include <qregexp.h>
#include <kdebug.h>

using namespace Kross::KexiDB;

/***************************************************************************
 *KexiDBSchema
 */

template<class T>
KexiDBSchema<T>::KexiDBSchema(QObject* parent, const QString& name, ::KexiDB::SchemaData* schema, ::KexiDB::FieldList* fieldlist, bool owner)
    : QObject(parent)
    , m_schema(schema)
    , m_fieldlist(fieldlist)
    , m_owner(owner)
{
    setObjectName(name);

/*
    this->template addFunction0<Kross::Api::Variant>("name", this, &KexiDBSchema<T>::name);
    this->template addFunction1<void, Kross::Api::Variant>("setName", this, &KexiDBSchema<T>::setName);

    this->template addFunction0<Kross::Api::Variant>("caption", this, &KexiDBSchema<T>::caption);
    this->template addFunction1<void, Kross::Api::Variant>("setCaption", this, &KexiDBSchema<T>::setCaption);

    this->template addFunction0<Kross::Api::Variant>("description", this, &KexiDBSchema<T>::description);
    this->template addFunction1<void, Kross::Api::Variant>("setDescription", this, &KexiDBSchema<T>::setDescription);

    this->template addFunction0<KexiDBFieldList>("fieldlist", this, &KexiDBSchema<T>::fieldlist);
*/
}

template<class T>
KexiDBSchema<T>::~KexiDBSchema<T>() {
}

#if 0
template<class T>
const QString KexiDBSchema<T>::name() const {
    return m_schema->name();
}

template<class T>
void KexiDBSchema<T>::setName(const QString& name) {
    m_schema->setName(name);
}

template<class T>
const QString KexiDBSchema<T>::caption() const {
    return m_schema->caption();
}

template<class T>
void KexiDBSchema<T>::setCaption(const QString& caption) {
    m_schema->setCaption(caption);
}

template<class T>
const QString KexiDBSchema<T>::description() const {
    return m_schema->description();
}

template<class T>
void KexiDBSchema<T>::setDescription(const QString& description) {
    m_schema->setDescription(description);
}

template<class T>
KexiDBFieldList* KexiDBSchema<T>::fieldlist() const {
    return new KexiDBFieldList(m_fieldlist);
}
#endif

/***************************************************************************
 * KexiDBTableSchema
 */

KexiDBTableSchema::KexiDBTableSchema(QObject* parent, ::KexiDB::TableSchema* tableschema, bool owner)
    : KexiDBSchema<KexiDBTableSchema>(parent, "KexiDBTableSchema", tableschema, tableschema, owner)
{
}

KexiDBTableSchema::~KexiDBTableSchema()
{
    if( m_owner )
        delete tableschema();
}

::KexiDB::TableSchema* KexiDBTableSchema::tableschema()
{
    return static_cast< ::KexiDB::TableSchema* >(m_schema);
}

#if 0
KexiDBQuerySchema* KexiDBTableSchema::query()
{
    return new KexiDBQuerySchema( tableschema()->query() );
}
#endif

/***************************************************************************
 * KexiDBQuerySchema
 */

KexiDBQuerySchema::KexiDBQuerySchema(QObject* parent, ::KexiDB::QuerySchema* queryschema, bool owner)
    : KexiDBSchema<KexiDBQuerySchema>(parent, "KexiDBQuerySchema", queryschema, queryschema, owner)
{
}

KexiDBQuerySchema::~KexiDBQuerySchema()
{
    if( m_owner )
        delete queryschema();
}

::KexiDB::QuerySchema* KexiDBQuerySchema::queryschema()
{
    return static_cast< ::KexiDB::QuerySchema* >(m_schema);
}

#if 0
const QString KexiDBQuerySchema::statement() const {
    return static_cast< ::KexiDB::QuerySchema* >(m_schema)->statement();
}

void KexiDBQuerySchema::setStatement(const QString& statement) {
    static_cast< ::KexiDB::QuerySchema* >(m_schema)->setStatement(statement);
}

bool KexiDBQuerySchema::setWhereExpression(const QString& whereexpression) {
    ::KexiDB::BaseExpr* oldexpr = static_cast< ::KexiDB::QuerySchema* >(m_schema)->whereExpression();

    ///@todo use ::KexiDB::Parser for such kind of parser-functionality.
    QString s = whereexpression;
    try {
        QRegExp re("[\"',]{1,1}");
        while(true) {
            s.remove(QRegExp("^[\\s,]+"));
            int pos = s.find('=');
            if(pos < 0) break;
            QString key = s.left(pos).trimmed();
            s = s.mid(pos + 1).trimmed();

            QString value;
            int sp = s.find(re);
            if(sp >= 0) {
                if(re.cap(0) == ",") {
                    value = s.left(sp).trimmed();
                    s = s.mid(sp+1).trimmed();
                }
                else {
                    int ep = s.find(re.cap(0),sp+1);
                    value = s.mid(sp+1,ep-1);
                    s = s.mid(ep + 1);
                }
            }
            else {
                value = s;
                s = QString::null;
            }

            ::KexiDB::Field* field = static_cast< ::KexiDB::QuerySchema* >(m_schema)->field(key);
            if(! field)
                throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("Invalid WHERE-expression: Field \"%1\" does not exists in tableschema \"%2\".").arg(key).arg(m_schema->name())) );

            QVariant v(value);
            if(! v.cast(field->variantType()))
                throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("Invalid WHERE-expression: The for Field \"%1\" defined value is of type \"%2\" rather then the expected type \"%3\"").arg(key).arg(v.typeName()).arg(field->variantType())) );

            static_cast< ::KexiDB::QuerySchema* >(m_schema)->addToWhereExpression(field,v);
        }
    }
    catch(Kross::Api::Exception::Ptr e) {
        kWarning() << "Exception in Kross::KexiDB::KexiDBQuerySchema::setWhereExpression: " << e->toString() << endl;
        static_cast< ::KexiDB::QuerySchema* >(m_schema)->setWhereExpression(oldexpr); // fallback
        return false;
    }
    return true;
}
#endif
