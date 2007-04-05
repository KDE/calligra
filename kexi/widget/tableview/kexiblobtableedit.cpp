/* This file is part of the KDE project
   Copyright (C) 2002 Peter Simonsson <psn@linux.se>
   Copyright (C) 2004, 2006 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexiblobtableedit.h"

#include <stdlib.h>

#include <qdatastream.h>
#include <qfile.h>
#include <qpopupmenu.h>
#include <qtextedit.h>
#include <qlayout.h>
#include <qstatusbar.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qpainter.h>
#include <qtooltip.h>
#include <qapplication.h>
#include <qclipboard.h>
#include <qbuffer.h>

#include <kdebug.h>
#include <ktempfile.h>
#include <kmimetype.h>
#include <kmimemagic.h>
#include <kuserprofile.h>
#include <kservice.h>
#include <kprocess.h>
#include <kopenwith.h>
#include <kurl.h>
#include <karrowbutton.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <kio/job.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kpopupmenu.h>
#include <kstdaccel.h>

#include <kexiutils/utils.h>
#include <widget/utils/kexidropdownbutton.h>
#include <widget/utils/kexicontextmenuutils.h>

//! @internal
class KexiBlobTableEdit::Private
{
public:
	Private()
	 : popup(0)
	 , readOnly(false)
	 , setValueInternalEnabled(true)
	{
	}

	QByteArray value;
	KexiDropDownButton *button;
	QSize totalSize;
	KexiImageContextMenu *popup;
	bool readOnly : 1; //!< cached for slotUpdateActionsAvailabilityRequested() 
	bool setValueInternalEnabled : 1; //!< used to disable KexiBlobTableEdit::setValueInternal()
};

//======================================================

KexiBlobTableEdit::KexiBlobTableEdit(KexiTableViewColumn &column, QWidget *parent)
 : KexiTableEdit(column, parent)
 , d ( new Private() )
{
	setName("KexiBlobTableEdit");
//	m_proc = 0;
//	m_content = 0;
	m_hasFocusableWidget = false;
	d->button = new KexiDropDownButton( parentWidget() /*usually a viewport*/ );
	d->button->hide();
	QToolTip::add(d->button, i18n("Click to show available actions for this cell"));

	d->popup = new KexiImageContextMenu(this);
	d->popup->installEventFilter(this);
	if (column.columnInfo)
		KexiImageContextMenu::updateTitle( d->popup, column.columnInfo->captionOrAliasOrName(),
//! @todo pixmaplabel icon is hardcoded...
			"pixmaplabel" );
	d->button->setPopup( d->popup );

	//force edit requested to start editing... (this will call slotUpdateActionsAvailabilityRequested())
	//connect(d->popup, SIGNAL(aboutToShow()), this, SIGNAL(editRequested()));

	connect(d->popup, SIGNAL(updateActionsAvailabilityRequested(bool&, bool&)), 
		this, SLOT(slotUpdateActionsAvailabilityRequested(bool&, bool&)));

	connect(d->popup, SIGNAL(insertFromFileRequested(const KURL&)),
		this, SLOT(handleInsertFromFileAction(const KURL&)));
	connect(d->popup, SIGNAL(saveAsRequested(const QString&)),
		this, SLOT(handleSaveAsAction(const QString&)));
	connect(d->popup, SIGNAL(cutRequested()),
		this, SLOT(handleCutAction()));
	connect(d->popup, SIGNAL(copyRequested()),
		this, SLOT(handleCopyAction()));
	connect(d->popup, SIGNAL(pasteRequested()),
		this, SLOT(handlePasteAction()));
	connect(d->popup, SIGNAL(clearRequested()),
		this, SLOT(clear()));
	connect(d->popup, SIGNAL(showPropertiesRequested()),
		this, SLOT(handleShowPropertiesAction()));
}

KexiBlobTableEdit::~KexiBlobTableEdit()
{
	delete d;
#if 0
	kdDebug() << "KexiBlobTableEdit: Cleaning up..." << endl;
	if (m_tempFile) {
		m_tempFile->unlink();
		//todo
	}
	delete m_proc;
	m_proc = 0;
	kdDebug() << "KexiBlobTableEdit: Ready." << endl;
#endif
}

