/* This file is part of the KDE project
   Copyright (C) 2002   Peter Simonsson <psn@linux.se>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
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

KexiBlobTableEdit::KexiBlobTableEdit(const QByteArray& val, QWidget* parent, const char* name)
	: KexiTableEdit(parent, name)
{
	kdDebug() << "KexiBlobTableEdit: Size of BLOB: " << val.size() << endl;
	m_tempFile = new KTempFile();
	m_tempFile->setAutoDelete(true);
	kdDebug() << "KexiBlobTableEdit: Creating temporary file: " << m_tempFile->name() << endl;
	m_tempFile->dataStream()->writeRawBytes(val.data(), val.size());
	m_tempFile->close();

	KMimeMagicResult* mmr = KMimeMagic::self()->findFileType(m_tempFile->name());
	kdDebug() << "KexiBlobTableEdit: Mimetype = " << mmr->mimeType() << endl;

	m_proc = 0;
	m_content = 0;

	m_view = new QWidget(this);
	QGridLayout *g = new QGridLayout(m_view);

	if(mmr->mimeType().contains("text/"))
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

}

QVariant
KexiBlobTableEdit::value()
{
	if(m_content && m_content->isModified())
	{
		return QVariant(m_content->text());
	}
	else
	{
		QByteArray value;
		QFile f(m_tempFile->name());
		f.open(IO_ReadOnly);
		QDataStream stream(&f);
		char* data = (char*) malloc(f.size());
		value.resize(f.size());
		stream.readRawBytes(data, f.size());
		value.duplicate(data, f.size());
		free(data);
		kdDebug() << "KexiBlobTableEdit: Size of BLOB: " << value.size() << endl;
		return QVariant(value);
	}
}

KexiBlobTableEdit::~KexiBlobTableEdit()
{
	kdDebug() << "KexiBlobTableEdit: Cleaning up..." << endl;
	m_tempFile->unlink();
	delete m_proc;
	m_proc = 0;
	kdDebug() << "KexiBlobTableEdit: Ready." << endl;
}

void
KexiBlobTableEdit::slotFinished(KProcess* /*p*/)
{
	kdDebug() << "Prorgam is finished!" << endl;


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
	menu->insertItem(i18n("Save To File..."), this, SLOT(saveFile()));

	QPoint pos = mapToGlobal(m_view->pos());
	pos.setY(pos.y() + m_view->height());
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
		KIO::FileCopyJob* job = KIO::file_copy(KURL(file), KURL(m_tempFile->name()), -1, true);
	}
}

void
KexiBlobTableEdit::saveFile()
{
	QString file = KFileDialog::getSaveFileName();

	if(!file.isEmpty())
	{
		KIO::FileCopyJob* job = KIO::file_copy(KURL(m_tempFile->name()), KURL(file), -1, true);
	}
}

#include "kexiblobtableedit.moc"
