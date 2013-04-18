/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004-2009 Jarosław Staniek <staniek@kde.org>

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

#include <kdebug.h>
//#include <klibloader.h>
#include <klocale.h>
#include <db/msghandler.h>
#include <kexi_global.h>

//! @internal
class KexiInternalPartManager : public QObject
{
public:
    KexiInternalPartManager() : QObject()
    {

    }

    ~KexiInternalPartManager()
    {
    }

    KexiInternalPart* findPart(KexiDB::MessageHandler *msgHdr, const char* partName)
    {
        KexiInternalPart *part = m_parts[partName];
        if (!part) {
            QByteArray fullname("kexihandler_");
            fullname += QByteArray(partName).toLower();
            KPluginLoader loader(fullname);
            const uint foundMajor = (loader.pluginVersion() >> 16) & 0xff;
            if (foundMajor != KEXI_PART_VERSION) {
                if (msgHdr) {
                    msgHdr->showErrorMessage(
                        i18n("Incompatible plugin \"%1\" version: found version %2, expected version %3.",
                            partName,
                            QString::number(foundMajor),
                            QString::number(KEXI_PART_VERSION)));
                }
                return 0;
            }
            
//2.0            part = KLibLoader::createInstance<KexiInternalPart>(fullname);
            KPluginFactory *factory = loader.factory();
            if (factory)
                part = factory->create<KexiInternalPart>(this);

            if (!part) {
                if (msgHdr)
                    msgHdr->showErrorMessage(i18n("Could not load \"%1\" plugin.", partName));
            }
            else {
                part->setObjectName(partName);
                m_parts.insert(partName, part);
            }
        }
        return part;
    }



private:
     QHash<QByteArray, KexiInternalPart*> m_parts;
};

KexiInternalPartManager internalPartManager;

//----------------------------------------------

class KexiInternalPart::Private
{
public:
    Private();

    ~Private();

    //! Unique dialog - we're using guarded ptr for the dialog so can know if it has been closed
    QPointer<QWidget> uniqueWidget;

    bool uniqueWindow; //!< true if createWidgetInstance() should return only one window

    bool cancelled; //!< Used in cancelled()

};

KexiInternalPart::Private::Private() : uniqueWindow(true)
                                       ,cancelled(false)
{

}

KexiInternalPart::Private::~Private()
{

}

KexiInternalPart::KexiInternalPart(QObject *parent, const QVariantList &)
        : QObject(parent)
        , d(new Private())
{
}

KexiInternalPart::~KexiInternalPart()
{
    delete d;
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
        QWidget *parent, const char *objName, QMap<QString, QString>* args)
{
    KexiInternalPart *part = internalPartManager.findPart(msgHdr, partName);
    if (!part)
        return 0; //fatal!
    return part->createWidget(widgetClass, parent, objName ? objName : partName, args);
}

KexiWindow* KexiInternalPart::findOrCreateKexiWindow(
    const char *objName)
{
    if (d->uniqueWindow && !d->uniqueWidget.isNull())
        return dynamic_cast<KexiWindow*>((QWidget*)d->uniqueWidget);
    KexiWindow * wnd = new KexiWindow();
    KexiView *view = createView(0, objName);
    if (!view)
        return 0;

    if (d->uniqueWindow)
        d->uniqueWidget = wnd; //recall unique!
    wnd->addView(view);
    wnd->setWindowTitle(view->windowTitle());
#ifdef __GNUC__
#warning TODO wnd->setTabCaption( view->caption() );
#else
#pragma WARNING( TODO wnd->setTabCaption( view->caption() ); )
#endif
    wnd->resize(view->sizeHint());
    wnd->setMinimumSize(view->minimumSizeHint().width(), view->minimumSizeHint().height());
    wnd->setId(KexiMainWindowIface::global()->project()->generatePrivateID());
    wnd->registerWindow();
    return wnd;
}

//static
KexiWindow* KexiInternalPart::createKexiWindowInstance(
    const char* partName,
    KexiDB::MessageHandler *msgHdr, const char *objName)
{
    KexiInternalPart *part = internalPartManager.findPart(msgHdr, partName);
    if (!part) {
        kWarning() << "!part";
        return 0; //fatal!
    }
    return part->findOrCreateKexiWindow(objName ? objName : partName);
}

//static
QDialog* KexiInternalPart::createModalDialogInstance(const char* partName,
        const char* dialogClass, KexiDB::MessageHandler *msgHdr,
        const char *objName, QMap<QString, QString>* args)
{
    KexiInternalPart *part = internalPartManager.findPart(msgHdr, partName);
    if (!part) {
        kWarning() << "!part";
        return 0; //fatal!
    }
    QWidget *w;
    if (part->uniqueWindow() && !part->d->uniqueWidget.isNull())
        w = part->d->uniqueWidget;
    else
        w = part->createWidget(dialogClass, KexiMainWindowIface::global()->thisWidget(), objName ? objName : partName, args);

    if (dynamic_cast<QDialog*>(w)) {
        if (part->uniqueWindow())
            part->d->uniqueWidget = w;
        return dynamic_cast<QDialog*>(w);
    }
    //sanity
    if (!(part->uniqueWindow() && !part->d->uniqueWidget.isNull()))
        delete w;
    return 0;
}

//static
bool KexiInternalPart::executeCommand(const char* partName,
                                      const char* commandName, QMap<QString, QString>* args)
{
    KexiInternalPart *part = internalPartManager.findPart(0, partName);
    if (!part) {
        kWarning() << "!part";
        return 0; //fatal!
    }
    return part->executeCommand(commandName, args);
}

QWidget* KexiInternalPart::createWidget(const char* widgetClass,
                                        QWidget * parent, const char * objName, QMap<QString, QString>* args)
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
                                      QMap<QString, QString>* args)
{
    Q_UNUSED(commandName);
    Q_UNUSED(args);
    return false;
}

bool KexiInternalPart::uniqueWindow() const
{
    return d->uniqueWindow;
}

void KexiInternalPart::setUniqueWindow(bool set)
{
    d->uniqueWindow = set;
}

bool KexiInternalPart::cancelled() const
{
    return d->cancelled;
}

void KexiInternalPart::setCancelled(bool set)
{
    d->cancelled = set;
}

#include "kexiinternalpart.moc"
