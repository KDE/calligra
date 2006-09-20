/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
   Copyright (C) 2004, 2005 Dag Andersen <danders@get2net.dk>
   Copyright (C) 2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>

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
 * Boston, MA 02110-1301, USA.
*/

#include "kptpart.h"
#include "kptview.h"
#include "kptfactory.h"
#include "kptproject.h"
#include "kptprojectdialog.h"
#include "kptresource.h"
#include "kptcontext.h"
#include "kptganttview.h"
#include "KDGanttViewTaskLink.h"

#include <qpainter.h>
#include <qfileinfo.h>
#include <QTimer>

#include <kdebug.h>
#include <kconfig.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kcommand.h>
#include <kcommand.h>
#include <KoGlobal.h>

#define CURRENT_SYNTAX_VERSION "0.5"

namespace KPlato
{

Part::Part(QWidget *parentWidget,
		 QObject *parent, bool singleViewMode)
    : KoDocument(parentWidget, parent, singleViewMode),
      m_project(0), m_projectDialog(0), m_parentWidget(parentWidget), m_view(0),
      m_embeddedGanttView(new GanttView(parentWidget)),
      m_embeddedContext(new Context()), m_embeddedContextInitialized(false),
      m_context(0), m_xmlLoader()
{
    m_update = m_calculate = false;
    m_commandHistory = new KCommandHistory(actionCollection());

    setInstance(Factory::global());
    setTemplateType("kplato_template");
    m_config.setReadWrite(isReadWrite()|| !isEmbedded());
    m_config.load();

    delete m_project;
    m_project = new Project(); // after config is loaded

    connect(m_commandHistory, SIGNAL(commandExecuted()), SLOT(slotCommandExecuted()));
    connect(m_commandHistory, SIGNAL(documentRestored()), SLOT(slotDocumentRestored()));

    //FIXME the following is really dirty, we should make KPlato::Context a real class
    //      with getter and setter and signals when content changes, thus we can keep track
    QTimer* timer = new QTimer(this,"context update timer");
    connect(timer,SIGNAL(timeout()),this,SLOT(slotCopyContextFromView()));
    timer->start(500);
}


Part::~Part() {
    m_config.save();
    delete m_commandHistory; // before project, in case of dependencies...
    delete m_project;
    delete m_projectDialog;
    if (m_embeddedGanttView)
      delete m_embeddedGanttView;
    if (m_embeddedContext)
      delete m_embeddedContext;
}


KoView *Part::createViewInstance(QWidget *parent) {
    m_view = new View(this, parent);
    connect(m_view,SIGNAL(destroyed()),this,SLOT(slotViewDestroyed()));

    // If there is a project dialog this should be deleted so it will
    // use the m_view as parent. If the dialog will be needed again,
    // it will be made at that point
    if (m_projectDialog != 0) {
	kDebug() << "Deleting m_projectDialog because of new ViewInstance\n";
	delete m_projectDialog;
	m_projectDialog = 0;
    }
    if (m_context)
        m_view->setContext( *m_context );
    else if (m_embeddedContext && m_embeddedContextInitialized)
        m_view->setContext( *m_embeddedContext );
    else {
        // Activate menu actions. Assumes ganttview, we don't get any
        // 'aboutToShow' signal. Need to redo action control.
        m_view->setTaskActionsEnabled(true);
    }
    //m_view->setBaselineMode(getProject().isBaselined()); FIXME: Removed for this release
    return m_view;
}


void Part::editProject() {

    QWidget* parent = m_parentWidget;
    if (m_view)
      parent = m_view;

    if (m_projectDialog == 0)
	// Make the dialog
	m_projectDialog = new ProjectDialog(*m_project, parent);

    m_projectDialog->exec();
}


bool Part::loadXML(QIODevice *, const QDomDocument &document) {
    QTime dt;
    dt.start();
    emit sigProgress( 0 );

    QString value;
    QDomElement plan = document.documentElement();

    // Check if this is the right app
    value = plan.attribute("mime", QString::null);
    if (value.isEmpty()) {
        kError() << "No mime type specified!" << endl;
        setErrorMessage(i18n("Invalid document. No mimetype specified."));
        return false;
    }
    else if (value != "application/x-vnd.kde.kplato") {
        kError() << "Unknown mime type " << value << endl;
        setErrorMessage(i18n("Invalid document. Expected mimetype application/x-vnd.kde.kplato, got %1", value));
        return false;
    }
    QString m_syntaxVersion = plan.attribute("version", CURRENT_SYNTAX_VERSION);
    if (m_syntaxVersion > CURRENT_SYNTAX_VERSION) {
        int ret = KMessageBox::warningContinueCancel(
            0, i18n("This document was created with a newer version of KPlato (syntax version: %1)\n"
                    "Opening it in this version of KPlato will lose some information.", m_syntaxVersion),
            i18n("File-Format Mismatch"), KGuiItem(i18n("Continue")) );
        if (ret == KMessageBox::Cancel)
        {
            setErrorMessage("USER_CANCELED");
            return false;
        }
    }
    emit sigProgress(5);

    QDomNodeList list = plan.childNodes();
    if (list.count() > 2) {
        // TODO: Make a proper bitching about this
        kDebug() << "*** Error ***\n";
        kDebug() << "  Children count should be 1 but is " << list.count()
                << "\n";
        return false;
    }
    m_xmlLoader.startLoad();
    for (unsigned int i = 0; i < list.count(); ++i) {
        if (list.item(i).isElement()) {
            QDomElement e = list.item(i).toElement();

            if (e.tagName() == "context") {
                delete m_context;
                m_context = new Context();
                m_context->load(e);
            } else if (e.tagName() == "project") {
                Project *newProject = new Project();
                if (newProject->load(e)) {
                    // The load went fine. Throw out the old project
                    delete m_project;
                    m_project = newProject;
                    delete m_projectDialog;
                    m_projectDialog = 0;
                }
                else {
                    delete newProject;
                    m_xmlLoader.addMsg(XMLLoaderObject::Errors, "Loading of project failed");
                    //TODO add some ui here
                }
            }
        }
    }
    m_xmlLoader.stopLoad();
    emit sigProgress(100); // the rest is only processing, not loading

    kDebug() << "Loading took " << (float)(dt.elapsed()) / 1000 << " seconds" << endl;

    // do some sanity checking on document.
    emit sigProgress(-1);

    m_commandHistory->clear();
    m_commandHistory->documentSaved();
    setModified( false );
    if (m_view)
        m_view->slotUpdate(false);
    return true;
}

QDomDocument Part::saveXML() {
    QDomDocument document("kplato");

    document.appendChild(document.createProcessingInstruction(
			"xml",
			"version=\"1.0\" encoding=\"UTF-8\""));

    QDomElement doc = document.createElement("kplato");
    doc.setAttribute("editor", "KPlato");
    doc.setAttribute("mime", "application/x-vnd.kde.kplato");
    doc.setAttribute("version", CURRENT_SYNTAX_VERSION);
    document.appendChild(doc);

    delete m_context;
    m_context = 0;
    if (m_view) {
        m_context = new Context();
        m_view->getContext(*m_context);
    }
    if (m_context) {
        m_context->save(doc);
    }
    // Save the project
    m_project->save(doc);

    m_commandHistory->documentSaved();
    return document;
}


void Part::slotDocumentRestored() {
    //kDebug()<<k_funcinfo<<endl;
    setModified(false);
}


void Part::paintContent(QPainter &painter, const QRect &rect,
			   bool /*transparent*/,
			   double zoomX, double /*zoomY*/)
{
    kDebug() << "----------- KPlato: Part::paintContent ------------" << endl;
    if (isEmbedded() && m_embeddedGanttView && m_project)
    {
        if (m_embeddedContext)
        {
            int ganttsize = m_embeddedContext->ganttview.ganttviewsize;
            int tasksize = m_embeddedContext->ganttview.taskviewsize;
            bool showtaskname = m_embeddedContext->ganttview.showTaskName;

//            m_embeddedContext->ganttview.ganttviewsize += m_embeddedContext->ganttview.taskviewsize;
//            m_embeddedContext->ganttview.taskviewsize = 0;  //TODO this doesn't have any effect?! (bug?)
            m_embeddedContext->ganttview.showTaskName = true;  //since task view is not shown(?), show name in the gantt itself

            m_embeddedGanttView->setContext( m_embeddedContext->ganttview, *m_project );

            m_embeddedContext->ganttview.ganttviewsize = ganttsize;
            m_embeddedContext->ganttview.taskviewsize = tasksize;
            m_embeddedContext->ganttview.showTaskName = showtaskname;
        }
        else
        {
            kWarning() << "Don't have any context to set!" << endl;
        }
        painter.setClipRect(rect);
        // We don't support zoom yet, so use the painters scaling
        double d_zoom = 1.0;
        setZoomAndResolution(100, KoGlobal::dpiX(), KoGlobal::dpiY());
        if ( m_zoomedResolutionX != zoomX ) {
            d_zoom *= ( zoomX / m_zoomedResolutionX );
            painter.scale(d_zoom, d_zoom);
        }

        m_embeddedGanttView->clear();
        m_embeddedGanttView->draw(*m_project);
        m_embeddedGanttView->drawOnPainter(&painter,rect);
    }
    // ####### handle transparency

    // Need to draw only the document rectangle described in the parameter rect.
//     int left = rect.left() / 20;
//     int right = rect.right() / 20 + 1;
//     int top = rect.top() / 20;
//     int bottom = rect.bottom() / 20 + 1;

//     for( int x = left; x < right; ++x )
//         painter.drawLine( x * 40, top * 20, 40 * 20, bottom * 20 );
//     for( int y = left; y < right; ++y )
//         painter.drawLine( left * 20, y * 20, right * 20, y * 20 );
}


void Part::addCommand(KCommand * cmd, bool execute)
{
    m_commandHistory->addCommand(cmd, execute);
}

void Part::slotCommandExecuted() {
    //kDebug()<<k_funcinfo<<endl;
    setModified(true);
    kDebug() << "------- KPlato, is embedded: " << isEmbedded() << endl;
    if (m_view == NULL)
      return;

    if (m_calculate)
        m_view->slotUpdate(false/*config().behavior().calculationMode == Behavior::OnChange*/);
    else if (m_update)
        m_view->slotUpdate(false);

    if (m_baseline)
        m_view->setBaselineMode(getProject().isBaselined());

    m_update = m_calculate = m_baseline = false;
}

void Part::slotCopyContextFromView()
{
    if (m_view)
    {
//         kDebug() << "Updating embedded context from view context." << endl;
        this->m_view->getContext( *m_embeddedContext );
        this->m_embeddedContextInitialized = true;
    }
//     else
//     {
//         kDebug() << "Not updating the context." << endl;
//         if (m_context)
//           kDebug() << "Current View: " << m_context->currentView << endl;
//     }
}

void Part::slotViewDestroyed()
{
    m_view = NULL;
}

void Part::setCommandType(int type) {
    //kDebug()<<k_funcinfo<<"type="<<type<<endl;
    if (type == 0)
        m_update = true;
    else if (type == 1)
        m_calculate = true;
    else if (type == 2)
        m_baseline = true;
}

void Part::generateWBS() {
    m_project->generateWBS(1, m_wbsDefinition);
}

}  //KPlato namespace

#include "kptpart.moc"
