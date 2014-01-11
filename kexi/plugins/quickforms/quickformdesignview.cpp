/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright 2014  Adam Pigg <email>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "quickformdesignview.h"


#include <QBoxLayout>
#include <QDomDocument>
#include <QDomElement>
#include <QDomText>

#include <kdebug.h>
#include <KexiMainWindowIface.h>
#include <db/connection.h>
#include <core/kexiproject.h>
#include <widget/kexieditor.h>

QuickFormDesignView::QuickFormDesignView(QWidget* parent) : KexiView(parent)
{
    m_editor = new KexiEditor(this);
    //m_editor->setTabStopWidth(20);
    m_editor->setHighlightMode("qml");
    setViewWidget(m_editor);
    connect(m_editor, SIGNAL(textChanged()), this, SLOT(setDirty()));
    
    //Properties
    QStringList keys, strings;
    keys = strings = queryList();
    
    m_propertySet = new KoProperty::Set(0, "QuickForm");
    m_recordSource = new KoProperty::Property("record-source", keys, strings, "", i18n("Record Source"));
    
    m_propertySet->addProperty(m_recordSource);
    
    connect(m_propertySet, SIGNAL(propertyChanged(KoProperty::Set&, KoProperty::Property&)),
            this, SLOT(slotPropertyChanged(KoProperty::Set&, KoProperty::Property&)));
    
    loadData();
}

QuickFormDesignView::~QuickFormDesignView()
{

}

KexiDB::SchemaData* QuickFormDesignView::storeNewData(const KexiDB::SchemaData& sdata, KexiView::StoreNewDataOptions options, bool& cancel)
{
    KexiDB::SchemaData* s = KexiView::storeNewData(sdata, options, cancel);
    kDebug() << "new id:" << s->id();

    if (!s || cancel) {
        delete s;
        return 0;
    }

    if (!storeData()) {
        kWarning() << "Failed to store the data.";
        //failure: remove object's schema data to avoid garbage
        KexiDB::Connection* conn = KexiMainWindowIface::global()->project()->dbConnection();
        conn->removeObject(s->id());
        delete s;
        return 0;
    }

    return s;
}

tristate QuickFormDesignView::storeData(bool dontAsk)
{
    kDebug(); //<< window()->partItem()->name() << " [" << window()->id() << "]";

    QDomDocument domdoc("quickform");
    QDomElement rootelem = domdoc.createElement("quickform");
    domdoc.appendChild(rootelem);

    QDomElement formelem = domdoc.createElement("quickform:definition");
    
    QDomText scriptcode = domdoc.createTextNode(m_editor->text());
    formelem.appendChild(scriptcode);
    rootelem.appendChild(formelem);
    
    QDomElement connection = domdoc.createElement("quickform:connection");
    connection.setAttribute("record-source", m_recordSource->value().toString());
    rootelem.appendChild(connection);
    
    kDebug() << "Saving form" << domdoc.toString();
    
    return storeDataBlock(domdoc.toString());
}

bool QuickFormDesignView::loadData()
{
    QString data;
    if (! loadDataBlock(data)) {
        kDebug() << "no DataBlock";
        return false;
    }

    QString errMsg;
    int errLine;
    int errCol;

    QDomDocument domdoc;
    bool parsed = domdoc.setContent(data, false, &errMsg, &errLine, &errCol);

    if (! parsed) {
        kDebug() << "XML parsing error line: " << errLine << " col: " << errCol << " message: " << errMsg;
        return false;
    }

    QDomElement rootelem = domdoc.namedItem("quickform").toElement();
    if (rootelem.isNull()) {
        kDebug() << "quickform root is null";
        return false;
    }
    
    QDomElement formelem = rootelem.namedItem("quickform:definition").toElement();
    if (rootelem.isNull()) {
        kDebug() << "quickform root is null";
        return false;
    }

    m_editor->setText(formelem.text());

    QDomElement cnxn = rootelem.namedItem("quickform:connection").toElement();
    if (cnxn.isNull()) {
        kDebug() << "quickform connection is null";
        return false;
    }
    
    m_recordSource->setValue(cnxn.attribute("record-source"));
    
    return true;
}

KoProperty::Set* QuickFormDesignView::propertySet()
{
    return m_propertySet;
}

QStringList QuickFormDesignView::queryList(){
    //Get the list of queries in the database
    QStringList qs;
    KexiDB::Connection *conn = KexiMainWindowIface::global()->project()->dbConnection();
    
    if (conn && conn->isConnected()) {
        QList<int> tids = conn->tableIds();
        qs << "";
        for (int i = 0; i < tids.size(); ++i) {
            KexiDB::TableSchema* tsc = conn->tableSchema(tids[i]);
            if (tsc)
                qs << tsc->name();
        }
        
        QList<int> qids = conn->queryIds();
        qs << "";
        for (int i = 0; i < qids.size(); ++i) {
            KexiDB::QuerySchema* qsc = conn->querySchema(qids[i]);
            if (qsc)
                qs << qsc->name();
        }
    }
    
    return qs;
}

void QuickFormDesignView::slotPropertyChanged(KoProperty::Set&, KoProperty::Property&){
        setDirty();
}