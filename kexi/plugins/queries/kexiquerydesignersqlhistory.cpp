/* This file is part of the KDE project
   Copyright (C) 2003   Lucijan Busch <lucijan@gmx.at>

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

#include <qpainter.h>
#include <qpopupmenu.h>
#include <qclipboard.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <qregexp.h>

#include <koApplication.h>

#include "kexiquerydesignersqlhistory.h"

KexiQueryDesignerSQLHistory::KexiQueryDesignerSQLHistory(QWidget *parent, const char *name)
 : QScrollView(parent, name)
{
	viewport()->setPaletteBackgroundColor(white);

	m_selected = 0;
}

void
KexiQueryDesignerSQLHistory::drawContents(QPainter *p, int cx, int cy, int cw, int ch)
{
	QRect clipping(cx, cy, cw, ch);

	int y = 0;
	for(HistoryEntry *it = m_history.first(); it; it = m_history.next())
	{
//		it->drawItem(p, visibleWidth());
		if(clipping.intersects(it->geometry(y, visibleWidth(), fontMetrics())))
		{
			p->saveWorldMatrix();
			p->translate(0, y);
			it->drawItem(p, visibleWidth(), colorGroup());
			p->restoreWorldMatrix();
		}
		y += it->geometry(y, visibleWidth(), fontMetrics()).height() + 5;
	}
}

void
KexiQueryDesignerSQLHistory::contentsMousePressEvent(QMouseEvent * e)
{
	int y = 0;
	for(HistoryEntry *it = m_history.first(); it; it = m_history.next())
	{
		if(it->isSelected())
		{
			it->setSelected(false);
			updateContents(it->geometry(y, visibleWidth(), fontMetrics()));
		}

		if(it->geometry(y, visibleWidth(), fontMetrics()).contains(e->pos()))
		{
			it->setSelected(true);
			m_selected = it;
			updateContents(it->geometry(y, visibleWidth(), fontMetrics()));
			if(e->button() == RightButton)
				contextMenu(e->globalPos(), it);
		}
		y += it->geometry(y, visibleWidth(), fontMetrics()).height() + 5;
	}
}

void
KexiQueryDesignerSQLHistory::addEvent(QString q, bool s)
{
	HistoryEntry *he=m_history.last();
	if (he) {
		if (he->statement()==q) {
			he->updateTime(QTime::currentTime());
			repaint();
			return;
		}
	}
	addEntry(new HistoryEntry(s, QTime::currentTime(), q, 0));
}

void
KexiQueryDesignerSQLHistory::addEntry(HistoryEntry *e)
{
	m_history.append(e);

	int y = 0;
	for(HistoryEntry *it = m_history.first(); it; it = m_history.next())
	{
		y += it->geometry(y, visibleWidth(), fontMetrics()).height() + 5;
	}

	resizeContents(visibleWidth() - 1, y);
	ensureVisible(0, y);
}

void
KexiQueryDesignerSQLHistory::contextMenu(const QPoint &pos, HistoryEntry *e)
{
	QPopupMenu p(this);
	p.insertItem(SmallIcon("editcopy"), i18n("Copy to Clipboard"), this, SLOT(slotToClipboard()));
#ifndef KEXI_NO_UNFINISHED
	p.insertSeparator();
	p.insertItem(SmallIcon("edit"), i18n("Edit"));
	p.insertItem(SmallIcon("reload"), i18n("Requery"));
#endif

	p.exec(pos);
}

void
KexiQueryDesignerSQLHistory::slotToClipboard()
{
	if(!m_selected)
		return;

	QApplication::clipboard()->setText(m_selected->statement(), QClipboard::Clipboard);
}

KexiQueryDesignerSQLHistory::~KexiQueryDesignerSQLHistory()
{
}

/*
   HISTORY ENTRY
 */

HistoryEntry::HistoryEntry(bool succeed, const QTime &execTime, const QString &statement, int y)
{
	m_succeed = succeed;
	m_execTime = execTime;
	m_statement = statement;
	m_selected = false;
	highlight();
}

