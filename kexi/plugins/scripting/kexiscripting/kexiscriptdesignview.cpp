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
#include <qdom.h>
#include <kdebug.h>

#include <kexidialogbase.h>
#include <kexidb/connection.h>

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
};

KexiScriptDesignView::KexiScriptDesignView(KexiMainWindow *mainWin, QWidget *parent, Kross::Api::ScriptAction* scriptaction)
    : KexiViewBase(mainWin, parent, "KexiScriptDesignView")
    , d( new KexiScriptDesignViewPrivate() )
{
    d->scriptaction = scriptaction;

    //d->scriptcontainer = scriptmanager->getScriptContainer( parentDialog()->partItem()->name() );
    plugSharedAction( "script_execute", d->scriptaction, SLOT(activate()) );

    QBoxLayout* layout = new QVBoxLayout(this);
    d->editor = new KexiScriptEditor(mainWin, this, "ScriptEditor");
    addChildView((KexiViewBase*)d->editor);
    setViewWidget((KexiViewBase*)d->editor);
    layout->addWidget((KexiViewBase*)d->editor);

    //if(KexiEditor::isAdvancedEditor()) // the configeditor is only in advanced mode avaiable.
    //    plugSharedAction( "script_config_editor", d->editor, SLOT(slotConfigureEditor()) );

    loadData();

    d->properties = new KoProperty::Set(this, "KexiScripting");
    updateProperties();

    d->editor->initialize( d->scriptaction );
}

KexiScriptDesignView::~KexiScriptDesignView()
{
    delete d;
}

void KexiScriptDesignView::updateProperties()
{
    QString interpretername = d->scriptaction->getInterpreterName();
    Kross::Api::InterpreterInfo* info = Kross::Api::Manager::scriptManager()->getInterpreterInfo( interpretername );
    if(info) {
        disconnect(d->properties, SIGNAL( propertyChanged(KoProperty::Set&, KoProperty::Property&) ),
                   this, SLOT( slotPropertyChanged(KoProperty::Set&, KoProperty::Property&) ));

        if(d->properties->contains("language")) {
            const QValueList<KoProperty::Property*>*  children = d->properties->property("language").children();
            QValueListConstIterator<KoProperty::Property*> itt = children->begin();
            for(; itt != children->end(); ++itt)
                (*itt)->setVisible(false);

            d->properties->property("language").setValue( interpretername );
            d->properties->property("language").setVisible(true);
        }
        else {
            QStringList interpreters = Kross::Api::Manager::scriptManager()->getInterpreters();
            KoProperty::Property::ListData* proplist = new KoProperty::Property::ListData(interpreters, interpreters);
            KoProperty::Property* prop = new KoProperty::Property(
                "language", // name
                proplist, // ListData
                d->scriptaction->getInterpreterName(), // value
                i18n("Interpreter"), // caption
                i18n("The used scripting interpreter."), // description
                KoProperty::List // type
            );
            //prop->setVisible(false);
            d->properties->addProperty(prop);
        }

        connect(d->properties, SIGNAL( propertyChanged(KoProperty::Set&, KoProperty::Property&) ),
                this, SLOT( slotPropertyChanged(KoProperty::Set&, KoProperty::Property&) ));

        Kross::Api::InterpreterInfo::Option::Map options = info->getOptions();
        Kross::Api::InterpreterInfo::Option::Map::Iterator it( options.begin() );
        for(; it != options.end(); ++it) {
            Kross::Api::InterpreterInfo::Option* option = it.data();

if(! d->properties->contains( it.key().latin1() )) {
            KoProperty::Property* prop = new KoProperty::Property(
                    it.key().latin1(), // id
                    option->value, // value
                    option->name, // caption
                    option->comment, // description
                    KoProperty::Auto, // type
                    & d->properties->property("language")
            );
}

        }
    }

    propertySetReloaded(true);
    //propertySetSwitched();
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
        kdDebug() << QString("KexiScriptDesignView::slotPropertyChanged() language=%1").arg(language) << endl;
        d->scriptaction->setInterpreterName( language );
        // We assume Kross and the HighlightingInterface are using same
        // names for the support languages...
        d->editor->setHighlightMode( language );

        updateProperties(); // update the properties
    }
    else {
        bool ok = d->scriptaction->getScriptContainer()->setOption( property.name(), property.value() );
        if(! ok) {
            kdWarning() << QString("KexiScriptDesignView::slotPropertyChanged() unknown property '%1'.").arg(property.name()) << endl;
            return;
        }
    }

    setDirty(true);
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
        kdDebug() << "KexiScriptDesignView::loadData() XML parsing error line: " << errLine << " col: " << errCol << " message: " << errMsg << endl;
        return false;
    }

    QDomElement scriptelem = domdoc.namedItem("script").toElement();
    if(scriptelem.isNull()) {
        kexipluginsdbg << "KexiScriptDesignView::loadData(): script domelement is null" << endl;
        return false;
    }

    QString language = scriptelem.attribute("language");
    if(! language.isEmpty())
        d->scriptaction->setInterpreterName(language);

    d->scriptaction->setCode( scriptelem.text() );

    return true;
}

KexiDB::SchemaData* KexiScriptDesignView::storeNewData(const KexiDB::SchemaData& sdata, bool &cancel)
{
    KexiDB::SchemaData *s = KexiViewBase::storeNewData(sdata, cancel);
    kexipluginsdbg << "KexiScriptDesignView::storeNewData(): new id:" << s->id() << endl;

    if(!s || cancel) {
        delete s;
        return 0;
    }

    if(! storeData()) {
        //failure: remove object's schema data to avoid garbage
        KexiDB::Connection *conn = parentDialog()->mainWin()->project()->dbConnection();
        conn->removeObject( s->id() );
        delete s;
        return 0;
    }

    return s;
}

tristate KexiScriptDesignView::storeData()
{
    kexipluginsdbg << "KexiScriptDesignView::storeData(): " << parentDialog()->partItem()->name() << " [" << parentDialog()->id() << "]" << endl;

    QDomDocument domdoc("script");
    QDomElement scriptelem = domdoc.createElement("script");
    domdoc.appendChild(scriptelem);

    scriptelem.setAttribute("language", d->scriptaction->getInterpreterName());

    QDomText scriptcode = domdoc.createTextNode(d->scriptaction->getCode());
    scriptelem.appendChild(scriptcode);

    return storeDataBlock( domdoc.toString() );
}

#include "kexiscriptdesignview.moc"