//! initializes this editor with \a add value
void KexiBlobTableEdit::setValueInternal(const QVariant& add, bool removeOld)
{
	if (!d->setValueInternalEnabled)
		return;
	if (removeOld)
		d->value = add.toByteArray();
	else //do not add "m_origValue" to "add" as this is QByteArray
		d->value = m_origValue.toByteArray();

#if 0 //todo?
	QByteArray val = m_origValue.toByteArray();
	kdDebug() << "KexiBlobTableEdit: Size of BLOB: " << val.size() << endl;
	m_tempFile = new KTempFile();
	m_tempFile->setAutoDelete(true);
	kdDebug() << "KexiBlobTableEdit: Creating temporary file: " << m_tempFile->name() << endl;
	m_tempFile->dataStream()->writeRawBytes(val.data(), val.size());
	m_tempFile->close();
	delete m_tempFile;
	m_tempFile = 0;

	KMimeMagicResult* mmr = KMimeMagic::self()->findFileType(m_tempFile->name());
	kdDebug() << "KexiBlobTableEdit: Mimetype = " << mmr->mimeType() << endl;

	setViewWidget( new QWidget(this) );
#endif
}

bool KexiBlobTableEdit::valueIsNull()
{
//TODO
	d->value.size();
	return d->value.isEmpty();
}

bool KexiBlobTableEdit::valueIsEmpty()
{
//TODO
	return d->value.isEmpty();
}

QVariant
KexiBlobTableEdit::value()
{
	return d->value;
#if 0
	//todo
//	ok = true;

	if(m_content && m_content->isModified())
	{
		return QVariant(m_content->text());
	}
	QByteArray value;
	QFile f( m_tempFile->name() );
	f.open(IO_ReadOnly);
	QDataStream stream(&f);
	char* data = (char*) malloc(f.size());
	value.resize(f.size());
	stream.readRawBytes(data, f.size());
	value.duplicate(data, f.size());
	free(data);
	kdDebug() << "KexiBlobTableEdit: Size of BLOB: " << value.size() << endl;
	return QVariant(value);
#endif
}

void KexiBlobTableEdit::paintFocusBorders( QPainter *p, QVariant &, int x, int y, int w, int h )
{
//	d->currentEditorWidth = w;
	if (!d->readOnly && w > d->button->width())
		w -= d->button->width();
	p->drawRect(x, y, w, h);
}

void
KexiBlobTableEdit::setupContents( QPainter *p, bool focused, const QVariant& val, 
	QString &txt, int &align, int &x, int &y_offset, int &w, int &h )
{
	Q_UNUSED(focused);
	Q_UNUSED(txt);
	Q_UNUSED(align);

//! @todo optimize: load to m_pixmap, downsize
	QPixmap pixmap;
	x = 0;
	w -= 1; //a place for border
	h -= 1; //a place for border
	if (p && val.canCast(QVariant::ByteArray) && pixmap.loadFromData(val.toByteArray())) {
		KexiUtils::drawPixmap( *p, 0/*lineWidth*/, QRect(x, y_offset, w, h), 
			pixmap, Qt::AlignCenter, true/*scaledContents*/, true/*keepAspectRatio*/);
	}
}

bool KexiBlobTableEdit::cursorAtStart()
{
	return true;
}

bool KexiBlobTableEdit::cursorAtEnd()
{
	return true;
}

void KexiBlobTableEdit::handleInsertFromFileAction(const KURL& url)
{
	if (isReadOnly())
		return;

	QString fileName( url.isLocalFile() ? url.path() : url.prettyURL() );

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
	f.close();
//	m_valueMimeType = KImageIO::mimeType( fileName ); 
	setValueInternal( ba, true );
	signalEditRequested();
	//emit acceptRequested();
}

void KexiBlobTableEdit::handleAboutToSaveAsAction(QString& origFilename, QString& fileExtension, bool& dataIsEmpty)
{
	Q_UNUSED(origFilename);
	Q_UNUSED(fileExtension);
	dataIsEmpty = valueIsEmpty();
//! @todo no fname stored for now
}

void KexiBlobTableEdit::handleSaveAsAction(const QString& fileName)
{
	QFile f(fileName);
	if (!f.open(IO_WriteOnly)) {
		//! @todo err msg
		return;
	}
	f.writeBlock( d->value );
	if (f.status()!=IO_Ok) {
		//! @todo err msg
		f.close();
		return;
	}
	f.close();
}

void KexiBlobTableEdit::handleCutAction()
{
	if (isReadOnly())
		return;
	handleCopyAction();
	clear();
}

void KexiBlobTableEdit::handleCopyAction()
{
	executeCopyAction(d->value);
}

