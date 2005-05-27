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

#include <qbitmap.h>
#include <qpainter.h>
#include <qapplication.h>
#include <qtimer.h>

#include <kdebug.h>

#include <kexidb/field.h>

#include "kexilabel.h"

#define SHADOW_OFFSET_X 3
#define SHADOW_OFFSET_Y 3
#define SHADOW_FACTOR 16.0
#define SHADOW_OPACITY 50.0
#define SHADOW_AXIS_FACTOR 2.0
#define SHADOW_DIAGONAL_FACTOR 1.0
#define SHADOW_THICKNESS 1

KexiLabelPrivate::KexiLabelPrivate( KexiLabel* parent )
	: QLabel( parent )
	, p_parentLabel(parent)
{
	updateFrame();
}

void KexiLabelPrivate::updateFrame()
{
	setIndent(p_parentLabel->indent());
	setMargin(p_parentLabel->margin());
	setFont(p_parentLabel->font());

	setFrameShadow(p_parentLabel->frameShadow());
	setFrameShape(p_parentLabel->frameShape());
	setFrameStyle(p_parentLabel->frameStyle());
	setMidLineWidth(p_parentLabel->midLineWidth());
	setLineWidth(p_parentLabel->lineWidth());
}

KexiLabelPrivate::~KexiLabelPrivate()
{
}

/*!
* This method is copied from kdebase/kdesktop/kshadowengine.cpp
* Some modifactions were made.
* --
* Christian Nitschkowski
*/
QImage KexiLabelPrivate::makeShadow( const QImage& textImage, 
	const QColor &bgColor, const QRect& boundingRect )
{
	QImage result;
	QString origText( text() );

	// create a new image for for the shaddow
	const int w = textImage.width();
	const int h = textImage.height();

	// avoid calling these methods for every pixel
	const int bgRed = bgColor.red();
	const int bgGreen = bgColor.green();
	const int bgBlue = bgColor.blue();

	const int startX = boundingRect.x() + SHADOW_THICKNESS;
	const int startY = boundingRect.y() + SHADOW_THICKNESS;
	const int effectWidth = boundingRect.bottomRight().x() - SHADOW_THICKNESS;
	const int effectHeight = boundingRect.bottomRight().y() - SHADOW_THICKNESS;
//	const int period = (effectWidth - startX) / 10;

	double alphaShadow;

	/*
	 *	This is the source pixmap
	 */
	QImage img = textImage.convertDepth( 32 );

	/*
	 *	Resize the image if necessary
	 */
	if ( ( result.width() != w ) || ( result.height() != h ) ) {
		result.create( w, h, 32 );
	}

//	result.fill( 0 ); // all black
	result.fill( SHADOW_OPACITY );
	result.setAlphaBuffer( true );

	for ( int i = startX; i < effectWidth; i++ ) {
		for ( int j = startY; j < effectHeight; j++ ) {
			/*!
			* This method is copied from kdebase/kdesktop/kshadowengine.cpp
			* Some modifactions were made.
			* --
			* Christian Nitschkowski
			*/
				if ( ( i < 1 ) || ( j < 1 ) || ( i > img.width() - 2 ) || ( j > img.height() - 2 ) )
					continue;
				else
					alphaShadow = ( qGray( img.pixel( i - 1, j - 1 ) ) * SHADOW_DIAGONAL_FACTOR +
						qGray( img.pixel( i - 1, j ) ) * SHADOW_AXIS_FACTOR +
						qGray( img.pixel( i - 1, j + 1 ) ) * SHADOW_DIAGONAL_FACTOR +
						qGray( img.pixel( i , j - 1 ) ) * SHADOW_AXIS_FACTOR +
						0 +
						qGray( img.pixel( i , j + 1 ) ) * SHADOW_AXIS_FACTOR +
						qGray( img.pixel( i + 1, j - 1 ) ) * SHADOW_DIAGONAL_FACTOR +
						qGray( img.pixel( i + 1, j ) ) * SHADOW_AXIS_FACTOR +
						qGray( img.pixel( i + 1, j + 1 ) ) * SHADOW_DIAGONAL_FACTOR ) / SHADOW_FACTOR;

			// update the shadow's i,j pixel.
			if (alphaShadow > 0)
				result.setPixel( i, j, qRgba( bgRed, bgGreen , bgBlue, 
					( int ) (( alphaShadow > SHADOW_OPACITY ) ? SHADOW_OPACITY : alphaShadow)
				) );
		}
/*caused too much redraw problems		if (period && i % period) {
			qApp->processEvents();
			if (text() != origText) //text has been changed in the meantime: abort
				return QImage();
		}*/
	}
	return result;
}

