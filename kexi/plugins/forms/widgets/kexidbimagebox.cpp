/* This file is part of the KDE project
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2006 Jaroslaw Staniek <js@iidea.pl>

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
#include <qfiledialog.h>
#include <qpainter.h>

#include <kdebug.h>
#include <kpopupmenu.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kfiledialog.h>
#include <kimageio.h>
#include <kstandarddirs.h>
#include <kstaticdeleter.h>
#include <kimageeffect.h>
#include <kstdaccel.h>
#include <kmessagebox.h>
#include <kguiitem.h>

#include <kexiutils/utils.h>
#include <kexidb/field.h>
#include <kexidb/queryschema.h>

#ifdef Q_WS_WIN
#include <win32_utils.h>
#include <krecentdirs.h>
#endif

#include "kexidbutils.h"

static KStaticDeleter<QPixmap> KexiDBImageBox_pmDeleter;
static QPixmap* KexiDBImageBox_pm = 0;

//! @internal A button class for KexiDBImageBox
class KexiDBImageBox::Button : public QToolButton
{
	public:
		Button(KexiDBImageBox *parent, KPopupMenu* aPopup)
		 : QToolButton(parent, "KexiDBImageBox::Button")
		 , disableMousePress(false)
		 , popup(aPopup)
		{
			setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
			setFixedWidth(QMAX(15, qApp->globalStrut().width()));
			//	setFixedWidth(m_chooser->minimumSizeHint().width()); //! @todo get this from a KStyle
//			setAutoRaise(true);
			setToggleButton(true);
		}
		
		virtual ~Button() {}

		virtual void drawButton( QPainter *p ) {
//			p->fillRect(0, 0, width(), height(), red);
			QToolButton::drawButton(p);
			QStyle::SFlags arrowFlags = QStyle::Style_Default;
			if (isDown() || state()==On)
				arrowFlags |= QStyle::Style_Down;
			if (isEnabled())
				arrowFlags |= QStyle::Style_Enabled;
			style().drawPrimitive(QStyle::PE_ArrowDown, p,
				QRect((width()-7)/2, height()-9, 7, 7), colorGroup(),
				arrowFlags, QStyleOption() );
		}
		
		virtual QSize sizeHint () const {
			return QSize( fontMetrics().maxWidth() + 2*2, fontMetrics().height()*2 + 2*2 );
		}

		virtual void mousePressEvent( QMouseEvent *e ) {
//			kexipluginsdbg << "#### mousePressEvent() " << e->button() << " " << state() 
//				<< " disableMousePress=" << disableMousePress << " popup->isVisible()=" << popup->isVisible() << endl;
			if (disableMousePress) {
				disableMousePress = false;
				if (popup && popup->isVisible())
					return;
			}
			QToolButton::mousePressEvent(e);
		}

		virtual void keyPressEvent ( QKeyEvent * e ) {
			const int k = e->key();
			if ( (e->state() == Qt::NoButton && (k==Qt::Key_Enter || k==Qt::Key_Return || k==Qt::Key_F2 || k==Qt::Key_F4))
			  || (e->state() == Qt::AltButton && e->key()==Qt::Key_Down) )
			{
				static_cast<KexiDBImageBox*>(parentWidget())->slotToggled(true);
				e->accept();
			}
			QToolButton::keyPressEvent(e);

		}

		QGuardedPtr<KPopupMenu> popup;
		bool disableMousePress : 1;
};

/////////

KexiDBImageBox::KexiDBImageBox( bool designMode, QWidget *parent, const char *name )
	: KexiFrame( parent, name, Qt::WNoAutoErase )
	, KexiFormDataItemInterface()
	, m_actionCollection(this)
	, m_alignment(Qt::AlignAuto|Qt::AlignTop)
	, m_designMode(designMode)
	, m_readOnly(false)
	, m_scaledContents(false)
	, m_keepAspectRatio(true)
	, m_insideSetData(false)
	, m_setFocusOnButtonAfterClosingPopup(false)
	, m_lineWidthChanged(false)
	, m_paletteBackgroundColorChanged(false)
	, m_paintEventEnabled(true)
	, m_dropDownButtonVisible(true)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	setBackgroundMode(Qt::NoBackground);
	setFrameShape(QFrame::Box);
	setFrameShadow(QFrame::Plain);
	setFrameColor(Qt::black);

	//setup popup menu
	m_popup = new KPopupMenu(this);
	m_popup->insertTitle(QString::null);

	if (m_designMode) {
		m_chooser = 0;
	}
	else {
		m_chooser = new Button(this, m_popup);
		m_chooser->setFocusPolicy(StrongFocus);
		setFocusProxy(m_chooser);
//		m_chooser->setPalette(qApp->palette());
//		hlyr->addWidget(m_chooser);
	}

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
		connect(m_chooser, SIGNAL(released()), this, SLOT(slotChooserReleased()));
		connect(m_chooser, SIGNAL(toggled(bool)), this, SLOT(slotToggled(bool)));
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
	return m_value; //todo
	//return QVariant(); //todo
}

void KexiDBImageBox::setValueInternal( const QVariant& add, bool removeOld, bool loadPixmap )
{
	if (isReadOnly())
		return;
//	const bool valueWasEmpty = m_value.isEmpty();
	if (removeOld) 
		m_value = add.toByteArray();
	else //do not add "m_origValue" to "add" as this is QByteArray
		m_value = m_origValue.toByteArray();
	bool ok = !m_value.isEmpty();
	if (ok) {
		///unused (m_valueMimeType is not available unless the px is inserted) QString type( KImageIO::typeForMime(m_valueMimeType) );
		///ok = KImageIO::canRead( type );
		ok = loadPixmap ? m_pixmap.loadFromData(m_value) : true; //, type.latin1());
		if (!ok) {
			//! @todo inform about error?
		}
	}
	if (!ok) {
		m_valueMimeType = QString::null;
		m_pixmap = QPixmap();
	}
	repaint();
//	if (m_value.isEmpty() != valueWasEmpty)
//		emit pixmapChanged();//valueChanged(m_value);
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
	QString fileName = QFileDialog::getOpenFileName(
		KFileDialog::getStartURL(":LastVisitedImagePath", recentDir).path(), 
		convertKFileDialogFilterToQFileDialogFilter(KImageIO::pattern(KImageIO::Reading)), 
		this, 0, i18n("Insert Image From File"));
	KURL url;
	url.setPath( fileName );
#else
	KURL url( KFileDialog::getImageOpenURL(
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
		repaint();
	}
	else {
		//db-aware
#ifndef Q_WS_WIN
		QString fileName = url.isLocalFile() ? url.path() : url.prettyURL();
#endif
		//! @todo download the file if remote, then set fileName properly
		QFile f(fileName);
		if (!f.open(IO_ReadOnly)) {
			//! @todo err msg
			return;
		}
		QByteArray ba = f.readAll();
		if (f.status()!=IO_Ok) {
			//! @todo err msg
			f.close();
			return;
		}
		m_valueMimeType = KImageIO::mimeType( fileName ); 
		setValueInternal( ba, true );
	}

//! @todo emit signal for setting "dirty" flag within the design

#ifdef Q_WS_WIN
	//save last visited path
//	KURL url(fileName);
	if (url.isLocalFile())
		KRecentDirs::add(":LastVisitedImagePath", url.directory());
#endif
	if (!dataSource().isEmpty()) {
		signalValueChanged();
	}
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
	if (data().isEmpty()) {
		kdWarning() << "KexiDBImageBox::saveAs(): no pixmap!" << endl;
		return;
	}
	QString origFilename, fileExtension;
	if (dataSource().isEmpty()) { //for static images filename and mimetype can be available
		origFilename = m_data.originalFileName();
		if (!origFilename.isEmpty())
			origFilename = QString("/") + origFilename;
		if (!m_data.mimeType().isEmpty())
			fileExtension = KImageIO::typeForMime(m_data.mimeType()).lower();
	}
	else {
		// PNG data is the default
		fileExtension = "png";
	}
	
#ifdef Q_WS_WIN
	QString recentDir;

	QString fileName = QFileDialog::getSaveFileName(
		KFileDialog::getStartURL(":LastVisitedImagePath", recentDir).path() + origFilename,
		convertKFileDialogFilterToQFileDialogFilter(KImageIO::pattern(KImageIO::Writing)), 
		this, 0, i18n("Save Image to File"));
#else
	//! @todo add originalFileName! (requires access to KRecentDirs)
	QString fileName = KFileDialog::getSaveFileName(
		":LastVisitedImagePath", KImageIO::pattern(KImageIO::Writing), this, i18n("Save Image to File"));
#endif
	if (fileName.isEmpty())
		return;
	if (QFileInfo(fileName).extension().isEmpty())
		fileName += (QString(".")+fileExtension);
	kexipluginsdbg << fileName << endl;
	KURL url;
	url.setPath( fileName );

	QFile f(fileName);
	if (f.exists() && KMessageBox::Yes != KMessageBox::warningYesNo(this, 
		"<qt>"+i18n("File %1 already exists."
		"<p>Do you want to replace it with a new one?")
		.arg(QDir::convertSeparators(fileName))+"</qt>",0, 
		KGuiItem(i18n("&Replace")), KGuiItem(i18n("&Don't Replace"))))
	{
		return;
	}

	if (!f.open(IO_WriteOnly)) {
		//! @todo err msg
		return;
	}
	f.writeBlock( data() );
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
	if (isReadOnly() || (!m_designMode && !hasFocus()))
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
    QByteArray ba;
    QBuffer buffer( ba );
    buffer.open( IO_WriteOnly );
		if (m_pixmap.save( &buffer, "PNG" )) {// write pixmap into ba in PNG format
			setValueInternal( ba, true, false/* !loadPixmap */ );
		}
		else {
			setValueInternal( QByteArray(), true );
		}
	}
	
	repaint();
	if (!dataSource().isEmpty()) {
//		emit pixmapChanged();
		signalValueChanged();
	}
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
		setValueInternal(QByteArray(), true);
		//m_pixmap = QPixmap();
	}

