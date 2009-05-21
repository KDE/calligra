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

#include <kdebug.h>
#include <kglobal.h>
#include <kstandarddirs.h>

#include "RootSection.h"
#include "SectionGroup.h"
#include "Section.h"
#include <KoStore.h>
#include <KoOdf.h>
#include <kio/netaccess.h>
#include <KoOdfWriteStore.h>
#include <KoEmbeddedDocumentSaver.h>
#include <KoGenStyles.h>
#include <KoShapeSavingContext.h>
#include <KoXmlWriter.h>
#include <KoOdfReadStore.h>
#include <KoXmlNS.h>
#include <KoShapeLoadingContext.h>
#include <KoOdfLoadingContext.h>
#include "SectionContainer.h"

SectionsIO::SectionsIO(RootSection* rootSection) : m_rootSection(rootSection), m_timer(new QTimer(this)), m_nextNumber(0)
{
  m_timer->start(1000000);
  connect(m_timer, SIGNAL(timeout()), SLOT(save()));
  m_directory = KGlobal::dirs()->localkdedir() + "share/apps/braindump/sections/";
  KGlobal::dirs()->makeDir(m_directory);
  
  // Finally load
  load();
}

SectionsIO::~SectionsIO()
{
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

bool SectionsIO::SaveContext::saveSection(SectionsIO* sectionsIO )
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
  KIO::NetAccess::del(fullFileNameTmpNew, 0);
  
  const char* mimeType = KoOdf::mimeType(KoOdf::Text);
  
  KoStore* store = KoStore::createStore(fullFileNameTmpNew, KoStore::Write, mimeType, KoStore::Directory);
  Finally finaly(store);

  KoOdfWriteStore odfStore(store);
  KoEmbeddedDocumentSaver embeddedSaver;
  
  KoXmlWriter* manifestWriter = odfStore.manifestWriter(mimeType);
  KoXmlWriter* contentWriter = odfStore.contentWriter();
  KoXmlWriter* bodyWriter = odfStore.bodyWriter();
  
  if(not manifestWriter or not contentWriter or not bodyWriter) {
    return false;
  }
    
  KoGenStyles mainStyles;
  KoShapeSavingContext * context = new KoShapeSavingContext(*bodyWriter, mainStyles, embeddedSaver);
  context->addOption(KoShapeSavingContext::DrawId);

  bodyWriter->startElement("office:body");
  bodyWriter->addAttribute("xmlns:braindump", "http://kde.org/braindump");
  bodyWriter->startElement(KoOdf::bodyContentElement(KoOdf::Text, true));

  section->sectionContainer()->saveOdf(*context);
  
  bodyWriter->endElement(); // office:element
  bodyWriter->endElement(); // office:body
  
  mainStyles.saveOdfAutomaticStyles(contentWriter, false);

  odfStore.closeContentWriter();

  //add manifest line for content.xml
  manifestWriter->addManifestEntry("content.xml", "text/xml");


  if (not mainStyles.saveOdfStylesDotXml(store, manifestWriter)) {
      return false;
  }

  if (not context->saveDataCenter(store, manifestWriter)) {
      kDebug() << "save data centers failed";
      return false;
  }

  // Save embedded objects
  KoDocument::SavingContext documentContext(odfStore, embeddedSaver);
  if (not embeddedSaver.saveEmbeddedDocuments(documentContext)) {
      kDebug() << "save embedded documents failed";
      return false;
  }

  // Write out manifest file
  if (not odfStore.closeManifestWriter()) {
      return false;
  }

  delete store;
  finaly.store = 0;
  delete context;
  
  KIO::NetAccess::del(fullFileNameTmpOld, 0);
  KIO::NetAccess::move( fullFileName, fullFileNameTmpOld, 0);
  KIO::NetAccess::move( fullFileNameTmpNew, fullFileName, 0);
  KIO::NetAccess::del( fullFileNameTmpOld, 0);
  return true;
}

