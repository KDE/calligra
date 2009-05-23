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

#include "StatesRegistry.h"

#include <QSvgRenderer>

#include <kcomponentdata.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <QFileInfo>
#include <QDomDocument>

State::State( const QString& _id, const QString& _name, Category* _category, const QString& _fileName) : m_id(_id), m_name(_name), m_category(_category), m_render(new QSvgRenderer(_fileName))
{
}

State::~State() {
  delete m_render;
}

const QString& State::name() const {
  return m_name;
}

const QString& State::id() const {
  return m_id;
}

const Category* State::category() const {
  return m_category;
}

QSvgRenderer* State::renderer() const {
  return m_render;
}

Category::Category( const QString& _id, const QString& _name) : m_id(_id), m_name(_name) {
}

Category::~Category() {
}

const QString& Category::name() const {
  return m_name;
}

const QString& Category::id() const {
  return m_id;
}

QList<QString> Category::stateIds() const {
  return m_states.keys();
}

const State* Category::state(const QString& _id) const {
  Q_ASSERT(m_states.contains(_id));
  return m_states[_id];
}

void StatesRegistry::parseStatesRC(const QString& _filename )
{
  QDomDocument doc;
  QFile file(_filename);
  if( file.open(QIODevice::ReadOnly) )
    return;
  if( not doc.setContent(&file) ) {
    file.close();
    return;
  }
  file.close();
  
  QDomElement docElem = doc.documentElement();
  if(docElem.nodeName() != "states") {
    kError() << "Invalid state file";
    return;
  }
  QDomNode nCat = docElem.firstChild();
  while(not nCat.isNull()) {
    QDomElement eCat = nCat.toElement(); // try to convert the node to an element.
    if(not eCat.isNull() and eCat.tagName() == "category")
    {
      QString catId = eCat.attribute("id");
      QString catName = eCat.attribute("name");
      
    } else {
      kError() << "Invalid tag: " << eCat.tagName();
    }
    nCat = nCat.nextSibling();
  }
  
}


StatesRegistry* StatesRegistry::s_instance = 0;

StatesRegistry::StatesRegistry() {
  KGlobal::mainComponent().dirs()->addResourceType("todoshape_states", "data", "todoshape/states/");
  QStringList statesFilenames = KGlobal::mainComponent().dirs()->findAllResources("todoshape_states", "*.rc",  KStandardDirs::Recursive);
  
  foreach(const QString& filename, statesFilenames)
  {
    kDebug() << "Load state: " << filename;
    parseStatesRC(filename);
  }
  
  qFatal("doh");
}

const StatesRegistry* StatesRegistry::instance() {
  if(not s_instance)
  {
    s_instance = new StatesRegistry;
  }
  return s_instance;
}

QList<QString> StatesRegistry::categorieIds() const {
  return m_categories.keys();
}

QList<QString> StatesRegistry::stateIds(const QString& _id) const {
  Q_ASSERT(m_categories.contains(_id));
  return m_categories[_id]->stateIds();
}

const State* StatesRegistry::state(const QString& _category, const QString& _state) const {
  Q_ASSERT(m_categories.contains(_category));
  return m_categories[_category]->state(_state);
}