KPixmap KexiLabelPrivate::getShadowPixmap() {
	/*!
	* Backup the default color used to draw text.
	*/
	const QColor textColor = colorGroup().foreground();

	/*!
	* Temporary storage for the generated shadow
	*/
	KPixmap finalPixmap, tempPixmap;
	QImage shadowImage, tempImage;
	QPainter painter;

	p_shadowRect = QRect();

	tempPixmap.resize( size() );
	tempPixmap.fill( Qt::black );
	tempPixmap.setMask( tempPixmap.createHeuristicMask( true ) );

	/*!
	* The textcolor has to be white for creating shadows!
	*/
	setPaletteForegroundColor( Qt::white );
	/*!
	Draw the label "as usual" in a pixmap
	*/
	painter.begin( &tempPixmap );
	painter.setFont( font() );
	drawContents( &painter );
	painter.end();
	setPaletteForegroundColor( textColor );

	/*!
	* Calculate the first bounding rect.
	* This will fit around the unmodified text.
	*/
	shadowImage = tempPixmap;
	tempPixmap.setMask( QBitmap() );

	/*!
	Get the first bounding rect.
	This may speed up makeShadow later.
	*/
	p_shadowRect = getBounding( shadowImage, p_shadowRect );

	/*!
	* Enlarge the bounding rect to make sure the shadow
	* will fit in.
	* The new rect has to fit in the pixmap.
	* I have to admit this isn't really nice code...
	*/
	p_shadowRect.setX( QMAX( p_shadowRect.x() - ( p_shadowRect.width() / 4 ), 0 ) );
	p_shadowRect.setY( QMAX( p_shadowRect.y() - ( p_shadowRect.height() / 4 ), 0 ) );
	p_shadowRect.setBottomRight( QPoint(
		QMIN( p_shadowRect.x() + ( p_shadowRect.width() * 3 / 2 ), shadowImage.width() ),
		QMIN( p_shadowRect.y() + ( p_shadowRect.height() * 3 / 2 ), shadowImage.height() ) ) );

	shadowImage = makeShadow( shadowImage,
		qGray( textColor.rgb() ) < 127 ? Qt::black : Qt::white,
		p_shadowRect );
	if (shadowImage.isNull())
		return KPixmap();

	/*!
	Now get the final bounding rect.
	*/
	p_shadowRect = getBounding( shadowImage, p_shadowRect );

	/*!
	Paint the labels background in a new pixmap.
	*/
	finalPixmap.resize( size() );
	painter.begin( &finalPixmap );
	painter.fillRect( 0, 0, finalPixmap.width(), finalPixmap.height(),
		palette().brush(
		isEnabled() ? QPalette::Active : QPalette::Disabled,
		QColorGroup::Background ) );
	painter.end();

	/*!
	Copy the part of the background the shadow will be on
	to another pixmap.
	*/
	tempPixmap.resize( p_shadowRect.size() );
	if (!finalPixmap.isNull()) {
		bitBlt( &tempPixmap, 0, 0, &finalPixmap,
			p_shadowRect.x() + SHADOW_OFFSET_X,
			p_shadowRect.y() + SHADOW_OFFSET_Y,
			p_shadowRect.width(),
			p_shadowRect.height() );
	}
	/*!
	Replace the big background pixmap with the
	part we could out just before.
	*/
	finalPixmap = tempPixmap;

	/*!
	Copy the "interesting" part of the shadow image
	to a new image.
	I tried to copy this to a pixmap directly,
	but it didn't work correctly.
	Maybe a Qt bug?
	*/
	tempImage = shadowImage.copy( p_shadowRect );
	tempPixmap.convertFromImage( tempImage );
	/*!
	Anyways, merge the shadow with the background.
	*/
	if (!tempPixmap.isNull()) {
		bitBlt( &finalPixmap, 0, 0, &tempPixmap );
	}

	/**
	Now move the rect.
	Don't do this before the shadow is copied from shadowImage!
	*/
	p_shadowRect.moveBy( SHADOW_OFFSET_X, SHADOW_OFFSET_Y );

	return finalPixmap;
}