bool SectionsIO::SaveContext::loadSection(SectionsIO* sectionsIO, SectionsIO::SaveContext::Version version)
{
  Q_UNUSED(version);
  // In case saving problem occured, try to recover a directory either new or old
  QString fullFileName = sectionsIO->m_directory + filename;
  QString fullFileNameTmpNew = fullFileName + ".tmp_new/";
  QString fullFileNameTmpOld = fullFileName + ".tmp_old";
  if( not QFileInfo(fullFileName).exists() )
  {
    if( QFileInfo(fullFileNameTmpNew).exists())
    {
      KIO::NetAccess::move(fullFileNameTmpNew, fullFileName);
    } else if( QFileInfo(fullFileNameTmpOld).exists()) {
      KIO::NetAccess::move(fullFileNameTmpOld, fullFileName);
    } else {
      return false;
    }
  }
  kDebug() << "Loading from " << fullFileName;
  
  const char* mimeType = KoOdf::mimeType(KoOdf::Text);
  KoStore* store = KoStore::createStore(fullFileName + "/", KoStore::Read, mimeType, KoStore::Directory);
  KoOdfReadStore odfStore(store);

  QString errorMessage;
  if (! odfStore.loadAndParse(errorMessage)) {
    kError() << "loading and parsing failed:" << errorMessage << endl;
    return false;
  }
  
  KoXmlElement content = odfStore.contentDoc().documentElement();
  KoXmlElement realBody(KoXml::namedItemNS(content, KoXmlNS::office, "body"));

  KoXmlElement body = KoXml::namedItemNS(realBody, KoXmlNS::office, KoOdf::bodyContentElement(KoOdf::Text, false));
  
  KoOdfLoadingContext loadingContext(odfStore.styles(), odfStore.store());
  KoShapeLoadingContext context(loadingContext, section->sectionContainer()->dataCenterMap());

  KoXmlElement element;
  forEachElement(element, body) {
    kDebug() << "loading shape" << element.nodeName();

    if(element.nodeName() == "braindump:section")
    {
      section->sectionContainer()->loadOdf(element, context);
      return true;
    }
  }
  return false;
}

void SectionsIO::saveTheStructure(QDomDocument& doc, QDomElement& elt, SectionGroup* root, QList<SaveContext*>& contextToRemove)
{
  foreach(Section* section, root->sections())
  {
    SaveContext* context = m_contextes[section];
    if(context)
    {
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
  foreach(SaveContext* saveContext, m_contextes)
  {
    if(saveContext->saveSection(this)) {
      kDebug() << "Sucessfully loaded: " << saveContext->section->name();
    } else {
      kDebug() << "Saving failed"; // TODO: Report it
    }
  }
  
  // Last remove unused sections
  foreach( SaveContext* saveContext, contextToRemove)
  {
    KIO::NetAccess::del(m_directory + saveContext->filename, 0);
    m_contextes.remove(saveContext->section);
    delete saveContext;
  }
}

#include <iostream>

void SectionsIO::loadTheStructure(QDomElement& elt, SectionGroup* root)
{
  QDomNode n = elt.firstChild();
  while(!n.isNull()) {
    QDomElement e = n.toElement(); // try to convert the node to an element.
    if(!e.isNull() and e.nodeName() == "Section" ) {
      Section* section = new Section();
      QString name = e.attribute("name", "");
      if(name.isEmpty())
      {
        name = SectionGroup::nextName();
      }
      section->setName(name);
      root->insertSection(section);
      SaveContext* context = new SaveContext;
      context->filename = e.attribute("filename", "");
      context->section = section;
      m_contextes[section] = context;
      loadTheStructure(e, section);
    }
    n = n.nextSibling();
  }
}

void SectionsIO::load()
{
  QDomDocument doc;
  QFile file(structureFileName());
  if (!file.open(QIODevice::ReadOnly))
    return;
  if (!doc.setContent(&file)) {
    file.close();
    return;
  }
  file.close();
  
  QDomElement docElem = doc.documentElement();
  if(docElem.nodeName() != "RootElement") return;

  loadTheStructure(docElem, m_rootSection);
  
  // Second: load each section
  foreach(SaveContext* saveContext, m_contextes)
  {
    if(not saveContext->loadSection(this, SaveContext::VERSION_1))
    {
      kDebug() << "Loading failed"; // TODO: Report it
    }
  }
}

QString SectionsIO::generateFileName()
{
  for(; true; ++m_nextNumber)
  {
    QString filename = "section" + QString::number(m_nextNumber);
    if( not QFileInfo(m_directory + filename).exists() and not usedFileName(filename))
    {
      return filename;
    }
  }
}

bool SectionsIO::usedFileName(const QString& filename)
{
  foreach(SaveContext* context, m_contextes.values())
  {
    if(context->filename == filename)
      return true;
  }
  return false;
}

QString SectionsIO::structureFileName()
{
  return m_directory + "structure.xml";
}

#include "SectionsIO.moc"
