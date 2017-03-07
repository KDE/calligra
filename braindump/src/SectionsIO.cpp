/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "SectionsIO.h"

#include <QDomDocument>
#include <QFileInfo>
#include <QTimer>
#include <QDebug>

#include <KoStore.h>
#include <KoOdf.h>
#include <KoOdfWriteStore.h>
#include <KoEmbeddedDocumentSaver.h>
#include <KoGenStyles.h>
#include <KoShapeSavingContext.h>
#include <KoXmlWriter.h>
#include <KoOdfReadStore.h>
#include <KoXmlNS.h>
#include <KoShapeLoadingContext.h>
#include <KoOdfLoadingContext.h>

#include "RootSection.h"
#include "SectionGroup.h"
#include "Section.h"
#include "SectionContainer.h"
#include "Layout.h"
#include "LayoutFactoryRegistry.h"
#include "Xml.h"

SectionsIO::SectionsIO(RootSection* rootSection) : m_rootSection(rootSection), m_timer(new QTimer(this)), m_nextNumber(0)
{
    m_timer->start(60 * 1000); // Every minute
    connect(m_timer, SIGNAL(timeout()), SLOT(save()));
    m_directory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/sections/";
    QDir().mkdir(m_directory);

    // Finally load
    load();
}

SectionsIO::~SectionsIO()
{
}

void SectionsIO::push(Section* _section, PushMode _pushMode)
{
    if(!m_sectionsToSave.contains(_section)) {
        m_sectionsToSave.push_back(_section);
    }
    if(_pushMode == RecursivePush) {
        foreach(Section * sec, _section->sections()) {
            push(sec, RecursivePush);
        }
    }
}

struct SectionsIO::SaveContext {
    enum Version {
        VERSION_1
    };
    Section* section;
    QString filename;
    bool saveSection(SectionsIO* sectionsIO);
    bool loadSection(SectionsIO* sectionsIO, Version version);
};

bool SectionsIO::SaveContext::saveSection(SectionsIO* sectionsIO)
{
    struct Finally {
        Finally(KoStore *s) : store(s) { }
        ~Finally() {
            delete store;
        }
        KoStore *store;
    };

    QString fullFileName = sectionsIO->m_directory + filename;
    QString fullFileNameTmpNew = fullFileName + ".tmp_new/";
    QString fullFileNameTmpOld = fullFileName + ".tmp_old";
    QDir(fullFileNameTmpNew).removeRecursively();

    const char* mimeType = KoOdf::mimeType(KoOdf::Text);

    QDir().mkdir(fullFileNameTmpNew);
    KoStore* store = KoStore::createStore(fullFileNameTmpNew, KoStore::Write, mimeType, KoStore::Directory);
    Finally finaly(store);

    KoOdfWriteStore odfStore(store);
    KoEmbeddedDocumentSaver embeddedSaver;

    KoXmlWriter* manifestWriter = odfStore.manifestWriter(mimeType);
    KoXmlWriter* contentWriter = odfStore.contentWriter();
    KoXmlWriter* bodyWriter = odfStore.bodyWriter();

    if(!manifestWriter || !contentWriter || !bodyWriter) {
        return false;
    }

    KoGenStyles mainStyles;
    KoShapeSavingContext * context = new KoShapeSavingContext(*bodyWriter, mainStyles, embeddedSaver);
    context->addOption(KoShapeSavingContext::DrawId);

    bodyWriter->startElement("office:body");
    Xml::writeBraindumpNS(*bodyWriter);
    bodyWriter->startElement(KoOdf::bodyContentElement(KoOdf::Text, true));

    section->sectionContainer()->saveOdf(*context);

    bodyWriter->startElement("braindump:layout");
    bodyWriter->addAttribute("braindump:type", section->layout()->id());
    bodyWriter->endElement(); // braindump:layout

    bodyWriter->endElement(); // office:element
    bodyWriter->endElement(); // office:body

    mainStyles.saveOdfStyles(KoGenStyles::DocumentAutomaticStyles, contentWriter);

    odfStore.closeContentWriter();

    //add manifest line for content.xml
    manifestWriter->addManifestEntry("content.xml", "text/xml");


    if(!mainStyles.saveOdfStylesDotXml(store, manifestWriter)) {
        return false;
    }

    if(!context->saveDataCenter(store, manifestWriter)) {
        qDebug() << "save data centers failed";
        return false;
    }

    // Save embedded objects
    KoDocumentBase::SavingContext documentContext(odfStore, embeddedSaver);
    if(!embeddedSaver.saveEmbeddedDocuments(documentContext)) {
        qDebug() << "save embedded documents failed";
        return false;
    }

    // Write out manifest file
    if(!odfStore.closeManifestWriter()) {
        return false;
    }

    delete store;
    finaly.store = 0;
    delete context;

    QDir(fullFileNameTmpOld).removeRecursively();
    QDir().rename(fullFileName, fullFileNameTmpOld);
    QDir().rename(fullFileNameTmpNew, fullFileName);
    QDir(fullFileNameTmpOld).removeRecursively();

    return true;
}