//	m_originalFileName = QString::null;

	//! @todo emit signal for setting "dirty" flag within the design

//	m_pixmap = QPixmap(); //will be loaded on demand
	repaint();
	if (!dataSource().isEmpty()) {
//		emit pixmapChanged();//valueChanged(data());
		signalValueChanged();
	}
}

void KexiDBImageBox::showProperties()
{
	//! @todo
}

void KexiDBImageBox::updateActionsAvailability()
{
	const bool notNull 
		= (dataSource().isEmpty() && !pixmap().isNull()) //static pixmap available
		|| (!dataSource().isEmpty() && !valueIsNull());  //db-aware pixmap available
	// read-only if static pixmap or db-aware pixmap for read-only widget:
	const bool readOnly = !m_designMode && dataSource().isEmpty() || !dataSource().isEmpty() && isReadOnly()
		|| m_designMode && !dataSource().isEmpty();

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
//	kexipluginsdbg << "##### slotAboutToHidePopupMenu() " << endl;
	m_clickTimer.start(50, true);
	if (m_chooser && m_chooser->isOn()) {
		m_chooser->toggle();
		if (m_setFocusOnButtonAfterClosingPopup) {
			m_setFocusOnButtonAfterClosingPopup = false;
			m_chooser->setFocus();
		}
	}
}

