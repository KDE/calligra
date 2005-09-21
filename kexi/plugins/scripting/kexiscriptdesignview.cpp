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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kexiscriptdesignview.h"
#include "kexiscripteditor.h"

#include <qlayout.h>
#include <qdom.h>
#include <kdebug.h>

#include <kexidialogbase.h>
#include <kexidb/connection.h>
#include <kexiscripting.h>

/// @internal
class KexiScriptDesignViewPrivate
{
    public:
        /// The \a KexiScriptContainer used as facade for the Kross functionality.
        KexiScriptContainer* scriptcontainer;

        /// The \a KoProperty::Set used in the propertyeditor.
        KoProperty::Set* propertyset;

        /// The \a KexiScriptEditor to edit the scripting code.
        KexiScriptEditor* editor;
};

KexiScriptDesignView::KexiScriptDesignView(KexiMainWindow *mainWin, QWidget *parent, const char *name)
    : KexiViewBase(mainWin, parent, name)
    , d( new KexiScriptDesignViewPrivate() )
{
    KexiScriptManager* scriptmanager = KexiScriptManager::self(mainWin);

    d->scriptcontainer = scriptmanager->getScriptContainer(parentDialog()->partItem()->name(), true);
    plugSharedAction( "script_execute", d->scriptcontainer, SLOT(execute()) );

    d->propertyset = new KoProperty::Set(this, "KexiScripting");

    QStringList interpreters = scriptmanager->getInterpreters();
    KoProperty::Property::ListData* proplist = new KoProperty::Property::ListData(interpreters, interpreters);
    KoProperty::Property* prop = new KoProperty::Property(
        "language", // name
        proplist, // ListData
        d->scriptcontainer->getInterpreterName(), // value
        i18n("Interpreter"), // caption
        i18n("The used scripting interpreter."), // description
        KoProperty::List // type
    );
    //prop->setVisible(false);
    d->propertyset->addProperty(prop);

    QBoxLayout* layout = new QVBoxLayout(this);
    d->editor = new KexiScriptEditor(mainWin, this, "ScriptEditor");
    addChildView((KexiViewBase*)d->editor);
    setViewWidget((KexiViewBase*)d->editor);
    layout->addWidget((KexiViewBase*)d->editor);

    if(KexiEditor::isAdvancedEditor()) // the configeditor is only in advanced mode avaiable.
        plugSharedAction( "script_config_editor", d->editor, SLOT(slotConfigureEditor()) );

    loadData();

    connect(d->propertyset, SIGNAL( propertyChanged(KoProperty::Set&, KoProperty::Property&) ),
            this, SLOT( slotPropertyChanged(KoProperty::Set&, KoProperty::Property&) ) );

    d->editor->initialize(d->scriptcontainer);
}

KexiScriptDesignView::~KexiScriptDesignView()
{
    delete d;
}

KoProperty::Set* KexiScriptDesignView::propertySet()
{
    return d->propertyset;
}

void KexiScriptDesignView::slotPropertyChanged(KoProperty::Set& set, KoProperty::Property& property)
{
    if(property.name() == "language") {
        QString language = property.value().toString();
        kdDebug() << QString("KexiScriptDesignView::slotPropertyChanged() language=%1").arg(language) << endl;
        d->scriptcontainer->setInterpreterName( language );
        d->editor->updateHighlightMode();
        setDirty(true);
    }
    else {
        kdWarning() << QString("KexiScriptDesignView::slotPropertyChanged() unknown property '%1'.").arg(property.name()) << endl;
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
        d->scriptcontainer->setInterpreterName(language);

    d->scriptcontainer->setCode( scriptelem.text() );
    //d->editor->initialize(d->scriptcontainer); //FIXME clear prev states...

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

    scriptelem.setAttribute("language", d->scriptcontainer->getInterpreterName());

    QDomText scriptcode = domdoc.createTextNode(d->scriptcontainer->getCode());
    scriptelem.appendChild(scriptcode);

    return storeDataBlock( domdoc.toString() );
}

#include "kexiscriptdesignview.moc"

