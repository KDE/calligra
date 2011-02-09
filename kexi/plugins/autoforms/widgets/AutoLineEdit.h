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


#ifndef AUTOLINEEDIT_H
#define AUTOLINEEDIT_H

#include "AutoWidget.h"
#include <QLineEdit>


class AutoLineEdit : public AutoWidget
{

public:
    AutoLineEdit(QWidget *parent);
    virtual ~AutoLineEdit();
    
    virtual void setReadOnly(bool readOnly);
    virtual void setInvalidState(const QString& displayText);
    virtual void setColumnInfo(KexiDB::QueryColumnInfo* cinfo);
protected:
    virtual void setValueInternal(const QVariant& add, bool removeOld);

public:
    virtual void clear();
    virtual bool cursorAtEnd();
    virtual bool cursorAtStart();
    virtual bool valueIsEmpty();
    virtual bool valueIsNull();
    virtual QVariant value();
    
private:
    QLineEdit *m_lineEdit;
};

#endif // AUTOLINEEDIT_H
