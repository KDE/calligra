/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#include <qsplitter.h>
#include <qlayout.h>
#include <qhbox.h>

#include <kdebug.h>
#include <kmessagebox.h>
#include <kiconloader.h>

#include <kexidb/connection.h>
#include <kexidb/parser/parser.h>

#include <kexiproject.h>
#include <keximainwindow.h>

#include "kexiquerydesigner.h"
#include "kexiquerydesignersqleditor.h"
#include "kexiquerydesignersqlhistory.h"
#include "kexiquerydesignersql.h"
#include "kexiquerypart.h"

#include "kexisectionheader.h"


static bool compareSQL(const QString& sql1, const QString& sql2)
{
	//TODO: use reformatting functions here
	return sql1!=sql2;
}

//===================

KexiQueryDesignerSQLView::KexiQueryDesignerSQLView(KexiMainWindow *mainWin, QWidget *parent, const char *name)
 : KexiViewBase(mainWin, parent, name)
 , m_statusPixmapOk( DesktopIcon("button_ok") )
 , m_statusPixmapErr( DesktopIcon("button_cancel") )
 , m_statusPixmapInfo( DesktopIcon("info") )
{
	m_splitter = new QSplitter(this);
	m_splitter->setOrientation(Vertical);
//	m_history = new KexiQueryDesignerSQLHistory(l, "sqlh");
	m_head = new KexiSectionHeader(i18n("SQL Query Text"), Vertical, m_splitter);
	m_editor = new KexiQueryDesignerSQLEditor(mainWin, m_head, "sqle");
	connect(m_editor, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));
	addChildView(m_editor);
	setViewWidget(m_editor);
	m_splitter->setFocusProxy(m_editor);
	setFocusProxy(m_editor);

	QHBox *hbox = new QHBox(m_splitter);
	m_splitter->setResizeMode(hbox, QSplitter::KeepSize);
	hbox->setSpacing(0);
	m_pixmapStatus = new QLabel(hbox);
	m_pixmapStatus->setFixedWidth(m_statusPixmapOk.width()*3/2);
	m_pixmapStatus->setAlignment(AlignHCenter | AlignTop);
	m_pixmapStatus->setMargin(m_statusPixmapOk.width()/4);
	m_pixmapStatus->setPaletteBackgroundColor( palette().active().color(QColorGroup::Base) );

	m_lblStatus = new QLabel(hbox);
	m_lblStatus->setAlignment(AlignLeft | AlignTop);
	m_lblStatus->setMargin(m_statusPixmapOk.width()/4);
	m_lblStatus->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Expanding );
	m_lblStatus->resize(m_lblStatus->width(),m_statusPixmapOk.width()*3);
	m_lblStatus->setPaletteBackgroundColor( palette().active().color(QColorGroup::Base) );
	setStatusOk();

	QHBoxLayout *b = new QHBoxLayout(this);
	b->addWidget(m_splitter);

	plugSharedAction("querypart_check_query", this, SLOT(slotCheckQuery())); 
//	connect(parent, SIGNAL(queryExecuted(QString, bool, const QString &)), m_history, SLOT(addEvent(QString, bool, const QString &)));
//	connect(m_history, SIGNAL(editRequested(const QString &)), m_editor, SLOT(setText(const QString &)));

//TODO	m_history->setHistory(doc->history());

//	connect(m_editor, SIGNAL(execQ()), parent, SLOT(fastQuery()));
}

KexiQueryDesignerSQLView::~KexiQueryDesignerSQLView()
{
}

void KexiQueryDesignerSQLView::setStatusOk()
{
	m_pixmapStatus->setPixmap(m_statusPixmapOk);
	setStatusText("<h2>"+i18n("The query is correct")+"</h2>");
}

void KexiQueryDesignerSQLView::setStatusError(const QString& msg)
{
	m_pixmapStatus->setPixmap(m_statusPixmapErr);
	setStatusText("<h2>"+i18n("The query is incorrect")+"</h2><p>"+msg+"</p>");
}

