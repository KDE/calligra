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

#include "kexidbimagebox.h"

#include <qapplication.h>
#include <qpixmap.h>
#include <qstyle.h>
#include <qtoolbutton.h>
#include <qclipboard.h>
#include <qtooltip.h>
#include <qimage.h>
#include <qbuffer.h>
#include <q3filedialog.h>
#include <qpainter.h>
//Added by qt3to4:
#include <QPaintEvent>
#include <QContextMenuEvent>

#include <kdebug.h>
#include <kmenu.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kfiledialog.h>
#include <kimageio.h>
#include <kstandarddirs.h>
#include <kstaticdeleter.h>
#include <kimageeffect.h>

#include <kexiutils/utils.h>
#include <kexidb/field.h>
#include <kexidb/queryschema.h>

#ifdef Q_WS_WIN
#include <win32_utils.h>
#include <krecentdirs.h>
#endif

static KStaticDeleter<QPixmap> KexiDBImageBox_pmDeleter;
static QPixmap* KexiDBImageBox_pm = 0;

//! @internal A button class for KexiDBImageBox
class KexiDBImageBox::Button : public QToolButton
{
	public:
		Button(QWidget *parent) : QToolButton(parent, "KexiDBImageBox::Button")
		{
			setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
			setFixedWidth(qMax(15, qApp->globalStrut().width()));
			//	setFixedWidth(m_chooser->minimumSizeHint().width()); //! @todo get this from a KStyle
			setAutoRaise(true);
		}
		~Button() {}
		virtual void drawButton( QPainter *p ) {
			QToolButton::drawButton(p);
			QStyle::State arrowFlags = QStyle::State_None;
			if (isDown())
				arrowFlags |= QStyle::State_DownArrow;
			if (isEnabled())
				arrowFlags |= QStyle::State_Enabled;
			style().drawPrimitive(QStyle::PE_ArrowDown, p,
				QRect((width()-7)/2, height()-9, 7, 7), colorGroup(),
				arrowFlags, QStyleOption() );
		}
};

/////////

KexiDBImageBox::KexiDBImageBox( bool designMode, QWidget *parent, const char *name )
	: QWidget( parent, name, Qt::WNoAutoErase )
	, KexiFormDataItemInterface()
	, m_actionCollection(this)
	, m_alignment(Qt::AlignLeft|Qt::AlignTop)
	, m_designMode(designMode)
	, m_readOnly(false)
	, m_scaledContents(false)
	, m_keepAspectRatio(true)
	, m_insideSetData(false)
{
	setBackgroundMode(Qt::NoBackground);

//	QHBoxLayout *hlyr = new QHBoxLayout(this, 2);
//	m_pixmapLabel = new ImageLabel(this);
//	hlyr->addWidget(m_pixmapLabel);

	if (m_designMode) {
		m_chooser = 0;
	}
	else {
		m_chooser = new Button(this);
//		hlyr->addWidget(m_chooser);
	}
	//setup popup menu
	m_popup = new KMenu(this);
	QString titleString = i18n("Image Box");
	m_titleID = m_popup->insertTitle(SmallIcon("pixmaplabel"), titleString);
	m_insertFromFileAction = new KAction(i18n("Insert From &File..."), SmallIconSet("fileopen"), 0,
			this, SLOT(insertFromFile()), &m_actionCollection, "insert");
	m_insertFromFileAction->plug(m_popup);
	m_saveAsAction = KStdAction::saveAs(this, SLOT(saveAs()), &m_actionCollection);
//	m_saveAsAction->setText(i18n("&Save &As..."));
	m_saveAsAction->plug(m_popup);
	m_popup->insertSeparator();
	m_cutAction = KStdAction::cut(this, SLOT(cut()), &m_actionCollection);
	m_cutAction->plug(m_popup);
	m_copyAction = KStdAction::copy(this, SLOT(copy()), &m_actionCollection);
	m_copyAction->plug(m_popup);
	m_pasteAction = KStdAction::paste(this, SLOT(paste()), &m_actionCollection);
	m_pasteAction->plug(m_popup);
	m_deleteAction = new KAction(i18n("&Clear"), SmallIconSet("editdelete"), 0,
		this, SLOT(clear()), &m_actionCollection, "delete");
	m_deleteAction->plug(m_popup);
#ifdef KEXI_NO_UNFINISHED 
	m_propertiesAction = 0;
#else
	m_popup->insertSeparator();
	m_propertiesAction = new KAction(i18n("Properties"), 0, 0,
		this, SLOT(showProperties()), &m_actionCollection, "properties");
	m_propertiesAction->plug(m_popup);
#endif
	connect(m_popup, SIGNAL(aboutToShow()), this, SLOT(updateActionsAvailability()));
	connect(m_popup, SIGNAL(aboutToHide()), this, SLOT(slotAboutToHidePopupMenu()));
	if (m_chooser) {
		//we couldn't use m_chooser->setPopup() because of drawing problems
		connect(m_chooser, SIGNAL(pressed()), this, SLOT(slotChooserPressed()));
	}

	setDataSource( QString::null ); //to initialize popup menu and actions availability

//	m_chooser->setPopupDelay(0);
//	m_chooser->setPopup(m_popup);
}

