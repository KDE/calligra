/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
   Copyright (C) 2004, 2005 Dag Andersen <danders@get2net.dk>

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

#include <qpainter.h>
#include <qfileinfo.h>

#include <kdebug.h>
#include <kconfig.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kcommand.h>
#include <koTemplateChooseDia.h>
#include <kocommandhistory.h>

#define CURRENT_SYNTAX_VERSION "0.1"

namespace KPlato
{

KPTPart::KPTPart(QWidget *parentWidget, const char *widgetName,
		 QObject *parent, const char *name, bool singleViewMode)
    : KoDocument(parentWidget, widgetName, parent, name, singleViewMode),
      m_project(0), m_projectDialog(0), m_view(0),
      m_context(0)
{
    m_update = m_calculate = false;
    m_commandHistory = new KoCommandHistory(actionCollection());

    setInstance(KPTFactory::global());
    m_config.setReadWrite(isReadWrite()|| !isEmbedded());
    m_config.load();

    m_project = new KPTProject(); // after config is loaded

    connect(m_commandHistory, SIGNAL(commandExecuted()), SLOT(slotCommandExecuted()));
    connect(m_commandHistory, SIGNAL(documentRestored()), SLOT(slotDocumentRestored()));

}


KPTPart::~KPTPart() {
    m_config.save();
    delete m_project;
    delete m_projectDialog;
    delete m_commandHistory;
}


bool KPTPart::initDoc(InitDocFlags flags, QWidget* parentWidget) {
    bool result = true;

    if (flags==KoDocument::InitDocEmpty)
    {
        m_project = new KPTProject();
        setAutoSave(0); // disable
        setModified(false);
        return true;
    }

    QString templateDoc;
    KoTemplateChooseDia::ReturnType ret;
    KoTemplateChooseDia::DialogType dlgtype;
    if (flags != KoDocument::InitDocFileNew )
	    dlgtype = KoTemplateChooseDia::Everything;
    else
	    dlgtype = KoTemplateChooseDia::OnlyTemplates;

    ret = KoTemplateChooseDia::choose(KPTFactory::global(), templateDoc,
				      dlgtype,
				      "kplato_template",
				      NULL);
    if (ret == KoTemplateChooseDia::Template) {
        resetURL();
        result = loadNativeFormat(templateDoc);
        if ( !result )
            showLoadingErrorDialog();
    } else if (ret == KoTemplateChooseDia::File) {
        KURL url(templateDoc);
        kdDebug() << "KPTPart::initDoc opening URL " << url.prettyURL() <<endl;
        result = openURL(url);
    } else if (ret == KoTemplateChooseDia::Empty) {
	// Make a fresh project and let the user enter some info
	m_project = new KPTProject();
	// an emty project should be empty
	// m_projectDialog = new KPTProjectDialog(*m_project, m_view);
	// m_projectDialog->exec();

	result = true;
    } else {
        result = false;
    }
    setAutoSave(0); // disable
    setModified(false);
    return result;
}


KoView *KPTPart::createViewInstance(QWidget *parent, const char *name) {
    m_view = new KPTView(this, parent, name);

    // If there is a project dialog this should be deleted so it will
    // use the m_view as parent. If the dialog will be needed again,
    // it will be made at that point
    if (m_projectDialog != 0) {
	kdDebug() << "Deleting m_projectDialog because of new ViewInstance\n";
	delete m_projectDialog;
	m_projectDialog = 0;
    }
    if (m_context)
        m_view->setContext(*m_context);

    m_view->setBaselineMode(getProject().isBaselined());
    return m_view;
}


void KPTPart::editProject() {
    if (m_projectDialog == 0)
	// Make the dialog
	m_projectDialog = new KPTProjectDialog(*m_project, m_view);

    m_projectDialog->exec();
}


bool KPTPart::loadXML(QIODevice *, const QDomDocument &document) {
    QTime dt;
    dt.start();
    emit sigProgress( 0 );

    QString value;
    QDomElement plan = document.documentElement();

    // Check if this is the right app
    value = plan.attribute("mime", QString::null);
    if (value.isEmpty()) {
        kdError() << "No mime type specified!" << endl;
        setErrorMessage(i18n("Invalid document. No mimetype specified."));
        return false;
    }
    else if (value != "application/x-vnd.kde.kplato") {
        kdError() << "Unknown mime type " << value << endl;
        setErrorMessage(i18n("Invalid document. Expected mimetype application/x-vnd.kde.kplato, got %1").arg(value));
        return false;
    }
    QString m_syntaxVersion = plan.attribute("version", CURRENT_SYNTAX_VERSION);
    if (m_syntaxVersion > CURRENT_SYNTAX_VERSION) {
        int ret = KMessageBox::warningContinueCancel(
            0, i18n("This document was created with a newer version of KPlato (syntax version: %1)\n"
                    "Opening it in this version of KPlato will lose some information.").arg(m_syntaxVersion),
            i18n("File-Format Mismatch"), i18n("Continue") );
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
        kdDebug() << "*** Error ***\n";
        kdDebug() << "  Children count should be 1 but is " << list.count()
                << "\n";
        return false;
    }

    for (unsigned int i = 0; i < list.count(); ++i) {
        if (list.item(i).isElement()) {
            QDomElement e = list.item(i).toElement();

            if (e.tagName() == "context") {
                delete m_context;
                m_context = new KPTContext();
                m_context->load(e);
            } else if (e.tagName() == "project") {
                KPTProject *newProject = new KPTProject();
                if (newProject->load(e)) {
                    // The load went fine. Throw out the old project
                    delete m_project;
                    m_project = newProject;
                    delete m_projectDialog;
                    m_projectDialog = 0;
                }
                else
                    delete newProject;
            }
        }
    }

    emit sigProgress(100); // the rest is only processing, not loading

    kdDebug() << "Loading took " << (float)(dt.elapsed()) / 1000 << " seconds" << endl;

    // do some sanity checking on document.
    emit sigProgress(-1);

    m_commandHistory->clear();
    m_commandHistory->documentSaved();
    setModified( false );
    return true;
}

QDomDocument KPTPart::saveXML() {
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
        m_context = new KPTContext();
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


void KPTPart::slotDocumentRestored() {
    //kdDebug()<<k_funcinfo<<endl;
    setModified(false);
}


void KPTPart::paintContent(QPainter &/*painter*/, const QRect &/*rect*/,
			   bool /*transparent*/,
			   double /*zoomX*/, double /*zoomY*/)
{
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


void KPTPart::addCommand(KCommand * cmd, bool execute)
{
    m_commandHistory->addCommand(cmd, execute);
}

void KPTPart::slotCommandExecuted() {
    //kdDebug()<<k_funcinfo<<endl;
    setModified(true);
    if (m_calculate)
        m_view->slotUpdate(config().behavior().calculationMode == KPTBehavior::OnChange);
    else if (m_update)
        m_view->slotUpdate(false);

    if (m_baseline)
        m_view->setBaselineMode(getProject().isBaselined());

    m_update = m_calculate = m_baseline = false;
}

void KPTPart::setCommandType(int type) {
    //kdDebug()<<k_funcinfo<<"type="<<type<<endl;
    if (type == 0)
        m_update = true;
    else if (type == 1)
        m_calculate = true;
    else if (type == 2)
        m_baseline = true;
}



}  //KPlato namespace

#include "kptpart.moc"
