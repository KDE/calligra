/* This file is part of the KDE project
Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

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

#include <qlayout.h>
#include <qlabel.h>
#include <qframe.h>
#include <qwizard.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qpen.h>
#include <qpainter.h>

#include <klocale.h>
#include <kdebug.h>
#include <kcombobox.h>
#include <ktextbrowser.h>
#include <kdialog.h>
#include <kiconloader.h>
#include <kapp.h>

#include <kexidb/drivermanager.h>
#include "kexicreateprojectpageengine.h"
#include "kexiproject.h"

#include "kexi_global.h"

//! Small extension: list item that can be shown as disabled, but still selectable
class KexiListBoxItem : public QListBoxPixmap {
	public:
	KexiListBoxItem( QListBox * listbox, const QPixmap & pix, const QString & text, bool enabled=true )
		: QListBoxPixmap(listbox, pix, text ), m_enabled(enabled)
	{}
	protected:
		virtual void paint( QPainter * painter ) {
			QPen p = painter->pen();
			if (!m_enabled && !isSelected())
					p.setColor( qApp->palette().disabled().text() );
//				if (listBox()->currentItem()==this)
//					;//p.setColor( isSelected() && listBox()->hasFocus() ? qApp->palette().disabled().highlightedText() : qApp->palette().disabled().text() );
//				else
//kdDebug() << isSelected() << " " <<  listBox()->hasFocus() << endl;
//					p.setColor( isSelected() && listBox()->hasFocus() ? qApp->palette().disabled().highlightedText() : qApp->palette().disabled().text() );
//			}
			painter->setPen(p);
			QListBoxPixmap::paint(painter);
		}
		bool m_enabled : 1;
};

KexiCreateProjectPageEngine::KexiCreateProjectPageEngine(KexiCreateProject *parent, QPixmap *wpic, const char *name)
 : KexiCreateProjectPage(parent, wpic, name)
{
	QLabel *lEngine = new QLabel(i18n("Driver: "), m_contents);

	m_engine = new KComboBox(m_contents);
	m_engine->setMaxCount(8);
	connect(m_engine, SIGNAL(activated(int)), this, SLOT(slotActivated(int)));

	m_summary = new KTextBrowser(m_contents);

	QGridLayout *g = new QGridLayout(m_contents);
	g->addWidget(lEngine,		0,	0);
	g->addWidget(m_engine,		0,	1);
	g->addMultiCellWidget(m_summary,1,	1,	0,	1);
	g->setSpacing(KDialog::spacingHint());

	fill();

	setProperty("section", QVariant("Both"));
	setProperty("caption", QVariant(i18n("Engine")));

	m_engine->setFocus();
}

void
KexiCreateProjectPageEngine::fill()
{
	if (m_engine->count()>0)
		return;
	QStringList drivers = project()->manager()->driverNames();
	for(QStringList::Iterator it = drivers.begin(); it != drivers.end(); ++it)
	{
		m_engine->insertItem( kapp->iconLoader()->loadIcon("exec", KIcon::Small), *it);
	}
#ifdef KEXI_ADD_EXAMPLE_ENGINES
	m_first_example_engine_nr = m_engine->count();
	m_example_engines 
		<< i18n("Default Kexi embedded engine")
		<< "PostgreSQL"
		<< "Interbase/Firebird"
		<< "CQL++"
		<< "SAP DB"
		<< "IBM DB2"
		<< "Oracle"
		<< "Sybase/Adaptive Server Enterprise"
		<< "ADABAS D"
		<< "Informix" 
		<< "Progress"
		<< "ODBC";
		//(uniwersalne polaczenie do baz) 

	QPixmap &icon = kapp->iconLoader()->loadIcon("exec", KIcon::Small);
	for ( QStringList::Iterator it = m_example_engines.begin(); it != m_example_engines.end(); ++it ) {
	m_engine->listBox()->insertItem( new KexiListBoxItem( 0, icon, *it, false ) );
//		m_engine->insertItem( kapp->iconLoader()->loadIcon("exec", KIcon::Small), *it);
	}

#endif
	if(!m_engine->currentText().isEmpty())
	{
		setProperty("engine", QVariant(m_engine->currentText()));
		setProperty("continue", QVariant(true));
	}

	fillSummary();
}

void
KexiCreateProjectPageEngine::fillSummary()
{
	QString engineSummary;
	QVariant location;
#ifdef KEXI_ADD_EXAMPLE_ENGINES
	if (m_engine->currentItem()>=m_first_example_engine_nr) {
		engineSummary = i18n("Sorry, this driver is not availabe with current version of %1.").arg(KEXI_APP_NAME);
		if (m_engine->currentText()=="ODBC") {
			engineSummary += i18n("<p>ODBC is an open specification for easier accessing Data Sources. Data Sources include both SQL databases availabe with Kexi drivers and other, like Microsoft SQL Server and Microsoft Access.");
		}
	} 
	else
#endif
	{      
		KService *ptr=project()->manager()->serviceInfo(m_engine->currentText());
		if (ptr) {
			engineSummary = ptr->comment();
			location = project()->manager()->serviceInfo(m_engine->currentText())->property("X-Kexi-Location");
			setProperty("location", location);
		} else engineSummary=i18n("Internal error while retrieving information. (No driver installed ?)");
//		userSummary = QString("<b>" + m_engine->currentText() + "</b><br><hr><br>" + engineSummary);
	}


	if(!m_engine->currentText().isEmpty())
	{
		m_summary->setText( QString("<b>") + m_engine->currentText() + "</b><br><hr><br>" + engineSummary );
	}

}

void
KexiCreateProjectPageEngine::slotActivated(int idx)
{
	bool continue_enabled;
#ifdef KEXI_ADD_EXAMPLE_ENGINES
	continue_enabled = idx<m_first_example_engine_nr;
#else
	continue_enabled=true;
#endif

	setProperty("continue", continue_enabled);
	setProperty("engine", QVariant(m_engine->text(idx)));
	fillSummary();
}

KexiCreateProjectPageEngine::~KexiCreateProjectPageEngine()
{
}

#include "kexicreateprojectpageengine.moc"
