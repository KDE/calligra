/* This file is part of the KDE project
   Copyright (C) 2006 Jaroslaw Staniek <js@iidea.pl>

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

#include "utils.h"
#include "utils_p.h"

#include <q3header.h>
#include <qlayout.h>

#include <ktabwidget.h>
#include <k3listview.h>
#include <kiconloader.h>
#include <kpagedialog.h>
#include <kpushbutton.h>
#include <kguiitem.h>

#ifdef KEXI_DEBUG_GUI

static DebugWindowDialog* debugWindow = 0;
static KTabWidget* debugWindowTab = 0;
static K3ListView* kexiDBDebugPage = 0;
static K3ListView* kexiAlterTableActionDebugPage = 0;

QWidget *KexiUtils::createDebugWindow(QWidget *parent)
{
	// (this is internal code - do not use i18n() here)
	debugWindow = new DebugWindowDialog(parent);
	debugWindow->setSizeGripEnabled( true );
	QBoxLayout *lyr = new QVBoxLayout(debugWindow);
	debugWindowTab = new KTabWidget(debugWindow);
	debugWindowTab->setObjectName("debugWindowTab");
	lyr->addWidget( debugWindowTab );
	debugWindow->resize(900, 600);
	debugWindow->setWindowIcon( KIcon("document-properties") );
	debugWindow->setWindowTitle("Kexi Internal Debugger");
	debugWindow->show();
	return debugWindow;
}

void KexiUtils::addKexiDBDebug(const QString& text)
{
	// (this is internal code - do not use i18n() here)
	if (!debugWindowTab)
		return;
	if (!kexiDBDebugPage) {
		QWidget *page = new QWidget(debugWindowTab);
		QVBoxLayout *vbox = new QVBoxLayout(page);
		QHBoxLayout *hbox = new QHBoxLayout(page);
		vbox->addLayout(hbox);
		hbox->addStretch(1);
		KPushButton *btn_clear = new KPushButton(KGuiItem("Clear", "edit-clear-locationbar-rtl"), page);
		hbox->addWidget(btn_clear);

		kexiDBDebugPage = new K3ListView(page);
		kexiDBDebugPage->setObjectName("kexiDbDebugPage");
		kexiDBDebugPage->setFont( KexiUtils::smallFont(kexiDBDebugPage) );
		QObject::connect(btn_clear, SIGNAL(clicked()), kexiDBDebugPage, SLOT(clear()));
		vbox->addWidget(kexiDBDebugPage);
		kexiDBDebugPage->addColumn("");
		kexiDBDebugPage->header()->hide();
		kexiDBDebugPage->setSorting(-1);
		kexiDBDebugPage->setAllColumnsShowFocus ( true );
		kexiDBDebugPage->setColumnWidthMode( 0, Q3ListView::Maximum );
		kexiDBDebugPage->setRootIsDecorated( true );
		debugWindowTab->addTab( page, "KexiDB" );
		debugWindowTab->setCurrentWidget(page);
		kexiDBDebugPage->show();
	}
	//add \n after (about) every 30 characters
//TODO	QString realText

	K3ListViewItem * li = new K3ListViewItem( kexiDBDebugPage, kexiDBDebugPage->lastItem(), text );
	li->setMultiLinesEnabled( true );
}

void KexiUtils::addAlterTableActionDebug(const QString& text, int nestingLevel)
{
	// (this is internal code - do not use i18n() here)
	if (!debugWindowTab)
		return;
	if (!kexiAlterTableActionDebugPage) {
		QWidget *page = new QWidget(debugWindowTab);
		QVBoxLayout *vbox = new QVBoxLayout(page);
		QHBoxLayout *hbox = new QHBoxLayout(page);
		vbox->addLayout(hbox);
		hbox->addStretch(1);
		KPushButton *btn_exec = new KPushButton(KGuiItem("Real Alter Table", "document-save"), page);
		btn_exec->setObjectName("executeRealAlterTable");
		hbox->addWidget(btn_exec);
		KPushButton *btn_clear = new KPushButton(KGuiItem("Clear", "edit-clear-locationbar-rtl"), page);
		hbox->addWidget(btn_clear);
		KPushButton *btn_sim = new KPushButton(KGuiItem("Simulate Execution", "exec"), page);
		btn_sim->setObjectName("simulateAlterTableExecution");
		hbox->addWidget(btn_sim);

		kexiAlterTableActionDebugPage = new K3ListView(page);
		kexiAlterTableActionDebugPage->setFont( KexiUtils::smallFont(kexiAlterTableActionDebugPage) );
		kexiAlterTableActionDebugPage->setObjectName("kexiAlterTableActionDebugPage");
		QObject::connect(btn_clear, SIGNAL(clicked()), kexiAlterTableActionDebugPage, SLOT(clear()));
		vbox->addWidget(kexiAlterTableActionDebugPage);
		kexiAlterTableActionDebugPage->addColumn("");
		kexiAlterTableActionDebugPage->header()->hide();
		kexiAlterTableActionDebugPage->setSorting(-1);
		kexiAlterTableActionDebugPage->setAllColumnsShowFocus ( true );
		kexiAlterTableActionDebugPage->setColumnWidthMode( 0, Q3ListView::Maximum );
		kexiAlterTableActionDebugPage->setRootIsDecorated( true );
		debugWindowTab->addTab( page, "AlterTable Actions" );
		debugWindowTab->setCurrentWidget(page);
		page->show();
	}
	if (text.isEmpty()) //don't move up!
		return;
	K3ListViewItem * li;
	int availableNestingLevels = 0;
	// compute availableNestingLevels
	Q3ListViewItem * lastItem = kexiAlterTableActionDebugPage->lastItem();
	//kDebug() << "lastItem: " << (lastItem ? lastItem->text(0) : QString()) << endl;
	while (lastItem) {
		lastItem = lastItem->parent();
		availableNestingLevels++;
	}
	//kDebug() << "availableNestingLevels: " << availableNestingLevels << endl;
	//go up (availableNestingLevels-levelsToGoUp) levels
	lastItem = kexiAlterTableActionDebugPage->lastItem();
	int levelsToGoUp = availableNestingLevels - nestingLevel;
	while (levelsToGoUp > 0 && lastItem) {
		lastItem = lastItem->parent();
		levelsToGoUp--;
	}
	//kDebug() << "lastItem2: " << (lastItem ? lastItem->text(0) : QString()) << endl;
	if (lastItem) {
		Q3ListViewItem *after = lastItem->firstChild(); //find last child so we can insert a new item after it
		while (after && after->nextSibling())
			after = after->nextSibling();
		if (after)
			li = new K3ListViewItem( lastItem, after, text ); //child, after
		else
			li = new K3ListViewItem( lastItem, text ); //1st child
	}
	else {
		lastItem = kexiAlterTableActionDebugPage->lastItem();
		while (lastItem && lastItem->parent())
			lastItem = lastItem->parent();
		//kDebug() << "lastItem2: " << (lastItem ? lastItem->text(0) : QString()) << endl;
		li = new K3ListViewItem( kexiAlterTableActionDebugPage, lastItem, text ); //after
	}
	li->setOpen(true);
	li->setMultiLinesEnabled( true );
}

void KexiUtils::connectPushButtonActionForDebugWindow(const char* actionName, 
	const QObject *receiver, const char* slot)
{
	if (debugWindow) {
		KPushButton* btn = KexiUtils::findFirstChild<KPushButton*>(
			debugWindow, "KPushButton", actionName);
		if (btn)
			QObject::connect(btn, SIGNAL(clicked()), receiver, slot);
	}
}

#endif //KEXI_DEBUG_GUI
