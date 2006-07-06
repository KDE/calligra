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

#include <kexiutils/utils.h>

KexiBlobTableEdit::KexiBlobTableEdit(KexiTableViewColumn &column, QScrollView *parent)
 : KexiTableEdit(column, parent,"KexiBlobTableEdit")
{
//	m_proc = 0;
//	m_content = 0;
}

KexiBlobTableEdit::~KexiBlobTableEdit()
{
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
	if (removeOld)
		m_value = add.toByteArray();
	else //do not add "m_origValue" to "add" as this is QByteArray
		m_value = m_origValue.toByteArray();

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

/*js: TODO
	QGridLayout *g = new QGridLayout(m_view);

	if(mmr->mimeType().contains("text/") || val.size() == 0)
	{
		m_content = new QTextEdit(m_view);
		m_content->setTextFormat(PlainText);
		m_content->setText(QString(val));
		g->addWidget(m_content,	0, 0);
	}
	else
	{
		QLabel *l = new QLabel("", this);
//		g->addMultiCellWidget(l, 0, 1, 0, 1);
		g->addMultiCellWidget(l, 0, 2, 0, 1);
		if(mmr->mimeType().contains("image/"))
		{
			//sale the image to the maximal allowed size by optaining the aspect ratio
			QImage pix(m_tempFile->name());
			pix = pix.smoothScale(m_view->width(), l->height(), QImage::ScaleMin);

			l->setScaledContents(true);
			l->setPixmap(QPixmap(pix));
		}
		else
		{
			l->setPixmap(KMimeType::pixmapForURL(KURL(m_tempFile->name())));
		}

//		QLabel *l = new QLabel(this);
		QLabel *lsize = new QLabel(i18n("Size:"), this);
		QLabel *size = new QLabel(QString::number(val.size()) + " bytes", this);
		g->addMultiCellWidget(lsize, 2, 2, 0, 0);
		g->addMultiCellWidget(size, 2, 2, 1, 1);
	}

	KArrowButton *menu = new KArrowButton(m_view, Qt::DownArrow, "Menu button");
	g->addWidget(menu, 3, 0);
	connect(menu, SIGNAL(clicked()), SLOT(menu()));
*/
}

bool KexiBlobTableEdit::valueIsNull()
{
//TODO
	return m_value.isEmpty();
}

bool KexiBlobTableEdit::valueIsEmpty()
{
//TODO
	return m_value.isEmpty();
}