void KexiDBImageBox::contextMenuEvent( QContextMenuEvent * e )
{
	if (popupMenuAvailable())
		m_popup->exec( e->globalPos(), -1 );
}

void KexiDBImageBox::slotChooserPressed()
{
//	if (!m_clickTimer.isActive())
//		return;
//	m_chooser->setDown( false );
}

void KexiDBImageBox::slotChooserReleased()
{
}

void KexiDBImageBox::slotToggled(bool on)
{
//	kexipluginsdbg << "##### slotToggled() " << on << endl;
	if (m_clickTimer.isActive() || !on) {
		m_chooser->disableMousePress = true;
		return;
	}
	m_chooser->disableMousePress = false;
	QRect screen = qApp->desktop()->availableGeometry( m_chooser );
	QPoint p;
	if ( QApplication::reverseLayout() ) {
		if ( (mapToGlobal( m_chooser->rect().bottomLeft() ).y() + m_popup->sizeHint().height()) <= screen.height() )
			p = m_chooser->mapToGlobal( m_chooser->rect().bottomRight() );
		else
			p = m_chooser->mapToGlobal( m_chooser->rect().topRight() - QPoint( 0, m_popup->sizeHint().height() ) );
		p.rx() -= m_popup->sizeHint().width();
	}
	else {
		if ( (m_chooser->mapToGlobal( m_chooser->rect().bottomLeft() ).y() + m_popup->sizeHint().height()) <= screen.height() )
			p = m_chooser->mapToGlobal( m_chooser->rect().bottomLeft() );
		else
			p = m_chooser->mapToGlobal( m_chooser->rect().topLeft() - QPoint( 0, m_popup->sizeHint().height() ) );
	}
	if (!m_popup->isVisible() && on) {
		m_popup->exec( p, -1 );
		m_popup->setFocus();
	}
	//m_chooser->setDown( false );
}

