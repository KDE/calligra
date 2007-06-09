/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004-2005 Jaroslaw Staniek <js@iidea.pl>
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2005 Sebastian Sauer <mail@dipe.org>

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

#include "kexiscriptdesignview.h"
#include "kexiscripteditor.h"

#include <kross/main/manager.h>
#include <kross/main/scriptcontainer.h>
#include <kross/main/scriptaction.h>
#include <kross/api/interpreter.h>

#include <qlayout.h>
#include <qsplitter.h>
#include <qtimer.h>
#include <qdatetime.h>
#include <qdom.h>
#include <q3stylesheet.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <ktextbrowser.h>
#include <kdebug.h>

#include <kexidialogbase.h>
#include <kexidb/connection.h>
#include <QTextDocument>

/// @internal
class KexiScriptDesignViewPrivate
{
    public:

        /**
         * The \a Kross::Api::ScriptAction instance which provides
         * us access to the scripting framework Kross.
         */
        Kross::Api::ScriptAction* scriptaction;

        /// The \a KexiScriptEditor to edit the scripting code.
        KexiScriptEditor* editor;

        /// The \a KoProperty::Set used in the propertyeditor.
        KoProperty::Set* properties;

        /// Boolean flag to avoid infinite recursion.
        bool updatesProperties;

        /// Used to display statusmessages.
        KTextBrowser* statusbrowser;
};

KexiScriptDesignView::KexiScriptDesignView(
	QWidget *parent, Kross::Api::ScriptAction* scriptaction)
    : KexiView(parent)
    , d( new KexiScriptDesignViewPrivate() )
{
    setObjectName("KexiScriptDesignView");
    d->scriptaction = scriptaction;
    d->updatesProperties = false;

    QSplitter* splitter = new QSplitter(this);
    splitter->setOrientation(Vertical);
    Q3HBoxLayout* layout = new Q3HBoxLayout(this);
    layout->addWidget(splitter);

    d->editor = new KexiScriptEditor(splitter);
    splitter->setFocusProxy(d->editor);
    addChildView(d->editor);
    setViewWidget(d->editor);

    d->statusbrowser = new KTextBrowser(splitter);
    d->statusbrowser->setObjectName("ScriptStatusBrowser");
    d->statusbrowser->setReadOnly(true);
    d->statusbrowser->setTextFormat(Q3TextBrowser::RichText);
    //d->browser->setWordWrap(QTextEdit::WidgetWidth);
    d->statusbrowser->installEventFilter(this);
    splitter->setResizeMode(d->statusbrowser, QSplitter::KeepSize);

    plugSharedAction( "data_execute", this, SLOT(execute()) );
    if(KexiEditor::isAdvancedEditor()) // the configeditor is only in advanced mode avaiable.
        plugSharedAction( "script_config_editor", d->editor, SLOT(slotConfigureEditor()) );

    loadData();

    d->properties = new KoProperty::Set(this, "KexiScripting");
    connect(d->properties, SIGNAL( propertyChanged(KoProperty::Set&, KoProperty::Property&) ),
            this, SLOT( slotPropertyChanged(KoProperty::Set&, KoProperty::Property&) ));

    // To schedule the initialize fixes a crasher in Kate.
    QTimer::singleShot(50, this, SLOT( initialize() ));
}

KexiScriptDesignView::~KexiScriptDesignView()
{
    delete d->properties;
    delete d;
}

Kross::Api::ScriptAction* KexiScriptDesignView::scriptAction() const
{
    return d->scriptaction;
}

void KexiScriptDesignView::initialize()
{
    updateProperties();
    d->editor->initialize( d->scriptaction );
}

void KexiScriptDesignView::updateProperties()
{
    if(d->updatesProperties)
        return;
    d->updatesProperties = true;

    Kross::Api::Manager* manager = Kross::Api::Manager::scriptManager();

    QString interpretername = d->scriptaction->getInterpreterName();
    Kross::Api::InterpreterInfo* info = interpretername.isEmpty() ? 0 : manager->getInterpreterInfo(interpretername);

    {
        // if interpreter isn't defined or invalid, try to fallback.
        QStringList list;
        list << "python" << "ruby";
        QStringList::ConstIterator it( list.constBegin() ), end( list.constEnd() );
        while( (! info) && (it != end) ) {
            interpretername = (*it);
            info = manager->getInterpreterInfo(interpretername);
            if(info)
                d->scriptaction->setInterpreterName(interpretername);
            ++it;
        }
    }

    if(info) {
        d->properties->clear();

        QStringList interpreters = manager->getInterpreters();
        KoProperty::Property::ListData* proplist = new KoProperty::Property::ListData(interpreters, interpreters);
        KoProperty::Property* prop = new KoProperty::Property(
            "language", // name
            proplist, // ListData
            d->scriptaction->getInterpreterName(), // value
            i18n("Interpreter"), // caption
            i18n("The used scripting interpreter."), // description
            KoProperty::List // type
        );
        d->properties->addProperty(prop);

        Kross::Api::InterpreterInfo::Option::Map options = info->getOptions();
        Kross::Api::InterpreterInfo::Option::Map::ConstIterator it, end( options.constEnd() );
        for( it = options.constBegin(); it != end; ++it) {
            Kross::Api::InterpreterInfo::Option* option = it.data();
            KoProperty::Property* prop = new KoProperty::Property(
                    it.key().toLatin1(), // name
                    d->scriptaction->getOption(it.key(), option->value), // value
                    option->name, // caption
                    option->comment, // description
                    KoProperty::Auto // type
            );
            d->properties->addProperty(prop);
        }
    }

    //propertySetSwitched();
    propertySetReloaded(true);
    d->updatesProperties = false;
}

