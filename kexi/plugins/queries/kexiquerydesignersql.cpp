/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
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

#include <qsplitter.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qtimer.h>

#include <kapplication.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kiconloader.h>

#include <kexiutils/utils.h>
#include <kexidb/driver.h>
#include <kexidb/connection.h>
#include <kexidb/parser/parser.h>

#include <kexiproject.h>
#include <keximainwindow.h>

#include "kexiquerydesignersqleditor.h"
#include "kexiquerydesignersqlhistory.h"
#include "kexiquerydesignersql.h"
#include "kexiquerypart.h"

#include "kexisectionheader.h"


static bool compareSQL(const QString& sql1, const QString& sql2)
{
	//TODO: use reformatting functions here
	return sql1.stripWhiteSpace()==sql2.stripWhiteSpace();
}

//===================

//! @internal
class KexiQueryDesignerSQLView::Private
{
	public:
		Private() :
		   history(0)
		 , historyHead(0)
		 , statusPixmapOk( DesktopIcon("button_ok") )
		 , statusPixmapErr( DesktopIcon("button_cancel") )
		 , statusPixmapInfo( DesktopIcon("messagebox_info") )
		 , parsedQuery(0)
		 , heightForStatusMode(-1)
		 , heightForHistoryMode(-1)
		 , eventFilterForSplitterEnabled(true)
		 , justSwitchedFromNoViewMode(false)
		 , slotTextChangedEnabled(true)
		{
		}
		KexiQueryDesignerSQLEditor *editor;
		KexiQueryDesignerSQLHistory *history;
		QLabel *pixmapStatus, *lblStatus;
		QHBox *status_hbox;
		QVBox *history_section;
		KexiSectionHeader *head, *historyHead;
		QPixmap statusPixmapOk, statusPixmapErr, statusPixmapInfo;
		QSplitter *splitter;
		KToggleAction *action_toggle_history;
		//! For internal use, this pointer is usually copied to TempData structure, 
		//! when switching out of this view (then it's cleared).
		KexiDB::QuerySchema *parsedQuery;
		//! For internal use, statement passed in switching to this view
		QString origStatement;
		//! needed to remember height for both modes, between switching
		int heightForStatusMode, heightForHistoryMode;
		//! helper for slotUpdateMode()
		bool action_toggle_history_was_checked : 1;
		//! helper for eventFilter()
		bool eventFilterForSplitterEnabled : 1;
		//! helper for beforeSwitchTo()
		bool justSwitchedFromNoViewMode : 1;
		//! helper for slotTextChanged()
		bool slotTextChangedEnabled : 1;
};

//===================

KexiQueryDesignerSQLView::KexiQueryDesignerSQLView(KexiMainWindow *mainWin, QWidget *parent, const char *name)
 : KexiViewBase(mainWin, parent, name)
 , d( new Private() )
{
	d->splitter = new QSplitter(this);
	d->splitter->setOrientation(Vertical);
	d->head = new KexiSectionHeader(i18n("SQL Query Text"), Vertical, d->splitter);
	d->editor = new KexiQueryDesignerSQLEditor(mainWin, d->head, "sqle");
//	d->editor->installEventFilter(this);//for keys
	connect(d->editor, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));
	addChildView(d->editor);
	setViewWidget(d->editor);
	d->splitter->setFocusProxy(d->editor);
	setFocusProxy(d->editor);

	d->history_section = new QVBox(d->splitter);

	d->status_hbox = new QHBox(d->history_section);
	d->status_hbox->installEventFilter(this);
	d->splitter->setResizeMode(d->history_section, QSplitter::KeepSize);
	d->status_hbox->setSpacing(0);
	d->pixmapStatus = new QLabel(d->status_hbox);
	d->pixmapStatus->setFixedWidth(d->statusPixmapOk.width()*3/2);
	d->pixmapStatus->setAlignment(AlignHCenter | AlignTop);
	d->pixmapStatus->setMargin(d->statusPixmapOk.width()/4);
	d->pixmapStatus->setPaletteBackgroundColor( palette().active().color(QColorGroup::Base) );

	d->lblStatus = new QLabel(d->status_hbox);
	d->lblStatus->setAlignment(AlignLeft | AlignTop | WordBreak);
	d->lblStatus->setMargin(d->statusPixmapOk.width()/4);
	d->lblStatus->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Expanding );
	d->lblStatus->resize(d->lblStatus->width(),d->statusPixmapOk.width()*3);
	d->lblStatus->setPaletteBackgroundColor( palette().active().color(QColorGroup::Base) );

	QHBoxLayout *b = new QHBoxLayout(this);
	b->addWidget(d->splitter);

	plugSharedAction("querypart_check_query", this, SLOT(slotCheckQuery())); 
	plugSharedAction("querypart_view_toggle_history", this, SLOT(slotUpdateMode()));
	d->action_toggle_history = static_cast<KToggleAction*>( sharedAction( "querypart_view_toggle_history" ) );

	d->historyHead = new KexiSectionHeader(i18n("SQL Query History"), Vertical, d->history_section);
	d->historyHead->installEventFilter(this);
	d->history = new KexiQueryDesignerSQLHistory(d->historyHead, "sql_history");

	static const QString msg_back = i18n("Back to Selected Query");
	static const QString msg_clear = i18n("Clear History");
	d->historyHead->addButton("select_item", msg_back, this, SLOT(slotSelectQuery()));
	d->historyHead->addButton("editclear", msg_clear, d->history, SLOT(clear()));
	d->history->popupMenu()->insertItem(SmallIcon("select_item"), msg_back, this, SLOT(slotSelectQuery()));
	d->history->popupMenu()->insertItem(SmallIcon("editclear"), msg_clear, d->history, SLOT(clear()));
	connect(d->history, SIGNAL(currentItemDoubleClicked()), this, SLOT(slotSelectQuery()));

	d->heightForHistoryMode = -1; //height() / 2;
	//d->historyHead->hide();
	d->action_toggle_history_was_checked = !d->action_toggle_history->isChecked(); //to force update
	slotUpdateMode();
	slotCheckQuery();
}

