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

#include "SectionsSaver.h"

#include <QDomDocument>
#include <QFileInfo>
#include <QTimer>

#include <kglobal.h>
#include <kstandarddirs.h>

#include "RootSection.h"
#include "SectionGroup.h"
#include "Section.h"

SectionsSaver::SectionsSaver(RootSection* rootSection) : m_rootSection(rootSection), m_timer(new QTimer(this))
{
  m_timer->start(1000000);
  connect(m_timer, SIGNAL(timeout()), SLOT(doSave()));
  m_directory = KGlobal::dirs()->localkdedir() + "share/apps/braindump/sections/";
  KGlobal::dirs()->makeDir(m_directory);
}

SectionsSaver::~SectionsSaver()
{
}

struct SectionsSaver::SaveContext {
  Section* section;
  QString filename;
};

void SectionsSaver::saveTheStructure(QDomDocument& doc, QDomElement& elt, SectionGroup* root, QList<SaveContext*>& contextToRemove)
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

void SectionsSaver::doSave()
{
  QList<SaveContext*> contextToRemove = m_contextes.values();
  // First: save the structure
  QDomDocument doc;
  QDomElement root = doc.createElement("RootElement");
  doc.appendChild(root);
  saveTheStructure(doc, root, m_rootSection, contextToRemove);
  QFile file(m_directory + "structure.xml");
  file.open(QIODevice::WriteOnly);
  file.write(doc.toString().toUtf8());
  file.close();
  
  // Second: save each section
}

QString SectionsSaver::generateFileName()
{
  for(int i = 0; true; ++i)
  {
    QString filename = m_directory + "section" + QString::number(i);
    if( not QFileInfo(filename).exists() and not usedFileName(filename))
    {
      return filename;
    }
  }
}

bool SectionsSaver::usedFileName(const QString& filename)
{
  foreach(SaveContext* context, m_contextes.values())
  {
    if(context->filename == filename)
      return true;
  }
  return false;
}

#include "SectionsSaver.moc"
