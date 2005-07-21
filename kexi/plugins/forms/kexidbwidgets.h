/* This file is part of the KDE project
  Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIDBWIDGETS_H
#define KEXIDBWIDGETS_H

#include "kexiformdataiteminterface.h"
#include <widget/utils/kexidisplayutils.h>

#include <qcheckbox.h>
#include <qdatetimeedit.h>

#include <klineedit.h>
#include <ktextedit.h>
#include <kpushbutton.h>
#include <knuminput.h>

class QDateTimeEditor;
class KDatePicker;
class KPopupMenu;

//! Interface for a few text editor's features
class KexiDBTextWidgetInterface
{
	public:
		KexiDBTextWidgetInterface()
		 : m_autonumberDisplayParameters(0)
		{
		}
		~KexiDBTextWidgetInterface() {
			delete m_autonumberDisplayParameters;
		}
		void setField(KexiDB::Field* field, QWidget *w);
		void paintEvent( QFrame *w, bool textIsEmpty, int alignment, bool hasFocus );
		void event( QEvent * e, QWidget *w, bool textIsEmpty );

		//! parameters for displaying autonumber sign
		KexiDisplayUtils::DisplayParameters *m_autonumberDisplayParameters;
};

//! Line edit widget for Kexi forms
class KexiDBLineEdit :
	public KLineEdit,
	protected KexiDBTextWidgetInterface,
	public KexiFormDataItemInterface
{
	Q_OBJECT
	Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource DESIGNABLE true)
	Q_PROPERTY(QCString dataSourceMimeType READ dataSourceMimeType WRITE setDataSourceMimeType DESIGNABLE true)

	public:
		KexiDBLineEdit(QWidget *parent, const char *name=0);
		virtual ~KexiDBLineEdit();

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

		/*! \return 'readOnly' flag for this item. The flag is usually taken from
		 the item's widget, e.g. KLineEdit::isReadOnly().
		 By default, always returns false. */
		virtual bool isReadOnly() const;

		/*! \return the view widget of this item, e.g. line edit widget. */
		virtual QWidget* widget();

		virtual bool cursorAtStart();
		virtual bool cursorAtEnd();
		virtual void clear();

		virtual void setField(KexiDB::Field* field);

	public slots:
		inline void setDataSource(const QString &ds) { KexiFormDataItemInterface::setDataSource(ds); }
		inline void setDataSourceMimeType(const QCString &ds) { KexiFormDataItemInterface::setDataSourceMimeType(ds); }

	protected slots:
		void slotTextChanged(const QString&);

	protected:
		virtual void paintEvent ( QPaintEvent * );
		virtual void setValueInternal(const QVariant& add, bool removeOld);
		virtual bool event ( QEvent * );
};

//! Multi line edit widget for Kexi forms
class KexiDBTextEdit :
	public KTextEdit,
	protected KexiDBTextWidgetInterface,
	public KexiFormDataItemInterface
{
	Q_OBJECT
	Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource DESIGNABLE true)
	Q_PROPERTY(QCString dataSourceMimeType READ dataSourceMimeType WRITE setDataSourceMimeType DESIGNABLE true)

	public:
		KexiDBTextEdit(QWidget *parent, const char *name=0);
		virtual ~KexiDBTextEdit();

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

		/*! \return 'readOnly' flag for this item. The flag is usually taken from
		 the item's widget, e.g. KLineEdit::isReadOnly().
		 By default, always returns false. */
		virtual bool isReadOnly() const;

		/*! \return the view widget of this item, e.g. line edit widget. */
		virtual QWidget* widget();

		virtual bool cursorAtStart();
		virtual bool cursorAtEnd();
		virtual void clear();

		virtual void setField(KexiDB::Field* field);

	public slots:
		inline void setDataSource(const QString &ds) { KexiFormDataItemInterface::setDataSource(ds); }
		inline void setDataSourceMimeType(const QCString &ds) { KexiFormDataItemInterface::setDataSourceMimeType(ds); }

	protected slots:
		void slotTextChanged();

	protected:
		virtual void paintEvent ( QPaintEvent * );
		virtual void setValueInternal(const QVariant& add, bool removeOld);
};

//! A db-aware check box
class KexiDBCheckBox : public QCheckBox, public KexiFormDataItemInterface
{
	Q_OBJECT
	Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource DESIGNABLE true)
	Q_PROPERTY(QCString dataSourceMimeType READ dataSourceMimeType WRITE setDataSourceMimeType DESIGNABLE true)

	public:
		KexiDBCheckBox(const QString &text, QWidget *parent, const char *name=0);
		virtual ~KexiDBCheckBox();

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

		/*! \return 'readOnly' flag for this item. The flag is usually taken from
		 the item's widget, e.g. KLineEdit::isReadOnly().
		 By default, always returns false. */
		virtual bool isReadOnly() const;

		/*! \return the view widget of this item, e.g. line edit widget. */
		virtual QWidget* widget();

		virtual bool cursorAtStart();
		virtual bool cursorAtEnd();
		virtual void clear();

		virtual void  setEnabled(bool enabled);

	public slots:
		inline void setDataSource(const QString &ds) { KexiFormDataItemInterface::setDataSource(ds); }
		inline void setDataSourceMimeType(const QCString &ds) { KexiFormDataItemInterface::setDataSourceMimeType(ds); }
		void  slotStateChanged(int state);

	protected:
		virtual void setValueInternal(const QVariant& add, bool removeOld);

	private:
		bool m_invalidState : 1;
};

