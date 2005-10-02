/* This file is part of the KDE project
   Copyright (C) 2003 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kptreportview.h"

#include "kptview.h"
#include "kptpart.h"
#include "kptcontext.h"

#include <mreportviewer.h>

#include <koStoreDevice.h>

#include <kdebug.h>
#include <kaction.h>
#include <kstdaction.h>
#include <ktoolbar.h>
#include <kstandarddirs.h>
#include <kurl.h>
#include <kmessagebox.h>
#include <kio/netaccess.h>
#include <klocale.h>
#include <kglobal.h>

#include <qfile.h>
#include <qfileinfo.h>
#include <qpopupmenu.h>
#include <qlayout.h>
#include <qdom.h>
#include <qstringlist.h>

namespace KPlato
{

class ReportTagsPrivate {
public:

    ReportTagsPrivate()
    :   m_project(0),
        m_task(0),
        m_resourcegroup(0),
        m_resource(0),
        alltasksLevel("-1"),
        summarytasksLevel("-1"),
        tasksLevel("-1"),
        milestonesLevel("-1"),
        resourcegroupsLevel("-1"),
        resourcesLevel("-1")
    {}

    ~ReportTagsPrivate() {}

    QString getData(QString source, QString tag) const {
        if (tag.contains("."))
            return getData(tag);

        return getData(source + "." + tag);
    }

    QString getData(QString tag) const {
        if (!tag.contains('.'))
            return QString::null;

        if (tag.section(".", 0, 0) == "project") {
            if (tag.section(".", 1, 1) == "name")
                return (m_project ? m_project->name() : QString::null);
            if (tag.section(".", 1, 1) == "leader")
                return (m_project ? m_project->leader() : QString::null);

        } else if (tag.section(".", 0, 0) == "task") {
            if (tag.section(".", 1, 1) == "name")
                return (m_task ? m_task->name() : QString::null);
            else if (tag.section(".", 1, 1) == "start")
                return (m_task ? m_task->startTime().toString() : QString::null);
            else if (tag.section(".", 1, 1) == "starttime")
                return (m_task ? m_task->startTime().time().toString() : QString::null);
            else if (tag.section(".", 1, 1) == "startdate")
                return (m_task ? m_task->startTime().date().toString() : QString::null);
            else if (tag.section(".", 1, 1) == "duration") {
                if (m_task) {
                    KPTDuration *d = m_task->getExpectedDuration();
                    QString s = d->toString(KPTDuration::Format_Hour);
                    delete d;
                    return s;
                }
                return QString::null;
            } else if (tag.section(".", 1, 1) == "plannedcost")
                return (m_task ? KGlobal::locale()->formatMoney(m_task->plannedCost()) : QString::null);

        } else if (tag.section(".", 0, 0) == "resource") {
            if (tag.section(".", 1, 1) == "name")
                return (m_resource ? m_resource->name() : QString::null);
            if (tag.section(".", 1, 1) == "normalrate")
                return (m_resource ? KGlobal::locale()->formatMoney(m_resource->normalRate()) : QString::null);
            if (tag.section(".", 1, 1) == "overtimerate")
                return (m_resource ? KGlobal::locale()->formatMoney(m_resource->overtimeRate()) : QString::null);
            if (tag.section(".", 1, 1) == "fixedcost")
                return (m_resource ? KGlobal::locale()->formatMoney(m_resource->fixedCost()) : QString::null);
        }
	return QString::null;
    }

    KPTProject *m_project;
    KPTTask *m_task;
    KPTResourceGroup *m_resourcegroup;
    KPTResource *m_resource;

    QString alltasksLevel;
    QStringList alltasksProps;
    QString summarytasksLevel;
    QStringList summarytasksProps;
    QString tasksLevel;
    QStringList tasksProps;
    QString milestonesLevel;
    QStringList milestonesProps;
    QString resourcegroupsLevel;
    QStringList resourcegroupsProps;
    QString resourcesLevel;
    QStringList resourcesProps;

};


KPTReportView::KPTReportView(KPTView *view, QWidget *parent)
    : QWidget(parent, "Report view"),
    m_mainview(view),
    m_reportTags(0)
{
    //kdDebug()<<k_funcinfo<<endl;
    QVBoxLayout *layout = new QVBoxLayout(this);
    m_reportview = new Kugar::MReportViewer(this);
    layout->add(m_reportview);

	//connect(m_reportview,SIGNAL(preferedTemplate(const QString &)), SLOT(slotPreferredTemplate(const QString &)));

	//setCentralWidget(m_reportview);

	// Create the user interface.
	//KStdAction::print(this,SLOT(slotPrint()),actionCollection());
	//KStdAction::quit(this,SLOT(slotFileQuit()),actionCollection());

//	KStdAction::showToolbar(this,SLOT(slotViewToolBar()),actionCollection());
//	KStdAction::showStatusbar(this,SLOT(slotViewStatusBar()),actionCollection());

// 	statusBar();

//	createGUI();

}


 KPTReportView::~KPTReportView() {
   //safe
   delete m_reportTags;
}

void KPTReportView::draw(const QString &report) {
    //kdDebug()<<k_funcinfo<<endl;
    m_reportview->clearReport();
    m_reportTags = new ReportTagsPrivate();
    getTemplateFile(report);
    m_reportview->setReportTemplate(templateDoc.toString());
    setReportData();
    m_reportview->renderReport();
    m_reportview->show();
    delete m_reportTags;
    m_reportTags=0L;
}

void KPTReportView::print(KPrinter &printer) {
    //kdDebug()<<k_funcinfo<<endl;
	m_reportview->printReport(printer);
}

// Generate report data based on info from the template file
void KPTReportView::setReportData() {
    QString s = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    s+="<KugarData>\n";
    s += setReportDetail();
    s+="</KugarData>\n";
    //kdDebug()<<s<<endl;
    m_reportview->setReportData(s);
}

QString KPTReportView::setReportDetail() {
    //kdDebug()<<k_funcinfo<<endl;
    QString s;
    if (m_reportTags->alltasksLevel != "-1") {
        //kdDebug()<<k_funcinfo<<"alltasks level="<<m_reportTags->alltasksLevel<<endl;
        if (m_reportTags->summarytasksLevel == "-1") {
            m_reportTags->summarytasksLevel = m_reportTags->alltasksLevel;
            m_reportTags->summarytasksProps = m_reportTags->alltasksProps;
        }
        if (m_reportTags->tasksLevel == "-1") {
            m_reportTags->tasksLevel = m_reportTags->alltasksLevel;
            m_reportTags->tasksProps = m_reportTags->alltasksProps;
        }
        if (m_reportTags->milestonesLevel == "-1") {
            m_reportTags->milestonesLevel = m_reportTags->alltasksLevel;
            m_reportTags->milestonesProps = m_reportTags->alltasksProps;
        }
        QPtrListIterator<KPTNode> it(mainView()->getProject().childNodeIterator());
        for (; it.current(); ++it) {
            s += setTaskDetail(it.current());
        }

    } else if (m_reportTags->summarytasksLevel == "0") {
        // make a report that has summarytasks as starting points
        QPtrListIterator<KPTNode> it(mainView()->getProject().childNodeIterator());
        for (; it.current(); ++it) {
            if (it.current()->type() == KPTNode::Type_Summarytask) {
                s += setTaskDetail(it.current());
                // Now do subtasks
                s+= setTaskChildren(it.current());
            }
        }

    } else if (m_reportTags->tasksLevel == "0") {
        // make a report that has tasks as starting points
        QPtrListIterator<KPTNode> it(mainView()->getProject().childNodeIterator());
        for (; it.current(); ++it) {
            if (it.current()->type() == KPTNode::Type_Task) {
                s += setTaskDetail(it.current());
            }
            if (it.current()->type() == KPTNode::Type_Summarytask) {
                s+= setTaskChildren(it.current());
                if (m_reportTags->summarytasksLevel != "-1") {
                    s += setTaskDetail(it.current());
                }
            }
        }

    } else if (m_reportTags->milestonesLevel == "0") {

    } else if (m_reportTags->resourcegroupsLevel == "0") {

    } else if (m_reportTags->resourcesLevel == "0") {
        // make a report that has resources as starting points
        QPtrListIterator<KPTResourceGroup> it(mainView()->getProject().resourceGroups());
        for (; it.current(); ++it) {
            QPtrListIterator<KPTResource> rit(it.current()->resources());
            for (; rit.current(); ++rit) {
                s += setResourceDetail(rit.current());
            }
        }
    }
    //kdDebug()<<k_funcinfo<<s<<endl;
    return s;
}

QString KPTReportView::setResourceDetail(KPTResource *res) {
    //kdDebug()<<k_funcinfo<<endl;
    QString s;
    if (m_reportTags->resourcesLevel != "-1") {
        m_reportTags->m_resource = res;;
        s = setDetail("resource", m_reportTags->resourcesProps, m_reportTags->resourcesLevel);
    }
    return s;
}

QString KPTReportView::setTaskChildren(KPTNode *node) {
    //kdDebug()<<k_funcinfo<<endl;
    QString s;
    QPtrListIterator<KPTNode> it(node->childNodeIterator());
    for (; it.current(); ++it) {
        s += setTaskDetail(it.current());
        if (node->type() == KPTNode::Type_Summarytask)
            s+= setTaskChildren(it.current());
    }
    return s;
}

QString KPTReportView::setTaskDetail(KPTNode *node) {
    //kdDebug()<<k_funcinfo<<endl;
    QString s;
    QStringList props;
    QString level = "-1";
    if (node->type() == KPTNode::Type_Task) {
        props = m_reportTags->tasksProps;
        level = m_reportTags->tasksLevel;
    } else if (node->type() == KPTNode::Type_Summarytask) {
        props = m_reportTags->summarytasksProps;
        level = m_reportTags->summarytasksLevel;
    } else if (node->type() == KPTNode::Type_Milestone) {
        props = m_reportTags->milestonesProps;
        level = m_reportTags->milestonesLevel;
    }
    if (level != "-1") {
        m_reportTags->m_task = static_cast<KPTTask *>(node);
        s = setDetail("task", props, level);
    }
    return s;
}

QString KPTReportView::setDetail(const QString & source, QStringList &properties, QString &level) {
    QString s = "<Row";
    s += " level=\"" + level + "\"";
    for (unsigned int i=0; i < properties.count(); ++i) {
        //kdDebug()<<k_funcinfo<<"Property: "<<properties[i]<<endl;
        s += " " + properties[i].section('=', 0, 0) + "="; // Field
        QString data = m_reportTags->getData(source, properties[i].section('=', 1, 1));
        if (data.isNull())
            data = "";
        data = data.replace('<', "&lt;");
        data = data.replace('>', "&gt;");
        s += "\"" + data + "\""; // Property
        //kdDebug()<<k_funcinfo<<s<<endl;
    }
    s += "/>\n";
    return s;
}

// Most of this is from KoDocument::loadNativeFormat
void KPTReportView::openTemplateFile(const QString &file) {
    if (!QFileInfo(file).isFile()) {
        KMessageBox::sorry( this, i18n("Cannot find report template file!"),
                                                        i18n("Generate Report"));
        return;
    }
    QFile in;
    in.setName(file);
    if (!in.open(IO_ReadOnly)) {
        KMessageBox::sorry( this, i18n("Cannot open report template file!"),
                                                        i18n("Generate Report"));
        return;
    }
    // Try to find out whether it is a mime multi part file
    char buf[5];
    if ( in.readBlock( buf, 4 ) < 4 )
    {
        in.close();
        KMessageBox::sorry( this, i18n("Cannot read report template file!"),
                                                        i18n("Generate Report"));
        return;
    }

    if (strncasecmp( buf, "<?xm", 4 ) == 0) { // xml file?
        in.at(0);
        // fake
        //m_reportview->setReportTemplate(&in);
        loadTemplate(in);
        in.close();
        return;
    }
    in.close();
    KoStore* store=KoStore::createStore(file, KoStore::Read);
    if (!store)
    {
        KMessageBox::sorry( this, i18n("Cannot open report template file!"),
                                                    i18n("Generate Report"));
        return;
    }
    bool b = store->open("maindoc.xml");
    if ( !b )
    {
        KMessageBox::sorry( this, i18n("Cannot find the proper report template file!"),
                                                    i18n("Generate Report"));
        delete store;
        return;
    }
    loadTemplate(*(store->device()));
    store->close();
}

void KPTReportView::loadTemplate(QIODevice &dev) {
    QString errorMsg;
    int errorLine;
    int errorColumn;
    if (!templateDoc.setContent( &dev , &errorMsg, &errorLine, &errorColumn)) {
        QString msg = "Parsing template file failed with ";
        KMessageBox::sorry( this, msg + errorMsg, i18n("Generate Report"));
        return;
    }
    loadTemplate(templateDoc);
}

void KPTReportView::loadTemplate(QDomDocument &doc) {
    QDomNode tpl;
    QDomNode child;
    for (tpl = doc.firstChild(); !tpl.isNull(); tpl = tpl.nextSibling())
        if (tpl.nodeName() == "KugarTemplate")
            break;

    if (tpl.isNull())
        return;

    m_reportTags->m_project = &(mainView()->getPart()->getProject());
    // Get all the child report elements
    QDomNodeList children = tpl.childNodes();
    int childCount = children.length();

    for(int j = 0; j < childCount; j++){
        child = children.item(j);
        if(child.nodeType() == QDomNode::ElementNode) {
            QDomElement e = child.toElement();
            //kdDebug()<<child.nodeName()<<endl;
            // Report Header
            if(child.nodeName() == "ReportHeader") {
                handleHeader(child);
            } else if (child.nodeName() == "PageHeader") {
                handleHeader(child);
            } else if(child.nodeName() == "DetailHeader") {
                handleDetailHeader(child);
            }
            else if(child.nodeName() == "Detail")
            {
                handleDetail(e);
            }
            else if(child.nodeName() == "DetailFooter")
            {
                handleDetailHeader(child);
            }
            else if(child.nodeName() == "PageFooter")
                handleHeader(child);
            else if(child.nodeName() == "ReportFooter")
                handleHeader(child);
            }
    }
}

void KPTReportView::handleHeader(QDomNode &node) {
    QDomNode child;
    QDomNodeList children = node.childNodes();
    int childCount = children.length();
    for (int j = 0; j < childCount; j++) {
        child = children.item(j);
        if (child.nodeName() == "Label") {
            QDomNode n = child.attributes().namedItem("Text");
            QString s = n.nodeValue();
            QString r = s;
            int i = 0, j = 0;
            do {
                i = j;
                if ( ((i = s.find('[', i)) != -1) && ((j = s.find(']', i+1)) != -1) ) {
                    QString tag = s.mid(i, j-i+1);
                    QString data = m_reportTags->getData(tag.mid(1, tag.length()-2));
                    r = r.replace(tag, data);
                }
            } while (i != -1 && j != -1);
            n.setNodeValue(r);
            //kdDebug()<<" new Text="<<n.nodeValue()<<endl;
        }
    }
}

void KPTReportView::handleDetailHeader(QDomNode &node) {
    QDomNode child;
    QDomNodeList children = node.childNodes();
    int childCount = children.length();
    for (int j = 0; j < childCount; j++) {
        child = children.item(j);
        if (child.nodeName() == "Label") {
//            replaceTags(child.attributes().namedItem("Text"));
        }
    }
}

QStringList KPTReportView::getProperties(QDomElement &elem) {
    QStringList props;
    QDomNodeList list(elem.childNodes());
    int childCount = list.length();
    for (int j = 0; j < childCount; j++) {
        QDomNode child = list.item(j);
        if (child.nodeName() == "Field") {
            props.append(child.attributes().namedItem("Field").nodeValue()+"="+child.attributes().namedItem("Property").nodeValue());
        }
    }
    return props;
}

void KPTReportView::handleDetail(QDomElement &elem) {

    QString source = elem.attribute("SelectFrom");
    if (source.isNull())
        return;

    QStringList list = QStringList::split(" ", source);
    QStringList::iterator it = list.begin();
    for (; it != list.end(); ++it) {
        if ((*it) == "alltasks") {
            m_reportTags->alltasksLevel = elem.attribute("Level", "-1");
            m_reportTags->alltasksProps = getProperties(elem);
        }
        if ((*it) == "summarytasks") {
            m_reportTags->summarytasksLevel = elem.attribute("Level", "-1");
            m_reportTags->summarytasksProps = getProperties(elem);
        }
        if ((*it) == "tasks") {
            m_reportTags->tasksLevel = elem.attribute("Level", "-1");
            m_reportTags->tasksProps = getProperties(elem);
        }
        if ((*it) == "milestones") {
            m_reportTags->milestonesLevel = elem.attribute("Level", "-1");
            m_reportTags->milestonesProps = getProperties(elem);
        }
        if ((*it) == "resourcegroups") {
            m_reportTags->resourcegroupsLevel = elem.attribute("Level", "-1");
            m_reportTags->resourcegroupsProps = getProperties(elem);
        }
        if ((*it) == "resources") {
            m_reportTags->resourcesLevel = elem.attribute("Level", "-1");
            m_reportTags->resourcesProps = getProperties(elem);
        }
    }
}

void KPTReportView::replaceTags(QDomNode &node) {
    if (node.isNull())
        return;
}

void KPTReportView::getTemplateFile(const QString &tpl) {

	KURL url(tpl);
	QString localtpl;
	bool isTemp = false;

	if (!url.isValid())
	{
			KMessageBox::sorry(this,i18n("Malformed template filename: %1").arg(url.prettyURL()));
	}
	else
	{
		if (KIO::NetAccess::download(url,localtpl,this))
			isTemp = true;
		else
			KMessageBox::sorry(this,i18n("Unable to download template file: %1").arg(url.prettyURL()));
	}

	if (!localtpl.isNull())
	{
        openTemplateFile(localtpl);
		if (isTemp)
			KIO::NetAccess::removeTempFile(localtpl);
	}
}

void KPTReportView::slotFirstPage() {
    m_reportview->slotFirstPage();
}

void KPTReportView::slotNextPage() {
    m_reportview->slotNextPage();
}

void KPTReportView::slotPrevPage() {
    m_reportview->slotPrevPage();
}

void KPTReportView::slotLastPage() {
    m_reportview->slotLastPage();
}

bool KPTReportView::setContext(KPTContext &context) {
    kdDebug()<<k_funcinfo<<endl;
    return true;
}

void KPTReportView::getContext(KPTContext &context) const {
    kdDebug()<<k_funcinfo<<endl;
}

}  //KPlato namespace

#include "kptreportview.moc"
