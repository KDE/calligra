/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kptpart.h"
#include "kptview.h"
#include "kptfactory.h"
#include "kptproject.h"
#include "kptprojectdialog.h"
#include "kptresource.h"

#include <qpainter.h>
#include <qfileinfo.h>
#include <kdebug.h>
#include <klocale.h>
#include <kcommand.h>
#include <kstandarddirs.h>
#include <koTemplateChooseDia.h>


KPTPart::KPTPart(QWidget *parentWidget, const char *widgetName,
		 QObject *parent, const char *name, bool singleViewMode)
    : KoDocument(parentWidget, widgetName, parent, name, singleViewMode),
      m_project(0), m_projectDialog(0), m_view(0)
{
    m_project = new KPTProject();
    m_commandHistory = new KCommandHistory(actionCollection());

    setInstance(KPTFactory::global());
}


KPTPart::~KPTPart() {
    delete m_project;
    delete m_projectDialog;
}


bool KPTPart::initDoc() {
    bool result = true;
    QString templateDoc;
    KoTemplateChooseDia::ReturnType ret;
    KoTemplateChooseDia::DialogType dlgtype;
    if (initDocFlags() != KoDocument::InitDocFileNew )
	    dlgtype = KoTemplateChooseDia::Everything;
    else
	    dlgtype = KoTemplateChooseDia::OnlyTemplates;

    ret = KoTemplateChooseDia::choose(KPTFactory::global(), templateDoc,
				      "application/x-vnd.kde.kplato", "*.kplato",
				      i18n("KPlato"), dlgtype,
				      "kplato_template");
    if (ret == KoTemplateChooseDia::Template) {
        QFileInfo fileInfo(templateDoc);
        QString fileName(fileInfo.dirPath(true) + "/" + fileInfo.baseName()
			 + ".kplatot" );
        resetURL();
        result = loadNativeFormat(fileName);
    } else if (ret == KoTemplateChooseDia::File) {
        KURL url(templateDoc);
        kdDebug() << "KPTPart::initDoc opening URL " << url.prettyURL() <<endl;
        result = openURL(url);
    } else if (ret == KoTemplateChooseDia::Empty) {
	// Make a fresh project and let the user enter some info
	m_project = new KPTProject();
	m_projectDialog = new KPTProjectDialog(*m_project, m_view);
	m_projectDialog->exec();

	result = true;
    }

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
    return m_view;
}


void KPTPart::editProject() {
    if (m_projectDialog == 0)
	// Make the dialog
	m_projectDialog = new KPTProjectDialog(*m_project, m_view);

    m_projectDialog->exec();
}


bool KPTPart::loadXML(QIODevice *, const QDomDocument &document) {
    kdDebug() << "Loading document\n";

    QDomElement doc = document.documentElement();

    // Check if this is the right app
    if (doc.attribute("mime") != "application/x-vnd.kde.kplato")
	return false;

    QDomNodeList list = doc.childNodes();
    if (list.count() > 1) {
	// TODO: Make a proper bitching about this
	kdDebug() << "*** Error ***\n";
	kdDebug() << "  Children count should be 1 but is " << list.count()
		  << "\n";
	return false;
    }

    for (unsigned int i=0; i<list.count(); ++i) {
	if (list.item(i).isElement()) {
	    QDomElement e = list.item(i).toElement();

	    if(e.tagName() == "project") {
		KPTProject *newProject = new KPTProject();
		if (newProject->load(e)) {
            newProject->completeLoad(newProject); // Now fix relations etc.
		    // The load went fine. Throw out the old project
		    delete m_project;
		    delete m_projectDialog;
		    m_project = newProject;
		    m_projectDialog = 0;
		}
	    }
	}
    }

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
    doc.setAttribute("version", "0.1");
    document.appendChild(doc);

    // Save the project
    m_project->save(doc);

    return document;
}


void KPTPart::slotDocumentRestored() {
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


#include "kptpart.moc"