//! A db-aware time editor
class KexiDBTimeEdit : public QTimeEdit, public KexiFormDataItemInterface
{
	Q_OBJECT
	Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource DESIGNABLE true)
	Q_PROPERTY(QCString dataSourceMimeType READ dataSourceMimeType WRITE setDataSourceMimeType DESIGNABLE true)

	public:
		KexiDBTimeEdit(const QTime &time, QWidget *parent, const char *name=0);
		virtual ~KexiDBTimeEdit();

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

		/*! \return 'readOnly' flag for this item. The flag is usually taken from
		 the item's widget, e.g. KLineEdit::isReadOnly().
		 By default, always returns false. */
		virtual bool isReadOnly() const;

		/*! \return the view widget of this item, e.g. line edit widget. */
		virtual QWidget* widget();

		virtual bool cursorAtStart();
		virtual bool cursorAtEnd();
		virtual void clear();

		virtual void  setEnabled(bool enabled);

	public slots:
		inline void setDataSource(const QString &ds) { KexiFormDataItemInterface::setDataSource(ds); }
		inline void setDataSourceMimeType(const QCString &ds) { KexiFormDataItemInterface::setDataSourceMimeType(ds); }

	protected slots:
		void  slotValueChanged(const QTime&);

	protected:
		virtual void setValueInternal(const QVariant& add, bool removeOld);

	private:
		QDateTimeEditor* m_dte_time;
		bool m_invalidState : 1;
		bool  m_cleared : 1;
};

//! A db-aware date editor
class KexiDBDateEdit : public QWidget, public KexiFormDataItemInterface
{
	Q_OBJECT
	Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource DESIGNABLE true)
	Q_PROPERTY(QCString dataSourceMimeType READ dataSourceMimeType WRITE setDataSourceMimeType DESIGNABLE true)
	// properties copied from QDateEdit
	Q_ENUMS( Order )
	Q_PROPERTY( Order order READ order WRITE setOrder DESIGNABLE true)
	Q_PROPERTY( QDate date READ date WRITE setDate DESIGNABLE true)
	Q_PROPERTY( bool autoAdvance READ autoAdvance WRITE setAutoAdvance DESIGNABLE true)
	Q_PROPERTY( QDate maxValue READ maxValue WRITE setMaxValue DESIGNABLE true)
	Q_PROPERTY( QDate minValue READ minValue WRITE setMinValue DESIGNABLE true)

	public:
		enum Order { DMY = QDateEdit::DMY, MDY = QDateEdit::MDY, YMD = QDateEdit::YMD,  YDM = QDateEdit::YDM };

		KexiDBDateEdit(const QDate &date, QWidget *parent, const char *name=0);
		virtual ~KexiDBDateEdit();

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

		/*! \return 'readOnly' flag for this item. The flag is usually taken from
		 the item's widget, e.g. KLineEdit::isReadOnly().
		 By default, always returns false. */
		virtual bool isReadOnly() const;

		/*! \return the view widget of this item, e.g. line edit widget. */
		virtual QWidget* widget();

		virtual bool cursorAtStart();
		virtual bool cursorAtEnd();
		virtual void clear();

		virtual void  setEnabled(bool enabled);

		// property functions
		inline QDate date() const { return m_edit->date(); }
		inline void setOrder(Order order) { m_edit->setOrder( (QDateEdit::Order) order); }
		inline Order order() const { return (Order)m_edit->order(); }
		inline void setAutoAdvance( bool advance ) { m_edit->setAutoAdvance(advance); }
		inline bool autoAdvance() const { return m_edit->autoAdvance(); }
		inline void setMinValue(const QDate& d) { m_edit->setMinValue(d); }
		inline QDate minValue() const { return m_edit->minValue(); }
		inline void setMaxValue(const QDate& d) { m_edit->setMaxValue(d); }
		inline QDate maxValue() const { return m_edit->maxValue(); }

	signals:
		void  dateChanged(const QDate &date);

	public slots:
		inline void setDataSource(const QString &ds) { KexiFormDataItemInterface::setDataSource(ds); }
		inline void setDataSourceMimeType(const QCString &ds) { KexiFormDataItemInterface::setDataSourceMimeType(ds); }
		inline void setDate(const QDate& date)  { m_edit->setDate(date); }

	protected slots:
		void slotValueChanged(const QDate&);
		void  slotShowDatePicker();
		void  acceptDate();

	protected:
		virtual void setValueInternal(const QVariant& add, bool removeOld);
		virtual bool  eventFilter(QObject *o, QEvent *e);

	private:
		KDatePicker *m_datePicker;
		QDateEdit *m_edit;
		KPopupMenu *m_datePickerPopupMenu;
		QDateTimeEditor *m_dte_date;
		bool m_invalidState : 1;
		bool  m_cleared : 1;
};

