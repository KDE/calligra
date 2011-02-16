/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2011  Adam Pigg <piggz1@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "AutoLineEdit.h"
#include <KDebug>
#include <kexidb/queryschema.h>

AutoLineEdit::AutoLineEdit(QWidget* parent): AutoWidget(parent)
{
    m_lineEdit = new QLineEdit(this);
    setWidget(m_lineEdit);
}

AutoLineEdit::~AutoLineEdit()
{

}

void AutoLineEdit::setReadOnly(bool readOnly)
{

}

void AutoLineEdit::setInvalidState(const QString& displayText)
{

}

void AutoLineEdit::setValueInternal(const QVariant& add, bool removeOld)
{
    if(removeOld) {
        m_lineEdit->setText(add.toString());
    } else {
        m_lineEdit->setText(m_origValue.toString() + add.toString());
    }
}

void AutoLineEdit::clear()
{

}

bool AutoLineEdit::cursorAtEnd()
{
    return false;
}

bool AutoLineEdit::cursorAtStart()
{
    return false;
}

bool AutoLineEdit::valueIsEmpty()
{
    return false;
}

bool AutoLineEdit::valueIsNull()
{
    return false;
}

QVariant AutoLineEdit::value()
{
    kDebug();
    return m_lineEdit->text();
}

void AutoLineEdit::setColumnInfo(KexiDB::QueryColumnInfo* cinfo)
{
    KexiFormDataItemInterface::setColumnInfo(cinfo);
    setLabel(cinfo->captionOrAliasOrName());
    setObjectName("AutoLineEdit_" + cinfo->field->name());
}


