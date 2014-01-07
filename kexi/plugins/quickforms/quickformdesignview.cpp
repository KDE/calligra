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

#include <QPlainTextEdit>
#include <QBoxLayout>
#include <QDomDocument>
#include <QDomElement>
#include <QDomText>

#include <kdebug.h>
#include <KexiMainWindowIface.h>
#include <db/connection.h>


QuickFormDesignView::QuickFormDesignView(QWidget* parent) : KexiView(parent)
{
    m_editor = new QPlainTextEdit("Some text", this);
    m_editor->setTabStopWidth(20);
    setViewWidget(m_editor);
    connect(m_editor, SIGNAL(textChanged()), this, SLOT(setDirty()));

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
    QDomElement scriptelem = domdoc.createElement("quickform");
    domdoc.appendChild(scriptelem);

    QDomText scriptcode = domdoc.createTextNode(m_editor->toPlainText());
    scriptelem.appendChild(scriptcode);

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

    QDomElement scriptelem = domdoc.namedItem("quickform").toElement();
    if (scriptelem.isNull()) {
        kDebug() << "quickform domelement is null";
        return false;
    }

    m_editor->setPlainText(scriptelem.text().toUtf8());

    return true;
}