void KexiQueryDesignerSQLView::setStatusEmpty()
{
	m_pixmapStatus->setPixmap(m_statusPixmapInfo);
	setStatusText(i18n("Please enter your query and execute \"Check query\" function to verify it."));
}

void KexiQueryDesignerSQLView::setStatusText(const QString& text)
{
	QSimpleRichText rt(text, m_lblStatus->font());
	rt.setWidth(m_lblStatus->width());
//	m_lblStatus->resize(m_lblStatus->width(), rt.height()+m_lblStatus->margin()*8);
	QValueList<int> sz = m_splitter->sizes();
	const int newHeight = rt.height()+m_lblStatus->margin()*2;
	if (sz[1]<newHeight) {
		sz[1] = newHeight;
		m_splitter->setSizes(sz);
	}
	m_lblStatus->setText(text);
//	m_lblStatus->adjustSize();
}

bool
KexiQueryDesignerSQLView::beforeSwitchTo(int mode, bool &cancelled, bool &dontStore)
{
	return true;
//TODO
	if (mode==Kexi::DesignViewMode || mode==Kexi::DataViewMode) {
		//parse SQL text
		KexiQueryPart::TempData * temp = static_cast<KexiQueryPart::TempData*>(parentDialog()->tempData());
		KexiDB::Parser *parser = mainWin()->project()->sqlParser();
		parser->parse( m_editor->text() );
		KexiDB::QuerySchema *query = parser->query();
		if (!query) {
			KexiDB::ParserError err = parser->error();
			KMessageBox::information(this, err.error());
			return false;
		}
		temp->query = query;
	}

	//TODO
	/*
	if (m_doc) {
		KexiDB::Parser *parser = new KexiDB::Parser(mainWin()->project()->dbConnection());
		parser->parse(getQuery());
		m_doc->setSchema(parser->select());

		if(parser->operation() == KexiDB::Parser::OP_Error)
		{
			m_history->addEvent(getQuery(), false, parser->error().error());
			kdDebug() << "KexiQueryDesignerSQLView::beforeSwitchTo(): syntax error!" << endl;
			return false;
		}
		delete parser;
	}

	setDirty(true);*/
	return true;
}

bool
KexiQueryDesignerSQLView::afterSwitchFrom(int mode, bool &cancelled)
{
	kdDebug() << "KexiQueryDesignerSQLView::afterSwitchFrom()" << endl;
	if (mode==Kexi::DesignViewMode || mode==Kexi::DataViewMode) {
		KexiQueryPart::TempData * temp = static_cast<KexiQueryPart::TempData*>(parentDialog()->tempData());
		if (!temp->query) {
			//TODO msg
			return false;
		}
		m_editor->setText( mainWin()->project()->dbConnection()->selectStatement( *temp->query ) );
	}

/*	if (m_doc && m_doc->schema()) {
		m_editor->setText(m_doc->schema()->connection()->selectStatement(*m_doc->schema()));
	}*/
	return true;
}

QString
KexiQueryDesignerSQLView::sqlText()
{
	return m_editor->text();
}

void KexiQueryDesignerSQLView::slotCheckQuery()
{
	kdDebug() << "KexiQueryDesignerSQLView::slotCheckQuery()" << endl;
	KexiQueryPart::TempData * temp = static_cast<KexiQueryPart::TempData*>(parentDialog()->tempData());
	KexiDB::Parser *parser = mainWin()->project()->sqlParser();
	parser->parse( m_editor->text() );
	KexiDB::QuerySchema *query = parser->query();
	if (!query || !parser->error().type().isEmpty()) {
		KexiDB::ParserError err = parser->error();
		setStatusError(err.error());
		m_editor->jump(err.at());
	}
	else {
		setStatusOk();
	}
}

void KexiQueryDesignerSQLView::slotTextChanged()
{
	setDirty(true);
	setStatusEmpty();
}

#include "kexiquerydesignersql.moc"

