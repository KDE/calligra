/* This file is part of the KDE project
   Copyright (C) 2004 Peter Simonsson <psn@linux.se>,

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
#include "kivioaddstencilsetpanel.h"

#include <qfile.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qiconview.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qtextbrowser.h>

#include <kstandarddirs.h>
#include <kglobal.h>
#include <klistview.h>

#include "kivio_stencil_spawner_set.h"

namespace Kivio {
  AddStencilSetPanel::AddStencilSetPanel(QWidget *parent, const char *name)
    : KivioStencilSetWidget(parent, name)
  {
    updateList();
    
    m_stencilIView->setResizeMode(QIconView::Adjust);
    m_addToDocBtn->setEnabled(false);
    
    connect(m_stencilSetLView, SIGNAL(clicked(QListViewItem*)), this, SLOT(changeStencilSet(QListViewItem*)));
    connect(m_addToDocBtn, SIGNAL(clicked()), this, SLOT(addToDocument()));
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
    
    // Load the stencil icons
    m_currentDir = li->text(1);
    QDir d(m_currentDir);
    d.setNameFilter("*.png *.xpm");
    QStringList files = d.entryList();
    
    // Don't use the stencil set icon
    files.remove("icon.png");
    files.remove("icon.xpm");
    
    // Remove the old icons
    m_stencilIView->clear();
    
    for(QStringList::Iterator it = files.begin(); it != files.end(); ++it)
    {
      (void) new QIconViewItem(m_stencilIView, "", QPixmap(m_currentDir + "/" + (*it)));
    }
  
    m_descriptionBrowser->setText(KivioStencilSpawnerSet::readDescription(m_currentDir));
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
