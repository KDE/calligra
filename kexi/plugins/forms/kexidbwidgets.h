/* This file is part of the KDE project
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2005 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIDBWIDGETS_H
#define KEXIDBWIDGETS_H

#include "kexiformdataiteminterface.h"
#include <widget/utils/kexidisplayutils.h>

#include <qtimer.h>
#include <qcheckbox.h>
#include <qdatetimeedit.h>

#include <klineedit.h>
#include <ktextedit.h>
#include <kpushbutton.h>
#include <knuminput.h>
#include <kactioncollection.h>

#include <core/kexiblobbuffer.h>

class QDateTimeEditor;
class QToolButton;
class KDatePicker;
class KPopupMenu;
class KAction;

//! Interface for a few text editor's features
class KEXIFORMUTILS_EXPORT KexiDBTextWidgetInterface
{
	public:
		KexiDBTextWidgetInterface()
		 : m_autonumberDisplayParameters(0)
		{
		}
		~KexiDBTextWidgetInterface() {
			delete m_autonumberDisplayParameters;
		}
		void setColumnInfo(KexiDB::QueryColumnInfo* cinfo, QWidget *w);
		void paintEvent( QFrame *w, bool textIsEmpty, int alignment, bool hasFocus );
		void event( QEvent * e, QWidget *w, bool textIsEmpty );

		//! parameters for displaying autonumber sign
		KexiDisplayUtils::DisplayParameters *m_autonumberDisplayParameters;
};

//! Line edit widget for Kexi forms
class KEXIFORMUTILS_EXPORT KexiDBLineEdit :
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

		/*! \return 'readOnly' flag for this widget. */
		virtual bool isReadOnly() const;

		/*! \return the view widget of this item, e.g. line edit widget. */
		virtual QWidget* widget();

		virtual bool cursorAtStart();
		virtual bool cursorAtEnd();
		virtual void clear();

		virtual void setColumnInfo(KexiDB::QueryColumnInfo* cinfo);

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
class KEXIFORMUTILS_EXPORT KexiDBTextEdit :
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

		/*! \return 'readOnly' flag for this widget. */
		virtual bool isReadOnly() const;

		/*! \return the view widget of this item, e.g. line edit widget. */
		virtual QWidget* widget();

		virtual bool cursorAtStart();
		virtual bool cursorAtEnd();
		virtual void clear();

		virtual void setColumnInfo(KexiDB::QueryColumnInfo* cinfo);

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
class KEXIFORMUTILS_EXPORT KexiDBCheckBox : public QCheckBox, public KexiFormDataItemInterface
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

		/*! \return 'readOnly' flag for this widget.  */
		virtual bool isReadOnly() const;

		/*! \return the view widget of this item, e.g. line edit widget. */
		virtual QWidget* widget();

		virtual bool cursorAtStart();
		virtual bool cursorAtEnd();
		virtual void clear();

		virtual void setEnabled(bool enabled);

	public slots:
		inline void setDataSource(const QString &ds) { KexiFormDataItemInterface::setDataSource(ds); }
		inline void setDataSourceMimeType(const QCString &ds) { KexiFormDataItemInterface::setDataSourceMimeType(ds); }
		void slotStateChanged(int state);

	protected:
		virtual void setValueInternal(const QVariant& add, bool removeOld);

	private:
		bool m_invalidState : 1;
};

//! A db-aware time editor
class KEXIFORMUTILS_EXPORT KexiDBTimeEdit : public QTimeEdit, public KexiFormDataItemInterface
{
	Q_OBJECT
	Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource DESIGNABLE true)
	Q_PROPERTY(QCString dataSourceMimeType READ dataSourceMimeType WRITE setDataSourceMimeType DESIGNABLE true)
	Q_PROPERTY( bool readOnly READ isReadOnly WRITE setReadOnly DESIGNABLE true )

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

		/*! \return 'readOnly' flag for this widget. */
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
		virtual void setReadOnly(bool set);

	protected slots:
		void  slotValueChanged(const QTime&);

	protected:
		virtual void setValueInternal(const QVariant& add, bool removeOld);

	private:
		QDateTimeEditor* m_dte_time;
		bool m_invalidState : 1;
		bool m_cleared : 1;
		bool m_readOnly : 1;
};

//! A db-aware date editor
class KEXIFORMUTILS_EXPORT KexiDBDateEdit : public QWidget, public KexiFormDataItemInterface
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
	Q_PROPERTY( bool readOnly READ isReadOnly WRITE setReadOnly DESIGNABLE true )

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

		/*! \return 'readOnly' flag for this widget. */
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
		QDateEdit *m_edit;
		KPopupMenu *m_datePickerPopupMenu;
		QDateTimeEditor *m_dte_date;
		bool m_invalidState : 1;
		bool m_cleared : 1;
		bool m_readOnly : 1;
};