QRect KexiLabelPrivate::getBounding( const QImage &image, const QRect& startRect ) {
	QPoint topLeft;
	QPoint bottomRight;

	const int startX = startRect.x();
	const int startY = startRect.y();
	/*!
	* Ugly beast to get the correct width and height
	*/
	const int width = QMIN( ( startRect.bottomRight().x() > 0
		? startRect.bottomRight().x() : QCOORD_MAX ),
		image.width() );
	const int height = QMIN( ( startRect.bottomRight().y() > 0
		? startRect.bottomRight().y() : QCOORD_MAX ),
		image.height() );

	/*!
	Assume the first pixel has the color of the
	background that has to be cut away.
	Qt uses the four corner pixels to guess the
	correct color, but in this case the topleft
	pixel should be enough.
	*/
	QRgb trans = image.pixel( 0, 0 );

	for ( int y = startY; y < height; y++ ) {
		for ( int x = startX; x < width; x++ ) {
			if ( image.pixel( x, y ) != trans ) {
				topLeft.setY( y );
				y = height;
				break;
			}
		}
	}

	for ( int x = startX; x < width; x++ ) {
		for ( int y = startY; y < height; y++ ) {
			if ( image.pixel( x, y ) != trans ) {
				topLeft.setX( x );
				x = width;
				break;
			}
		}
	}

	for ( int y = height - 1; y > topLeft.y(); y-- ) {
		for ( int x = width - 1; x > topLeft.x(); x-- ) {
			if ( image.pixel( x, y ) != trans ) {
				bottomRight.setY( y + 1 );
				y = 0;
				break;
			}
		}
	}

	for ( int x = width - 1; x > topLeft.x(); x-- ) {
		for ( int y = height - 1; y > topLeft.y(); y-- ) {
			if ( image.pixel( x, y ) != trans ) {
				bottomRight.setX( x + 1 );
				x = 0;
				break;
			}
		}
	}

	return QRect(
		topLeft.x(),
		topLeft.y(),
		bottomRight.x() - topLeft.x(),
		bottomRight.y() - topLeft.y() );
}

//=========================================================

KexiLabel::KexiLabel( QWidget *parent, const char *name, WFlags f )
		: QLabel( parent, name, f )
		, KexiFormDataItemInterface()
		, p_timer(0)
		, p_autonumberDisplayParameters(0)
		, p_pixmapDirty( true )
		, p_shadowEnabled( false )
		, p_resizeEvent( false )
{
	m_hasFocusableWidget = false;
	p_privateLabel = new KexiLabelPrivate( this );
	p_privateLabel->hide();
}

KexiLabel::KexiLabel( const QString& text, QWidget *parent, const char *name, WFlags f )
		: QLabel( parent, name, f )
		, KexiFormDataItemInterface()
		, p_timer(0)
		, p_autonumberDisplayParameters(0)
		, p_pixmapDirty( true )
		, p_shadowEnabled( false )
		, p_resizeEvent( false )
{
	m_hasFocusableWidget = false;
	p_privateLabel = new KexiLabelPrivate( this );
	p_privateLabel->hide();
	setText( text );
}

KexiLabel::~KexiLabel()
{
	delete p_autonumberDisplayParameters;
}

void KexiLabel::updatePixmapLater() {
	if (p_resizeEvent) {
		if (!p_timer) {
			p_timer = new QTimer(this, "KexiLabelTimer");
			connect(p_timer, SIGNAL(timeout()), this, SLOT(updatePixmap()));
		}
		p_timer->start(100, true);
		p_resizeEvent = false;
		return;
	}
	if (p_timer && p_timer->isActive())
		return;
	updatePixmap();
}

void KexiLabel::updatePixmap() {
	/*!
	Whatever has changed in KexiLabel,
	every parameter is set to our private-label.
	Just in case...
	*/
	p_privateLabel->setText( text() );
	p_privateLabel->setFixedSize( size() );
	p_privateLabel->setPalette( palette() );
	p_privateLabel->setAlignment( alignment() );
//	p_shadowPixmap = KPixmap(); //parallel repaints won't hurt us cause incomplete pixmap
	KPixmap shadowPixmap = p_privateLabel->getShadowPixmap();
	if (shadowPixmap.isNull())
		return;
	p_shadowPixmap = shadowPixmap;
	p_shadowPosition = p_privateLabel->p_shadowRect.topLeft();

	p_pixmapDirty = false;
	repaint();
}

