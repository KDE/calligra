/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004-2007 Jaroslaw Staniek <js@iidea.pl>

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

#include <qpainter.h>
#include <qclipboard.h>
#include <qregexp.h>
#include <QMouseEvent>
#include <q3simplerichtext.h>

#include <kmenu.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <kglobalsettings.h>
#include <kapplication.h>
#include <KIcon>

#include "kexiquerydesignersqlhistory.h"

KexiQueryDesignerSQLHistory::KexiQueryDesignerSQLHistory(QWidget *parent)
 : Q3ScrollView(parent)
{
/*Qt 3	QPalette pal(viewport()->palette());
	pal.setBrush(QPalette::Active, QPalette::Background, QBrush(Qt::white));
	viewport()->setPalette(pal);*/
	viewport()->setBackgroundRole(QPalette::Base);

	m_selected = 0;
	m_prevColorGroup = QPalette::Active;
	m_history = new History();

	m_popup = new KMenu(this);
	m_popup->addAction(KIcon("edit-copy"), i18n("Copy to Clipboard"),
		this, SLOT(slotToClipboard()));
	installEventFilter(this);
}

KexiQueryDesignerSQLHistory::~KexiQueryDesignerSQLHistory()
{
	qDeleteAll(*m_history);
	m_history->clear();
}

void KexiQueryDesignerSQLHistory::drawContents(QPainter *p, int cx, int cy, int cw, int ch)
{
	QRect clipping(cx, cy, cw, ch);

	qreal y = cy;
	foreach (HistoryEntry *historyEntry, *m_history) {
//		if (clipping.intersects(historyEntry->geometry((int)y, visibleWidth(), fontMetrics()))) {
			//Qt3 p->saveWorldMatrix();
			p->translate(0.0, y);
			historyEntry->drawItem(p, visibleWidth());
			//Qt3 p->restoreWorldMatrix();
			p->resetTransform();
//		}
		y += qreal( historyEntry->geometry(y, visibleWidth(), fontMetrics()).height() ) + 5.0;
	}
}

void KexiQueryDesignerSQLHistory::contentsMousePressEvent(QMouseEvent * e)
{
	int y = 0;
	HistoryEntry *popupHistory = 0;
	int pos;
	foreach (HistoryEntry *historyEntry, *m_history) {
		if (historyEntry->isSelected()) {
			//clear
			historyEntry->setSelected(false);
			updateContents(historyEntry->geometry(y, visibleWidth(), fontMetrics()));
		}

		if (historyEntry->geometry(y, visibleWidth(), fontMetrics()).contains(e->pos())) {
			popupHistory = historyEntry;
			pos = y;
		}
		y += historyEntry->geometry(y, visibleWidth(), fontMetrics()).height() + 5;
	}

	//now do update
	if (popupHistory) {
		if (m_selected && m_selected != popupHistory) {
			m_selected->setSelected(false);
			updateContents(m_selected->geometry(pos, visibleWidth(), fontMetrics()));
		}
		m_selected = popupHistory;
		m_selected->setSelected(true);
		updateContents(m_selected->geometry(pos, visibleWidth(), fontMetrics()));
		if(e->button() == Qt::RightButton) {
			m_popup->exec(e->globalPos());
		}
	}
}

void KexiQueryDesignerSQLHistory::contentsMouseDoubleClickEvent(QMouseEvent * e)
{
	contentsMousePressEvent(e);
	if (m_selected)
		emit currentItemDoubleClicked();
}

void KexiQueryDesignerSQLHistory::addEvent(const QString& q, bool s, const QString &error)
{
	if (!m_history->isEmpty()) {
		HistoryEntry *he = m_history->last();
		if (he->statement()==q) {
			he->setTime(QTime::currentTime());
			repaint();
			return;
		}
	}
	addEntry(new HistoryEntry(this, s, QTime::currentTime(), q, error));
}

void KexiQueryDesignerSQLHistory::addEntry(HistoryEntry *e)
{
	m_history->append(e);
//	m_history->prepend(e);

	int y = 0;
	foreach (HistoryEntry *historyEntry, *m_history) {
		y += historyEntry->geometry(y, visibleWidth(), fontMetrics()).height() + 5;
	}

	resizeContents(visibleWidth() - 1, y);
	if (m_selected) {
		m_selected->setSelected(false);
	}
	m_selected = e;
	m_selected->setSelected(true);
	ensureVisible(0,y+5);
	updateContents();
/*	ensureVisible(0, 0);
	if (m_selected) {
		m_selected->setSelected(false, colorGroup());
	}
	m_selected = e;
	m_selected->setSelected(true, colorGroup());
//	updateContents();
	updateContents(m_selected->geometry(0, visibleWidth(), fontMetrics()));*/
}