KexiDBImageBox::~KexiDBImageBox()
{
}

QVariant KexiDBImageBox::value()
{
	if (dataSource().isEmpty()) {
		//not db-aware
		return QVariant();
	}
	//db-aware mode
	return QVariant(); //todo
}

void KexiDBImageBox::setValueInternal( const QVariant& add, bool /* irrelevant here: removeOld*/ )
{
	if (isReadOnly())
		return;
	m_value = add.toByteArray();
//	m_pixmap = QPixmap(); //will be loaded on demand
	repaint();
	emit valueChanged(m_value);
}

void KexiDBImageBox::setInvalidState( const QString& displayText )
{
	Q_UNUSED( displayText );

//	m_pixmapLabel->setPixmap(QPixmap());
	if (!dataSource().isEmpty()) {
		m_value = QByteArray();
	}
//	m_pixmap = QPixmap();
//	m_originalFileName = QString::null;

//! @todo m_pixmapLabel->setText( displayText );

	if (m_chooser)
		m_chooser->hide();
	setReadOnly(true);
}

bool KexiDBImageBox::valueIsNull()
{
	return m_value.isEmpty();
//	return !m_pixmapLabel->pixmap() || m_pixmapLabel->pixmap()->isNull();
}

bool KexiDBImageBox::valueIsEmpty()
{
	return false;
}

bool KexiDBImageBox::isReadOnly() const
{
	return m_readOnly;
}

void KexiDBImageBox::setReadOnly(bool set)
{
	m_readOnly = set;
}

QPixmap KexiDBImageBox::pixmap() const
{
	if (dataSource().isEmpty()) {
		//not db-aware
		return m_data.pixmap();
	}
	//db-aware mode
	return m_pixmap;
}

uint KexiDBImageBox::pixmapId() const
{
	if (dataSource().isEmpty()) {// && !m_data.stored()) {
		//not db-aware
		return m_data.id();
	}
	return 0;
}

void KexiDBImageBox::setPixmapId(uint id)
{
	if (m_insideSetData) //avoid recursion
		return;
	setData(KexiBLOBBuffer::self()->objectForId( id, /*unstored*/false ));
	repaint();
}

uint KexiDBImageBox::storedPixmapId() const
{
	if (dataSource().isEmpty() && m_data.stored()) {
		//not db-aware
		return m_data.id();
	}
	return 0;
}

void KexiDBImageBox::setStoredPixmapId(uint id)
{
	setData(KexiBLOBBuffer::self()->objectForId( id, /*stored*/true ));
	repaint();
}

bool KexiDBImageBox::hasScaledContents() const
{
	return m_scaledContents;
//	return m_pixmapLabel->hasScaledContents();
}

/*void KexiDBImageBox::setPixmap(const QByteArray& pixmap)
{
	setValueInternal(pixmap, true);
//	setBackgroundMode(pixmap.isNull() ? Qt::NoBackground : Qt::PaletteBackground);
}*/

void KexiDBImageBox::setScaledContents(bool set)
{
//todo	m_pixmapLabel->setScaledContents(set);
	m_scaledContents = set;
	repaint();
}

