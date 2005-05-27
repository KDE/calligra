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
#include "kexiscriptmanager.h"

#include <qlayout.h>
#include <qdom.h>
#include <kdebug.h>

#include <kexidialogbase.h>
#include <kexidb/connection.h>

KexiScriptDesignView::KexiScriptDesignView(KexiScriptManager* manager, KexiMainWindow *mainWin, QWidget *parent, const char *name)
    : KexiViewBase(mainWin, parent, name)
    , m_manager(manager)
{
    m_scriptcontainer = m_manager->getScriptContainer( parentDialog()->partItem()->name() );
    plugSharedAction( "script_execute", m_scriptcontainer, SLOT(execute()) );

    QBoxLayout* layout = new QVBoxLayout(this);
    m_editor = new KexiScriptEditor(mainWin, this, "ScriptEditor");
    addChildView((KexiViewBase*)m_editor);
    setViewWidget((KexiViewBase*)m_editor);
    layout->addWidget((KexiViewBase*)m_editor);

    loadData();
    m_editor->initialize(m_scriptcontainer);
}

KexiScriptDesignView::~KexiScriptDesignView()
{
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

    m_scriptcontainer->setInterpreterName( scriptelem.attribute("language") );
    m_scriptcontainer->setCode( scriptelem.text() );
    //m_editor->initialize(m_scriptcontainer); //FIXME clear prev states...

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

    scriptelem.setAttribute("language", m_scriptcontainer->getInterpreterName());

    QDomText scriptcode = domdoc.createTextNode(m_scriptcontainer->getCode());
    scriptelem.appendChild(scriptcode);

    return storeDataBlock( domdoc.toString() );
}

#include "kexiscriptdesignview.moc"

