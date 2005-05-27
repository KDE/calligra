/* This file is part of the KDE project
   Copyright (C) 2005 Christian Nitschkowski <segfault_ii@web.de>
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXILABEL_H
#define KEXILABEL_H

#include <qimage.h>
#include <qlabel.h>

#include <kpixmap.h>

#include "kexiformdataiteminterface.h"
#include <widget/utils/kexidisplayutils.h>

class QPainter;
class QTimer;
class KexiLabel;

class KexiLabelPrivate : public QLabel {
		friend class KexiLabel;
	public:
		KexiLabelPrivate( KexiLabel* );
		virtual ~KexiLabelPrivate();

	protected:
		void updateFrame();

		QImage makeShadow( const QImage& textImage, const QColor &bgColor, const QRect& boundingRect );
		QRect getBounding( const QImage &image, const QRect& startRect );
//		double defaultDecay( QImage& source, int i, int j );
		KPixmap getShadowPixmap();

		QRect p_shadowRect;
		KexiLabel *p_parentLabel;
};

/**
An extended, data-aware, read-only text label.
It's text may have a drop-shadow.

@author Christian Nitschkowski
*/
class KexiLabel : public QLabel, public KexiFormDataItemInterface {
		Q_OBJECT
		Q_PROPERTY( QString dataSource READ dataSource WRITE setDataSource DESIGNABLE true )
		Q_PROPERTY( bool shadowEnabled READ shadowEnabled WRITE setShadowEnabled DESIGNABLE true )
		Q_OVERRIDE( QPixmap pixmap DESIGNABLE false )
		Q_OVERRIDE( bool scaledContents DESIGNABLE false )

	public:
		KexiLabel( QWidget *parent, const char *name = 0, WFlags f = 0 );
		KexiLabel( const QString& text, QWidget *parent, const char *name = 0, WFlags f = 0 );
		virtual ~KexiLabel();

		inline QString dataSource() const {
			return KexiFormDataItemInterface::dataSource();
		}

		virtual QVariant value();

		inline bool shadowEnabled() const { return p_shadowEnabled; }

		virtual void setInvalidState( const QString& displayText );

		virtual bool valueIsNull();

		virtual bool valueIsEmpty();

		//! always true
		virtual bool isReadOnly() const;

		virtual QWidget* widget();

		//! always false
		virtual bool cursorAtStart();

		//! always false
		virtual bool cursorAtEnd();

		virtual void clear();

		//! used to catch setIndent(), etc.
		virtual bool setProperty ( const char * name, const QVariant & value );

	public slots:
		/*!
		Sets the datasource to \a ds
		*/
		inline void setDataSource( const QString &ds ) {
			KexiFormDataItemInterface::setDataSource( ds );
		}

		virtual void setText( const QString& text );

		/*!
		Enable/Disable the shadow effect.
		KexiLabel acts just like a normal QLabel when shadow is disabled.
		*/
		void setShadowEnabled( bool state );

	protected slots:
		void updatePixmap();

	protected:
		virtual void setField(KexiDB::Field* field);
		virtual void paintEvent( QPaintEvent* );
		virtual void resizeEvent( QResizeEvent* e );
//		virtual bool event( QEvent* );

		/*!
		Sets value \a value for a widget.
		*/
		virtual void setValueInternal( const QVariant& add, bool removeOld );

		virtual void fontChange( const QFont& font );
		virtual void styleChange( QStyle& style );
		virtual void enabledChange( bool enabled );

		virtual void paletteChange( const QPalette& pal );
		virtual void frameChanged();
		virtual void showEvent( QShowEvent* e );

	private:
		void updatePixmapLater();

		KPixmap p_shadowPixmap;
		QPoint p_shadowPosition;
		KexiLabelPrivate* p_privateLabel;
		QTimer* p_timer;
		KexiDisplayUtils::DisplayParameters *p_autonumberDisplayParameters;
		bool p_pixmapDirty : 1;
		bool p_shadowEnabled : 1;
		bool p_resizeEvent : 1;
	};

#endif
