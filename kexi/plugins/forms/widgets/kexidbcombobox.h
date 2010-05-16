/* This file is part of the KDE project
   Copyright (C) 2006-2007 Jarosław Staniek <staniek@kde.org>

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

#ifndef KexiDBComboBox_H
#define KexiDBComboBox_H

#include "kexidbutils.h"
#include "kexidbautofield.h"
#include <widget/tableview/kexicomboboxbase.h>

//! @short Combo box widget for Kexi forms
/*! This widget is implemented on top of KexiDBAutoField,
 so as it uses KexiDBAutoField's ability of embedding subwidgets,
 it can display not only a line edit but also text edit or image box
 (more can be added in the future).
 A drop-down button is added to mimic native combo box widget's functionality.
*/
class KEXIFORMUTILS_EXPORT KexiDBComboBox : public KexiDBAutoField,
                                            public KexiComboBoxBase
{
    Q_OBJECT
    Q_PROPERTY(bool editable READ isEditable WRITE setEditable)
    //properties from KexiDBAutoField that should not be visible:
//2.0    Q_PROPERTY(QColor paletteBackgroundColor READ paletteBackgroundColor WRITE setPaletteBackgroundColor RESET unsetPalette)

public:
    KexiDBComboBox(QWidget *parent);
    virtual ~KexiDBComboBox();

    //! Implemented for KexiComboBoxBase: form has no 'related data' model (only the full database model)
    virtual KexiTableViewColumn *column() const {
        return 0;
    }

    //! Implemented for KexiComboBoxBase
    virtual KexiDB::Field *field() const {
        return KexiDBAutoField::field();
    }

    //! Implemented for KexiComboBoxBase
    virtual QVariant origValue() const {
        return m_origValue;
    }

    void setEditable(bool set);
    bool isEditable() const;

    virtual void setLabelPosition(LabelPosition position);

    virtual QVariant value() {
        return KexiComboBoxBase::value();
    }

    virtual QVariant visibleValue();

    //! Reimpemented because to avoid taking value from the internal editor (index is taken from the popup instead)
    virtual bool valueChanged();

    virtual QSize sizeHint() const;

    //! Reimplemented after KexiDBAutoField: jsut sets \a cinfo without initializing a subwidget.
    //! Initialization is performed by \ref setVisibleColumnInfo().
    virtual void setColumnInfo(KexiDB::QueryColumnInfo* cinfo);

    /*! Used internally to set visible database column information.
     Reimplemented: performs initialization of the subwidget. */
    virtual void setVisibleColumnInfo(KexiDB::QueryColumnInfo* cinfo);

    /*! \return visible database column information for this item.
     Reimplemented. */
    virtual KexiDB::QueryColumnInfo* visibleColumnInfo() const;

    QColor paletteBackgroundColor() const {
        return KexiDBAutoField::paletteBackgroundColor();
    }

    //! Reimplemented to also set 'this' widget's background color, not only subwidget's.
    virtual void setPaletteBackgroundColor(const QColor & color);

    /*! Undoes changes made to this item - just resets the widget to original value.
     Reimplemented after KexiFormDataItemInterface to also revert the visible value
     (i.e. text) to the original state. */
    virtual void undoChanges();

public slots:
    void slotRowAccepted(KexiDB::RecordData *record, int row);
    void slotItemSelected(KexiDB::RecordData *record) {
        KexiComboBoxBase::slotItemSelected(record);
    }

protected slots:
    void slotInternalEditorValueChanged(const QVariant& v) {
        KexiComboBoxBase::slotInternalEditorValueChanged(v);
    }

protected:
    QRect buttonGeometry() const;

    virtual void paintEvent(QPaintEvent *);

    virtual void mousePressEvent(QMouseEvent *e);

    void mouseDoubleClickEvent(QMouseEvent *e);

    virtual bool eventFilter(QObject *o, QEvent *e);

    //! \return internal editor's geometry
    QRect editorGeometry() const;

    //! Creates editor. Reimplemented, because if the combo box is not editable,
    //! editor should not be created.
    virtual void createEditor();

    /*! Reimplemented */
    virtual void styleChange(QStyle& oldStyle);

    /*! Reimplemented */
    virtual void fontChange(const QFont & oldFont);

    virtual bool subwidgetStretchRequired(KexiDBAutoField* autoField) const;

    //! Implemented for KexiComboBoxBase
    virtual QWidget *internalEditor() const {
        return /*WidgetWithSubpropertiesInterface*/m_subwidget;
    }

    //! Implemented for KexiComboBoxBase. Does nothing if the widget is not editable.
    virtual void moveCursorToEndInInternalEditor();

    //! Implemented for KexiComboBoxBase. Does nothing if the widget is not editable.
    virtual void selectAllInInternalEditor();

    //! Implemented for KexiComboBoxBase
    virtual void setValueInInternalEditor(const QVariant& value);

    //! Implemented for KexiComboBoxBase
    virtual QVariant valueFromInternalEditor();

    //! Implemented for KexiComboBoxBase
    virtual void editRequested();

    //! Implemented for KexiComboBoxBase
    virtual void acceptRequested();

    //! Implement this to return a position \a pos mapped from parent (e.g. viewport)
    //! to global coordinates. QPoint(-1, -1) should be returned if this cannot be computed.
    virtual QPoint mapFromParentToGlobal(const QPoint& pos) const;

    //! Implement this to return a hint for popup width.
    virtual int popupWidthHint() const;

    virtual void setValueInternal(const QVariant& add, bool removeOld);

    //! Implemented to handle visible value instead of index
    virtual void setVisibleValueInternal(const QVariant& value);

    bool handleMousePressEvent(QMouseEvent *e);

    bool handleKeyPressEvent(QKeyEvent *ke);

    //! Implemented for KexiDataItemInterface
    virtual void beforeSignalValueChanged();

    virtual KexiComboBoxPopup *popup() const;
    virtual void setPopup(KexiComboBoxPopup *popup);

    /*! Called by top-level form on key press event.
     Used for Key_Escape to if the popup is visible,
     so the key press won't be consumed to perform "cancel editing".
     Also used for grabbing page down/up keys. */
    virtual bool keyPressed(QKeyEvent *ke);

    class Private;
    Private * const d;
};

#endif
