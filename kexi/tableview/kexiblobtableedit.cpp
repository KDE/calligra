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

#include <kdebug.h>
#include <ktempfile.h>
#include <kmimetype.h>
#include <kmimemagic.h>
#include <ktrader.h>
#include <kservice.h>
#include <kprocess.h>

KexiBlobTableEdit::KexiBlobTableEdit(QByteArray val, QWidget* parent, const char* name)
	: KexiTableEdit(parent, name)
{
	m_value = val;
	kdDebug() << "KexiBlobTableEdit: Size of BLOB: " << m_value.size() << endl;
	m_view = new QWidget(this, "BLOB Edit");
	m_tempFile = new KTempFile();
	m_tempFile->setAutoDelete(true);
	kdDebug() << "KexiBlobTableEdit: Creating temporary file: " << m_tempFile->name() << endl;
	m_tempFile->dataStream()->writeRawBytes(m_value.data(), m_value.size());
	m_tempFile->close();
	
	KMimeMagicResult* mmr = KMimeMagic::self()->findFileType(m_tempFile->name());
	kdDebug() << "KexiBlobTableEdit: Mimetype = " << mmr->mimeType() << endl;
	KTrader::OfferList offers = KTrader::self()->query(mmr->mimeType(), "Type == 'Application'");
	KService::Ptr ptr = offers.first();
	kdDebug() << "KexiBlobTableEdit: Exec = " << ptr->exec().section(' ', 0, 0) << endl;
	m_proc = new KProcess();
	*m_proc << ptr->exec().section(' ', 0, 0);
	*m_proc << m_tempFile->name();
	connect(m_proc, SIGNAL(processExited(KProcess *)), SLOT(slotFinished(KProcess *)));
	m_proc->start();
}

QVariant
KexiBlobTableEdit::value()
{
	return QVariant(m_value);
}

KexiBlobTableEdit::~KexiBlobTableEdit()
{
	kdDebug() << "KexiBlobTableEdit: Cleaning up..." << endl;
	m_tempFile->unlink();
	delete m_proc;
	kdDebug() << "KexiBlobTableEdit: Ready." << endl;
}

void
KexiBlobTableEdit::slotFinished(KProcess* /*p*/)
{
	kdDebug() << "Prorgam is finished!" << endl;
	
	QFile f(m_tempFile->name());
	f.open(IO_ReadOnly);
	QDataStream stream(&f);
	char* data = (char*) malloc(f.size());
	m_value.resize(0);
	m_value.resize(f.size());
	kdDebug() << "KexiBlobTableEdit: Size of BLOB: " << m_value.size() << endl;
	stream.readRawBytes(data, f.size());
	m_value.duplicate(data, f.size());
	free(data);
	kdDebug() << "KexiBlobTableEdit: Size of BLOB: " << m_value.size() << endl;
}

#include "kexiblobtableedit.moc"
