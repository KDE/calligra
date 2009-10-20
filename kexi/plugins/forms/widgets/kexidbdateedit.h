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

#ifndef KexiDBDateEdit_H
#define KexiDBDateEdit_H

#include "kexiformdataiteminterface.h"
#include <formeditor/FormWidgetInterface.h>
#include <q3datetimeedit.h>

class KMenu;
class KDatePicker;
class QDateTimeEditor;

//! @short A db-aware date editor
class KEXIFORMUTILS_EXPORT KexiDBDateEdit : public QWidget,
                                            public KexiFormDataItemInterface,
                                            public KFormDesigner::FormWidgetInterface
{
    Q_OBJECT
    Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource)
    Q_PROPERTY(QString dataSourcePartClass READ dataSourcePartClass WRITE setDataSourcePartClass)
    // properties copied from QDateEdit
    Q_ENUMS(Order)
    Q_PROPERTY(Order order READ order WRITE setOrder)
    Q_PROPERTY(QDate date READ date WRITE setDate)
    Q_PROPERTY(bool autoAdvance READ autoAdvance WRITE setAutoAdvance)
    Q_PROPERTY(QDate maxValue READ maxValue WRITE setMaxValue)
    Q_PROPERTY(QDate minValue READ minValue WRITE setMinValue)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly)

public:
    enum Order { DMY = Q3DateEdit::DMY, MDY = Q3DateEdit::MDY, YMD = Q3DateEdit::YMD,  YDM = Q3DateEdit::YDM };

    KexiDBDateEdit(const QDate &date, QWidget *parent);
    virtual ~KexiDBDateEdit();

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
    inline QDate date() const {
        return m_edit->date();
    }
    inline void setOrder(Order order) {
        m_edit->setOrder((Q3DateEdit::Order) order);
    }
    inline Order order() const {
        return (Order)m_edit->order();
    }
    inline void setAutoAdvance(bool advance) {
        m_edit->setAutoAdvance(advance);
    }
    inline bool autoAdvance() const {
        return m_edit->autoAdvance();
    }
    inline void setMinValue(const QDate& d) {
        m_edit->setMinValue(d);
    }
    inline QDate minValue() const {
        return m_edit->minValue();
    }
    inline void setMaxValue(const QDate& d) {
        m_edit->setMaxValue(d);
    }
    inline QDate maxValue() const {
        return m_edit->maxValue();
    }

signals:
    void  dateChanged(const QDate &date);

public slots:
    inline void setDataSource(const QString &ds) {
        KexiFormDataItemInterface::setDataSource(ds);
    }
    inline void setDataSourcePartClass(const QString &partClass) {
        KexiFormDataItemInterface::setDataSourcePartClass(partClass);
    }
    inline void setDate(const QDate& date)  {
        m_edit->setDate(date);
    }
    virtual void setReadOnly(bool set);

protected slots:
    void slotValueChanged(const QDate&);
    void  slotShowDatePicker();
    void  acceptDate();

protected:
    virtual void setValueInternal(const QVariant& add, bool removeOld);
    virtual bool  eventFilter(QObject *o, QEvent *e);

private:
    KDatePicker *m_datePicker;
    Q3DateEdit *m_edit;
    KMenu *m_datePickerPopupMenu;
    QDateTimeEditor *m_dte_date;
    bool m_invalidState : 1;
    bool m_cleared : 1;
    bool m_readOnly : 1;
};

#endif