void KexiBlobTableEdit::executeCopyAction(const QByteArray& data)
{
	QPixmap pixmap;
	if (!pixmap.loadFromData(data))
		return;
	qApp->clipboard()->setPixmap(pixmap, QClipboard::Clipboard);
}

void KexiBlobTableEdit::handlePasteAction()
{
	if (isReadOnly())
		return;
	QPixmap pm( qApp->clipboard()->pixmap(QClipboard::Clipboard) );
	QByteArray ba;
	QBuffer buffer( ba );
	buffer.open( IO_WriteOnly );
	if (pm.save( &buffer, "PNG" )) {// write pixmap into ba in PNG format
		setValueInternal( ba, true );
	}
	else {
		setValueInternal( QByteArray(), true );
	}
	signalEditRequested();
	//emit acceptRequested();
	repaintRelatedCell();
}

void KexiBlobTableEdit::clear()
{
	setValueInternal( QByteArray(), true );
	signalEditRequested();
	//emit acceptRequested();
	repaintRelatedCell();
}

void KexiBlobTableEdit::handleShowPropertiesAction()
{
	//! @todo
}

void KexiBlobTableEdit::showFocus( const QRect& r, bool readOnly )
{
	d->readOnly = readOnly; //cache for slotUpdateActionsAvailabilityRequested() 
//	d->button->move( pos().x()+ width(), pos().y() );
	updateFocus( r );
//	d->button->setEnabled(!readOnly);
	if (d->readOnly) 
		d->button->hide();
	else
		d->button->show();
}

void KexiBlobTableEdit::resize(int w, int h)
{
	d->totalSize = QSize(w,h);
	const int addWidth = d->readOnly ? 0 : d->button->width();
	QWidget::resize(w - addWidth, h);
	if (!d->readOnly)
		d->button->resize( h, h );
	m_rightMarginWhenFocused = m_rightMargin + addWidth;
	QRect r( pos().x(), pos().y(), w+1, h+1 );
	r.moveBy(m_scrollView->contentsX(),m_scrollView->contentsY());
	updateFocus( r );
//todo	if (d->popup) {
//todo		d->popup->updateSize();
//todo	}
}

void KexiBlobTableEdit::updateFocus( const QRect& r )
{
	if (!d->readOnly) {
		if (d->button->width() > r.width())
			moveChild(d->button, r.right() + 1, r.top());
		else
			moveChild(d->button, r.right() - d->button->width(), r.top() );
	}
}

void KexiBlobTableEdit::hideFocus()
{
	d->button->hide();
}

QSize KexiBlobTableEdit::totalSize() const
{
	return d->totalSize;
}

void KexiBlobTableEdit::slotUpdateActionsAvailabilityRequested(bool& valueIsNull, bool& valueIsReadOnly)
{
	emit editRequested();
	valueIsNull = this->valueIsNull();
	valueIsReadOnly = d->readOnly || isReadOnly();
}

void KexiBlobTableEdit::signalEditRequested()
{
	d->setValueInternalEnabled = false;
	emit editRequested();
	d->setValueInternalEnabled = true;
}

bool KexiBlobTableEdit::handleKeyPress( QKeyEvent* ke, bool editorActive )
{
	Q_UNUSED(editorActive);

	const int k = ke->key();
	KKey kkey(ke);
	if (!d->readOnly) {
		if ((ke->state()==Qt::NoButton && k==Qt::Key_F4)
			|| (ke->state()==Qt::AltButton && k==Qt::Key_Down)) {
			d->button->animateClick();
			QMouseEvent me( QEvent::MouseButtonPress, QPoint(2,2), Qt::LeftButton, Qt::NoButton );
			QApplication::sendEvent( d->button, &me );
		}
		else if ((ke->state()==NoButton && (k==Qt::Key_F2 || k==Qt::Key_Space || k==Qt::Key_Enter || k==Qt::Key_Return))) {
			d->popup->insertFromFile();
		}
		else
			return false;
	}
	else
		return false;
	return true;
}

bool KexiBlobTableEdit::handleDoubleClick()
{
	d->popup->insertFromFile();
	return true;
}

void KexiBlobTableEdit::handleCopyAction(const QVariant& value, const QVariant& visibleValue)
{
	Q_UNUSED(visibleValue);
	executeCopyAction(value.toByteArray());
}

