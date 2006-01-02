/***************************************************************************
 * scriptaction.cpp
 * This file is part of the KDE project
 * copyright (C) 2005 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#include "scriptaction.h"
#include "manager.h"

#include <qstylesheet.h>
#include <kurl.h>
#include <kstandarddirs.h>
#include <kmimetype.h>
#include <kdebug.h>

using namespace Kross::Api;

namespace Kross { namespace Api {

    /// @internal
    class ScriptActionPrivate
    {
        public:

            /**
            * List of all kind of logs this \a ScriptAction does
            * remember.
            */
            QStringList logs;

            QValueList<ScriptActionCollection*> collections;
    };

}}

ScriptAction::ScriptAction(const QString& file)
    : KAction(0, file.latin1())
    , Kross::Api::ScriptContainer(file)
    , d( new ScriptActionPrivate() ) // initialize d-pointer class
{
    //kdDebug() << QString("Kross::Api::ScriptAction::ScriptAction(const char*, const QString&) name='%1' text='%2'").arg(name).arg(text) << endl;

    KURL url(file);
    if(url.isLocalFile()) {
        setFile( file );
        setText( url.fileName() );
        setIcon( KMimeType::iconForURL(url) );
    }
    else {
        setText( file );
    }

    setEnabled( false );
}

ScriptAction::ScriptAction(const QDomElement& element)
    : KAction()
    , Kross::Api::ScriptContainer()
    , d( new ScriptActionPrivate() ) // initialize d-pointer class
{
    //kdDebug() << "Kross::Api::ScriptAction::ScriptAction(const QDomElement&)" << endl;

    QString name = element.attribute("name");
    QString text = element.attribute("text");
    QString file = element.attribute("file");
    QString icon = element.attribute("icon");

    if(file.isEmpty()) {
        if(text.isEmpty())
            text = name;
    }
    else {
        if(name.isEmpty())
            name = file;
        if(text.isEmpty())
            text = file;
    }

    //d->scriptcontainer = Manager::scriptManager()->getScriptContainer(name);

    QString interpreter = element.attribute("interpreter");
    if(interpreter.isNull())
        setEnabled(false);
    else
        setInterpreterName( interpreter );

    if(file.isNull()) {
        setCode( element.text().stripWhiteSpace() );
    }
    else {
        QString res = element.attribute("resource");
        if(! res.isNull()) {
            QString f = KGlobal::dirs()->findResource(res.latin1(), file);
            if(f.isNull()) { // if we failed to find the resource just pass both to provide later a better errormessage.
                file = QString("%1://%2").arg(res).arg(file);
                setEnabled(false);
            }
            else // else everything seems to be fine.
                file = f;

        }
        setFile(file);
        if(icon.isNull())
            icon = KMimeType::iconForURL( KURL(file) );
    }

    ScriptContainer::setName( name );
    KAction::setName( name.latin1() );
    KAction::setText( text );
    KAction::setIcon( icon );

    // connect signal
    connect(this, SIGNAL(activated()), this, SLOT(activate()));
}

ScriptAction::~ScriptAction()
{
    //kdDebug() << QString("Kross::Api::ScriptAction::~ScriptAction() name='%1' text='%2'").arg(name()).arg(text()) << endl;
    detachAll();
    delete d;
}

void ScriptAction::setInterpreterName(const QString& name)
{
    setEnabled( Manager::scriptManager()->hasInterpreterInfo(name) );
    Kross::Api::ScriptContainer::setInterpreterName(name);
}

const QStringList& ScriptAction::getLogs() const
{
    return d->logs;
}

void ScriptAction::attach(ScriptActionCollection* collection)
{
    d->collections.append( collection );
}

void ScriptAction::detach(ScriptActionCollection* collection)
{
    d->collections.remove( collection );
}

void ScriptAction::detachAll()
{
    for(QValueList<ScriptActionCollection*>::Iterator it = d->collections.begin(); it != d->collections.end(); ++it)
        (*it)->detach( this );
}

void ScriptAction::activate()
{
    Kross::Api::ScriptContainer::execute();
    if( Kross::Api::ScriptContainer::hadException() ) {
        QString errormessage = Kross::Api::ScriptContainer::getException()->getError();
        QString tracedetails = Kross::Api::ScriptContainer::getException()->getTrace();
        d->logs << QString("<b>%1</b><br>%2")
                   .arg( QStyleSheet::escape(errormessage) )
                   .arg( QStyleSheet::escape(tracedetails) );
        emit failed(errormessage, tracedetails);
    }
    else {
        emit success();
    }
}

void ScriptAction::finalize()
{
    Kross::Api::ScriptContainer::finalize();
}

#include "scriptaction.moc"