//! A db-aware datetime editor
class KexiDBDateTimeEdit : public QWidget, public KexiFormDataItemInterface
{
	Q_OBJECT
	Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource DESIGNABLE true)
	Q_PROPERTY(QCString dataSourceMimeType READ dataSourceMimeType WRITE setDataSourceMimeType DESIGNABLE true)
	// properties copied from QDateTimeEdit
	Q_PROPERTY( QDateTime dateTime READ dateTime WRITE setDateTime )

	public:
		enum Order { DMY, MDY, YMD, YDM };

		KexiDBDateTimeEdit(const QDateTime &datetime, QWidget *parent, const char *name=0);
		virtual ~KexiDBDateTimeEdit();

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

		/*! \return 'readOnly' flag for this item. The flag is usually taken from
		 the item's widget, e.g. KLineEdit::isReadOnly().
		 By default, always returns false. */
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
		inline void setDataSource(const QString &ds) { KexiFormDataItemInterface::setDataSource(ds); }
		inline void setDataSourceMimeType(const QCString &ds) { KexiFormDataItemInterface::setDataSourceMimeType(ds); }
		void setDateTime(const QDateTime &dt);

	protected:
		virtual void setValueInternal(const QVariant& add, bool removeOld);
		virtual bool eventFilter(QObject *o, QEvent *e);

	protected slots:
		void slotValueChanged();
		void  slotShowDatePicker();
		void  acceptDate();

	private:
		KDatePicker *m_datePicker;
		QDateEdit* m_dateEdit;
		QTimeEdit* m_timeEdit;
		QDateTimeEditor *m_dte_date, *m_dte_time;
		KPopupMenu *m_datePickerPopupMenu;
		bool m_invalidState : 1;
		bool  m_cleared : 1;
};

//! A db-aware int spin box
class KexiDBIntSpinBox : public KIntSpinBox, public KexiFormDataItemInterface
{
	Q_OBJECT
	Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource DESIGNABLE true)
	Q_PROPERTY(QCString dataSourceMimeType READ dataSourceMimeType WRITE setDataSourceMimeType DESIGNABLE true)

	public:
		KexiDBIntSpinBox(QWidget *parent, const char *name=0);
		virtual ~KexiDBIntSpinBox();

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

		/*! \return 'readOnly' flag for this item. The flag is usually taken from
		 the item's widget, e.g. KLineEdit::isReadOnly().
		 By default, always returns false. */
		virtual bool isReadOnly() const;

		/*! \return the view widget of this item, e.g. line edit widget. */
		virtual QWidget* widget();

		virtual bool cursorAtStart();
		virtual bool cursorAtEnd();
		virtual void clear();

		virtual void  setEnabled(bool enabled);

	public slots:
		inline void setDataSource(const QString &ds) { KexiFormDataItemInterface::setDataSource(ds); }
		inline void setDataSourceMimeType(const QCString &ds) { KexiFormDataItemInterface::setDataSourceMimeType(ds); }
		void  slotValueChanged();

	protected:
		virtual void setValueInternal(const QVariant& add, bool removeOld);

	private:
		bool m_invalidState : 1;
};

//! A db-aware int spin box
class KexiDBDoubleSpinBox : public KDoubleSpinBox, public KexiFormDataItemInterface
{
	Q_OBJECT
	Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource DESIGNABLE true)
	Q_PROPERTY(QCString dataSourceMimeType READ dataSourceMimeType WRITE setDataSourceMimeType DESIGNABLE true)

	public:
		KexiDBDoubleSpinBox(QWidget *parent, const char *name=0);
		virtual ~KexiDBDoubleSpinBox();

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

		/*! \return 'readOnly' flag for this item. The flag is usually taken from
		 the item's widget, e.g. KLineEdit::isReadOnly().
		 By default, always returns false. */
		virtual bool isReadOnly() const;

		/*! \return the view widget of this item, e.g. line edit widget. */
		virtual QWidget* widget();

		virtual bool cursorAtStart();
		virtual bool cursorAtEnd();
		virtual void clear();

		virtual void  setEnabled(bool enabled);

	public slots:
		inline void setDataSource(const QString &ds) { KexiFormDataItemInterface::setDataSource(ds); }
		inline void setDataSourceMimeType(const QCString &ds) { KexiFormDataItemInterface::setDataSourceMimeType(ds); }
		void  slotValueChanged();

	protected:
		virtual void setValueInternal(const QVariant& add, bool removeOld);

	private:
		bool m_invalidState : 1;
};

//! Push Button widget for Kexi forms
class KexiPushButton : public KPushButton
{
	Q_OBJECT
	Q_PROPERTY(QCString onClickAction READ onClickAction WRITE setOnClickAction DESIGNABLE true)

	public:
		KexiPushButton( const QString & text, QWidget * parent, const char * name = 0 );
		~KexiPushButton();

	public slots:
		QCString onClickAction() const { return m_onClickAction; }
		void setOnClickAction(const QCString& actionName) { m_onClickAction = actionName; }

	protected:
		QCString m_onClickAction;
};

#endif