void KexiDBImageBox::setKeepAspectRatio(bool set)
{
	m_keepAspectRatio = set;
	if (m_scaledContents)
		repaint();
}

QWidget* KexiDBImageBox::widget()
{
	//! @todo
//	return m_pixmapLabel;
	return this;
}

bool KexiDBImageBox::cursorAtStart()
{
	return true;
}

bool KexiDBImageBox::cursorAtEnd()
{
	return true;
}

/*void KexiDBImageBox::clear()
{
	if (isReadOnly())
		return;
	m_pixmap = QPixmap();
	repaint();
//	m_pixmapLabel->setPixmap(QPixmap());
//	m_pixmapLabel->setText(QString::null);
	emit valueChanged(QPixmap());
}*/

void KexiDBImageBox::insertFromFile()
{
	if (!dataSource().isEmpty() && isReadOnly())
		return;

#ifdef Q_WS_WIN
	QString recentDir;
	QString fileName = Q3FileDialog::getOpenFileName(
		KFileDialog::getStartURL(":LastVisitedImagePath", recentDir).path(), 
		convertKFileDialogFilterToQFileDialogFilter(KImageIO::pattern(KImageIO::Reading)), 
		this, 0, i18n("Insert Image From File"));
	KUrl url;
	url.setPath( fileName );
#else
	KUrl url( KFileDialog::getImageOpenURL(
		":LastVisitedImagePath", this, i18n("Insert Image From File")) );
//	QString fileName = url.isLocalFile() ? url.path() : url.prettyURL();

	//! @todo download the file if remote, then set fileName properly
#endif
	if (!url.isValid())
		return;
	kexipluginsdbg << "fname=" << url.prettyURL() << endl;

	if (dataSource().isEmpty()) {
		//static mode
		KexiBLOBBuffer::Handle h = KexiBLOBBuffer::self()->insertPixmap( url );
		if (!h)
			return;
		setData(h);
	}
	else {
		//db-aware
#ifndef Q_WS_WIN
		QString fileName = url.isLocalFile() ? url.path() : url.prettyURL();
#endif
		//! @todo download the file if remote, then set fileName properly
		QFile f(fileName);
		if (!f.open(QIODevice::ReadOnly)) {
			//! @todo err msg
			return;
		}
		m_value = f.readAll();
		if (f.status()!=IO_Ok) {
			//! @todo err msg
			f.close();
			return;
		}
	}
	repaint();

//! @todo emit signal for setting "dirty" flag within the design

#ifdef Q_WS_WIN
	//save last visited path
//	KUrl url(fileName);
	if (url.isLocalFile())
		KRecentDirs::add(":LastVisitedImagePath", url.directory());
#endif
}

QByteArray KexiDBImageBox::data() const
{
	if (dataSource().isEmpty()) {
		//static mode
		return m_data.data();
	}
	else {
		//db-aware mode
		return m_value;
	}
}

void KexiDBImageBox::saveAs()
{
//	if (!m_pixmapLabel->pixmap() || m_pixmapLabel->pixmap()->isNull()) {
	if (data().isEmpty()) {
		kWarning() << "KexiDBImageBox::saveAs(): no pixmap!" << endl;
		return;
	}
#ifdef Q_WS_WIN
	QString recentDir;

	QString fileName = Q3FileDialog::getSaveFileName(
		KFileDialog::getStartURL(":LastVisitedImagePath", recentDir).path()
		+"/"+m_data.originalFileName(), 
		convertKFileDialogFilterToQFileDialogFilter(KImageIO::pattern(KImageIO::Writing)), 
		this, 0, i18n("Save Image to File"));
#else
	//! @todo add originalFileName! (requires access to KRecentDirs)
	QString fileName = KFileDialog::getSaveFileName(
		":LastVisitedImagePath", KImageIO::pattern(KImageIO::Writing), this, i18n("Save Image to File"));
#endif
	if (fileName.isEmpty())
		return;
	kexipluginsdbg << fileName << endl;
	KUrl url;
	url.setPath( fileName );

	QFile f(fileName);
	if (!f.open(QIODevice::WriteOnly)) {
		//! @todo err msg
		return;
	}
	f.write( data() );
	if (f.status()!=IO_Ok) {
		//! @todo err msg
		f.close();
		return;
	}
	f.close();

//	if (!m_value.pixmap().save(fileName, KImageIO::type(fileName).latin1())) {
//		//! @todo err msg
//		return;
//	}

#ifdef Q_WS_WIN
	//save last visited path
//	KUrl url(fileName);
	if (url.isLocalFile())
		KRecentDirs::add(":LastVisitedImagePath", url.directory());
#endif
}

