/* This file is part of the KDE project
   Copyright (C) 2003 Peter Simonsson <psn@linux.se>,
   theKompany.com & Dave Marotti

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

#include "kiviostencilsetaction.h"

#include <kpopupmenu.h>
#include <kstandarddirs.h>
#include <kapplication.h>
#include <kdebug.h>
#include <ktoolbar.h>
#include <ktoolbarbutton.h>
#include <kiconloader.h>
#include <klocale.h>

#include <qfile.h>
#include <qdir.h>
#include <qpixmap.h>
#include <qwhatsthis.h>
#include <qmenubar.h>
#include <qstringlist.h>
#include <qvaluelist.h>

#include "kivio_stencil_spawner_set.h"

static const char * default_plug_xpm[] = {
"16 16 5 1",
" 	c None",
".	c #000000",
"+	c #C4C7FF",
"@	c #5961FF",
"#	c #FFFFFF",
"                ",
"                ",
" ...            ",
" .++..          ",
" .+@++..        ",
" .+@@@++..      ",
" .+@@@@@++..    ",
" .+@@@@@@@++..  ",
" .+@@@@@@@@@++. ",
" .+@@@@@@@@@@@.#",
" .+@@@@@@@@@@@.#",
" .+@@@@@@@@@@@.#",
" .+@@@@@@@@@@@.#",
" .+@@@@@@@@@@@.#",
" ..............#",
"  ##############"};

KivioStencilSetAction::KivioStencilSetAction(const QString &text, const QString &pix,
  KActionCollection *parent, const char *name)
  : KAction(text, pix, 0, parent, name)
{
  setShortcutConfigurable( false );

  m_pathList.setAutoDelete(true);

  m_popup = new KPopupMenu(0L,"KivioStencilSetAction::popup");
  updateMenu();
}

KivioStencilSetAction::~KivioStencilSetAction()
{
  clearCollectionMenuList();
  delete m_popup;
  m_popup = 0;
}

KPopupMenu* KivioStencilSetAction::popupMenu() const
{
  return m_popup;
}

void KivioStencilSetAction::popup( const QPoint& global )
{
  popupMenu()->popup(global);
}

int KivioStencilSetAction::plug( QWidget* widget, int index)
{
  // This function is copied from KActionMenu::plug
  if (kapp && !kapp->authorizeKAction(name()))
    return -1;
  kdDebug(129) << "KAction::plug( " << widget << ", " << index << " )" << endl; // remove -- ellis
  if ( widget->inherits("QPopupMenu") )
  {
    QPopupMenu* menu = static_cast<QPopupMenu*>( widget );
    int id;

    if ( hasIconSet() )
      id = menu->insertItem( iconSet(), text(), popupMenu(), -1, index );
    else
      id = menu->insertItem( kapp->iconLoader()->loadIcon(icon(), KIcon::Small),
        text(), popupMenu(), -1, index );

    if ( !isEnabled() )
      menu->setItemEnabled( id, false );

    addContainer( menu, id );
    connect( menu, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

    return containerCount() - 1;
  }
  else if ( widget->inherits( "KToolBar" ) )
  {
    KToolBar *bar = static_cast<KToolBar *>( widget );

    int id_ = KAction::getToolButtonID();

    if ( icon().isEmpty() && !iconSet().isNull() )
      bar->insertButton( iconSet().pixmap(), id_, SIGNAL( clicked() ), this,
                          SLOT( slotActivated() ), isEnabled(), plainText(),
                          index );
    else
    {
      KInstance *instance;

      if ( m_parentCollection )
        instance = m_parentCollection->instance();
      else
        instance = KGlobal::instance();

      bar->insertButton( icon(), id_, SIGNAL( clicked() ), this,
                          SLOT( slotActivated() ), isEnabled(), plainText(),
                          index, instance );
    }

    addContainer( bar, id_ );

    if (!whatsThis().isEmpty())
      QWhatsThis::add( bar->getButton(id_), whatsThis() );

    connect( bar, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

    bar->getButton(id_)->setPopup(popupMenu(), true );

    return containerCount() - 1;
  }
  else if ( widget->inherits( "QMenuBar" ) )
  {
    QMenuBar *bar = static_cast<QMenuBar *>( widget );

    int id;

    id = bar->insertItem( text(), popupMenu(), -1, index );

    if ( !isEnabled() )
      bar->setItemEnabled( id, false );

    addContainer( bar, id );
    connect( bar, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

    return containerCount() - 1;
  }

  return -1;
}

void KivioStencilSetAction::updateMenu()
{
  m_ssId = 0;
  m_popup->clear();

  m_popup->insertItem(i18n("Show Stencil Set Chooser"), this, SIGNAL(showDialog()));
  m_popup->insertSeparator();

  m_pathList.clear();
  m_collectionIdList.clear();
  clearCollectionMenuList();

  KStandardDirs *dirs = KGlobal::dirs();
  QStringList dirList = dirs->findDirs("data", "kivio/stencils");
  dirList.sort();

  for( QStringList::Iterator it = dirList.begin(); it != dirList.end(); ++it )
  {
    QString dir = (*it);
    loadCollections(dir);
  }
}

void KivioStencilSetAction::loadCollections( const QString& rootDirStr )
{
  QDir rootDir( rootDirStr );

  rootDir.setFilter( QDir::Dirs );
  rootDir.setSorting( QDir::Name );

  const QFileInfoList *colList = rootDir.entryInfoList();
  QFileInfoListIterator colIt( *colList );
  QFileInfo *colFInfo;
  QString cId;

  while((colFInfo = colIt.current()))
  {
    if(colFInfo->fileName() != ".." && colFInfo->fileName() != ".")
    {
      cId = KivioStencilSpawnerSet::readId(colFInfo->absFilePath());
      KPopupMenu* ch;
      int index = m_collectionIdList.findIndex(cId);
      
      if(index < 0) {
        ch = new KPopupMenu;
        connect(ch,SIGNAL(activated(int)),SLOT(slotActivated(int)));
        m_popup->insertItem(QIconSet(dirtPixmap(colFInfo->absFilePath())),
          KivioStencilSpawnerSet::readTitle(colFInfo->absFilePath()),ch);
        m_collectionIdList.append(cId);
        m_collectionMenuList.append(ch);
      } else {
        ch = m_collectionMenuList[index];
      }
    
      loadSet( ch, rootDirStr + "/" + colFInfo->fileName() );
    }
    
    ++colIt;
  }
}

void KivioStencilSetAction::loadSet( KPopupMenu* menu, const QString& rootDirStr )
{
  QDir rootDir( rootDirStr );

  rootDir.setFilter( QDir::Dirs );
  rootDir.setSorting( QDir::Name );

  const QFileInfoList *setList = rootDir.entryInfoList();
  QFileInfoListIterator setIt( *setList );
  QFileInfo *setFInfo;

  while( (setFInfo = setIt.current()) )
  {
    if( setFInfo->fileName() != ".." && setFInfo->fileName() != "." )
    {
      menu->insertItem(QIconSet(dirtPixmap(setFInfo->absFilePath())),
        KivioStencilSpawnerSet::readTitle(setFInfo->absFilePath()),m_ssId);
      m_pathList.insert( m_ssId, new QString(rootDirStr + "/" + setFInfo->fileName()) );
      m_ssId++;
    }
    ++setIt;
  }
}

QPixmap KivioStencilSetAction::dirtPixmap( const QString& dir )
{
    QString fs;

    if( QFile::exists(dir + "/icon.xpm") ) {
      fs = dir + "/icon.xpm";
    } else if( QFile::exists(dir + "/icon.png") ) {
      fs = dir + "/icon.png";
    } else {
      return QPixmap( (const char **)default_plug_xpm );
    }

    QFile file( fs );
    QFileInfo finfo( file );
    return QPixmap( finfo.absFilePath() );
}

void KivioStencilSetAction::slotActivated(int id)
{
  if (id < 0 || !m_pathList.at((uint)id))
    return;

  QString path = *m_pathList.at((uint)id);

  emit activated(path);
}

void KivioStencilSetAction::clearCollectionMenuList()
{
  QValueList<KPopupMenu*>::Iterator it;
  
  for(it = m_collectionMenuList.begin(); it != m_collectionMenuList.end(); ++it) {
    KPopupMenu* m = (*it);
    delete m;
  }
  
  m_collectionMenuList.clear();
}

#include "kiviostencilsetaction.moc"