/*void
KexiQueryDesignerSQLHistory::contextMenu(const QPoint &pos, HistoryEntry *)
{
	KMenu p(this);
	p.insertItem(SmallIcon("edit-copy"), i18n("Copy to Clipboard"), this, SLOT(slotToClipboard()));
	

#ifndef KEXI_NO_UNFINISHED
	p.insertSeparator();
	p.insertItem(SmallIcon("document-properties"), i18n("Edit"), this, SLOT(slotEdit()));
	p.insertItem(SmallIcon("view-refresh"), i18n("Requery"));
#endif

	p.exec(pos);
}*/

void KexiQueryDesignerSQLHistory::slotToClipboard()
{
	if(!m_selected)
		return;

	QApplication::clipboard()->setText(m_selected->statement(), QClipboard::Clipboard);
}

void KexiQueryDesignerSQLHistory::slotEdit()
{
	emit editRequested(m_selected->statement());
}

QString KexiQueryDesignerSQLHistory::selectedStatement() const
{
	return m_selected ? m_selected->statement() : QString();
}

void KexiQueryDesignerSQLHistory::setHistory(History *h)
{
	m_history = h;
	update();
}

void KexiQueryDesignerSQLHistory::clear()
{
	m_selected = 0;
	qDeleteAll(*m_history);
	m_history->clear();
	updateContents();
}

KMenu* KexiQueryDesignerSQLHistory::popupMenu() const
{
	return m_popup;
}

bool KexiQueryDesignerSQLHistory::eventFilter(QObject *obj, QEvent *event)
{
	const bool res = Q3ScrollView::eventFilter(obj, event);
	if (m_selected && (event->type() == QEvent::FocusIn || event->type() == QEvent::FocusOut
		|| event->type() == QEvent::WindowActivate || event->type() == QEvent::WindowDeactivate))
	{
		if (m_prevColorGroup != palette().currentColorGroup()) {
			// update palette of selected text
			m_selected->highlight();
			m_prevColorGroup = palette().currentColorGroup();
		}
	}
	return res;
}

//==================================

HistoryEntry::HistoryEntry(KexiQueryDesignerSQLHistory *parent, bool succeed, const QTime &execTime, 
	const QString &statement, /*int ,*/ const QString &err)
{
	m_parent = parent;
	m_formated = 0;
	m_succeeded = succeed;
	m_execTime = execTime;
	m_statement = statement;
	m_error = err;
	m_selected = false;
	highlight();
}

HistoryEntry::~HistoryEntry()
{
	delete m_formated;
}

void HistoryEntry::drawItem(QPainter *p, int width)
{
	QBrush bgBrush, fgBrush;
	QBrush textBrush, highlightedTextBrush, highlightedBgBrush;
	bgBrush = m_parent->palette().brush( QPalette::Button );
	fgBrush = m_parent->palette().brush( QPalette::ButtonText );
	textBrush = m_parent->palette().brush( QPalette::Text );
	highlightedTextBrush = m_parent->palette().brush( QPalette::HighlightedText );
	highlightedBgBrush = m_parent->palette().brush( QPalette::Highlight );

	p->setBrush(bgBrush);
	p->setPen(Qt::transparent);
	int lineHeight = QFontMetrics(p->font()).height();
	p->drawRect(2, 2, 150, lineHeight);

	if(m_succeeded)
		p->drawPixmap(4, 4, SmallIcon("dialog-ok"));
	else
		p->drawPixmap(4, 4, SmallIcon("dialog-error"));

	p->setPen(fgBrush.color());
	p->setBrush(fgBrush);
	p->drawText(22, 2, 150, lineHeight, Qt::AlignLeft | Qt::AlignVCenter, m_execTimeString);
	p->setPen(bgBrush.color());
	p->setBrush(bgBrush);
	m_formated->setWidth(width - 2);
	QRect content(2, lineHeight+1, width - 2, m_formated->height());
//	QRect content = p->fontMetrics().boundingRect(2, 21, width - 2, 0, Qt::TextWordWrap | Qt::AlignLeft | Qt::AlignVCenter, m_statement);
//	QRect content(2, 21, width - 2, p->fontMetrics().height() + 4);
//	content = QRect(2, 21, width - 2, m_for.height());

	if (m_selected)
		p->setBrush(highlightedBgBrush);
	
	p->drawRect(content);

	if (m_selected)
		p->setPen( highlightedTextBrush.color() );
	else
		p->setPen( textBrush.color() );

	content.setX(content.x() + 2);
	content.setWidth(content.width() - 2);
//	p->drawText(content, Qt::TextWordWrap | Qt::AlignLeft | Qt::AlignVCenter, m_statement);
	m_formated->draw(p, content.x(), content.y(), content, QColorGroup(m_parent->palette()));
}