//! A db-aware datetime editor
class KEXIFORMUTILS_EXPORT KexiDBDateTimeEdit : public QWidget, public KexiFormDataItemInterface
{
	Q_OBJECT
	Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource DESIGNABLE true)
	Q_PROPERTY(QCString dataSourceMimeType READ dataSourceMimeType WRITE setDataSourceMimeType DESIGNABLE true)
	// properties copied from QDateTimeEdit
	Q_PROPERTY( QDateTime dateTime READ dateTime WRITE setDateTime )
	Q_PROPERTY( bool readOnly READ isReadOnly WRITE setReadOnly DESIGNABLE true )

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
		inline void setDataSource(const QString &ds) { KexiFormDataItemInterface::setDataSource(ds); }
		inline void setDataSourceMimeType(const QCString &ds) { KexiFormDataItemInterface::setDataSourceMimeType(ds); }
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
		QDateEdit* m_dateEdit;
		QTimeEdit* m_timeEdit;
		QDateTimeEditor *m_dte_date, *m_dte_time;
		KPopupMenu *m_datePickerPopupMenu;
		bool m_invalidState : 1;
		bool m_cleared : 1;
		bool m_readOnly : 1;
};

//! A db-aware int spin box
class KEXIFORMUTILS_EXPORT KexiDBIntSpinBox : public KIntSpinBox, public KexiFormDataItemInterface
{
	Q_OBJECT
	Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource DESIGNABLE true)
	Q_PROPERTY(QCString dataSourceMimeType READ dataSourceMimeType WRITE setDataSourceMimeType DESIGNABLE true)
	Q_PROPERTY( bool readOnly READ isReadOnly WRITE setReadOnly DESIGNABLE true )

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

		/*! \return 'readOnly' flag for this widget. */
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
		void slotValueChanged();
		virtual void setReadOnly(bool set);

	protected:
		virtual void setValueInternal(const QVariant& add, bool removeOld);

	private:
		bool m_invalidState : 1;
};

//! A db-aware int spin box
class KEXIFORMUTILS_EXPORT KexiDBDoubleSpinBox : public KDoubleSpinBox, public KexiFormDataItemInterface
{
	Q_OBJECT
	Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource DESIGNABLE true)
	Q_PROPERTY(QCString dataSourceMimeType READ dataSourceMimeType WRITE setDataSourceMimeType DESIGNABLE true)
	Q_PROPERTY( bool readOnly READ isReadOnly WRITE setReadOnly DESIGNABLE true )

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

		/*! \return 'readOnly' flag for this widget.  */
		virtual bool isReadOnly() const;

		/*! \return the view widget of this item, e.g. line edit widget. */
		virtual QWidget* widget();

		virtual bool cursorAtStart();
		virtual bool cursorAtEnd();
		virtual void clear();

	public slots:
		virtual void setEnabled(bool enabled);
		inline void setDataSource(const QString &ds) { KexiFormDataItemInterface::setDataSource(ds); }
		inline void setDataSourceMimeType(const QCString &ds) { KexiFormDataItemInterface::setDataSourceMimeType(ds); }
		void slotValueChanged();
		virtual void setReadOnly(bool set);

	protected:
		virtual void setValueInternal(const QVariant& add, bool removeOld);

	private:
		bool m_invalidState : 1;
};

//! Push Button widget for Kexi forms
class KEXIFORMUTILS_EXPORT KexiPushButton : public KPushButton
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


class KEXIFORMUTILS_EXPORT PixmapData
{
	public:
		PixmapData();
		~PixmapData();

		QPixmap pixmap() const;
		void setPixmap(const QPixmap& pixmap, const QString& url = QString::null);
		QByteArray data() const;
		void setData(const QByteArray& data, const QString& url = QString::null);
		bool isEmpty() const;
		QString originalFileName() const { return m_originalFileName; }
		void setOriginalFileName(const QString& url);
		void clear();

	private:
		QByteArray m_data;
		QPixmap m_pixmap;
		QString m_originalFileName;
};

//! A data-aware, editable image box.
/*! Can also act as a normal static image box.
*/
class KEXIFORMUTILS_EXPORT KexiImageBox : public QWidget, public KexiFormDataItemInterface
{
	Q_OBJECT
	Q_PROPERTY( QString dataSource READ dataSource WRITE setDataSource )
	Q_PROPERTY( QCString dataSourceMimeType READ dataSourceMimeType WRITE setDataSourceMimeType )
	Q_PROPERTY( bool readOnly READ isReadOnly WRITE setReadOnly )
//	Q_PROPERTY( QPixmap pixmap READ pixmap WRITE setPixmap )
//	Q_PROPERTY( QByteArray pixmapData READ pixmapData WRITE setPixmapData )
	Q_PROPERTY( uint pixmapId READ pixmapId WRITE setPixmapId DESIGNABLE true STORED false )
	Q_PROPERTY( uint storedPixmapId READ storedPixmapId WRITE setStoredPixmapId DESIGNABLE false STORED true )
	Q_PROPERTY( bool scaledContents READ hasScaledContents WRITE setScaledContents )
	Q_PROPERTY( bool keepAspectRatio READ keepAspectRatio WRITE setKeepAspectRatio )
	Q_PROPERTY( Alignment alignment READ alignment WRITE setAlignment )
//	Q_PROPERTY( QString originalFileName READ originalFileName WRITE setOriginalFileName DESIGNABLE false )

