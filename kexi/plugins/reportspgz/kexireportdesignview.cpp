/*
* Kexi Report Plugin
* Copyright (C) 2007-2009 by Adam Pigg (adam@piggz.co.uk)
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/


#include "kexireportdesignview.h"
#include <core/KexiMainWindowIface.h>
#include <kdebug.h>
#include <QScrollArea>
#include <core/KexiWindow.h>
#include "kexisourceselector.h"
#include <KPushButton>

KexiReportDesignView::KexiReportDesignView(QWidget *parent, KexiSourceSelector *s)
        : KexiView(parent)
{
    m_scrollArea = new QScrollArea(this);
    layout()->addWidget(m_scrollArea);
    m_sourceSelector = s;

    m_reportDesigner = 0;

    m_editCutAction = new KAction(KIcon("edit-cut"), i18n("Cut"), this);
    m_editCutAction->setObjectName("editcut");
    m_editCopyAction = new KAction(KIcon("edit-copy"), i18n("Copy"), this);
    m_editCopyAction->setObjectName("editcopy");
    m_editPasteAction = new KAction(KIcon("edit-paste"), i18n("Paste"), this);
    m_editPasteAction->setObjectName("editpaste");
    m_editDeleteAction = new KAction(KIcon("edit-delete"), i18n("Delete"), this);
    m_editDeleteAction->setObjectName("editdelete");

    m_sectionEdit = new KAction(i18n("Section Editor"), this);
    m_sectionEdit->setObjectName("sectionedit");

    m_itemRaiseAction = new KAction(KIcon("arrow-up"), i18n("Raise"), this);
    m_itemRaiseAction->setObjectName("itemraise");
    m_itemLowerAction = new KAction(KIcon("arrow-down"), i18n("Lower"), this);
    m_itemLowerAction->setObjectName("itemlower");
    //parameterEdit = new KAction ( i18n ( "Parameter Editor" ), this );
    //parameterEdit->setObjectName("parameteredit");
    QList<QAction*> al;
    KAction *sep = new KAction("", this);
    sep->setSeparator(true);

    al << m_editCutAction << m_editCopyAction << m_editPasteAction << m_editDeleteAction << sep << m_sectionEdit << sep << m_itemLowerAction << m_itemRaiseAction;
    setViewActions(al);

}

KexiReportDesignView::~KexiReportDesignView()
{
}

KoProperty::Set *KexiReportDesignView::propertySet()
{
    return m_reportDesigner->itemPropertySet();
}

void KexiReportDesignView::slotDesignerPropertySetChanged()
{
    propertySetReloaded(true);
    propertySetSwitched();
}

KexiDB::SchemaData* KexiReportDesignView::storeNewData(const KexiDB::SchemaData& sdata, bool &cancel)
{
    KexiDB::SchemaData *s = KexiView::storeNewData(sdata, cancel);
    kDebug() << "new id:" << s->id();

    if (!s || cancel) {
        delete s;
        return 0;
    }
    if (!storeData()) {
        //failure: remove object's schema data to avoid garbage
        KexiDB::Connection *conn = KexiMainWindowIface::global()->project()->dbConnection();
        conn->removeObject(s->id());
        delete s;
        return 0;
    }
    return s;

}

tristate KexiReportDesignView::storeData(bool dontAsk)
{
    Q_UNUSED(dontAsk);

    QDomDocument doc("kexireport");
    QDomElement root = doc.createElement("kexireport");
    QDomElement conndata = m_sourceSelector->connectionData();

    if (conndata.isNull())
        kDebug() << "Null conn data!";

    root.appendChild(m_reportDesigner->document());
    root.appendChild(conndata);
    doc.appendChild(root);

    QString src  = doc.toString();
    kDebug() << src;

    if (storeDataBlock(src, "pgzreport_layout")) {
        kDebug() << "Saved OK";
        setDirty(false);
        return true;
    } else {
        kDebug() << "NOT Saved OK";
    }

    return false;
}

tristate KexiReportDesignView::beforeSwitchTo(Kexi::ViewMode mode, bool &dontStore)
{
    kDebug() << mode;
    dontStore = true;
    if (m_reportDesigner && mode == Kexi::DataViewMode) {
        tempData()->reportDefinition = m_reportDesigner->document();
        tempData()->reportSchemaChangedInPreviousView = true;
    }
    return true;
}

tristate KexiReportDesignView::afterSwitchFrom(Kexi::ViewMode mode)
{
    Q_UNUSED(mode);

    kDebug() << tempData()->document;
    if (tempData()->document.isEmpty()) {
        m_reportDesigner = new ReportDesigner(this);
    } else {
        if (m_reportDesigner) {
            m_scrollArea->takeWidget();
            delete m_reportDesigner;
            m_reportDesigner = 0;
        }

        QDomDocument doc;
        doc.setContent(tempData()->document);
        QDomElement root = doc.documentElement();
        QDomElement korep = root.firstChildElement("report:content");
        QDomElement conn = root.firstChildElement("connection");
        if (!korep.isNull()) {
            m_reportDesigner = new ReportDesigner(this, korep);
            if (!conn.isNull()) {
                m_sourceSelector->setConnectionData(conn);
            }
        } else {
            kDebug() << "no koreport section";

            //TODO remove...just create a blank document
            //Temp - allow load old style report definitions (no data)
            root.setTagName("koreport");
            m_reportDesigner = new ReportDesigner(this, root);
        }
    }

    m_scrollArea->setWidget(m_reportDesigner);

    //plugSharedAction ( "edit_copy", _rd, SLOT ( slotEditCopy() ) );
    //plugSharedAction ( "edit_cut", _rd, SLOT ( slotEditCut() ) );
    //plugSharedAction ( "edit_paste", _rd, SLOT ( slotEditPaste() ) );
    //plugSharedAction ( "edit_delete", _rd, SLOT ( slotEditDelete() ) );

    connect(m_reportDesigner, SIGNAL(propertySetChanged()), this, SLOT(slotDesignerPropertySetChanged()));
    connect(m_reportDesigner, SIGNAL(dirty()), this, SLOT(setDirty()));

    //Edit Actions
    connect(m_editCutAction, SIGNAL(activated()), m_reportDesigner, SLOT(slotEditCut()));
    connect(m_editCopyAction, SIGNAL(activated()), m_reportDesigner, SLOT(slotEditCopy()));
    connect(m_editPasteAction, SIGNAL(activated()), m_reportDesigner, SLOT(slotEditPaste()));
    connect(m_editDeleteAction, SIGNAL(activated()), m_reportDesigner, SLOT(slotEditDelete()));

    connect(m_sectionEdit, SIGNAL(activated()), m_reportDesigner, SLOT(slotSectionEditor()));

    //Raise/Lower
    connect(m_itemRaiseAction, SIGNAL(activated()), m_reportDesigner, SLOT(slotRaiseSelected()));
    connect(m_itemLowerAction, SIGNAL(activated()), m_reportDesigner, SLOT(slotLowerSelected()));
    return true;
}

KexiReportPart::TempData* KexiReportDesignView::tempData() const
{
    return static_cast<KexiReportPart::TempData*>(window()->data());
}

void KexiReportDesignView::slotSetData(KoReportData* kodata)
{
    m_reportDesigner->setReportData(kodata);
}

void KexiReportDesignView::triggerAction(const QString &action)
{
    m_reportDesigner->slotItem(action);
}

