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

#include <QDomDocument>
#include <QFile>
#include <QSvgRenderer>

#include <kcomponentdata.h>
#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <QFileInfo>
#include <QDir>

State::State( const QString& _id, const QString& _name, Category* _category, const QString& _fileName, int _priority) : m_id(_id), m_name(_name), m_category(_category), m_render(new QSvgRenderer(_fileName)), m_priority(_priority)
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

int State::priority() const {
  return m_priority;
}

Category::Category( const QString& _id, const QString& _name, int _priority) : m_id(_id), m_name(_name), m_priority(_priority) {
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
  if(m_states.contains(_id)) return m_states[_id];
  kWarning() << "No shape " << _id << " found in category " << name() << " choices: " << m_states.keys();
  return 0;
}

int Category::priority() const {
  return m_priority;
}

void StatesRegistry::parseStatesRC(const QString& _filename )
{
  QDomDocument doc;
  QFile file(_filename);
  if(not file.open(QIODevice::ReadOnly) )
  {
    kError() << "Can't open " << _filename;
    return;
  }
  QString errMsg;
  int line, column;
  if( not doc.setContent(&file, &errMsg, &line, &column) ) {
    kError() << "At (" << line << ", " << column << ") " << errMsg;
    file.close();
    return;
  }
  file.close();
  
  QDir directory = QFileInfo(_filename).absoluteDir();
  
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
      int catPriority = eCat.attribute("priority", "1000").toInt();
      Category* category = 0;
      if(catId.isEmpty()) {
        kError() << "Missing category id";
      } else {
        if( m_categories.contains(catId) )
        {
          category = m_categories[catId];
        } else if( not catName.isEmpty() ) {
          category = new Category(catId, i18n(catName.toUtf8()), catPriority );
          m_categories[catId] = category;
        }
        if(category){
          // Parse the states
          QDomNode nState = eCat.firstChild();
          while(not nState.isNull())
          {
            QDomElement eState = nState.toElement();
            if(not eState.isNull() and eState.tagName() == "state")
            {
              QString stateId = eState.attribute("id");
              QString stateName = eState.attribute("name");
              QString stateFilename = eState.attribute("filename");
              int statePriority = eState.attribute("priority", "1000").toInt();
              if(stateId.isEmpty() or stateName.isEmpty() or stateFilename.isEmpty())
              {
                kError() << "Missing attribute: id = " << stateId << " name = " << stateName << " filename = " << stateFilename;
              } else {
                QString file = directory.absoluteFilePath(stateFilename);
                if(QFileInfo(file).exists())
                {
                  if(category->m_states.contains(stateId))
                  {
                    delete category->m_states[stateId];
                  }
                  kDebug() << "Adding state id = " << stateId << " name = " << stateName << " filename = " << stateFilename;
                  category->m_states[stateId] = new State(stateId, stateName, category, file, statePriority);
                } else {
                  kError() << "Missing file " << file;
                }
              }
            } else {
              kError() << "Invalid node in category " << catId;
            }
            nState = nState.nextSibling();
          }
        } else {
          kError() << "Couldn't make a category for " << catId;
        }
      }
    } else {
      kError() << "Invalid XML node.";
    }
    nCat = nCat.nextSibling();
  }
}


StatesRegistry* StatesRegistry::s_instance = 0;

StatesRegistry::StatesRegistry() {
  KGlobal::mainComponent().dirs()->addResourceType("stateshape_states", "data", "stateshape/states/");
  QStringList statesFilenames = KGlobal::mainComponent().dirs()->findAllResources("stateshape_states", "*.rc",  KStandardDirs::Recursive);

  foreach(const QString& filename, statesFilenames)
  {
    kDebug() << "Load state: " << filename;
    parseStatesRC(filename);
  }
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
  if(m_categories.contains(_category)) return m_categories[_category]->state(_state);
  kWarning() << "No category " << _category << " found among " << m_categories.keys();
  return 0;
}
