/* This file is part of the KDE project
   Copyright (C) 2005 Christian Nitschkowski <segfault_ii@web.de>

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

class QPainter;

class KexiLabelPrivate : public QLabel {
		friend class KexiLabel;
	public:
		KexiLabelPrivate( KexiLabel* );
		virtual ~KexiLabelPrivate() {}
	private:
		QImage makeShadow( const QImage& textPixmap, const QColor &bgColor, const QRect& boundingRect );
		QRect getBounding( const QImage &image, const QRect& startRect );
		double defaultDecay( QImage& source, int i, int j );
		KPixmap getShadowPixmap( void );
	private:
		QRect p_shadowRect;
};

/**
A simple label.
The text on it may have a drop-shadow.

@author Christian Nitschkowski
*/
class KexiLabel : public QLabel {
		Q_OBJECT
		Q_PROPERTY( bool shadowEnabled READ shadowEnabled WRITE setShadowEnabled DESIGNABLE true )
		Q_OVERRIDE( QPixmap pixmap DESIGNABLE false )
		Q_OVERRIDE( bool scaledContents DESIGNABLE false )

	public:
		KexiLabel( QWidget *parent, const char *name = 0, WFlags f = 0 );
		KexiLabel( const QString& text, QWidget *parent, const char *name = 0, WFlags f = 0 );

		virtual ~KexiLabel() {}

		bool shadowEnabled() const {
			return p_shadowEnabled;
		}

		/*!
		Enable/Disable the shadow effect.
		KexiLabel acts just like a normal QLabel when shadow is disabled.
		*/
		void setShadowEnabled( bool state ) {
			p_shadowEnabled = state;
			repaint();
		}

	protected:
		virtual void paintEvent( QPaintEvent* );
		virtual void resizeEvent( QResizeEvent* e ) {
			p_pixmapDirty = true;
			QLabel::resizeEvent( e );
		}

		virtual void fontChange( const QFont& font ) {
			p_pixmapDirty = true;
			p_privateLabel->setFont( font );
			QLabel::fontChange( font );
		}

		virtual void styleChange( QStyle& style ) {
			p_pixmapDirty = true;
			QLabel::styleChange( style );
		}

		virtual void enabledChange( const bool enabled ) {
			p_pixmapDirty = true;
			p_privateLabel->setEnabled( enabled );
			QLabel::enabledChange( enabled );
		}

		virtual void paletteChange( const QPalette& pal ) {
			p_pixmapDirty = true;
			p_privateLabel->setPalette( pal );
			QLabel::paletteChange( pal );
		}

		virtual void frameChanged() {
			p_pixmapDirty = true;
			p_privateLabel->frameChanged();
			QFrame::frameChanged();
		}

		virtual void showEvent( QShowEvent* e ) {
			p_pixmapDirty = true;
			QLabel::showEvent( e );
		}

	private:
		void updatePixmap();

	private:
		KPixmap p_shadowPixmap;
		QPoint p_shadowPosition;
		KexiLabelPrivate* p_privateLabel;
		bool p_pixmapDirty;
		bool p_shadowEnabled;

	public slots:
		virtual void setText( const QString& text ) {
			p_pixmapDirty = true;
			QLabel::setText( text );
		}
	};

#endif