void KexiBlobTableEdit::handleAction(const QString& actionName)
{
	if (actionName=="edit_paste") {
		d->popup->paste();
	}
	else if (actionName=="edit_cut") {
		emit editRequested();
		d->popup->cut();
	}
}

bool KexiBlobTableEdit::eventFilter( QObject *o, QEvent *e )
{
	if (o == d->popup && e->type()==QEvent::KeyPress) {
		QKeyEvent* ke = static_cast<QKeyEvent*>(e);
		const int state = ke->state();
		const int k = ke->key();
		if (   (state==Qt::NoButton && (k==Qt::Key_Tab || k==Qt::Key_Left || k==Qt::Key_Right))
		    || (state==Qt::ShiftButton && k==Qt::Key_Backtab)
		   )
		{
			d->popup->hide();
	    QApplication::sendEvent( this, ke ); //re-send to move cursor
			return true;
		}
	}
	return false;
}

KEXI_CELLEDITOR_FACTORY_ITEM_IMPL(KexiBlobEditorFactoryItem, KexiBlobTableEdit)

//=======================
// KexiKIconTableEdit class is temporarily here:

//! @internal
class KexiKIconTableEdit::Private
{
public:
	Private()
	 : pixmapCache(17, 17, false)
	{
	}
	//! We've no editor widget that would store current value, so we do this here
	QVariant currentValue;

	QCache<QPixmap> pixmapCache;
};

KexiKIconTableEdit::KexiKIconTableEdit(KexiTableViewColumn &column, QWidget *parent)
 : KexiTableEdit(column, parent)
 , d( new Private() )
{
	setName("KexiKIconTableEdit");
	init();
}

KexiKIconTableEdit::~KexiKIconTableEdit()
{
	delete d;
}

void KexiKIconTableEdit::init()
{
	m_hasFocusableWidget = false;
	d->pixmapCache.setAutoDelete(true);
}
	
void KexiKIconTableEdit::setValueInternal(const QVariant& /*add*/, bool /*removeOld*/)
{
	d->currentValue = m_origValue;
}

bool KexiKIconTableEdit::valueIsNull()
{
	return d->currentValue.isNull();
}

bool KexiKIconTableEdit::valueIsEmpty()
{
	return d->currentValue.isNull();
}

QVariant KexiKIconTableEdit::value()
{
	return d->currentValue;
}

void KexiKIconTableEdit::clear()
{
	d->currentValue = QVariant();
}

bool KexiKIconTableEdit::cursorAtStart()
{
	return true;
}

bool KexiKIconTableEdit::cursorAtEnd()
{
	return true;
}

void KexiKIconTableEdit::setupContents( QPainter *p, bool /*focused*/, const QVariant& val, 
	QString &/*txt*/, int &/*align*/, int &/*x*/, int &y_offset, int &w, int &h  )
{
	Q_UNUSED( y_offset );

#if 0
#ifdef Q_WS_WIN
	y_offset = -1;
#else
	y_offset = 0;
#endif
	int s = QMAX(h - 5, 12);
	s = QMIN( h-3, s );
	s = QMIN( w-3, s );//avoid too large box
	QRect r( QMAX( w/2 - s/2, 0 ) , h/2 - s/2 /*- 1*/, s, s);
	p->setPen(QPen(colorGroup().text(), 1));
	p->drawRect(r);
	if (val.asBool()) {
		p->drawLine(r.x(), r.y(), r.right(), r.bottom());
		p->drawLine(r.x(), r.bottom(), r.right(), r.y());
	}
#endif

	QString key = val.toString();
	QPixmap *pix = 0;
	if (!key.isEmpty() && !(pix = d->pixmapCache[ key ])) {
		//cache pixmap
		QPixmap pm = KGlobal::iconLoader()->loadIcon( key, KIcon::Small, 
			0, KIcon::DefaultState, 0L, true/*canReturnNull*/ );
		if (!pm.isNull()) {
			pix = new QPixmap(pm);
			d->pixmapCache.insert(key, pix);
		}
	}

	if (p && pix) {
		p->drawPixmap( (w-pix->width())/2, (h-pix->height())/2, *pix );
	}
}

void KexiKIconTableEdit::handleCopyAction(const QVariant& value, const QVariant& visibleValue)
{
	Q_UNUSED(value);
	Q_UNUSED(visibleValue);
}

KEXI_CELLEDITOR_FACTORY_ITEM_IMPL(KexiKIconTableEditorFactoryItem, KexiKIconTableEdit)

#include "kexiblobtableedit.moc"