void HistoryEntry::highlight()
{
	QString text;
	bool quote = false;
	bool dblquote = false;
	QString statement( m_statement );
	statement.replace("<", "&lt;");
	statement.replace(">", "&gt;");
	statement.replace("\r\n", "<br>"); //(js) first win32 specific pair
	statement.replace("\n", "<br>"); // now single \n
	statement.replace(" ", "&nbsp;");
	statement.replace("\t", "&nbsp;&nbsp;&nbsp;");

	// getting quoting...
	if(!m_selected) {
		for (int i=0; i < (int)statement.length(); i++) {
			QString beginTag;
			QString endTag;
			QChar curr( statement[i] );

			if (curr == '\'' && !dblquote && statement[i-1] != '\\') {
				if(!quote) {
					quote = true;
					beginTag += "<font color=\"#ff0000\">";
				}
				else {
					quote = false;
					endTag += "</font>";
				}
			}
			if(curr == '"' && !quote && statement[i-1] != '\\') {
				if(!dblquote) {
					dblquote = true;
					beginTag += "<font color=\"#ff0000\">";
				}
				else {
					dblquote = false;
					endTag += "</font>";
				}
			}
			if (QRegExp("[0-9]").exactMatch(QString(curr)) && !quote && !dblquote) {
				beginTag += "<font color=\"#0000ff\">";
				endTag += "</font>";
			}
			text += beginTag + curr + endTag;
		}
	}
	else {
		QColor highlightedTextColor = m_parent->palette().color( 
			m_parent->palette().currentColorGroup()==QPalette::Active ? QPalette::HighlightedText : QPalette::Text );
		text = QString("<font color=\"%1\">").arg(highlightedTextColor.name()) + statement;
	}

//! @todo reuse KTextEditor's SQL highlighting or KexiDB parser's set of reserved words
	QRegExp keywords("\\b(SELECT|UPDATE|INSERT|DELETE|DROP|FROM|WHERE|AND|OR|NOT|"
		"NULL|JOIN|LEFT|RIGHT|ON|INTO|TABLE)\\b");
	keywords.setCaseSensitivity(Qt::CaseInsensitive);
	text = text.replace(keywords, "<b>\\1</b>");

	if (!m_error.isEmpty()) {
//		text += ("<br>"+i18n("Error: %1").arg(m_error));
//		text += QString("<br><font face=\"") + KGlobalSettings::generalFont().family() + QString("\" size=\"-1\">") + i18n("Error: %1").arg(m_error) + "</font>";
		text += QString("<br><font face=\"") + KGlobalSettings::generalFont().family() 
			+ QString("\">") + i18n("Error: %1", m_error) + "</font>";
	}

	//kDebug() << "HistoryEntry::highlight() text:" << text << endl;
	delete m_formated;
	m_formated = new Q3SimpleRichText(text, KGlobalSettings::fixedFont());
}

void
HistoryEntry::setSelected(bool selected)
{
	m_selected = selected;
	highlight();
}

QRect HistoryEntry::geometry(int y, int width, const QFontMetrics& f)
{
//	int h = 21 + f.boundingRect(2, 21, width - 2, 0, Qt::TextWordWrap | Qt::AlignLeft | Qt::AlignVCenter, m_statement).height();
//	return QRect(0, y, width, h);
	int lineHeight = f.height();
	m_formated->setWidth(width - 2);
	return QRect(0, y, width, m_formated->height() + lineHeight + 1);
}

void HistoryEntry::setTime(const QTime &execTime)
{
	m_execTime = execTime;
	m_execTimeString = KGlobal::locale()->formatTime(m_execTime);
}

#include "kexiquerydesignersqlhistory.moc"
