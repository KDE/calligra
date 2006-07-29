/* This file is part of the KDE project
   Copyright (C) 2004-2005 Peter Simonsson <psn@linux.se>,

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
   Boston, MA 02110-1301, USA.
*/
#include "kivioaddstencilsetpanel.h"

#include <qfile.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qiconview.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qtextbrowser.h>
#include <qsplitter.h>

#include <kstandarddirs.h>
#include <kglobal.h>
#include <klistview.h>

#include "kivio_stencil_spawner_set.h"
#include "kivio_stencil_spawner.h"
#include "kivio_stencil_spawner_info.h"
#include "kivio_stencil.h"
#include "kivioglobal.h"

namespace Kivio {
  AddStencilSetPanel::AddStencilSetPanel(QWidget *parent, const char *name)
    : KivioStencilSetWidget(parent, name)
  {
    int height = m_stencilsetGBox->height() / 2;
    QValueList<int> sizes;
    sizes << height << height;
    m_stencilSetSplitter->setSizes(sizes);
    updateList();

    connect(m_stencilSetLView, SIGNAL(selectionChanged(QListViewItem*)), this, SLOT(changeStencilSet(QListViewItem*)));
    connect(m_addToDocBtn, SIGNAL(clicked()), this, SLOT(addToDocument()));

    QListViewItem* tmp = m_stencilSetLView->firstChild();

    if(tmp) {
      tmp = tmp->firstChild();

      if(tmp) {
        m_stencilSetLView->setSelected(tmp, true);
        m_stencilSetLView->ensureItemVisible(tmp);
      }
    }
  }
  
  
  AddStencilSetPanel::~AddStencilSetPanel()
  {
  }
  
  void AddStencilSetPanel::updateList()
  {
    KStandardDirs *dirs = KGlobal::dirs();
    QStringList dirList = dirs->findDirs("data", "kivio/stencils");
    dirList.sort();
    m_stencilSetLView->clear();
    
    for(QStringList::Iterator it = dirList.begin(); it != dirList.end(); ++it)
    {
      QString dir = (*it);
      loadCollections(dir);
    }
  }

  void AddStencilSetPanel::loadCollections(const QString& dir)
  {
    QDir rootDir( dir );
  
    rootDir.setFilter( QDir::Dirs );
    rootDir.setSorting( QDir::Name );
  
    const QFileInfoList *colList = rootDir.entryInfoList();
    QFileInfoListIterator colIt( *colList );
    QFileInfo *colFInfo;
    QString cId;
  
    while((colFInfo = colIt.current()))
    {
      if((colFInfo->fileName() != "..") && (colFInfo->fileName() != "."))
      {
        cId = KivioStencilSpawnerSet::readId(colFInfo->absFilePath());
        QListViewItem* li = m_stencilSetLView->firstChild();
        
        while(li) {
          if(li->text(1) == cId) {
            break;
          }
          
          li = li->nextSibling();
        }
        
        if(!li) {
          li = new KListViewItem(m_stencilSetLView,
            KivioStencilSpawnerSet::readTitle(colFInfo->absFilePath()),
            KivioStencilSpawnerSet::readId(colFInfo->absFilePath()));
          li->setPixmap(0, loadIcon("icon", colFInfo->absFilePath()));
        }
        
        loadStencilSet(li, dir + "/" + colFInfo->fileName());
      }
      
      ++colIt;
    }
  }
  
  void AddStencilSetPanel::loadStencilSet(QListViewItem* li, const QString& dir)
  {
    QDir rootDir( dir );
  
    rootDir.setFilter( QDir::Dirs );
    rootDir.setSorting( QDir::Name );
  
    const QFileInfoList *setList = rootDir.entryInfoList();
    QFileInfoListIterator setIt( *setList );
    QFileInfo *setFInfo;
  
    while( (setFInfo = setIt.current()) )
    {
      if( setFInfo->fileName() != ".." && setFInfo->fileName() != "." )
      {
        KListViewItem* tmp = new KListViewItem(li, KivioStencilSpawnerSet::readTitle(setFInfo->absFilePath()),
          dir + "/" + setFInfo->fileName());
        tmp->setPixmap(0, loadIcon("icon", setFInfo->absFilePath()));

        if(m_currentDir == setFInfo->absFilePath()) {
          tmp->setSelected(true);
          m_stencilSetLView->ensureItemVisible(tmp);
        }
      }

      ++setIt;
    }
  }

  QPixmap AddStencilSetPanel::loadIcon(const QString& name, const QString& dir)
  {
    QString fs;

    if( QFile::exists(dir + "/" + name + ".xpm") ) {
      fs = dir + "/" + name + ".xpm";
    } else if( QFile::exists(dir + "/" + name + ".png") ) {
      fs = dir + "/" + name + ".png";
    } else {
      return QPixmap();
    }

    return QPixmap( fs );
  }

  void AddStencilSetPanel::changeStencilSet(QListViewItem* li)
  {
    if(!li || !li->parent()) {
      return;
    }
    
    m_stencilsetGBox->setTitle(li->text(0));
    m_addToDocBtn->setEnabled(true);
    KivioStencilSpawnerSet* spawnerSet = new KivioStencilSpawnerSet(li->text(0));
    // Load the stencil icons
    m_currentDir = li->text(1);
    spawnerSet->loadDir(m_currentDir);
    QStringList files = spawnerSet->files();

    // Remove the old icons
    m_stencilIView->clear();
    KivioStencilSpawner* spawner = 0;
    KivioStencil* stencil = 0;
    int count = 0;

    for(QStringList::Iterator it = files.begin(); it != files.end(); ++it)
    {
      spawner = spawnerSet->loadFile(m_currentDir + "/" + (*it));

      if(spawner) {
        stencil = spawner->newStencil();

        if(stencil) {
          (void) new QIconViewItem(m_stencilIView, spawner->info()->title(), Kivio::generatePixmapFromStencil(32, 32, stencil));
          delete stencil;
          count++;
        }
      }

      if(count == 10) { // Limit to max 10 stencils
        break;
      }
    }

    m_descriptionBrowser->setText(KivioStencilSpawnerSet::readDescription(m_currentDir));

    delete spawnerSet;
  }

  void AddStencilSetPanel::addToDocument()
  {
    if(m_currentDir.isEmpty()) {
      return;
    }
    
    emit addStencilSet(m_currentDir);
  }
}

#include "kivioaddstencilsetpanel.moc"