bool SectionsIO::SaveContext::loadSection(SectionsIO* sectionsIO, SectionsIO::SaveContext::Version version)
{
    Q_UNUSED(version);
    // In case saving problem occurred, try to recover a directory either new or old
    QString fullFileName = sectionsIO->m_directory + filename;
    QString fullFileNameTmpNew = fullFileName + ".tmp_new/";
    QString fullFileNameTmpOld = fullFileName + ".tmp_old";
    if(!QFileInfo(fullFileName).exists()) {
        if(QFileInfo(fullFileNameTmpNew).exists()) {
            QDir().rename(fullFileNameTmpNew, fullFileName);
        } else if(QFileInfo(fullFileNameTmpOld).exists()) {
            QDir().rename(fullFileNameTmpOld, fullFileName);
        } else {
            return false;
        }
    }
    qDebug() << "Loading from " << fullFileName;

    const char* mimeType = KoOdf::mimeType(KoOdf::Text);
    KoStore* store = KoStore::createStore(fullFileName + '/', KoStore::Read, mimeType, KoStore::Directory);
    KoOdfReadStore odfStore(store);

    QString errorMessage;
    if(! odfStore.loadAndParse(errorMessage)) {
        qCritical() << "loading and parsing failed:" << errorMessage << endl;
        return false;
    }

    KoXmlElement content = odfStore.contentDoc().documentElement();
    KoXmlElement realBody(KoXml::namedItemNS(content, KoXmlNS::office, "body"));

    KoXmlElement body = KoXml::namedItemNS(realBody, KoXmlNS::office, KoOdf::bodyContentElement(KoOdf::Text, false));

    KoOdfLoadingContext loadingContext(odfStore.styles(), odfStore.store());
    KoShapeLoadingContext context(loadingContext, section->sectionContainer()->resourceManager());

    KoXmlElement element;
    QList<KoShape*> shapes;
    forEachElement(element, body) {
        qDebug() << "loading shape" << element.nodeName();

        if(element.nodeName() == "braindump:section") {
            section->sectionContainer()->loadOdf(element, context, shapes);
        } else if(element.nodeName() == "braindump:layout") {
            QString type = element.attribute("type");
            Layout* layout = LayoutFactoryRegistry::instance()->createLayout(type);
            if(layout) {
                section->setLayout(layout);
            }
        }
    }
    section->layout()->addShapes(shapes);
    return true;
}

void SectionsIO::saveTheStructure(QDomDocument& doc, QDomElement& elt, SectionGroup* root, QList<SaveContext*>& contextToRemove)
{
    foreach(Section * section, root->sections()) {
        SaveContext* context = m_contextes[section];
        if(context) {
            contextToRemove.removeAll(context);
        } else {
            context = new SaveContext;
            m_contextes[section] = context;
            context->section = section;
            context->filename = generateFileName();
        }
        Q_ASSERT(context);
        QDomElement celt = doc.createElement("Section");
        elt.appendChild(celt);
        celt.setAttribute("filename", context->filename);
        celt.setAttribute("name", section->name());
        saveTheStructure(doc, celt, section, contextToRemove);
    }
}

void SectionsIO::save()
{
    qDebug() << "Start saving";
    if(m_sectionsToSave.isEmpty()) {
        qDebug() << "No section to save";
        return;
    }
    QList<SaveContext*> contextToRemove = m_contextes.values();
    // First: save the structure
    QDomDocument doc;
    QDomElement root = doc.createElement("RootElement");
    doc.appendChild(root);
    saveTheStructure(doc, root, m_rootSection, contextToRemove);
    QFile file(structureFileName());
    file.open(QIODevice::WriteOnly);
    file.write(doc.toString().toUtf8());
    file.close();

    // Second: save each section
    foreach(SaveContext * saveContext, m_contextes) {
        if(m_sectionsToSave.contains(saveContext->section)) {
            if(saveContext->saveSection(this)) {
                qDebug() << "Successfully loaded: " << saveContext->section->name();
            } else {
                qDebug() << "Saving failed"; // TODO: Report it
            }
        }
    }
    m_sectionsToSave.clear();

    // Last remove unused sections
    foreach(SaveContext * saveContext, contextToRemove) {
        QDir(m_directory + saveContext->filename).removeRecursively();
        m_contextes.remove(saveContext->section);
        delete saveContext;
    }
}

void SectionsIO::loadTheStructure(QDomElement& elt, SectionGroup* parent, RootSection* _rootSection)
{
    QDomNode n = elt.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull() && e.nodeName() == "Section") {
            Section* section = new Section(_rootSection);
            QString name = e.attribute("name", "");
            if(name.isEmpty()) {
                name = SectionGroup::nextName();
            }
            section->setName(name);
            parent->insertSection(section);
            SaveContext* context = new SaveContext;
            context->filename = e.attribute("filename", "");
            context->section = section;
            m_contextes[section] = context;
            loadTheStructure(e, section, _rootSection);
        }
        n = n.nextSibling();
    }
}

void SectionsIO::load()
{
    QDomDocument doc;
    QFile file(structureFileName());
    if(!file.open(QIODevice::ReadOnly))
        return;
    if(!doc.setContent(&file)) {
        file.close();
        return;
    }
    file.close();

    QDomElement docElem = doc.documentElement();
    if(docElem.nodeName() != "RootElement") return;

    loadTheStructure(docElem, m_rootSection, m_rootSection);

    // Second: load each section
    foreach(SaveContext * saveContext, m_contextes) {
        if(!saveContext->loadSection(this, SaveContext::VERSION_1)) {
            qDebug() << "Loading failed"; // TODO: Report it
        }
    }
}

QString SectionsIO::generateFileName()
{
    for(; true; ++m_nextNumber) {
        QString filename = "section" + QString::number(m_nextNumber);
        if(!QFileInfo(m_directory + filename).exists() && !usedFileName(filename)) {
            return filename;
        }
    }
}

bool SectionsIO::usedFileName(const QString& filename)
{
    foreach(SaveContext * context, m_contextes.values()) {
        if(context->filename == filename)
            return true;
    }
    return false;
}

QString SectionsIO::structureFileName()
{
    return m_directory + "structure.xml";
}
