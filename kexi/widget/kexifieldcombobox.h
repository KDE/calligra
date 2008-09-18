/* This file is part of the KDE project
   Copyright (C) 2005-2006 Jarosław Staniek <staniek@kde.org>

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
*/

#ifndef KEXIFIELDCOMBOBOX_H
#define KEXIFIELDCOMBOBOX_H

#include <KComboBox>
#include <kexi_export.h>

namespace KexiDB
{
class TableOrQuerySchema;
}
class KexiProject;

/*! This widget provides a list of fields from a table or query
 within a combobox, so user can pick one of them.
*/
class KEXIEXTWIDGETS_EXPORT KexiFieldComboBox : public KComboBox
{
    Q_OBJECT

public:
    KexiFieldComboBox(QWidget *parent);
    virtual ~KexiFieldComboBox();

//  /*! Sets table or query schema \a schema.
//   The schema object will be owned by the KexiFieldComboBox object. */
//  void setSchema(KexiDB::TableOrQuerySchema* schema);

//  KexiDB::TableOrQuerySchema* schema() const { return m_schema; }

public slots:
    //! \return global project that is used to retrieve schema informationm for this combo box.
    KexiProject* project() const;

    //! Sets global project that is used to retrieve schema informationm for this combo box.
    void setProject(KexiProject *prj);

    void setTableOrQuery(const QString& name, bool table);
    QString tableOrQueryName() const;
    bool isTableAssigned() const;
    void setFieldOrExpression(const QString& string);
    void setFieldOrExpression(int index);
    QString fieldOrExpression() const;
    QString fieldOrExpressionCaption() const;

    /*! \return index of selected table or query field.
     -1 is returned if there is nothing selected or expression is selected
     of project is not assigned or table or query is not assigned. */
    int indexOfField() const;

signals:
    void selected();

protected slots:
    void slotActivated(int);
    void slotReturnPressed(const QString & text);

protected:
    virtual void focusOutEvent(QFocusEvent *e);

    class Private;
    Private * const d;
};

#endif