void KexiLabel::paintEvent( QPaintEvent* e ) {
	if ( p_shadowEnabled ) {
		/*!
		If required, update the pixmap-cache.
		*/
		if ( p_pixmapDirty ) {
			updatePixmapLater();
		}

		/*!
		If the part that should be redrawn intersects with our shadow,
		redraw the shadow where it intersects with e->rect().
		Have to move the clipping rect around a bit because
		the shadow has to be drawn using an offset relative to
		the widgets border.
		*/
		if ( !p_pixmapDirty && e->rect().contains( p_shadowPosition ) && !p_shadowPixmap.isNull()) {
			QPainter p( this );
			QRect clipRect = QRect(
				QMAX( e->rect().x() - p_shadowPosition.x(), 0 ),
				QMAX( e->rect().y() - p_shadowPosition.y(), 0 ),
				QMIN( e->rect().width() + p_shadowPosition.x(), p_shadowPixmap.width() ),
				QMIN( e->rect().height() + p_shadowPosition.y(), p_shadowPixmap.height() ) );
			p.drawPixmap( p_privateLabel->p_shadowRect.topLeft(), p_shadowPixmap, clipRect );
		}
	}

	if (m_field && m_field->isAutoIncrement() && p_autonumberDisplayParameters 
		&& cursorAtNewRow() && text().isEmpty())
	{
		QPainter p( this );
		int m = lineWidth()+midLineWidth();
		KexiDisplayUtils::drawAutonumberSign(*p_autonumberDisplayParameters, &p, 
			2+m+margin(), m, width()-m*2 -2-2, height()-m*2 -2, alignment(), false);
	}

	QLabel::paintEvent( e );
}

void KexiLabel::setValueInternal( const QVariant& add, bool removeOld ) {
	if (removeOld) 
		setText(add.toString());
	else
		setText( m_origValue.toString() + add.toString() );
}

QVariant KexiLabel::value() {
	return text();
}

void KexiLabel::setInvalidState( const QString& displayText )
{
	setText( displayText );
}

bool KexiLabel::valueIsNull()
{
	return text().isNull();
}

bool KexiLabel::valueIsEmpty()
{
	return text().isEmpty();
}

bool KexiLabel::isReadOnly() const
{
	return true;
}

QWidget* KexiLabel::widget()
{
	return this;
}

bool KexiLabel::cursorAtStart()
{
	return false;
}

bool KexiLabel::cursorAtEnd()
{
	return false;
}

void KexiLabel::clear()
{
	setText(QString::null);
}

bool KexiLabel::setProperty( const char * name, const QVariant & value )
{
	const bool ret = QLabel::setProperty(name, value);
	if (p_shadowEnabled) {
		if (0==qstrcmp("indent", name) || 0==qstrcmp("font", name) || 0==qstrcmp("margin", name)
			|| 0==qstrcmp("frameShadow", name) || 0==qstrcmp("frameShape", name)
			|| 0==qstrcmp("frameStyle", name) || 0==qstrcmp("midLineWidth", name)
			|| 0==qstrcmp("lineWidth", name)) {
			p_privateLabel->setProperty(name, value);
			updatePixmap();
		}
	}
	return ret;
}

void KexiLabel::setField(KexiDB::Field* field)
{
	KexiFormDataItemInterface::setField(field);

	if (m_field->isAutoIncrement()) {
		if (!p_autonumberDisplayParameters)
			p_autonumberDisplayParameters = new KexiDisplayUtils::DisplayParameters();
		KexiDisplayUtils::initDisplayForAutonumberSign(*p_autonumberDisplayParameters, this);
	}
}

void KexiLabel::setShadowEnabled( bool state ) {
	p_shadowEnabled = state;
	p_pixmapDirty = true;
	if (state)
		p_privateLabel->updateFrame();
	repaint();
}

void KexiLabel::resizeEvent( QResizeEvent* e ) {
	if (isVisible())
		p_resizeEvent = true;
	p_pixmapDirty = true;
	QLabel::resizeEvent( e );
}

void KexiLabel::fontChange( const QFont& font ) {
	p_pixmapDirty = true;
	p_privateLabel->setFont( font );
	QLabel::fontChange( font );
}

void KexiLabel::styleChange( QStyle& style ) {
	p_pixmapDirty = true;
	QLabel::styleChange( style );
}

void KexiLabel::enabledChange( bool enabled ) {
	p_pixmapDirty = true;
	p_privateLabel->setEnabled( enabled );
	QLabel::enabledChange( enabled );
}

void KexiLabel::paletteChange( const QPalette& pal ) {
	p_pixmapDirty = true;
	p_privateLabel->setPalette( pal );
	QLabel::paletteChange( pal );
}

void KexiLabel::frameChanged() {
	p_pixmapDirty = true;
	p_privateLabel->updateFrame();
	QFrame::frameChanged();
}

void KexiLabel::showEvent( QShowEvent* e ) {
	p_pixmapDirty = true;
	QLabel::showEvent( e );
}

void KexiLabel::setText( const QString& text ) {
	p_pixmapDirty = true;
	QLabel::setText( text );
	//This is necessary for KexiFormDataItemInterface
	valueChanged();
	repaint();
}

/*bool KexiLabel::event( QEvent* e )
{
	if (e->type()==QEvent::FocusOut) {
		repaint();
		kdDebug() << "FFFFFFFFFFFFFFFFFF!" << endl;
	}
	return QLabel::event(e);
}*/

#include "kexilabel.moc"