KexiQueryDesignerSQLView::~KexiQueryDesignerSQLView()
{
	delete d;
}

KexiQueryDesignerSQLEditor *KexiQueryDesignerSQLView::editor() const
{
	return d->editor;
}

void KexiQueryDesignerSQLView::setStatusOk()
{
	d->pixmapStatus->setPixmap(d->statusPixmapOk);
	setStatusText("<h2>"+i18n("The query is correct")+"</h2>");
	d->history->addEvent(d->editor->text().stripWhiteSpace(), true, QString::null);
}

void KexiQueryDesignerSQLView::setStatusError(const QString& msg)
{
	d->pixmapStatus->setPixmap(d->statusPixmapErr);
	setStatusText("<h2>"+i18n("The query is incorrect")+"</h2><p>"+msg+"</p>");
	d->history->addEvent(d->editor->text().stripWhiteSpace(), false, msg);
}

void KexiQueryDesignerSQLView::setStatusEmpty()
{
	d->pixmapStatus->setPixmap(d->statusPixmapInfo);
	setStatusText(i18n("Please enter your query and execute \"Check query\" function to verify it."));
}

void KexiQueryDesignerSQLView::setStatusText(const QString& text)
{
	if (!d->action_toggle_history->isChecked()) {
		QSimpleRichText rt(text, d->lblStatus->font());
		rt.setWidth(d->lblStatus->width());
		QValueList<int> sz = d->splitter->sizes();
		const int newHeight = rt.height()+d->lblStatus->margin()*2;
		if (sz[1]<newHeight) {
			sz[1] = newHeight;
			d->splitter->setSizes(sz);
		}
		d->lblStatus->setText(text);
	}
}

tristate
KexiQueryDesignerSQLView::beforeSwitchTo(int mode, bool &dontStore)
{
//TODO
	dontStore = true;
	if (mode==Kexi::DesignViewMode || mode==Kexi::DataViewMode) {
		QString sqlText = d->editor->text().stripWhiteSpace();
		KexiQueryPart::TempData * temp = tempData();
		if (sqlText.isEmpty()) {
			//special case: empty SQL text
			if (temp->query()) {
				temp->queryChangedInPreviousView = true; //query changed
				temp->setQuery(0);
//				delete temp->query; //safe?
//				temp->query = 0;
			}
		}
		else {
			const bool designViewWasVisible = parentDialog()->viewForMode(mode)!=0;
			//should we check SQL text?
			if (designViewWasVisible 
				&& !d->justSwitchedFromNoViewMode //unchanged, but we should check SQL text
				&& compareSQL(d->origStatement, d->editor->text())) {
				//statement unchanged! - nothing to do
				temp->queryChangedInPreviousView = false;
			}
			else {
				//yes: parse SQL text
				if (!slotCheckQuery()) {
					if (KMessageBox::No==KMessageBox::warningYesNo(this, "<p>"+i18n("The query you entered is incorrect.")
						+"</p><p>"+i18n("Do you want to cancel any changes made to this SQL text?")+"</p>"
						+"</p><p>"+i18n("Answering \"No\" allows you to make corrections.")+"</p>"))
					{
						return cancelled;
					}
					//do not change original query - it's invalid
					temp->queryChangedInPreviousView = false;
					//this view is no longer _just_ switched from "NoViewMode"
					d->justSwitchedFromNoViewMode = false;
					return true;
				}
				//this view is no longer _just_ switched from "NoViewMode"
				d->justSwitchedFromNoViewMode = false;
				//replace old query schema with new one
				temp->setQuery( d->parsedQuery ); //this wil also delete temp->query()
//				delete temp->query; //safe?
//				temp->query = d->parsedQuery;
				d->parsedQuery = 0;
				temp->queryChangedInPreviousView = true;
			}
		}
	}

	//TODO
	/*
	if (d->doc) {
		KexiDB::Parser *parser = new KexiDB::Parser(mainWin()->project()->dbConnection());
		parser->parse(getQuery());
		d->doc->setSchema(parser->select());

		if(parser->operation() == KexiDB::Parser::OP_Error)
		{
			d->history->addEvent(getQuery(), false, parser->error().error());
			kdDebug() << "KexiQueryDesignerSQLView::beforeSwitchTo(): syntax error!" << endl;
			return false;
		}
		delete parser;
	}

	setDirty(true);*/
//	if (parentDialog()->hasFocus())
	d->editor->setFocus();
	return true;
}