void KexiDBImageBox::cut()
{
	if (!dataSource().isEmpty() && isReadOnly())
		return;
	copy();
	clear();
}

void KexiDBImageBox::copy()
{
//	if (m_pixmapLabel->pixmap())
	qApp->clipboard()->setPixmap(pixmap(), QClipboard::Clipboard);
}

void KexiDBImageBox::paste()
{
	if (isReadOnly())
		return;
	QPixmap pm( qApp->clipboard()->pixmap(QClipboard::Clipboard) );
//	if (!pm.isNull())
//		setValueInternal(pm, true);
	if (dataSource().isEmpty()) {
		//static mode
		setData(KexiBLOBBuffer::self()->insertPixmap( pm ));
	}
	else {
		//db-aware mode
		m_pixmap = pm;
//todo m_value
	}
	
	repaint();
	emit valueChanged(data());
}

void KexiDBImageBox::clear()
{
	if (dataSource().isEmpty()) {
		//static mode
		setData(KexiBLOBBuffer::Handle());
	}
	else {
		if (isReadOnly())
			return;
		//db-aware mode
		m_pixmap = QPixmap();
		m_value = QByteArray();
	}
	
//	setValueInternal(QByteArray(), true);
//	m_originalFileName = QString::null;

	//! @todo emit signal for setting "dirty" flag within the design

//	m_pixmap = QPixmap(); //will be loaded on demand
	repaint();
	emit valueChanged(data());
}

void KexiDBImageBox::showProperties()
{
	//! @todo
}

void KexiDBImageBox::updateActionsAvailability()
{
	const bool notNull 
		= (dataSource().isEmpty() && !pixmap().isNull())
		|| (!dataSource().isEmpty() && !valueIsNull());
	const bool readOnly = !dataSource().isEmpty() && !isReadOnly();

	m_insertFromFileAction->setEnabled( !readOnly );
	m_saveAsAction->setEnabled( notNull );
	m_cutAction->setEnabled( notNull && !readOnly );
	m_copyAction->setEnabled( notNull );
	m_pasteAction->setEnabled( !readOnly );
	m_deleteAction->setEnabled( notNull && !readOnly );
	if (m_propertiesAction)
		m_propertiesAction->setEnabled( notNull );
}

void KexiDBImageBox::slotAboutToHidePopupMenu()
{
	m_clickTimer.start(50, true);
}

void KexiDBImageBox::contextMenuEvent( QContextMenuEvent * e )
{
	if (popupMenuAvailable())
		m_popup->exec( e->globalPos(), -1 );
}

void KexiDBImageBox::slotChooserPressed()
{
	if (m_clickTimer.isActive())
		return;
	QRect screen = qApp->desktop()->availableGeometry( m_chooser );
	QPoint p;
	if ( QApplication::isRightToLeft() ) {
		if ( mapToGlobal( m_chooser->rect().bottomLeft() ).y() + m_popup->sizeHint().height() <= screen.height() )
			p = m_chooser->mapToGlobal( m_chooser->rect().bottomRight() );
		else
			p = m_chooser->mapToGlobal( m_chooser->rect().topRight() - QPoint( 0, m_popup->sizeHint().height() ) );
		p.rx() -= m_popup->sizeHint().width();
	}
	else {
		if ( m_chooser->mapToGlobal( m_chooser->rect().bottomLeft() ).y() + m_popup->sizeHint().height() <= screen.height() )
			p = m_chooser->mapToGlobal( m_chooser->rect().bottomLeft() );
		else
			p = m_chooser->mapToGlobal( m_chooser->rect().topLeft() - QPoint( 0, m_popup->sizeHint().height() ) );
	}
	if (!m_popup->isVisible()) {
		m_popup->exec( p, -1 );
	}
	m_chooser->setDown( false );
}