	public:
		KexiImageBox( bool designMode, QWidget *parent, const char *name = 0 );
		virtual ~KexiImageBox();

		inline QString dataSource() const { return KexiFormDataItemInterface::dataSource(); }
		inline QCString dataSourceMimeType() const { return KexiFormDataItemInterface::dataSourceMimeType(); }

		virtual QVariant value(); // { return m_value.data(); }

//		QByteArray pixmapData() const { return m_value.data(); }

		QPixmap pixmap() const;

		uint pixmapId() const;

		uint storedPixmapId() const;
// 
		virtual void setInvalidState( const QString& displayText );

		virtual bool valueIsNull();

		virtual bool valueIsEmpty();

		virtual QWidget* widget();

		//! always true
		virtual bool cursorAtStart();

		//! always true
		virtual bool cursorAtEnd();

//		virtual void clear();

//		//! used to catch setIndent(), etc.
//		virtual bool setProperty ( const char * name, const QVariant & value );

		virtual bool isReadOnly() const;

		bool hasScaledContents() const;

//		bool designMode() const { return m_designMode; }

		int alignment() const { return m_alignment; }

		bool keepAspectRatio() const { return m_keepAspectRatio; }

		virtual QSize sizeHint() const;

		KActionCollection* actionCollection() { return &m_actionCollection; }

		/*! \return original file name of image loaded from a file. 
		 This can be later reused for displaying the image within a collection (to be implemented)
		 or on saving the image data back to file. */
//todo		QString originalFileName() const { return m_value.originalFileName(); }

	public slots:
		void setPixmapId(uint id);

		void setStoredPixmapId(uint id);

		//! Sets the datasource to \a ds
		virtual void setDataSource( const QString &ds );

		inline void setDataSourceMimeType(const QCString &ds) { KexiFormDataItemInterface::setDataSourceMimeType(ds); }

		virtual void setReadOnly(bool set);

		//! Sets \a pixmapData data for this widget. If the widget has data source set, 
		//! the pixmap will be also placed inside of the buffer and saved later.
//todo		void setPixmapData(const QByteArray& pixmapData) { m_value.setData(pixmapData); }

		/*! Sets original file name of image loaded from a file. 
		 @see originalFileName() */
//todo		void setOriginalFileName(const QString& name) { m_value.setOriginalFileName(name); }

		void setScaledContents(bool set);

		void insertFromFile();

		void setAlignment(int alignment);

		void setKeepAspectRatio(bool set);

		void updateActionsAvailability();

		void saveAs();

		void cut();

		void copy();

		void paste();

		virtual void clear();

		void showProperties();

	signals:
		//! Emitted when value has been changed. Actual value can be obtained using value().
		virtual void valueChanged(const QByteArray& data);
		void idChanged(long id);

	protected slots:
		void slotAboutToHidePopupMenu();
		void slotChooserPressed();

	protected:
		//! \return data depending on the current mode (db-aware or static)
		QByteArray data() const;
	
		virtual void contextMenuEvent ( QContextMenuEvent * e );
//		virtual void mousePressEvent( QMouseEvent *e );
		//		virtual void setColumnInfo(KexiDB::QueryColumnInfo* cinfo);
		virtual void paintEvent( QPaintEvent* );
//		virtual void resizeEvent( QResizeEvent* e );

		//! Sets value \a value for a widget.
		virtual void setValueInternal( const QVariant& add, bool /*removeOld*/ );

		//! Updates i18n'd action strings after datasource change
		void updateActionStrings();
		void updatePixmap();

		//! @internal
		void setData(const KexiBLOBBuffer::Handle& handle);

//		virtual void drawContents ( QPainter *p );

//		virtual void fontChange( const QFont& font );
//		virtual void styleChange( QStyle& style );
//		virtual void enabledChange( bool enabled );

//		virtual void paletteChange( const QPalette& pal );
//		virtual void frameChanged();
//		virtual void showEvent( QShowEvent* e );

//		void updatePixmapLater();
//		class ImageLabel;
//		ImageLabel *m_pixmapLabel;
		QPixmap m_pixmap;
		QByteArray m_value; //!< for db-aware mode
//		PixmapData m_value;
		KexiBLOBBuffer::Handle m_data;
//		QString m_originalFileName;
		class Button;
		Button *m_chooser;
		KPopupMenu *m_popup;
		KActionCollection m_actionCollection;
		KAction *m_insertFromFileAction, *m_saveAsAction, *m_cutAction, *m_copyAction, *m_pasteAction,
			*m_deleteAction, *m_propertiesAction;
		QTimer m_clickTimer;
		int m_titleID;
		int m_alignment;
		bool m_designMode : 1;
		bool m_readOnly : 1;
		bool m_scaledContents : 1;
		bool m_keepAspectRatio : 1;
		bool m_insideSetData : 1;
//		friend class ImageLabel;
};

#endif