void
HistoryEntry::drawItem(QPainter *p, int width, const QColorGroup &cg)
{
	p->setPen(QColor(200, 200, 200));
	p->setBrush(QColor(200, 200, 200));
	p->drawRect(2, 2, 200, 20);
	p->setPen(QColor(0, 0, 0));

	if(m_succeed)
		p->drawPixmap(4, 4, SmallIcon("button_ok"));
	else
		p->drawPixmap(4, 4, SmallIcon("button_cancel"));

	p->drawText(22, 2, 180, 20, Qt::AlignLeft | Qt::AlignVCenter, m_execTime.toString());
	p->setPen(QColor(200, 200, 200));
	p->setBrush(QColor(255, 255, 255));
	m_formated->setWidth(width - 2);
	QRect content(2, 21, width - 2, m_formated->height());
//	QRect content = p->fontMetrics().boundingRect(2, 21, width - 2, 0, Qt::WordBreak | Qt::AlignLeft | Qt::AlignVCenter, m_statement);
//	QRect content(2, 21, width - 2, p->fontMetrics().height() + 4);
//	content = QRect(2, 21, width - 2, m_for.height());

	if(m_selected)
		p->setBrush(cg.highlight());

	p->drawRect(content);

	if(!m_selected)
		p->setPen(cg.text());
	else
		p->setPen(cg.highlightedText());

	content.setX(content.x() + 2);
	content.setWidth(content.width() - 2);
//	p->drawText(content, Qt::WordBreak | Qt::AlignLeft | Qt::AlignVCenter, m_statement);
	m_formated->draw(p, content.x(), content.y(), content, cg);
}

void
HistoryEntry::highlight()
{
	QString statement;
	QString text;
	bool quote = false;
	bool dblquote = false;

	statement = m_statement;
#if QT_VERSION >= 0x030100 //(js)
	statement.replace("<", "&lt;");
	statement.replace(">", "&gt;");
	statement.replace("\r\n", "<br>"); //(js) first win32 specific pair
	statement.replace("\n", "<br>"); // now single \n
	statement.replace(" ", "&nbsp;");
	statement.replace("\t", "&nbsp;&nbsp;&nbsp;");
#else //temporary
	statement.replace(QRegExp("<"), "&lt;");
	statement.replace(QRegExp(">"), "&gt;");
	statement.replace(QRegExp("\r\n"), "<br>"); //(js) first win32 specific pair
	statement.replace(QRegExp("\n"), "<br>"); // now single \n
	statement.replace(QRegExp(" "), "&nbsp;");
	statement.replace(QRegExp("\t"), "&nbsp;&nbsp;&nbsp;");
#endif
	// getting quoting...
	for(int i=0; i < statement.length(); i++)
	{
		QString beginTag;
		QString endTag;
		QChar curr = QChar(statement[i]);

		if(curr == "'" && !dblquote && QChar(statement[i-1]) != "\\")
		{
			if(!quote)
			{
				quote = true;
				beginTag += "<font color=\"#ff0000\">";
			}
			else
			{
				quote = false;
				endTag += "</font>";
			}
		}
		if(curr == "\"" && !quote && QChar(statement[i-1]) != "\\")
		{
			if(!dblquote)
			{
				dblquote = true;
				beginTag += "<font color=\"#ff0000\">";
			}
			else
			{
				dblquote = false;
				endTag += "</font>";
			}
		}
		if(QRegExp("[0-9]").exactMatch(QString(curr)) && !quote && !dblquote)
		{
			beginTag += "<font color=\"#0000ff\">";
			endTag += "</font>";
		}

		text += beginTag + curr + endTag;
	}

	QRegExp keywords("\\b(SELECT|UPDATE|INSERT|DELETE|DROP|FROM|WHERE|AND|OR|NOT|NULL)\\b");
	keywords.setCaseSensitive(false);
	text.replace(keywords, "<b>\\1</b>");

	kdDebug() << "HistoryEntry::highlight() text:" << text << endl;
	m_formated = new QSimpleRichText(text, QFont("courier", 8));

}

QRect
HistoryEntry::geometry(int y, int width, QFontMetrics f)
{
	int h = 21 + f.boundingRect(2, 21, width - 2, 0, Qt::WordBreak | Qt::AlignLeft | Qt::AlignVCenter, m_statement).height();
	return QRect(0, y, width, h);
}

void HistoryEntry::updateTime(const QTime &execTime) {
	m_execTime=execTime;
}

HistoryEntry::~HistoryEntry()
{
}

#include "kexiquerydesignersqlhistory.moc"