void KexiDBImageBox::updateActionStrings()
{
	if (!m_popup)
		return;
	QString titleString = i18n("Image Box");
	if (!dataSource().isEmpty())
		titleString += (": " + dataSource());
	m_popup->changeTitle(m_titleID, m_popup->titlePixmap(m_titleID), titleString);

	if (m_chooser) {
		if (popupMenuAvailable() && dataSource().isEmpty()) //this may work in the future (see @todo below)
			m_chooser->setToolTip( i18n("Click to show actions for this image box"));
		else
			m_chooser->setToolTip( i18n("Click to show actions for \"%1\" image box").arg(dataSource()));
	}
}

bool KexiDBImageBox::popupMenuAvailable()
{
/*! @todo add kexi-global setting which anyway, allows to show this button
          (read-only actions like copy/save as/print can be available) */
	//chooser button can be only visible when data source is specified
	return !dataSource().isEmpty();
}

void KexiDBImageBox::setDataSource( const QString &ds )
{
	KexiFormDataItemInterface::setDataSource( ds );
	setData(KexiBLOBBuffer::Handle());
	updateActionStrings();

	if (m_chooser) {
		if (popupMenuAvailable()) {
			m_chooser->show();
		}
		else {
			m_chooser->hide();
		}
	}
}

QSize KexiDBImageBox::sizeHint() const
{
	if (pixmap().isNull())
		return QSize(80, 80);
	return pixmap().size();
}