tristate
KexiQueryDesignerSQLView::afterSwitchFrom(int mode)
{
	kdDebug() << "KexiQueryDesignerSQLView::afterSwitchFrom()" << endl;
//	if (mode==Kexi::DesignViewMode || mode==Kexi::DataViewMode) {
	if (mode==Kexi::NoViewMode) {
		//User opened text view _directly_. 
		//This flag is set to indicate for beforeSwitchTo() that even if text has not been changed,
		//SQL text should be invalidated.
		d->justSwitchedFromNoViewMode = true;
	}
	KexiQueryPart::TempData * temp = tempData();
	KexiDB::QuerySchema *query = temp->query();
	if (!query) {//try to just get saved schema, instead of temporary one
		query = dynamic_cast<KexiDB::QuerySchema *>(parentDialog()->schemaData());
	}

	if (mode!=0/*failure only if it is switching from prev. view*/ && !query) {
		//TODO msg
		return false;
	}

	if (!query) {
		//no valid query schema delivered: just load sql text, no matter if it's valid
		if (!loadDataBlock( d->origStatement, "sql", true /*canBeEmpty*/ ))
			return false;
	}
	else {
	// Use query with Kexi keywords (but not driver-specific keywords) escaped.
		temp->setQuery( query );
//		temp->query = query;
		KexiDB::Connection* conn = mainWin()->project()->dbConnection();
		KexiDB::Connection::SelectStatementOptions options;
		options.identifierEscaping = KexiDB::Driver::EscapeKexi;
		d->origStatement = conn->selectStatement(*query, options).stripWhiteSpace();
	}

	d->slotTextChangedEnabled = false;
	 d->editor->setText( d->origStatement );
	d->slotTextChangedEnabled = true;
	QTimer::singleShot(100, d->editor, SLOT(setFocus()));
	return true;
}

QString
KexiQueryDesignerSQLView::sqlText() const
{
	return d->editor->text();
}

bool KexiQueryDesignerSQLView::slotCheckQuery()
{
	QString sqlText( d->editor->text().stripWhiteSpace() );
	if (sqlText.isEmpty()) {
		delete d->parsedQuery;
		d->parsedQuery = 0;
		setStatusEmpty();
		return true;
	}

	kdDebug() << "KexiQueryDesignerSQLView::slotCheckQuery()" << endl;
	//KexiQueryPart::TempData * temp = tempData();
	KexiDB::Parser *parser = mainWin()->project()->sqlParser();
	const bool ok = parser->parse( sqlText );
	delete d->parsedQuery;
	d->parsedQuery = parser->query();
	if (!d->parsedQuery || !ok || !parser->error().type().isEmpty()) {
		KexiDB::ParserError err = parser->error();
		setStatusError(err.error());
		d->editor->jump(err.at());
		delete d->parsedQuery;
		d->parsedQuery = 0;
		return false;
	}

	setStatusOk();
	return true;
}

void KexiQueryDesignerSQLView::slotUpdateMode()
{
	if (d->action_toggle_history->isChecked() == d->action_toggle_history_was_checked)
		return;

	d->eventFilterForSplitterEnabled = false;

	QValueList<int> sz = d->splitter->sizes();
	d->action_toggle_history_was_checked = d->action_toggle_history->isChecked();
	int heightToSet = -1;
	if (d->action_toggle_history->isChecked()) {
		d->status_hbox->hide();
		d->historyHead->show();
		d->history->show();
		if (d->heightForHistoryMode==-1)
			d->heightForHistoryMode = m_dialog->height() / 2;
		heightToSet = d->heightForHistoryMode;
		d->heightForStatusMode = sz[1]; //remember
	}
	else {
		if (d->historyHead)
			d->historyHead->hide();
		d->status_hbox->show();
		if (d->heightForStatusMode>=0) {
			heightToSet = d->heightForStatusMode;
		} else {
			d->heightForStatusMode = d->status_hbox->height();
		}
		if (d->heightForHistoryMode>=0)
			d->heightForHistoryMode = sz[1];
	}
	
	if (heightToSet>=0) {
		sz[1] = heightToSet;
		d->splitter->setSizes(sz);
	}
	d->eventFilterForSplitterEnabled = true;
	slotCheckQuery();
}

