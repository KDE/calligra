/* This file is part of the KDE project
   Copyright (C) 2005-2006 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIFORMDATAITEMINTERFACE_H
#define KEXIFORMDATAITEMINTERFACE_H

#include <widget/utils/kexidisplayutils.h>
#include <kexidataiteminterface.h>
#include <qwidget.h>

namespace KexiDB
{
class Field;
}

//! An interface for declaring form widgets to be data-aware.
class KEXIFORMUTILS_EXPORT KexiFormDataItemInterface : public KexiDataItemInterface
{
public:
    KexiFormDataItemInterface();
    virtual ~KexiFormDataItemInterface();

    //! \return the name of the data source for this widget.
    //! Data source usually means here a table or query, a field name or an expression.
    inline QString dataSource() const {
        return m_dataSource;
    }

    //! Sets the name of the data source for this widget.
    //! Data source usually means here a table or query or field name name.
    inline void setDataSource(const QString &ds) {
        m_dataSource = ds;
    }

    /*! \return the mime type of the data source for this widget.
     Data source mime type means here types like "kexi/table" or "kexi/query"
     in.the data source is set to object (as within form or subform) or is empty
     if the data source is set to table field or query column. */
    inline QString dataSourceMimeType() const {
        return m_dataSourceMimeType;
    }

    /*! Sets the mime type of the data source for this widget.
     Data source usually means here a "kexi/table" or "kexi/query".
     @see dataSourceMimeType() */
    inline void setDataSourceMimeType(const QString &ds) {
        m_dataSourceMimeType = ds;
    }

    /*! If \a displayDefaultValue is true, the value set by KexiDataItemInterface::setValue()
     is displayed in a special way. Used by KexiFormDataProvider::fillDataItems().
     \a widget is equal to 'this'.
     You can reimplement this in the widget. Always call the superclass' implementation.
     setDisplayDefaultValue(.., false) is called in KexiFormScrollView::valueChanged()
     as a response on data change performed by user. */
    virtual void setDisplayDefaultValue(QWidget* widget, bool displayDefaultValue);

    /*! \return true if default value is displayed for this item. */
    virtual bool hasDisplayedDefaultValue() const {
        return m_displayDefaultValue;
    }

    /*! Convenience function: casts this item to a QWidget.
     Can return 0 if the item is not a QWidget-derived object. */
    virtual QWidget* widget() {
        return dynamic_cast<QWidget*>(this);
    }

    /*! Sets 'invalid' state, e.g. a text editor widget should display
     text \a displayText and become read only to prevent entering data,
     because updating at the database backend is not available.
     \a displayText is usually set to something i18n'd like "#NAME?".
     Note: that even widgets that usualy do not display texts (e.g. pixmaps)
     should display \a displayText too.
    */
    virtual void setInvalidState(const QString& displayText) = 0;

    /*! Changes 'read only' flag, for this widget.
     Typically this flag can be passed to a widget itself,
     e.g. KLineEdit::setReadOnly(bool). */
    virtual void setReadOnly(bool readOnly) = 0;

    //! \return database column information for this item
    virtual KexiDB::Field* field() const;

    //! \return database column information for this item
    virtual KexiDB::QueryColumnInfo* columnInfo() const {
        return m_columnInfo;
    }

    /*! Used internally to set database column information.
     Reimplement if you need to do additional actions,
     e.g. set data validator based on field type. Don't forget about
     calling superclass implementation. */
    virtual void setColumnInfo(KexiDB::QueryColumnInfo* cinfo) {
        m_columnInfo = cinfo;
    }

    /*! Used internally to set visible database column information.
     Reimplemented in KexiDBComboBox: except for combo box, this does nothing. */
    virtual void setVisibleColumnInfo(KexiDB::QueryColumnInfo* cinfo) {
        Q_UNUSED(cinfo);
    }

    /*! \return visible database column information for this item.
     Except for combo box, this is exactly the same as columnInfo(). */
    virtual KexiDB::QueryColumnInfo* visibleColumnInfo() const {
        return columnInfo();
    }

    /*! Does nothing, because within forms, widgets are always visible. */
    virtual void hideWidget() { }

    /*! Does nothing, because within forms, widgets are always visible. */
    virtual void showWidget() { }

    /*! Undoes changes made to this item - just resets the widget to original value.
     Note: This is internal method called by KexiFormScrollView::cancelEditor().
     To cancel editing of the widget's data from the widget's code,
     use KexiFormDataItemInterface::cancelEditor().
     Reimplemented in KexiDBComboBox to also revert the visible value (i.e. text) to the original state.
     */
    virtual void undoChanges();

    /* Cancels editing of the widget's data. This method just looks for
     the (grand)parent KexiFormScrollView object and calls
     KexiFormScrollView::cancelEditor(). */
    void cancelEditor();

    /*! @internal
     Called by top-level form on key press event.
     Default implementation does nothing.
     Implement this if you want to handle key presses from within the editor widget item.
     \return true if \a ke should be accepted by the widget item.
     This method is used e.g. in KexiDBImageBox for Key_Escape to if the popup is visible,
     so the key press won't be consumed to perform "cancel editing". */
    virtual bool keyPressed(QKeyEvent *ke) {
        Q_UNUSED(ke); return false;
    }

protected:
    QString m_dataSource;
    QString m_dataSourceMimeType;
    KexiDB::QueryColumnInfo* m_columnInfo;
    KexiDisplayUtils::DisplayParameters *m_displayParametersForEnteredValue; //!< used in setDisplayDefaultValue()
    KexiDisplayUtils::DisplayParameters *m_displayParametersForDefaultValue; //!< used in setDisplayDefaultValue()
bool m_displayDefaultValue : 1; //!< used by setDisplayDefaultValue()

    friend class KexiDBAutoField;
};

#endif
