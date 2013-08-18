/*
   This file is part of the KDE libraries

   Copyright (c) 2001 Michael Goffioul <kdeprint@swing.be>
   Copyright (C) 2004 Frans Englich <frans.englich@telia.com>
   Copyright (C) 2009 Dario Freddi <drf@kde.org>

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
   Boston, MA 02110-1301, USA.

*/

#define KDE3_SUPPORT
#include "kcmodule.h"
#undef KDE3_SUPPORT

#include <QLayout>
#include <QTimer>
#include <QDebug>

#include <kaboutdata.h>
#include <kconfigskeleton.h>
#include <kconfigdialogmanager.h>
#include <klocalizedstring.h>
#include <kauthexecutejob.h>

class KCModulePrivate
{
public:
    KCModulePrivate():
        _buttons( KCModule::Help | KCModule::Default | KCModule::Apply ),
        _about( 0 ),
        _useRootOnlyMessage( false ),
        _firstshow(true),
        _needsAuthorization(false),
        _authAction(0),
        _unmanagedWidgetChangeState( false )
        { }

    void authStatusChanged(int status);

    KCModule::Buttons _buttons;
    const KAboutData *_about;
    QString _rootOnlyMessage;
    QList<KConfigDialogManager*> managers;
    QString _quickHelp;
    QString m_ExportText;
    bool _useRootOnlyMessage : 1;
    bool _firstshow : 1;

    bool  _needsAuthorization : 1;
    KAuth::Action _authAction;

    // this member is used to record the state on non-automatically
    // managed widgets, allowing for mixed KConfigXT-drive and manual
    // widgets to coexist peacefully and do the correct thing with
    // the changed(bool) signal
    bool _unmanagedWidgetChangeState : 1;
};

KCModule::KCModule(const KAboutData *aboutData, QWidget *parent, const QVariantList &)
    : QWidget( parent ), d(new KCModulePrivate)
{
    setAboutData(aboutData);
}

KCModule::KCModule(QWidget *parent, const QVariantList &)
    : QWidget( parent ), d(new KCModulePrivate)
{
}

void KCModule::showEvent(QShowEvent *ev)
{
    if (d->_firstshow) {
        d->_firstshow = false;
        QMetaObject::invokeMethod(this, "load", Qt::QueuedConnection);
        QMetaObject::invokeMethod(this, "changed", Qt::QueuedConnection, Q_ARG(bool, false));
    }

    QWidget::showEvent(ev);
}

KCModule::Buttons KCModule::buttons() const
{
    return d->_buttons;
}

void KCModule::setButtons( Buttons buttons )
{
    d->_buttons = buttons;
}

KConfigDialogManager* KCModule::addConfig( KCoreConfigSkeleton *config, QWidget* widget )
{
    KConfigDialogManager* manager = new KConfigDialogManager( widget, config );
    manager->setObjectName( objectName() );
    connect( manager, SIGNAL(widgetModified()), SLOT(widgetChanged()));
    d->managers.append( manager );
    return manager;
}

KConfigDialogManager* KCModule::addConfig( KConfigSkeleton *config, QWidget* widget )
{
    KConfigDialogManager* manager = new KConfigDialogManager( widget, config );
    manager->setObjectName( objectName() );
    connect( manager, SIGNAL(widgetModified()), SLOT(widgetChanged()));
    d->managers.append( manager );
    return manager;
}

void KCModule::setNeedsAuthorization(bool needsAuth)
{
    d->_needsAuthorization = needsAuth;
    if (needsAuth && d->_about) {
        d->_authAction = KAuth::Action(QString("org.kde.kcontrol." + d->_about->componentName() + ".save"));
        d->_needsAuthorization = d->_authAction.isValid();
        d->_authAction.setHelperId("org.kde.kcontrol." + d->_about->componentName());
        d->_authAction.setParentWidget(this);
        authStatusChanged(d->_authAction.status());
    } else {
        d->_authAction = KAuth::Action();
    }
}

bool KCModule::needsAuthorization() const
{
    return d->_needsAuthorization;
}

KAuth::Action KCModule::authAction() const
{
    return d->_authAction;
}

void KCModule::authStatusChanged(KAuth::Action::AuthStatus status)
{
    switch(status) {
        case KAuth::Action::AuthorizedStatus:
            setUseRootOnlyMessage(false);
            break;
        case KAuth::Action::AuthRequiredStatus:
            setUseRootOnlyMessage(true);
            setRootOnlyMessage(i18n("You will be asked to authenticate before saving"));
            break;
        default:
            setUseRootOnlyMessage(true);
            setRootOnlyMessage(i18n("You are not allowed to save the configuration"));
            break;
    }

    qDebug() << useRootOnlyMessage();
}

KCModule::~KCModule()
{
    qDeleteAll(d->managers);
    d->managers.clear();
    delete d->_about;
    delete d;
}

void KCModule::load()
{
    KConfigDialogManager* manager;
    Q_FOREACH( manager , d->managers )
        manager->updateWidgets();
    emit( changed( false ));
}

void KCModule::save()
{
    KConfigDialogManager* manager;
    Q_FOREACH( manager , d->managers )
        manager->updateSettings();
    emit( changed( false ));
}

void KCModule::defaults()
{
    KConfigDialogManager* manager;
    Q_FOREACH( manager , d->managers )
        manager->updateWidgetsDefault();
}

void KCModule::widgetChanged()
{
    emit changed(d->_unmanagedWidgetChangeState || managedWidgetChangeState());
}

bool KCModule::managedWidgetChangeState() const
{
    KConfigDialogManager* manager;
    Q_FOREACH( manager , d->managers )
    {
        if ( manager->hasChanged() )
            return true;
    }

    return false;
}

void KCModule::unmanagedWidgetChangeState(bool changed)
{
    d->_unmanagedWidgetChangeState = changed;
    widgetChanged();
}

const KAboutData *KCModule::aboutData() const
{
    return d->_about;
}

void KCModule::setAboutData(const KAboutData *about)
{
    delete d->_about;
    d->_about = about;
}

void KCModule::setRootOnlyMessage(const QString& message)
{
    d->_rootOnlyMessage = message;
    emit rootOnlyMessageChanged(d->_useRootOnlyMessage, d->_rootOnlyMessage);
}

QString KCModule::rootOnlyMessage() const
{
    return d->_rootOnlyMessage;
}

void KCModule::setUseRootOnlyMessage(bool on)
{
    d->_useRootOnlyMessage = on;
    emit rootOnlyMessageChanged(d->_useRootOnlyMessage, d->_rootOnlyMessage);
}

bool KCModule::useRootOnlyMessage() const
{
    return d->_useRootOnlyMessage;
}

void KCModule::changed()
{
    emit changed(true);
}

KAboutData KCModule::componentData() const
{
    return *d->_about;
}

QString KCModule::exportText() const
{
  return d->m_ExportText;
}

void KCModule::setExportText(const QString& text)
{
  d->m_ExportText = text;
}

void KCModule::setQuickHelp( const QString& help )
{
    d->_quickHelp = help;
    emit( quickHelpChanged() );
}

QString KCModule::quickHelp() const
{
    return d->_quickHelp;
}

QList<KConfigDialogManager*> KCModule::configs() const
{
    return d->managers;
}

// vim: sw=4 et sts=4