//void KexiDBImageBox::drawContents( QPainter *p )
void KexiDBImageBox::paintEvent( QPaintEvent*pe )
{
	QPainter p(this);
	p.setClipRect(pe->rect());
	const int m = 0; //todo margin();
//	QPainter ptr(this);
//	ptr.fillRect(0,0,width(),height(), green);
//	p->setClipRect(0, 0, width(), height());
//	QFrame::drawContents( p );
//	QFrame::drawFrame( p );
//	QColor bg(palette().active().background());//parentWidget()->palette().active().background()
	QColor bg(eraseColor());
	if (m_designMode && pixmap().isNull()) {
		QPixmap pm(size());
		QPainter p2;
		p2.begin(&pm, this);
//			QLabel::drawContents( p );
		p2.fillRect(0,0,width(),height(), bg);

		updatePixmap();
		QImage img(KexiDBImageBox_pm->convertToImage());
		img = KImageEffect::flatten(img, bg.dark(150),
			qGray( bg.rgb() ) <= 20 ? Qt::darkGray : bg.light(105));
//				m_scalledDown = (pix.width() > (width()/2) || pix.height() > (height()/2));
//				if (m_scalledDown)
//					img = img.smoothScale(width()/2, height()/2, QImage::ScaleMin);
	
//				KexiDBImageBox_pmDeleter.setObject( KexiDBImageBox_pm, new QPixmap() );
//				KexiDBImageBox_pm->convertFromImage(img);

		QPixmap converted;
		converted.convertFromImage(img);
		p2.drawPixmap(m+2, height()-m-KexiDBImageBox_pm->height()-2, converted);
		QFont f(qApp->font());
//		f.setPointSize(f.pointSize());
		p2.setFont(f);
		p2.setPen( KexiUtils::contrastColor( bg ) );
		p2.drawText(pm.rect(), Qt::AlignCenter|Qt::TextWordWrap, i18n("No Image"));
		p2.end();
		bitBlt(this, 0, 0, &pm);
	}
	else {
//		QFrame::drawContents( p );
		if (pixmap().isNull())
			p.fillRect(0,0,width(),height(), bg);
		else {
			const bool fast = pixmap().width()>1000 && pixmap().height()>800; //fast drawing needed
//! @todo we can optimize drawing by drawing rescaled pixmap here 
//! and performing detailed painting later (using QTimer)
			QPixmap pm;
			QPainter p2;
			QPainter *target;
			if (fast) {
				target = &p;
			}
			else {
				pm.resize(size());
				p2.begin(&pm, this);
				target = &p2;
			}
			//clearing needed here because we may need to draw a pixmap with transparency
			target->fillRect(0,0,width(),height(), bg);
			if (m_scaledContents) {
				if (m_keepAspectRatio) {
					QImage img(pixmap().convertToImage());
					img = img.smoothScale(width(), height(), Qt::KeepAspectRatio);
					QPoint pos(0,0);
					if (img.width()<width()) {
						int hAlign = QApplication::horizontalAlignment( m_alignment );
						if ( hAlign & Qt::AlignRight )
							pos.setX(width()-img.width());
						else if ( hAlign & Qt::AlignHCenter )
							pos.setX(width()/2-img.width()/2);
					}
					else if (img.height()<height()) {
						if ( m_alignment & Qt::AlignBottom )
							pos.setY(height()-img.height());
						else if ( m_alignment & Qt::AlignVCenter )
							pos.setY(height()/2-img.height()/2);
					}
					QPixmap px;
					px.convertFromImage(img);
					target->drawPixmap(pos, px);
				}
				else {
					target->drawPixmap(QRect(m, m, width()-m*2, height()-m*2), pixmap());
				}
			}
			else {
				int hAlign = QApplication::horizontalAlignment( m_alignment );
				QPoint pos;
				if ( hAlign & Qt::AlignRight )
					pos.setX(width()-pixmap().width()-m);
				else if ( hAlign & Qt::AlignHCenter )
					pos.setX(width()/2-pixmap().width()/2);
				else //left, etc.
					pos.setX(m);

				if ( m_alignment & Qt::AlignBottom )
					pos.setY(height()-pixmap().height()-m);
				else if ( m_alignment & Qt::AlignVCenter )
					pos.setY(height()/2-pixmap().height()/2);
				else //top, etc. 
					pos.setY(m);
				target->drawPixmap(pos, pixmap());
			}
			if (!fast) {
				p2.end();
				bitBlt(this, 0, 0, &pm);
			}
		}
	}
}
/*		virtual void KexiDBImageBox::paletteChange ( const QPalette & oldPalette )
{
	QFrame::paletteChange(oldPalette);
	if (oldPalette.active().background()!=palette().active().background()) {
		delete KexiDBImageBox_pm;
		KexiDBImageBox_pm = 0;
		repaint();
	}
}*/
void KexiDBImageBox::updatePixmap() {
	if (! (m_designMode && pixmap().isNull()) )
		return;

//			if (KexiDBImageBox_pm) {
//				QSize size = KexiDBImageBox_pm->size();
//				if ((KexiDBImageBox_pm->width() > (width()/2) || KexiDBImageBox_pm->height() > (height()/2))) {
//					int maxSize = qMax(width()/2, height()/2);
//					size = QSize(maxSize,maxSize);
//					delete KexiDBImageBox_pm;
//					KexiDBImageBox_pm = 0;
//				}
//			}
	if (!KexiDBImageBox_pm) {
		QString fname( locate("data", QString("kexi/pics/imagebox.png")) );
		KexiDBImageBox_pmDeleter.setObject( KexiDBImageBox_pm, new QPixmap(fname, "PNG") );
	}
}

void KexiDBImageBox::setAlignment(int alignment)
{
	m_alignment = alignment;
	if (!m_scaledContents || m_keepAspectRatio)
		repaint();
}

void KexiDBImageBox::setData(const KexiBLOBBuffer::Handle& handle)
{
	if (m_insideSetData) //avoid recursion
		return;
	m_insideSetData = true;
	m_data = handle;
	emit idChanged(handle.id());
	m_insideSetData = false;
}

//		virtual void resizeEvent( QResizeEvent *e )
//		{
//			updatePixmap();
//			QWidget::resizeEvent(e);
//		}

/*void KexiDBImageBox::paintEvent( QPaintEvent* )
{
}
*/
/*
bool KexiDBImageBox::setProperty( const char * name, const QVariant & value )
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

void KexiDBImageBox::setColumnInfo(KexiDB::QueryColumnInfo* cinfo)
{
	KexiFormDataItemInterface::setColumnInfo(cinfo);
	KexiDBTextWidgetInterface::setColumnInfo(cinfo, this);
}*/

#include "kexidbimagebox.moc"