void KexiDBImageBox::updateActionStrings()
{
	if (!m_popup)
		return;
	if (m_designMode) {
		QString titleString( i18n("Image Box") );
		if (!dataSource().isEmpty())
			titleString += (": " + dataSource());
		m_popup->changeTitle(m_popup->idAt(0), m_popup->titlePixmap(m_popup->idAt(0)), titleString);
	}
	else {
		//update title in data view mode, based on the data source
		KexiDBWidgetContextMenuExtender::updateContextMenuTitleForDataItem(m_popup, this);
	}

	if (m_chooser) {
		if (popupMenuAvailable() && dataSource().isEmpty()) { //this may work in the future (see @todo below)
			QToolTip::add(m_chooser, i18n("Click to show actions for this image box"));
		} else {
			QString beautifiedImageBoxName;
			if (m_designMode) {
				beautifiedImageBoxName = dataSource();
			}
			else {
				beautifiedImageBoxName = columnInfo() ? columnInfo()->captionOrAliasOrName() : QString::null;
				/*! @todo look at makeFirstCharacterUpperCaseInCaptions setting [bool]
				 (see doc/dev/settings.txt) */
				beautifiedImageBoxName = beautifiedImageBoxName[0].upper() + beautifiedImageBoxName.mid(1);
			}
			QToolTip::add(m_chooser, i18n("Click to show actions for \"%1\" image box").arg(beautifiedImageBoxName));
		}
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

	// update some properties s not changed by user
//! @todo get default line width from global style settings
	if (!m_lineWidthChanged) {
		KexiFrame::setLineWidth( ds.isEmpty() ? 0 : 1 );
	}
	if (!m_paletteBackgroundColorChanged && parentWidget()) {
		KexiFrame::setPaletteBackgroundColor( 
			dataSource().isEmpty() ? parentWidget()->paletteBackgroundColor() : palette().active().base() );
	}
}

QSize KexiDBImageBox::sizeHint() const
{
	if (pixmap().isNull())
		return QSize(80, 80);
	return pixmap().size();
}

int KexiDBImageBox::realLineWidth() const
{
	if (frameShape()==QFrame::Box && (frameShadow()==QFrame::Sunken || frameShadow()==QFrame::Raised))
		return 2 * lineWidth();
	else
		return lineWidth();
}

void KexiDBImageBox::paintEvent( QPaintEvent *pe )
{
	if (!m_paintEventEnabled)
		return;
	QPainter p(this);
	p.setClipRect(pe->rect());
	const int m = realLineWidth();
	const int w = width()-m-m;
	const int h = height()-m-m;
	QColor bg(eraseColor());
	if (m_designMode && pixmap().isNull()) {
		QPixmap pm(size()-QSize(m, m));
		QPainter p2;
		p2.begin(&pm, this);
		p2.fillRect(0,0,width(),height(), bg);

		updatePixmap();
		QImage img(KexiDBImageBox_pm->convertToImage());
		img = KImageEffect::flatten(img, bg.dark(150),
			qGray( bg.rgb() ) <= 20 ? QColor(Qt::gray).dark(150) : bg.light(105));

		QPixmap converted;
		converted.convertFromImage(img);
		p2.drawPixmap(2, height()-m-m-KexiDBImageBox_pm->height()-2, converted);
		QFont f(qApp->font());
		p2.setFont(f);
		p2.setPen( KexiUtils::contrastColor( bg ) );
		p2.drawText(pm.rect(), Qt::AlignCenter|Qt::WordBreak, 
			dataSource().isEmpty() ? i18n("No Image") : dataSource());
		p2.end();
		bitBlt(this, m, m, &pm);
	}
	else {
		QSize internalSize(size());
		if (m_chooser && m_dropDownButtonVisible && !dataSource().isEmpty())
			internalSize.setWidth( internalSize.width() - m_chooser->width() );
		
		//clearing needed here because we may need to draw a pixmap with transparency
		p.fillRect(0,0,width(),height(), bg);

		KexiUtils::drawPixmap( p, bg, m, QRect(QPoint(0,0), internalSize), pixmap(), m_alignment, 
			m_scaledContents, m_keepAspectRatio );

#if 0 //moved to KexiUtils::drawPixmap()
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
				pm.resize(size()-QSize(m, m));
				p2.begin(&pm, this);
				target = &p2;
			}
			//clearing needed here because we may need to draw a pixmap with transparency
			target->fillRect(0,0,width(),height(), bg);
			if (m_scaledContents) {
				if (m_keepAspectRatio) {
					QImage img(pixmap().convertToImage());
					img = img.smoothScale(w, h, QImage::ScaleMin);
					QPoint pos(0, 0);
					if (img.width() < w) {
						int hAlign = QApplication::horizontalAlignment( m_alignment );
						if ( hAlign & Qt::AlignRight )
							pos.setX(w-img.width());
						else if ( hAlign & Qt::AlignHCenter )
							pos.setX(w/2-img.width()/2);
					}
					else if (img.height() < h) {
						if ( m_alignment & Qt::AlignBottom )
							pos.setY(h-img.height());
						else if ( m_alignment & Qt::AlignVCenter )
							pos.setY(h/2-img.height()/2);
					}
					QPixmap px;
					px.convertFromImage(img);
					target->drawPixmap(pos, px);
				}
				else {
					target->drawPixmap(QRect(0, 0, w, h), pixmap());
				}
			}
			else {
				int hAlign = QApplication::horizontalAlignment( m_alignment );
				QPoint pos;
				if ( hAlign & Qt::AlignRight )
					pos.setX(w-pixmap().width());
				else if ( hAlign & Qt::AlignHCenter )
					pos.setX(w/2-pixmap().width()/2);
				else //left, etc.
					pos.setX(0);

				if ( m_alignment & Qt::AlignBottom )
					pos.setY(h-pixmap().height());
				else if ( m_alignment & Qt::AlignVCenter )
					pos.setY(h/2-pixmap().height()/2);
				else //top, etc. 
					pos.setY(0);
				target->drawPixmap(pos, pixmap());
			}
			if (!fast) {
				p2.end();
				bitBlt(this, m, m, &pm);
			}
		}
#endif
	}
	KexiFrame::drawFrame( &p );
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
//					int maxSize = QMAX(width()/2, height()/2);
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
	update();
}