KoProperty::Set* KexiScriptDesignView::propertySet()
{
    return d->properties;
}

void KexiScriptDesignView::slotPropertyChanged(KoProperty::Set& /*set*/, KoProperty::Property& property)
{
    if(property.isNull()) 
        return;

    if(property.name() == "language") {
        QString language = property.value().toString();
        kDebug() << QString("KexiScriptDesignView::slotPropertyChanged() language=%1").arg(language) << endl;
        d->scriptaction->setInterpreterName( language );
        // We assume Kross and the HighlightingInterface are using same
        // names for the support languages...
        d->editor->setHighlightMode( language );
        updateProperties();
    }
    else {
        bool ok = d->scriptaction->setOption( property.name(), property.value() );
        if(! ok) {
            kWarning() << QString("KexiScriptDesignView::slotPropertyChanged() unknown property '%1'.").arg(property.name()) << endl;
            return;
        }
    }

    setDirty(true);
}

void KexiScriptDesignView::execute()
{
    d->statusbrowser->clear();
    QTime time;
    time.start();
    d->statusbrowser->append( i18n("Execution of the script \"%1\" started.").arg(d->scriptaction->name()) );

    d->scriptaction->activate();
    if( d->scriptaction->hadException() ) {
        QString errormessage = d->scriptaction->getException()->getError();
        d->statusbrowser->append(QString("<b>%2</b><br>").arg(Qt::escape(errormessage)) );

        QString tracedetails = d->scriptaction->getException()->getTrace();
        d->statusbrowser->append( Qt::escape(tracedetails) );

        long lineno = d->scriptaction->getException()->getLineNo();
        if(lineno >= 0)
            d->editor->setLineNo(lineno);
    }
    else {
        d->statusbrowser->append( i18n("Successfully executed. Time elapsed: %1ms").arg(time.elapsed()) );
    }
}

bool KexiScriptDesignView::loadData()
{
    QString data;
    if(! loadDataBlock(data)) {
        kexipluginsdbg << "KexiScriptDesignView::loadData(): no DataBlock" << endl;
        return false;
    }

    QString errMsg;
    int errLine;
    int errCol;

    QDomDocument domdoc;
    bool parsed = domdoc.setContent(data, false, &errMsg, &errLine, &errCol);

    if(! parsed) {
        kexipluginsdbg << "KexiScriptDesignView::loadData() XML parsing error line: " << errLine << " col: " << errCol << " message: " << errMsg << endl;
        return false;
    }

    QDomElement scriptelem = domdoc.namedItem("script").toElement();
    if(scriptelem.isNull()) {
        kexipluginsdbg << "KexiScriptDesignView::loadData(): script domelement is null" << endl;
        return false;
    }

    QString interpretername = scriptelem.attribute("language");
    Kross::Api::Manager* manager = Kross::Api::Manager::scriptManager();
    Kross::Api::InterpreterInfo* info = interpretername.isEmpty() ? 0 : manager->getInterpreterInfo(interpretername);
    if(info) {
        d->scriptaction->setInterpreterName(interpretername);

        Kross::Api::InterpreterInfo::Option::Map options = info->getOptions();
        Kross::Api::InterpreterInfo::Option::Map::ConstIterator it, end = options.constEnd();
        for( it = options.constBegin(); it != end; ++it) {
            QString value = scriptelem.attribute( it.data()->name );
            if(! value.isNull()) {
                QVariant v(value);
                if( v.cast( it.data()->value.type() ) ) // preserve the QVariant's type
                    d->scriptaction->setOption(it.data()->name, v);
            }
        }
    }

    d->scriptaction->setCode( scriptelem.text() );

    return true;
}

KexiDB::SchemaData* KexiScriptDesignView::storeNewData(const KexiDB::SchemaData& sdata, bool &cancel)
{
    KexiDB::SchemaData *s = KexiView::storeNewData(sdata, cancel);
    kexipluginsdbg << "KexiScriptDesignView::storeNewData(): new id:" << s->id() << endl;

    if(!s || cancel) {
        delete s;
        return 0;
    }

    if(! storeData()) {
        kWarning() << "KexiScriptDesignView::storeNewData Failed to store the data." << endl;
        //failure: remove object's schema data to avoid garbage
        KexiDB::Connection *conn = KexiMainWindowIface::global()->project()->dbConnection();
        conn->removeObject( s->id() );
        delete s;
        return 0;
    }

    return s;
}

tristate KexiScriptDesignView::storeData(bool /*dontAsk*/)
{
    kexipluginsdbg << "KexiScriptDesignView::storeData(): " << window()->partItem()->name() << " [" << window()->id() << "]" << endl;

    QDomDocument domdoc("script");
    QDomElement scriptelem = domdoc.createElement("script");
    domdoc.appendChild(scriptelem);

    QString language = d->scriptaction->getInterpreterName();
    scriptelem.setAttribute("language", language);

    Kross::Api::InterpreterInfo* info = Kross::Api::Manager::scriptManager()->getInterpreterInfo(language);
    if(info) {
        Kross::Api::InterpreterInfo::Option::Map defoptions = info->getOptions();
        QMap<QString, QVariant>& options = d->scriptaction->getOptions();
	QMap<QString, QVariant>::ConstIterator it, end( options.constEnd() );
        for( it = options.constBegin(); it != end; ++it) {
            if( defoptions.contains(it.key()) ) { // only remember options which the InterpreterInfo knows about...
                scriptelem.setAttribute(it.key(), it.data().toString());
            }
        }
    }

    QDomText scriptcode = domdoc.createTextNode(d->scriptaction->getCode());
    scriptelem.appendChild(scriptcode);

    return storeDataBlock( domdoc.toString() );
}

#include "kexiscriptdesignview.moc"

