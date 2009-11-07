/* This file is part of the KDE project
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2009 Jarosław Staniek <staniek@kde.org>

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

#ifndef KexiDBLineEdit_H
#define KexiDBLineEdit_H

#include <QValidator>
#include <QEvent>
#include <QPaintEvent>
#include <klineedit.h>
#include <qvalidator.h>

#include "kexiformdataiteminterface.h"
#include "kexidbtextwidgetinterface.h"
#include "kexidbutils.h"
#include <kexi_global.h>
#include <widget/tableview/kexitextformatter.h>
#include <formeditor/FormWidgetInterface.h>

class KexiDBWidgetContextMenuExtender;

//! @short Line edit widget for Kexi forms
/*! Handles many data types. User input is validated by using validators
 and/or input masks.
*/
class KEXIFORMUTILS_EXPORT KexiDBLineEdit : public KLineEdit,
                                            protected KexiDBTextWidgetInterface,
                                            public KexiFormDataItemInterface,
                                            public KexiSubwidgetInterface,
                                            public KFormDesigner::FormWidgetInterface
{
    Q_OBJECT
    Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource)
    Q_PROPERTY(QString dataSourcePartClass READ dataSourcePartClass WRITE setDataSourcePartClass)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly)

public:
    KexiDBLineEdit(QWidget *parent);
    virtual ~KexiDBLineEdit();

//    //! Sets design mode on or off. Reimplemented to alter the cursor.
//    virtual void setDesignMode(bool design);

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

    /*! \return true if the value is valid */
    virtual bool valueIsValid();

    /*! \return 'readOnly' flag for this widget. */
    virtual bool isReadOnly() const;

    /*! If \a displayDefaultValue is true, the value set by KexiDataItemInterface::setValue()
     is displayed in a special way. Used by KexiFormDataProvider::fillDataItems().
     \a widget is equal to 'this'.
     Reimplemented after KexiFormDataItemInterface. */
    virtual void setDisplayDefaultValue(QWidget* widget, bool displayDefaultValue);

    /*! \return the view widget of this item, e.g. line edit widget. */
    virtual QWidget* widget();

    virtual bool cursorAtStart();
    virtual bool cursorAtEnd();
    virtual void clear();

    virtual void setColumnInfo(KexiDB::QueryColumnInfo* cinfo);

    /*! Handles action having standard name \a actionName.
     Action could be: "edit_copy", "edit_paste", etc.
     Reimplemented after KexiDataItemChangesListener. */
    virtual void handleAction(const QString& actionName);

    /*! Called by top-level form on key press event to consume widget-specific shortcuts. */
    virtual bool keyPressed(QKeyEvent *ke);

public slots:
    void setDataSource(const QString &ds);

    void setDataSourcePartClass(const QString &partClass);

    virtual void setReadOnly(bool readOnly);

    //! Reimplemented, so "undo" means the same as "cancelEditor" action
    virtual void undo();

    //! Implemented for KexiDataItemInterface
    virtual void moveCursorToEnd();

    //! Implemented for KexiDataItemInterface
    virtual void moveCursorToStart();

    //! Implemented for KexiDataItemInterface
    virtual void selectAll();

protected slots:
    void slotTextChanged(const QString&);

    //! Used to protecte m_readWriteValidator against after validator is destroyed
    void slotReadWriteValidatorDestroyed(QObject*);

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void setValueInternal(const QVariant& add, bool removeOld);
    virtual bool event(QEvent *);

    virtual QMenu * createPopupMenu();

    //! Implemented for KexiSubwidgetInterface
    virtual bool appendStretchRequired(KexiDBAutoField* autoField) const;

    void updateTextForDataSource();

    //! Used to format text
    KexiTextFormatter m_textFormatter;

    //! Used for read only flag to disable editing
    QPointer<QValidator> m_readOnlyValidator;

    //! Used to remember the previous validator used for r/w mode, after setting
    //! the read only flag
    const QValidator* m_readWriteValidator;

    //! Used for extending context menu
    KexiDBWidgetContextMenuExtender m_menuExtender;

    //! Used in isReadOnly, as sometimes we want to have the flag set tot true when KLineEdit::isReadOnly
    //! is still false.
    bool m_internalReadOnly : 1;

    //! Used in slotTextChanged()
    bool m_slotTextChanged_enabled : 1;
};

#endif
