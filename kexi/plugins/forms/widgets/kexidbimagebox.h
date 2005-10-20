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

#ifndef KexiDBImageBox_H
#define KexiDBImageBox_H

#include "kexiformdataiteminterface.h"
#include <qwidget.h>
#include <kactioncollection.h>
#include <kexiblobbuffer.h>
#include <qtimer.h>

//! A data-aware, editable image box.
/*! Can also act as a normal static image box.
*/
class KEXIFORMUTILS_EXPORT KexiDBImageBox : public QWidget, public KexiFormDataItemInterface
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
		KexiDBImageBox( bool designMode, QWidget *parent, const char *name = 0 );
		virtual ~KexiDBImageBox();

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

		bool popupMenuAvailable();

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
