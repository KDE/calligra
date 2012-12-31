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

#include <db/field.h>
#include <db/queryschema.h>
#include <db/roweditbuffer.h>
#include <db/cursor.h>
#include <db/utils.h>
#include <kexi.h>

#include <kdebug.h>
#include <klocale.h>

class KexiTableViewColumn::Private
{
public:
  Private()
      : data(0)
      , field(0)
      , columnInfo(0)
      , visibleLookupColumnInfo(0)
    {
    }

    //! Data that this column is assigned to. Set by KexiTableViewColumn::setData()
    KexiTableViewData* data;

    QString captionAliasOrName;

    QIcon icon;

    KexiUtils::Validator* validator;

    KexiTableViewData* relatedData;
    uint relatedDataPKeyID;

    KexiDB::Field* field;

    //! @see columnInfo()
    KexiDB::QueryColumnInfo* columnInfo;

    //! @see visibleLookupColumnInfo()
    KexiDB::QueryColumnInfo* visibleLookupColumnInfo;

    uint width;
    bool isDBAware; //!< true if data is stored in DB, not only in memeory
    bool readOnly;
    bool fieldOwned;
    bool visible;
    bool relatedDataEditable;
    bool headerTextVisible;
};

//------------------------

KexiTableViewColumn::KexiTableViewColumn(KexiDB::Field& f, bool owner)
        : d(new Private)
{
    d->field = &f;
    d->isDBAware = false;
    d->fieldOwned = owner;
    d->captionAliasOrName = d->field->captionOrName();
    init();
}

KexiTableViewColumn::KexiTableViewColumn(const QString& name, KexiDB::Field::Type ctype,
        uint cconst,
        uint options,
        uint length, uint precision,
        QVariant defaultValue,
        const QString& caption, const QString& description)
        : d(new Private)
{
    d->field = new KexiDB::Field(
        name, ctype,
        cconst,
        options,
        length, precision,
        defaultValue,
        caption, description);

    d->isDBAware = false;
    d->fieldOwned = true;
    d->captionAliasOrName = d->field->captionOrName();
    init();
}

KexiTableViewColumn::KexiTableViewColumn(const QString& name, KexiDB::Field::Type ctype,
        const QString& caption, const QString& description)
        : d(new Private)
{
    d->field = new KexiDB::Field(
        name, ctype,
        KexiDB::Field::NoConstraints,
        KexiDB::Field::NoOptions,
        0, 0,
        QVariant(),
        caption, description);

    d->isDBAware = false;
    d->fieldOwned = true;
    d->captionAliasOrName = d->field->captionOrName();
    init();
}

// db-aware
KexiTableViewColumn::KexiTableViewColumn(
    const KexiDB::QuerySchema &query, KexiDB::QueryColumnInfo& aColumnInfo,
    KexiDB::QueryColumnInfo* aVisibleLookupColumnInfo)
        : d(new Private)
{
    d->field = aColumnInfo.field;
    d->columnInfo = &aColumnInfo;
    d->visibleLookupColumnInfo = aVisibleLookupColumnInfo;
    d->isDBAware = true;
    d->fieldOwned = false;

    //setup column's caption:
    if (!d->columnInfo->field->caption().isEmpty()) {
        d->captionAliasOrName = d->columnInfo->field->caption();
    } else {
        //reuse alias if available:
        d->captionAliasOrName = d->columnInfo->alias;
        //last hance: use field name
        if (d->captionAliasOrName.isEmpty())
            d->captionAliasOrName = d->columnInfo->field->name();
        //todo: compute other auto-name?
    }
    init();
    //setup column's readonly flag: true, if
    // - it's not from parent table's field, or
    // - if the query itself is coming from read-only connection, or
    // - if the query itself is stored (i.e. has connection) and lookup column is defined
    const bool columnFromMasterTable = query.masterTable() == d->columnInfo->field->table();
    d->readOnly = !columnFromMasterTable
                 || (query.connection() && query.connection()->isReadOnly());
//  || (query.connection() && (query.connection()->isReadOnly() || visibleLookupColumnInfo));
//! @todo 2.0: remove this when queries become editable            ^^^^^^^^^^^^^^
// kDebug() << "KexiTableViewColumn: query.masterTable()=="
//  << (query.masterTable() ? query.masterTable()->name() : "notable") << ", columnInfo->field->table()=="
//  << (columnInfo->field->table() ? columnInfo->field->table()->name()  : "notable");

// d->visible = query.isFieldVisible(&f);
}