void KexiQueryDesignerSQLView::slotTextChanged()
{
	if (!d->slotTextChangedEnabled)
		return;
	setDirty(true);
	setStatusEmpty();
}

bool KexiQueryDesignerSQLView::eventFilter( QObject *o, QEvent *e )
{
	if (d->eventFilterForSplitterEnabled) {
		if (e->type()==QEvent::Resize && o && o==d->historyHead && d->historyHead->isVisible()) {
			d->heightForHistoryMode = d->historyHead->height();
		}
		else if (e->type()==QEvent::Resize && o && o==d->status_hbox && d->status_hbox->isVisible()) {
			d->heightForStatusMode = d->status_hbox->height();
		}
	}
	return KexiViewBase::eventFilter(o, e);
}

void KexiQueryDesignerSQLView::updateActions(bool activated)
{
	if (activated) {
		slotUpdateMode();
	}
	setAvailable("querypart_check_query", true);
	setAvailable("querypart_view_toggle_history", true);
	KexiViewBase::updateActions(activated);
}

void KexiQueryDesignerSQLView::slotSelectQuery()
{
	QString sql = d->history->selectedStatement();
	if (!sql.isEmpty()) {
		d->editor->setText( sql );
	}
}

KexiQueryPart::TempData *
KexiQueryDesignerSQLView::tempData() const
{	
	return dynamic_cast<KexiQueryPart::TempData*>(parentDialog()->tempData());
}

KexiDB::SchemaData*
KexiQueryDesignerSQLView::storeNewData(const KexiDB::SchemaData& sdata, bool &cancel)
{
	Q_UNUSED( cancel );

	//here: we won't store query layout: it will be recreated 'by hand' in GUI Query Editor
	bool queryOK = slotCheckQuery();
	bool ok = true;
	KexiDB::SchemaData* query = 0;
	if (queryOK) {
		//query is ok
		if (d->parsedQuery) {
			query = d->parsedQuery; //will be returned, so: don't keep it
			d->parsedQuery = 0;
		}
		else {//empty query
			query = new KexiDB::SchemaData(); //just empty
		}

		(KexiDB::SchemaData&)*query = sdata; //copy main attributes
		ok = m_mainWin->project()->dbConnection()->storeObjectSchemaData( *query, true /*newObject*/ );
		if (ok) {
			m_dialog->setId( query->id() );
			ok = storeDataBlock( d->editor->text(), "sql" );
		}
	}
	else {
		//query is not ok
//#if 0
	//TODO: allow saving invalid queries
	//TODO: just ask this question:
		query = new KexiDB::SchemaData(); //just empty

		ok = (KMessageBox::questionYesNo(this, i18n("Do you want to save invalid query?"),
			0, KStdGuiItem::yes(), KStdGuiItem::no(), "askBeforeSavingInvalidQueries"/*config entry*/)==KMessageBox::Yes);
		if (ok) {
			(KexiDB::SchemaData&)*query = sdata; //copy main attributes
			ok = m_mainWin->project()->dbConnection()->storeObjectSchemaData( *query, true /*newObject*/ );
		}
		if (ok) {
			m_dialog->setId( query->id() );
			ok = storeDataBlock( d->editor->text(), "sql" );
		}
//#else
		//ok = false;
//#endif
	}
	if (!ok) {
		delete query;
		query = 0;
	}
	return query;
}

tristate KexiQueryDesignerSQLView::storeData(bool dontAsk)
{
	tristate res = KexiViewBase::storeData(dontAsk);
	if (~res)
		return res;
	if (res == true) {
		res = storeDataBlock( d->editor->text(), "sql" );
#if 0
		bool queryOK = slotCheckQuery();
		if (queryOK) {
			res = storeDataBlock( d->editor->text(), "sql" );
		}
		else {
			//query is not ok
			//TODO: allow saving invalid queries
			//TODO: just ask this question:
			res = false;
		}
#endif
	}
	if (res == true) {
		QString empty_xml;
		res = storeDataBlock( empty_xml, "query_layout" ); //clear
	}
	if (!res)
		setDirty(true);
	return res;
}


/*void KexiQueryDesignerSQLView::slotHistoryHeaderButtonClicked(const QString& buttonIdentifier)
{
	if (buttonIdentifier=="select_query") {
		slotSelectQuery();
	}
	else if (buttonIdentifier=="clear_history") {
		d->history->clear();
	}
}*/

#include "kexiquerydesignersql.moc"