void KexiDBImageBox::resizeEvent( QResizeEvent * e )
{
	KexiFrame::resizeEvent(e);
	if (m_chooser) {
		QSize s( m_chooser->sizeHint() );
		QSize margin( realLineWidth(), realLineWidth() );
		s.setHeight( height() - 2*margin.height() );
		s = s.boundedTo( size()-2*margin );
		m_chooser->resize( s );
		m_chooser->move( QRect(QPoint(0,0), e->size() - m_chooser->size() - margin + QSize(1,1)).bottomRight() );
	}
}

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
*/

void KexiDBImageBox::setColumnInfo(KexiDB::QueryColumnInfo* cinfo)
{
	KexiFormDataItemInterface::setColumnInfo(cinfo);
	//updating strings and title is needed
	updateActionStrings();
}

bool KexiDBImageBox::keyPressed(QKeyEvent *ke)
{
	// Esc key should close the popup
	if (ke->state() == Qt::NoButton && ke->key() == Qt::Key_Escape) {
		if (m_popup->isVisible()) {
			m_setFocusOnButtonAfterClosingPopup = true;
			return true;
		}
	}
//	else if (ke->state() == Qt::ControlButton && KStdAccel::shortcut(KStdAccel::Copy).keyCodeQt() == (ke->key()|Qt::CTRL)) {
//	}
	return false;
}

void KexiDBImageBox::setLineWidth( int width )
{
	m_lineWidthChanged = true;
	KexiFrame::setLineWidth(width);
}

void KexiDBImageBox::setPaletteBackgroundColor( const QColor & color )
{
	m_paletteBackgroundColorChanged = true;
	KexiFrame::setPaletteBackgroundColor(color);
}

bool KexiDBImageBox::dropDownButtonVisible() const
{
	return m_dropDownButtonVisible;
}

void KexiDBImageBox::setDropDownButtonVisible( bool set )
{
//! @todo use global default setting for this property
	if (m_dropDownButtonVisible == set)
		return;
	m_dropDownButtonVisible = set;
	if (m_chooser) {
		if (m_dropDownButtonVisible)
			m_chooser->show();
		else
			m_chooser->hide();
	}
}

#include "kexidbimagebox.moc"