KexiTableViewColumn::KexiTableViewColumn(bool)
        : d(new Private)
{
    d->isDBAware = false;
    init();
}

KexiTableViewColumn::~KexiTableViewColumn()
{
    if (d->fieldOwned)
        delete d->field;
    setValidator(0);
    delete d->relatedData;
    delete d;
}

void KexiTableViewColumn::init()
{
    d->relatedData = 0;
    d->readOnly = false;
    d->visible = true;
    d->validator = 0;
    d->relatedDataEditable = false;
    d->headerTextVisible = true;
    d->width = 0;
}

void KexiTableViewColumn::setValidator(KexiUtils::Validator* v)
{
    if (d->validator) {//remove old one
        if (!d->validator->parent()) //destroy if has no parent
            delete d->validator;
    }
    d->validator = v;
}

void KexiTableViewColumn::setData(KexiTableViewData* data)
{
    d->data = data;
}

void KexiTableViewColumn::setRelatedData(KexiTableViewData *data)
{
    if (d->isDBAware)
        return;
    if (d->relatedData)
        delete d->relatedData;
    d->relatedData = 0;
    if (!data)
        return;
    //find a primary key
    const KexiTableViewColumn::List *columns = data->columns();
    int id = -1;
    foreach(KexiTableViewColumn* col, *columns) {
        id++;
        if (col->field()->isPrimaryKey()) {
            //found, remember
            d->relatedDataPKeyID = id;
            d->relatedData = data;
            return;
        }
    }
}

bool KexiTableViewColumn::isReadOnly() const
{
    return d->readOnly || (d->data && d->data->isReadOnly());
}

void KexiTableViewColumn::setReadOnly(bool ro)
{
    d->readOnly = ro;
}

bool KexiTableViewColumn::isVisible() const
{
    return d->columnInfo ? d->columnInfo->visible : d->visible;
}

void KexiTableViewColumn::setVisible(bool v)
{
    if (d->columnInfo)
        d->columnInfo->visible = v;
    d->visible = v;
}

void KexiTableViewColumn::setIcon(const QIcon& icon)
{
    d->icon = icon;
}

QIcon KexiTableViewColumn::icon() const
{
    return d->icon;
}

void KexiTableViewColumn::setHeaderTextVisible(bool visible)
{
    d->headerTextVisible = visible;
}

bool KexiTableViewColumn::isHeaderTextVisible() const
{
    return d->headerTextVisible;
}

QString KexiTableViewColumn::captionAliasOrName() const
{
    return d->captionAliasOrName;
}

KexiUtils::Validator* KexiTableViewColumn::validator() const
{
    return d->validator;
}

KexiTableViewData *KexiTableViewColumn::relatedData() const
{
    return d->relatedData;
}

KexiDB::Field* KexiTableViewColumn::field() const
{
    return d->field;
}

void KexiTableViewColumn::setRelatedDataEditable(bool set)
{
    d->relatedDataEditable = set;
}

bool KexiTableViewColumn::isRelatedDataEditable() const
{
    return d->relatedDataEditable;
}

KexiDB::QueryColumnInfo* KexiTableViewColumn::columnInfo() const
{
    return d->columnInfo;
}

KexiDB::QueryColumnInfo* KexiTableViewColumn::visibleLookupColumnInfo() const
{
    return d->visibleLookupColumnInfo;
}

//! \return true if data is stored in DB, not only in memeory.
bool KexiTableViewColumn::isDBAware() const
{
    return d->isDBAware;
}


bool KexiTableViewColumn::acceptsFirstChar(const QChar& ch) const
{
    // the field we're looking at can be related to "visible lookup column"
    // if lookup column is present
    KexiDB::Field *visibleField = d->visibleLookupColumnInfo
                                  ? d->visibleLookupColumnInfo->field : d->field;
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

void KexiTableViewColumn::setWidth(uint w)
{
    d->width = w;
}

uint KexiTableViewColumn::width() const
{
    return d->width;
}
