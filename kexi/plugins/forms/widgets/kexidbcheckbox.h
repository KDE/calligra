/* This file is part of the KDE project
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2006 Jarosław Staniek <staniek@kde.org>

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

#ifndef KexiDBCheckBox_H
#define KexiDBCheckBox_H

#include "kexiformdataiteminterface.h"
#include <qcheckbox.h>
//Added by qt3to4:
#include <Q3CString>

//! @short A db-aware check box
class KEXIFORMUTILS_EXPORT KexiDBCheckBox : public QCheckBox, public KexiFormDataItemInterface
{
    Q_OBJECT
    Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource DESIGNABLE true)
    Q_PROPERTY(QString dataSourceMimeType READ dataSourceMimeType WRITE setDataSourceMimeType DESIGNABLE true)
    Q_OVERRIDE(Tristate tristate READ isTristate WRITE setTristate)
    Q_ENUMS(Tristate)

public:
    KexiDBCheckBox(const QString &text, QWidget *parent);
    virtual ~KexiDBCheckBox();

    inline QString dataSource() const {
        return KexiFormDataItemInterface::dataSource();
    }
    inline QString dataSourceMimeType() const {
        return KexiFormDataItemInterface::dataSourceMimeType();
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

    /*! \return 'readOnly' flag for this widget.  */
    virtual bool isReadOnly() const;

    /*! \return the view widget of this item, e.g. line edit widget. */
    virtual QWidget* widget();

    virtual bool cursorAtStart();
    virtual bool cursorAtEnd();
    virtual void clear();

    virtual void setEnabled(bool enabled);

    enum Tristate { TristateDefault, TristateOn, TristateOff };

    void setTristate(Tristate tristate);
    Tristate isTristate() const;

    /*! Reimplemented after KexiFormDataItemInterface. */
    virtual void setDisplayDefaultValue(QWidget* widget, bool displayDefaultValue);

public slots:
    void setDataSource(const QString &ds);
    inline void setDataSourceMimeType(const QString &ds) {
        KexiFormDataItemInterface::setDataSourceMimeType(ds);
    }
    void slotStateChanged(int state);

    //! This implementation just disables read only widget
    virtual void setReadOnly(bool readOnly);

protected:
    virtual void setValueInternal(const QVariant& add, bool removeOld);

    //! \return true in isTristate() == TristateDefault and the widget has bound data source
    //! or if isTristate() == TristateOn, else false is returned.
    bool isTristateInternal() const;

    //! Updates tristate in QCheckBox itself according to m_tristate.
    void updateTristate();

private:
bool m_invalidState : 1;
bool m_tristateChanged : 1; //!< used in setTristate()
    Tristate m_tristate; //!< used in isTristate() and setTristate()
};

#endif
