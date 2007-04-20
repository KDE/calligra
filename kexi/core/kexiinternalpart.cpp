/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004-2006 Jaroslaw Staniek <js@iidea.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexiinternalpart.h"

#include "KexiWindow.h"
#include "KexiView.h"
#include "KexiMainWindowIface.h"

#include <q3asciidict.h>
#include <qdialog.h>
//Added by qt3to4:
#include <Q3CString>

#include <kdebug.h>
#include <klibloader.h>
#include <klocale.h>
//#include <kmimetypetrader.h>
#include <kparts/componentfactory.h>
#include <kexidb/msghandler.h>

//! @internal
class KexiInternalPartManager
{
	public:
		KexiInternalPartManager()
		{
		}
		
		KexiInternalPart* findPart(KexiDB::MessageHandler *msgHdr, const char* partName)
		{
			KexiInternalPart *part = m_parts[partName];
			if (!part) {
				QByteArray fullname("kexihandler_");
				fullname += QByteArray(partName).toLower();
				part = KLibLoader::createInstance<KexiInternalPart>(fullname);
				if (!part) {
					if (msgHdr)
						msgHdr->showErrorMessage(i18n("Could not load \"%1\" plugin.").arg(partName));
				}
				else
					m_parts.insert(partName, part);
			}
			return part;
		}
	
	private:
		
		QHash<QByteArray, KexiInternalPart*> m_parts;
};

KexiInternalPartManager internalPartManager;

//----------------------------------------------

KexiInternalPart::KexiInternalPart(QObject *parent, const char *name, const QStringList &)
 : QObject(parent)
 , m_uniqueDialog(true)
 , m_cancelled(false)
{
	setObjectName(name);
}

KexiInternalPart::~KexiInternalPart()
{
}

//static
const KexiInternalPart *
KexiInternalPart::part(KexiDB::MessageHandler *msgHdr, const char* partName)
{
	return internalPartManager.findPart(msgHdr, partName);
}

//static
QWidget* KexiInternalPart::createWidgetInstance(const char* partName, 
	const char* widgetClass, KexiDB::MessageHandler *msgHdr, 
	QWidget *parent, const char *objName, QMap<QString,QString>* args)
{
	KexiInternalPart *part = internalPartManager.findPart(msgHdr, partName);
	if (!part)
		return 0; //fatal!
	return part->createWidget(widgetClass, parent, objName ? objName : partName, args);
}

KexiWindow* KexiInternalPart::findOrCreateKexiWindow(
	const char *objName)
{
	if (m_uniqueDialog && !m_uniqueWidget.isNull())
		return dynamic_cast<KexiWindow*>((QWidget*)m_uniqueWidget);
	KexiWindow * wnd = new KexiWindow();
	KexiView *view = createView(0, objName);
	if (!view)
		return 0;

	if (m_uniqueDialog)
		m_uniqueWidget = wnd; //recall unique!
	wnd->addView(view);
	wnd->setWindowTitle( view->windowTitle() );
#warning TODO	wnd->setTabCaption( view->caption() );
	wnd->resize(view->sizeHint());
	wnd->setMinimumSize(view->minimumSizeHint().width(),view->minimumSizeHint().height());
	wnd->setId( KexiMainWindowIface::global()->project()->generatePrivateID() );
	wnd->registerDialog();
	return wnd;
}

//static
KexiWindow* KexiInternalPart::createKexiWindowInstance(
	const char* partName, 
	KexiDB::MessageHandler *msgHdr, const char *objName)
{
	KexiInternalPart *part = internalPartManager.findPart(msgHdr, partName);
	if (!part) {
		kDebug() << "KexiInternalPart::createDialogInstance() !part" << endl;
		return 0; //fatal!
	}
	return part->findOrCreateKexiWindow(objName ? objName : partName);
}

//static
QDialog* KexiInternalPart::createModalDialogInstance(const char* partName, 
	const char* dialogClass, KexiDB::MessageHandler *msgHdr, 
	const char *objName, QMap<QString,QString>* args)
{
	KexiInternalPart *part = internalPartManager.findPart(msgHdr, partName);
	if (!part) {
		kDebug() << "KexiInternalPart::createDialogInstance() !part" << endl;
		return 0; //fatal!
	}
	QWidget *w;
	if (part->uniqueDialog() && !part->m_uniqueWidget.isNull())
		w = part->m_uniqueWidget;
	else
		w = part->createWidget(dialogClass, KexiMainWindowIface::global()->thisWidget(), objName ? objName : partName, args);

	if (dynamic_cast<QDialog*>(w)) {
		if (part->uniqueDialog())
			part->m_uniqueWidget = w;
		return dynamic_cast<QDialog*>(w);
	}
	//sanity
	if (! (part->uniqueDialog() && !part->m_uniqueWidget.isNull()))
		delete w;
	return 0;
}

//static 
bool KexiInternalPart::executeCommand(const char* partName, 
	const char* commandName, QMap<QString,QString>* args)
{
	KexiInternalPart *part = internalPartManager.findPart(0, partName);
	if (!part) {
		kDebug() << "KexiInternalPart::createDialogInstance() !part" << endl;
		return 0; //fatal!
	}
	return part->executeCommand(commandName, args);
}

QWidget* KexiInternalPart::createWidget(const char* widgetClass,
	QWidget * parent, const char * objName, QMap<QString,QString>* args)
{
	Q_UNUSED(widgetClass);
	Q_UNUSED(parent);
	Q_UNUSED(objName);
	Q_UNUSED(args);
	return 0;
}

KexiView* KexiInternalPart::createView(QWidget * parent,
 const char * objName)
{
	Q_UNUSED(parent);
	Q_UNUSED(objName);
	return 0;
}

bool KexiInternalPart::executeCommand(const char* commandName, 
	QMap<QString,QString>* args)
{
	Q_UNUSED(commandName);
	Q_UNUSED(args);
	return false;
}

#include "kexiinternalpart.moc"