QVariant
KexiBlobTableEdit::value()
{
	return m_value;
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

void
KexiBlobTableEdit::setupContents( QPainter *p, bool focused, const QVariant& val, 
	QString &txt, int &align, int &x, int &y_offset, int &w, int &h )
{
//! @todo optimize: load to m_pixmap, downsize
	QPixmap pixmap;
	x = 0;
	w -= 1; //a place for border
	h -= 1; //a place for border
	if (val.canCast(QVariant::ByteArray) && pixmap.loadFromData(val.toByteArray())) {
		KexiUtils::drawPixmap( *p, 0/*lineWidth*/, QRect(x, y_offset, w, h), 
			pixmap, Qt::AlignCenter, true/*scaledContents*/, true/*keepAspectRatio*/);
	}
}

/*todo
void
KexiBlobTableEdit::slotFinished(KProcess* p)
{
	kdDebug() << "Program is finished!" << endl;

	// No need for m_proc now that the app has exited
	delete m_proc;
	m_proc = 0;
}

QString
KexiBlobTableEdit::openWithDlg(const QString& file)
{
	KURL::List ul;
	KURL url;
	url.setPath(file);
	ul.append(url);
	QString exec = QString::null;

	KOpenWithDlg* dlg = new KOpenWithDlg(ul, this);

	if(dlg->exec() == QDialog::Accepted)
	{
		exec = dlg->text().section(' ', 0, 0);
	}

	delete dlg;
	dlg = 0;

	return exec;
}

void
KexiBlobTableEdit::execute(const QString& app, const QString& file)
{
	kdDebug() << "KexiBlobTableEdit: App = " << app << "File = " << file << endl;

	// only execute if there isn't any other app already running
	if(!m_proc)
	{
		m_proc = new KProcess();
		*m_proc << app;
		*m_proc << file;
		connect(m_proc, SIGNAL(processExited(KProcess *)), SLOT(slotFinished(KProcess *)));
		m_proc->start();
	}
}

void
KexiBlobTableEdit::open()
{
	KMimeMagicResult* mmr = KMimeMagic::self()->findFileType(m_tempFile->name());
	kdDebug() << "KexiBlobTableEdit: Mimetype = " << mmr->mimeType() << endl;
	KService::Ptr ptr = KServiceTypeProfile::preferredService(mmr->mimeType(), "Application");
	QString exec;

	if(!ptr.data())
	{
		exec = openWithDlg(m_tempFile->name());
	}
	else
	{
		exec = ptr->exec().section(' ', 0, 0);
	}

	if(!exec.isEmpty())
	{
		execute(exec, m_tempFile->name());
	}
}

void
KexiBlobTableEdit::openWith()
{
	QString exec = openWithDlg(m_tempFile->name());

	if(!exec.isEmpty())
	{
		execute(exec, m_tempFile->name());
	}
}

void
KexiBlobTableEdit::menu()
{
	QPopupMenu* menu = new QPopupMenu(this, "BLOB Menu");

	menu->insertItem(i18n("Open"), this, SLOT(open()));
	menu->insertItem(i18n("Open With..."), this, SLOT(openWith()));
	menu->insertSeparator();
	menu->insertItem(i18n("Load From File..."), this, SLOT(loadFile()));
	menu->insertItem(i18n("Save to File..."), this, SLOT(saveFile()));

	QPoint pos = mapToGlobal(widget()->pos());
	pos.setY(pos.y() + widget()->height());
	menu->move(pos);
	menu->exec();

	delete menu;
	menu = 0;
}

void
KexiBlobTableEdit::loadFile()
{
	QString file = KFileDialog::getOpenFileName();

	if(!file.isEmpty())
	{
		(void) KIO::file_copy(KURL(file), KURL(m_tempFile->name()), -1, true);
	}
}

void
KexiBlobTableEdit::saveFile()
{
	QString file = KFileDialog::getSaveFileName();

	if(!file.isEmpty())
	{
		(void)KIO::file_copy(KURL(m_tempFile->name()), KURL(file), -1, true);
	}
}*/

bool KexiBlobTableEdit::cursorAtStart()
{
	return true;
}

bool KexiBlobTableEdit::cursorAtEnd()
{
	return true;
}

void KexiBlobTableEdit::clear()
{
	//TODO??
}

KEXI_CELLEDITOR_FACTORY_ITEM_IMPL(KexiBlobEditorFactoryItem, KexiBlobTableEdit)

//=======================
//This class is temporarily here:


KexiKIconTableEdit::KexiKIconTableEdit(KexiTableViewColumn &column, QScrollView *parent)
 : KexiTableEdit(column, parent, "KexiKIconTableEdit")
 , m_pixmapCache(17, 17, false)
{
	init();
}

KexiKIconTableEdit::~KexiKIconTableEdit()
{
}

void KexiKIconTableEdit::init()
{
	m_hasFocusableWidget = false;
	m_pixmapCache.setAutoDelete(true);
}
	
void KexiKIconTableEdit::setValueInternal(const QVariant& /*add*/, bool /*removeOld*/)
{
	m_currentValue = m_origValue;
}

bool KexiKIconTableEdit::valueIsNull()
{
	return m_currentValue.isNull();
}

bool KexiKIconTableEdit::valueIsEmpty()
{
	return m_currentValue.isNull();
}

QVariant KexiKIconTableEdit::value()
{
	return m_currentValue;
}

void KexiKIconTableEdit::clear()
{
	m_currentValue = QVariant();
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
	if (!key.isEmpty() && !(pix = m_pixmapCache[ key ])) {
		//cache pixmap
		QPixmap p = KGlobal::iconLoader()->loadIcon( key, KIcon::Small, 
			0, KIcon::DefaultState, 0L, true/*canReturnNull*/ );
		if (!p.isNull()) {
			pix = new QPixmap(p);
			m_pixmapCache.insert(key, pix);
		}
	}

	if (pix) {
		p->drawPixmap( (w-pix->width())/2, (h-pix->height())/2, *pix );
	}
}

KEXI_CELLEDITOR_FACTORY_ITEM_IMPL(KexiKIconTableEditorFactoryItem, KexiKIconTableEdit)

#include "kexiblobtableedit.moc"
