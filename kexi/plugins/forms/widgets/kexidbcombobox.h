/* This file is part of the KDE project
   Copyright (C) 2006 Jaroslaw Staniek <js@iidea.pl>

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
class KEXIFORMUTILS_EXPORT KexiDBComboBox : 
	public KexiDBAutoField, public KexiComboBoxBase
{
	Q_OBJECT
//	Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource DESIGNABLE true)
//	Q_PROPERTY(QCString dataSourceMimeType READ dataSourceMimeType WRITE setDataSourceMimeType DESIGNABLE true)
	Q_PROPERTY( bool editable READ isEditable WRITE setEditable )
	//properties from KexiDBAutoField that should not be visible:
	Q_OVERRIDE(QColor paletteBackgroundColor READ paletteBackgroundColor WRITE setPaletteBackgroundColor DESIGNABLE true RESET unsetPalette)
	Q_OVERRIDE(QColor foregroundLabelColor DESIGNABLE false)
	Q_OVERRIDE(QColor backgroundLabelColor DESIGNABLE false)
	Q_OVERRIDE(bool autoCaption DESIGNABLE false)

	public:
		KexiDBComboBox(QWidget *parent, const char *name=0, bool designMode = true);
		virtual ~KexiDBComboBox();

		//! Implemented for KexiComboBoxBase: form has no 'related data' model (only the full database model)
		virtual KexiTableViewColumn *column() const { return 0; }

		//! Implemented for KexiComboBoxBase
		virtual KexiDB::Field *field() const { return KexiDBAutoField::field(); }

		//! Implemented for KexiComboBoxBase
		virtual QVariant origValue() const { return m_origValue; }

		void setEditable(bool set);
		bool isEditable() const;

//		void showPopup();

		virtual void setLabelPosition(LabelPosition position);

		virtual QVariant value() { return KexiComboBoxBase::value(); }

		virtual QSize sizeHint() const;

/*
		inline QString dataSource() const { return KexiFormDataItemInterface::dataSource(); }
		inline QCString dataSourceMimeType() const { return KexiFormDataItemInterface::dataSourceMimeType(); }
		virtual QVariant value();
		virtual void setInvalidState( const QString& displayText );

		//! \return true if editor's value is null (not empty)
		//! Used for checking if a given constraint within table of form is met.
		virtual bool valueIsNull();

		//! \return true if editor's value is empty (not necessary null).
		//! Only few data types can accept "EMPTY" property
		//! (use KexiDB::Field::hasEmptyProperty() to check this).
		//! Used for checking if a given constraint within table or form is met.
		virtual bool valueIsEmpty();

		//! \return 'readOnly' flag for this widget.
		virtual bool isReadOnly() const;

		//! \return the view widget of this item, e.g. line edit widget.
		virtual QWidget* widget();

		virtual bool cursorAtStart();
		virtual bool cursorAtEnd();
		virtual void clear();

		virtual void setColumnInfo(KexiDB::QueryColumnInfo* cinfo);
*/
		/*! If \a displayDefaultValue is true, the value set by KexiDataItemInterface::setValue() 
		 is displayed in a special way. Used by KexiFormDataProvider::fillDataItems(). 
		 \a widget is equal to 'this'.
		 Reimplemented after KexiFormDataItemInterface. */
//		virtual void setDisplayDefaultValue(QWidget* widget, bool displayDefaultValue);

		const QColor & paletteBackgroundColor() const { return KexiDBAutoField::paletteBackgroundColor(); }

		//! Reimplemented to also set 'this' widget's background color, not only subwidget's.
		virtual void setPaletteBackgroundColor( const QColor & color );

	public slots:
		void slotRowAccepted(KexiTableItem *item, int row) { KexiComboBoxBase::slotRowAccepted(item, row); }
		void slotItemSelected(KexiTableItem* item) { KexiComboBoxBase::slotItemSelected(item); }
//		inline void setDataSource(const QString &ds) { KexiFormDataItemInterface::setDataSource(ds); }
//		inline void setDataSourceMimeType(const QCString &ds) { KexiFormDataItemInterface::setDataSourceMimeType(ds); }
//		virtual void setReadOnly( bool readOnly );
//		virtual void setText( const QString & text, const QString & context );

		//! Reimplemented, so "undo" means the same as "cancelEditor" action
//! @todo enable "real" undo internally so user can use ctrl+z while editing
//		virtual void undo();

	protected slots:
//		void slotTextChanged();

	protected:
		QRect buttonGeometry() const;

		virtual void paintEvent( QPaintEvent * );

		virtual void mousePressEvent( QMouseEvent *e );

		void mouseDoubleClickEvent( QMouseEvent *e );

		virtual bool eventFilter( QObject *o, QEvent *e );

		//! \return internal editor's geometry
		QRect editorGeometry() const;

		//! Creates editor. Reimplemented, because if the combo box is not editable, 
		//! editor should not be created.
		virtual void createEditor();

		/*! Reimplemented */
		virtual void styleChange( QStyle& oldStyle );

		/*! Reimplemented */
		virtual void fontChange( const QFont & oldFont );

		virtual bool subwidgetStretchRequired(KexiDBAutoField* autoField) const;

		//! Implemented for KexiComboBoxBase
		virtual QWidget *internalEditor() const { return /*WidgetWithSubpropertiesInterface*/m_subwidget; }

		//! Implemented for KexiComboBoxBase
		virtual void moveCursorToEndInInternalEditor();

		//! Implemented for KexiComboBoxBase
		virtual void selectAllInInternalEditor();

		//! Implemented for KexiComboBoxBase
		virtual void setValueInInternalEditor(const QVariant& value);

		//! Implemented for KexiComboBoxBase
		virtual QVariant valueFromInternalEditor() const;

		//! Implemented for KexiComboBoxBase
		virtual void editRequested() {};

		//! Implemented for KexiComboBoxBase
		virtual void acceptRequested() {};

		//! Implement this to return a position \a pos mapped from parent (e.g. viewport) 
		//! to global coordinates. QPoint(-1, -1) should be returned if this cannot be computed.
		virtual QPoint mapFromParentToGlobal(const QPoint& pos) const;

		//! Implement this to return a hint for popup width.
		virtual int popupWidthHint() const;

/*		virtual void paintEvent ( QPaintEvent * );
		virtual void setValueInternal(const QVariant& add, bool removeOld);
		QPopupMenu * createPopupMenu(const QPoint & pos);

		//! Used for extending context menu
		KexiDBWidgetContextMenuExtender m_menuExtender;

		//! Used to disable slotTextChanged()
		bool m_slotTextChanged_enabled : 1;*/
		class Private;
		Private * const d;
};

#endif
