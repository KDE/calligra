/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003   Daniel Molkentin <molkentin@kde.org>
   Copyright (C) 2003-2007 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.

   Original Author:  Till Busch <till@bux.at>
   Original Project: buX (www.bux.at)
*/

#include "KexiTableViewColumn.h"
#include "kexitableviewdata.h"

#include <kexiutils/validator.h>

#include <kexidb/field.h>
#include <kexidb/queryschema.h>
#include <kexidb/roweditbuffer.h>
#include <kexidb/cursor.h>
#include <kexidb/utils.h>
#include <kexi.h>

#include <kdebug.h>
#include <klocale.h>

class KexiTableViewColumn::Private
{
public:
    Private()
            : data(0) {
    }

    //! Data that this column is assigned to. Set by KexiTableViewColumn::setData()
    KexiTableViewData* data;
};

//------------------------

KexiTableViewColumn::KexiTableViewColumn(KexiDB::Field& f, bool owner)
        : m_columnInfo(0)
        , m_visibleLookupColumnInfo(0)
        , m_field(&f)
        , d(new Private)
{
    m_isDBAware = false;
    m_fieldOwned = owner;
    m_captionAliasOrName = m_field->captionOrName();
    init();
}

KexiTableViewColumn::KexiTableViewColumn(const QString& name, KexiDB::Field::Type ctype,
        uint cconst,
        uint options,
        uint length, uint precision,
        QVariant defaultValue,
        const QString& caption, const QString& description, uint width
                                        )
        : m_columnInfo(0)
        , m_visibleLookupColumnInfo(0)
        , d(new Private)
{
    m_field = new KexiDB::Field(
        name, ctype,
        cconst,
        options,
        length, precision,
        defaultValue,
        caption, description, width);

    m_isDBAware = false;
    m_fieldOwned = true;
    m_captionAliasOrName = m_field->captionOrName();
    init();
}

KexiTableViewColumn::KexiTableViewColumn(const QString& name, KexiDB::Field::Type ctype,
        const QString& caption, const QString& description)
        : m_columnInfo(0)
        , m_visibleLookupColumnInfo(0)
        , d(new Private)
{
    m_field = new KexiDB::Field(
        name, ctype,
        KexiDB::Field::NoConstraints,
        KexiDB::Field::NoOptions,
        0, 0,
        QVariant(),
        caption, description);

    m_isDBAware = false;
    m_fieldOwned = true;
    m_captionAliasOrName = m_field->captionOrName();
    init();
}

// db-aware
KexiTableViewColumn::KexiTableViewColumn(
    const KexiDB::QuerySchema &query, KexiDB::QueryColumnInfo& aColumnInfo,
    KexiDB::QueryColumnInfo* aVisibleLookupColumnInfo)
        : m_columnInfo(&aColumnInfo)
        , m_visibleLookupColumnInfo(aVisibleLookupColumnInfo)
        , m_field(aColumnInfo.field)
        , d(new Private)
{
    m_isDBAware = true;
    m_fieldOwned = false;

    //setup column's caption:
    if (!m_columnInfo->field->caption().isEmpty()) {
        m_captionAliasOrName = m_columnInfo->field->caption();
    } else {
        //reuse alias if available:
        m_captionAliasOrName = m_columnInfo->alias;
        //last hance: use field name
        if (m_captionAliasOrName.isEmpty())
            m_captionAliasOrName = m_columnInfo->field->name();
        //todo: compute other auto-name?
    }
    init();
    //setup column's readonly flag: true, if
    // - it's not from parent table's field, or
    // - if the query itself is coming from read-only connection, or
    // - if the query itself is stored (i.e. has connection) and lookup column is defined
    const bool columnFromMasterTable = query.masterTable() == m_columnInfo->field->table();
    m_readOnly = !columnFromMasterTable
                 || (query.connection() && query.connection()->isReadOnly());
//  || (query.connection() && (query.connection()->isReadOnly() || visibleLookupColumnInfo));
//! @todo 2.0: remove this when queries become editable            ^^^^^^^^^^^^^^
// kDebug() << "KexiTableViewColumn: query.masterTable()=="
//  << (query.masterTable() ? query.masterTable()->name() : "notable") << ", columnInfo->field->table()=="
//  << (columnInfo->field->table() ? columnInfo->fielm_table()->name()  : "notable");

// m_visible = query.isFieldVisible(&f);
}

KexiTableViewColumn::KexiTableViewColumn(bool)
        : m_columnInfo(0)
        , m_visibleLookupColumnInfo(0)
        , m_field(0)
        , d(new Private)
{
    m_isDBAware = false;
    init();
}

KexiTableViewColumn::~KexiTableViewColumn()
{
    if (m_fieldOwned)
        delete m_field;
    setValidator(0);
    delete m_relatedData;
    delete d;
}

void KexiTableViewColumn::init()
{
    m_relatedData = 0;
    m_readOnly = false;
    m_visible = true;
    m_validator = 0;
    m_relatedDataEditable = false;
    m_headerTextVisible = true;
}

void KexiTableViewColumn::setValidator(KexiUtils::Validator* v)
{
    if (m_validator) {//remove old one
        if (!m_validator->parent()) //destroy if has no parent
            delete m_validator;
    }
    m_validator = v;
}

void KexiTableViewColumn::setData(KexiTableViewData* data)
{
    d->data = data;
}

void KexiTableViewColumn::setRelatedData(KexiTableViewData *data)
{
    if (m_isDBAware)
        return;
    if (m_relatedData)
        delete m_relatedData;
    m_relatedData = 0;
    if (!data)
        return;
    //find a primary key
    const KexiTableViewColumn::List columns(data->columns());
    int id = -1;
    foreach(KexiTableViewColumn* col, columns) {
        id++;
        if (col->field()->isPrimaryKey()) {
            //found, remember
            m_relatedDataPKeyID = id;
            m_relatedData = data;
            return;
        }
    }
}

bool KexiTableViewColumn::isReadOnly() const
{
    return m_readOnly || (d->data && d->data->isReadOnly());
}

bool KexiTableViewColumn::acceptsFirstChar(const QChar& ch) const
{
    // the field we're looking at can be related to "visible lookup column"
    // if lookup column is present
    KexiDB::Field *visibleField = m_visibleLookupColumnInfo
                                  ? m_visibleLookupColumnInfo->field : m_field;
    if (visibleField->isNumericType()) {
        if (ch == '.' || ch == ',')
            return visibleField->isFPNumericType();
        if (ch == '-')
            return !visibleField->isUnsigned();
        if (ch == '+' || (ch >= '0' && ch <= '9'))
            return true;
        return false;
    }

    switch (visibleField->type()) {
    case KexiDB::Field::Boolean:
        return false;
    case KexiDB::Field::Date:
    case KexiDB::Field::DateTime:
    case KexiDB::Field::Time:
        return ch >= '0' && ch <= '9';
    default:;
    }
    return true;
}
