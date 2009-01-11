/* This file is part of the KDE project
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2005 Jarosław Staniek <staniek@kde.org>

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

#ifndef KexiDBDateTimeEdit_H
#define KexiDBDateTimeEdit_H

#include "kexiformdataiteminterface.h"
#include <q3datetimeedit.h>

class KDatePicker;
class QDateTimeEditor;
class KMenu;

//! @short A db-aware datetime editor
class KEXIFORMUTILS_EXPORT KexiDBDateTimeEdit : public QWidget, public KexiFormDataItemInterface
{
    Q_OBJECT
    Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource DESIGNABLE true)
    Q_PROPERTY(QString dataSourcePartClass READ dataSourcePartClass WRITE setDataSourcePartClass DESIGNABLE true)
    // properties copied from QDateTimeEdit
    Q_PROPERTY(QDateTime dateTime READ dateTime WRITE setDateTime)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly DESIGNABLE true)

public:
    enum Order { DMY, MDY, YMD, YDM };

    KexiDBDateTimeEdit(const QDateTime &datetime, QWidget *parent);
    virtual ~KexiDBDateTimeEdit();

    inline QString dataSource() const {
        return KexiFormDataItemInterface::dataSource();
    }
    inline QString dataSourcePartClass() const {
        return KexiFormDataItemInterface::dataSourcePartClass();
    }
    virtual QVariant value();
    virtual void setInvalidState(const QString& displayText);

    //! \return true if editor's value is null (not empty)
    //! Used for checking if a given constraint within table of form is met.
    virtual bool valueIsNull();

    //! \return true if editor's value is empty (not necessary null).
    //! Only few data types can accept "EMPTY" property
    //! (use KexiDB::Field::hasEmptyProperty() to check this).
    //! Used for checking if a given constraint within table or form is met.
    virtual bool valueIsEmpty();

    /*! \return 'readOnly' flag for this widget. */
    virtual bool isReadOnly() const;

    /*! \return the view widget of this item, e.g. line edit widget. */
    virtual QWidget* widget();

    virtual bool cursorAtStart();
    virtual bool cursorAtEnd();
    virtual void clear();

    virtual void  setEnabled(bool enabled);

    // property functions
    QDateTime dateTime() const;

signals:
    void  dateTimeChanged();

public slots:
    inline void setDataSource(const QString &ds) {
        KexiFormDataItemInterface::setDataSource(ds);
    }
    inline void setDataSourcePartClass(const QString &partClass) {
        KexiFormDataItemInterface::setDataSourcePartClass(partClass);
    }
    void setDateTime(const QDateTime &dt);
    virtual void setReadOnly(bool set);

protected:
    virtual void setValueInternal(const QVariant& add, bool removeOld);
    virtual bool eventFilter(QObject *o, QEvent *e);

protected slots:
    void slotValueChanged();
    void slotShowDatePicker();
    void acceptDate();

private:
    KDatePicker *m_datePicker;
    Q3DateEdit* m_dateEdit;
    Q3TimeEdit* m_timeEdit;
    QDateTimeEditor *m_dte_date, *m_dte_time;
    KMenu *m_datePickerPopupMenu;
    bool m_invalidState : 1;
    bool m_cleared : 1;
    bool m_readOnly : 1;
};

#endif
