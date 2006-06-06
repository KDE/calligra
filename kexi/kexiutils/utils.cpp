/* This file is part of the KDE project
   Copyright (C) 2003-2006 Jaroslaw Staniek <js@iidea.pl>

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

#include <qregexp.h>
#include <qheader.h>
#include <qlayout.h>
#include <qlayout.h>

#include <kdebug.h>
#include <kcursor.h>
#include <kapplication.h>
#include <ktabwidget.h>
#include <klistview.h>
#include <kiconloader.h>
#include <kdialogbase.h>
#include <kpushbutton.h>
#include <kguiitem.h>

using namespace KexiUtils;

DelayedCursorHandler::DelayedCursorHandler() {
	connect(&timer, SIGNAL(timeout()), this, SLOT(show()));
}
void DelayedCursorHandler::start(bool noDelay) {
	timer.start(noDelay ? 0 : 1000, true);
}
void DelayedCursorHandler::stop() {
	timer.stop();
	QApplication::restoreOverrideCursor();
}
void DelayedCursorHandler::show() {
	QApplication::setOverrideCursor( KCursor::waitCursor() );
}

DelayedCursorHandler _delayedCursorHandler;

void KexiUtils::setWaitCursor(bool noDelay) {
	if (kapp->guiEnabled())
		_delayedCursorHandler.start(noDelay);
}
void KexiUtils::removeWaitCursor() {
	if (kapp->guiEnabled())
		_delayedCursorHandler.stop();
}

WaitCursor::WaitCursor(bool noDelay)
{
	setWaitCursor(noDelay);
}

WaitCursor::~WaitCursor()
{
	removeWaitCursor();
}

//--------------------------------------------------------------------------------

QString KexiUtils::fileDialogFilterString(const KMimeType::Ptr& mime, bool kdeFormat)
{
	if (mime==0)
		return QString::null;

	QString str;
	if (kdeFormat) {
		if (mime->patterns().isEmpty())
			str = "*";
		else
			str = mime->patterns().join(" ");
		str += "|";
	}
	str += mime->comment();
	if (!mime->patterns().isEmpty() || !kdeFormat) {
		str += " (";
		if (mime->patterns().isEmpty())
			str += "*";
		else
			str += mime->patterns().join("; ");
		str += ")";
	}
	if (kdeFormat)
		str += "\n";
	else
		str += ";;";
	return str;
}

QString KexiUtils::fileDialogFilterString(const QString& mimeString, bool kdeFormat)
{
	KMimeType::Ptr ptr = KMimeType::mimeType(mimeString);
	return fileDialogFilterString( ptr, kdeFormat );
}

QString KexiUtils::fileDialogFilterStrings(const QStringList& mimeStrings, bool kdeFormat)
{
	QString ret;
	QStringList::ConstIterator endIt = mimeStrings.constEnd();
	for(QStringList::ConstIterator it = mimeStrings.constBegin(); it != endIt; ++it)
		ret += fileDialogFilterString(*it, kdeFormat);
	return ret;
}

QColor KexiUtils::blendedColors(const QColor& c1, const QColor& c2, int factor1, int factor2)
{
	return QColor(
		int( (c1.red()*factor1+c2.red()*factor2)/(factor1+factor2) ),
		int( (c1.green()*factor1+c2.green()*factor2)/(factor1+factor2) ),
		int( (c1.blue()*factor1+c2.blue()*factor2)/(factor1+factor2) ) );
}

QColor KexiUtils::contrastColor(const QColor& c)
{
	int g = qGray( c.rgb() );
	if (g>110)
		return c.dark(200);
	else if (g>80)
		return c.light(150);
	else if (g>20)
		return c.light(300);
	return Qt::gray;
}

QColor KexiUtils::bleachedColor(const QColor& c, int factor)
{
	int h, s, v;
	c.getHsv( &h, &s, &v );
	QColor c2;
	if (factor < 100)
		factor = 100;
	if (s>=250 && v>=250) //for colors like cyan or red, make the result more white
		s = QMAX(0, s - factor - 50);
	else if (s<=5 && s<=5)
		v += factor-50;
	c2.setHsv(h, s, QMIN(255,v + factor-100));
	return c2;
}

void KexiUtils::serializeMap(const QMap<QString,QString>& map, const QByteArray& array)
{
	QDataStream ds(array, IO_WriteOnly);
	ds << map;
}

void KexiUtils::serializeMap(const QMap<QString,QString>& map, QString& string)
{
	QByteArray array;
	QDataStream ds(array, IO_WriteOnly);
	ds << map;
	kdDebug() << array[3] << " " << array[4] << " " << array[5] << endl;
	const uint size = array.size();
	string = QString::null;
	string.reserve(size);
	for (uint i=0; i<size; i++) {
		string[i]=QChar(ushort(array[i]+1));
	}
}

QMap<QString,QString> KexiUtils::deserializeMap(const QByteArray& array)
{
	QMap<QString,QString> map;
	QDataStream ds(array, IO_ReadOnly);
	ds >> map;
	return map;
}

QMap<QString,QString> KexiUtils::deserializeMap(const QString& string)
{
	const uint size = string.length();
	QCString cstr(string.latin1());
	QByteArray array( size );
	for (uint i=0; i<size; i++) {
		array[i] = char(string[i].unicode()-1);
	}
	QMap<QString,QString> map;
	QDataStream ds(array, IO_ReadOnly);
	ds >> map;
	return map;
}

QString KexiUtils::stringToFileName(const QString& string)
{
	QString _string(string);
	_string.replace(QRegExp("[\\\\/:\\*?\"<>|]"), " ");
	return _string.simplifyWhiteSpace();
}

void KexiUtils::simpleCrypt(QString& string)
{
	for (uint i=0; i<string.length(); i++)
		string[i] = QChar( string[i].unicode() + 47 + i );
}

void KexiUtils::simpleDecrypt(QString& string)
{
	for (uint i=0; i<string.length(); i++)
		string[i] = QChar( string[i].unicode() - 47 - i );
}

static DebugWindowDialog* debugWindow = 0;
static KTabWidget* debugWindowTab = 0;
static KListView* kexiDbCursorDebugPage = 0;
static KListView* kexiAlterTableActionDebugPage = 0;

#ifdef KEXI_DEBUG_GUI
QWidget *KexiUtils::createDebugWindow(QWidget *parent)
{
	// (this is internal code - do not use i18n() here)
	debugWindow = new DebugWindowDialog(parent);
	debugWindow->setSizeGripEnabled( true );
	QBoxLayout *lyr = new QVBoxLayout(debugWindow, KDialogBase::marginHint());
	debugWindowTab = new KTabWidget(debugWindow, "debugWindowTab");
	lyr->addWidget( debugWindowTab );
	debugWindow->resize(900, 600);
	debugWindow->setIcon( DesktopIcon("info") );
	debugWindow->setCaption("Kexi Internal Debugger");
	debugWindow->show();
	return debugWindow;
}

void KexiUtils::addDBCursorDebug(const QString& text)
{
	// (this is internal code - do not use i18n() here)
	if (!debugWindowTab)
		return;
	if (!kexiDbCursorDebugPage) {
		QWidget *page = new QWidget(debugWindowTab);
		QVBoxLayout *vbox = new QVBoxLayout(page);
		QHBoxLayout *hbox = new QHBoxLayout(page);
		vbox->addLayout(hbox);
		hbox->addStretch(1);
		KPushButton *btn_clear = new KPushButton(KGuiItem("Clear", "clear_left"), page);
		hbox->addWidget(btn_clear);

		kexiDbCursorDebugPage = new KListView(page, "kexiDbCursorDebugPage");
		QObject::connect(btn_clear, SIGNAL(clicked()), kexiDbCursorDebugPage, SLOT(clear()));
		vbox->addWidget(kexiDbCursorDebugPage);
		kexiDbCursorDebugPage->addColumn("");
		kexiDbCursorDebugPage->header()->hide();
		kexiDbCursorDebugPage->setSorting(-1);
		kexiDbCursorDebugPage->setAllColumnsShowFocus ( true );
		kexiDbCursorDebugPage->setColumnWidthMode( 0, QListView::Maximum );
		kexiDbCursorDebugPage->setRootIsDecorated( true );
		debugWindowTab->addTab( page, "DB Cursors" );
		debugWindowTab->showPage(page);
		kexiDbCursorDebugPage->show();
	}
	//add \n after (about) every 30 characters
//TODO	QString realText

	KListViewItem * li = new KListViewItem( kexiDbCursorDebugPage, kexiDbCursorDebugPage->lastItem(), text );
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
		KPushButton *btn_exec = new KPushButton(KGuiItem("Real Alter Table", "filesave"), page);
		btn_exec->setName("executeRealAlterTable");
		hbox->addWidget(btn_exec);
		KPushButton *btn_clear = new KPushButton(KGuiItem("Clear", "clear_left"), page);
		hbox->addWidget(btn_clear);
		KPushButton *btn_sim = new KPushButton(KGuiItem("Simulate Execution", "exec"), page);
		btn_sim->setName("simulateAlterTableExecution");
		hbox->addWidget(btn_sim);

		kexiAlterTableActionDebugPage = new KListView(page, "kexiAlterTableActionDebugPage");
		QObject::connect(btn_clear, SIGNAL(clicked()), kexiAlterTableActionDebugPage, SLOT(clear()));
		vbox->addWidget(kexiAlterTableActionDebugPage);
		kexiAlterTableActionDebugPage->addColumn("");
		kexiAlterTableActionDebugPage->header()->hide();
		kexiAlterTableActionDebugPage->setSorting(-1);
		kexiAlterTableActionDebugPage->setAllColumnsShowFocus ( true );
		kexiAlterTableActionDebugPage->setColumnWidthMode( 0, QListView::Maximum );
		kexiAlterTableActionDebugPage->setRootIsDecorated( true );
		debugWindowTab->addTab( page, "AlterTable Actions" );
		debugWindowTab->showPage(page);
		page->show();
	}
	if (text.isEmpty()) //don't move up!
		return;
	KListViewItem * li;
	int availableNestingLevels = 0;
	// compute availableNestingLevels
	QListViewItem * lastItem = kexiAlterTableActionDebugPage->lastItem();
	//kdDebug() << "lastItem: " << (lastItem ? lastItem->text(0) : QString::null) << endl;
	while (lastItem) {
		lastItem = lastItem->parent();
		availableNestingLevels++;
	}
	//kdDebug() << "availableNestingLevels: " << availableNestingLevels << endl;
	//go up (availableNestingLevels-levelsToGoUp) levels
	lastItem = kexiAlterTableActionDebugPage->lastItem();
	int levelsToGoUp = availableNestingLevels - nestingLevel;
	while (levelsToGoUp > 0 && lastItem) {
		lastItem = lastItem->parent();
		levelsToGoUp--;
	}
	//kdDebug() << "lastItem2: " << (lastItem ? lastItem->text(0) : QString::null) << endl;
	if (lastItem) {
		QListViewItem *after = lastItem->firstChild(); //find last child so we can insert a new item after it
		while (after && after->nextSibling())
			after = after->nextSibling();
		if (after)
			li = new KListViewItem( lastItem, after, text ); //child, after
		else
			li = new KListViewItem( lastItem, text ); //1st child
	}
	else {
		lastItem = kexiAlterTableActionDebugPage->lastItem();
		while (lastItem && lastItem->parent())
			lastItem = lastItem->parent();
		//kdDebug() << "lastItem2: " << (lastItem ? lastItem->text(0) : QString::null) << endl;
		li = new KListViewItem( kexiAlterTableActionDebugPage, lastItem, text ); //after
	}
	li->setOpen(true);
	li->setMultiLinesEnabled( true );
}

void KexiUtils::connectPushButtonActionForDebugWindow(const char* actionName, 
	const QObject *receiver, const char* slot)
{
	if (debugWindow) {
		KPushButton* btn = findFirstChild<KPushButton>(debugWindow, "KPushButton", actionName);
		if (btn)
			QObject::connect(btn, SIGNAL(clicked()), receiver, slot);
	}
}

#endif //KEXI_DEBUG_GUI

#include "utils_p.moc"
