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

#include <kglobal.h>
#include <kstandarddirs.h>

#include "RootSection.h"
#include "SectionGroup.h"
#include "Section.h"

SectionsIO::SectionsIO(RootSection* rootSection) : m_rootSection(rootSection), m_timer(new QTimer(this)), m_nextNumber(0)
{
  m_timer->start(1000000);
  connect(m_timer, SIGNAL(timeout()), SLOT(doSave()));
  m_directory = KGlobal::dirs()->localkdedir() + "share/apps/braindump/sections/";
  KGlobal::dirs()->makeDir(m_directory);
  
  // Finally load
  load();
}

SectionsIO::~SectionsIO()
{
}

struct SectionsIO::SaveContext {
  Section* section;
  QString filename;
};

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
    saveTheStructure(doc, celt, section, contextToRemove);
  }
}

void SectionsIO::doSave()
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
}

#include <iostream>

void SectionsIO::loadTheStructure(QDomElement& elt, SectionGroup* root)
{
  QDomNode n = elt.firstChild();
  while(!n.isNull()) {
     QDomElement e = n.toElement(); // try to convert the node to an element.
     if(!e.isNull() and e.nodeName() == "Section" ) {
       Section* section = new Section();
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
