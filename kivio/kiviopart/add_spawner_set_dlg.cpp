/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "add_spawner_set_dlg.h"
#include "tktoolbarbutton.h"

#include <ktoolbar.h>
#include <kbuttonbox.h>
#include <klocale.h>
#include <kglobal.h>
#include <kstddirs.h>

#include <qdialog.h>
#include <qpushbutton.h>
#include <qlistview.h>
#include <qlayout.h>
#include <qdir.h>
#include <qfile.h>

static const char * default_plug_xpm[] = {
"16 16 5 1",
"       c None",
".      c #000000",
"+      c #C4C7FF",
"@      c #5961FF",
"#      c #FFFFFF",
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


AddSpawnerSetAction::AddSpawnerSetAction( const QString& text, const QString& icon, int /*accel*/, QObject* parent, const char* name )
: TKAction(parent,name)
{
  setText(text);
  setIcon(icon);
  childMenuList.setAutoDelete(true);
  pathList.setAutoDelete(true);

  m_pPopupMenu = new QPopupMenu();
  connect(m_pPopupMenu,SIGNAL(aboutToShow()),SLOT(updateMenu()));
}

AddSpawnerSetAction::~AddSpawnerSetAction()
{
  delete m_pPopupMenu;
}

void AddSpawnerSetAction::updateMenu()
{
  m_id = 0;
  m_pPopupMenu->clear();

  childMenuList.clear();
  pathList.clear();

  KStandardDirs *dirs = KGlobal::dirs();
  QStringList dirList = dirs->findDirs("data", "kivio/stencils");
  dirList.sort();
  for( QStringList::Iterator it = dirList.begin(); it != dirList.end(); ++it )
  {
    QString dir = (*it);
    loadCollections(dir);
  }
}

void AddSpawnerSetAction::loadCollections( const QString& rootDirStr )
{
    QDir rootDir( rootDirStr );

    rootDir.setFilter( QDir::Dirs );
    rootDir.setSorting( QDir::Name );

    const QFileInfoList *colList = rootDir.entryInfoList();
    QFileInfoListIterator colIt( *colList );
    QFileInfo *colFInfo;

    while( (colFInfo = colIt.current()) )
    {
        if( colFInfo->fileName() != ".." &&
            colFInfo->fileName() != "." )
        {
            QPopupMenu* ch = new QPopupMenu;
            connect(ch,SIGNAL(activated(int)),SLOT(slotActivated(int)));
            loadSet( ch, rootDirStr + "/" + colFInfo->fileName() );
            m_pPopupMenu->insertItem(QIconSet(dirtPixmap(colFInfo->absFilePath())),dirDesc(colFInfo->absFilePath()),ch);
            childMenuList.append(ch);
        }
        ++colIt;
    }
}

void AddSpawnerSetAction::loadSet( QPopupMenu* menu, const QString& rootDirStr )
{
    QDir rootDir( rootDirStr );

    rootDir.setFilter( QDir::Dirs );
    rootDir.setSorting( QDir::Name );

    const QFileInfoList *setList = rootDir.entryInfoList();
    QFileInfoListIterator setIt( *setList );
    QFileInfo *setFInfo;

    while( (setFInfo = setIt.current()) )
    {
        if( setFInfo->fileName() != ".." &&
            setFInfo->fileName() != "." )
        {
            menu->insertItem(QIconSet(dirtPixmap(setFInfo->absFilePath())),dirDesc(setFInfo->absFilePath()),m_id);
            pathList.insert( m_id, new QString(rootDirStr + "/" + setFInfo->fileName()) );
            m_id++;
        }
        ++setIt;
    }
}

QString AddSpawnerSetAction::dirDesc( const QString& dir )
{
    QFile file( dir + "/desc" );

    if( file.exists()==false )
        return "Unknown - .desc does not exist";

    if( file.open( IO_ReadOnly )==false )
        return "Unknown - .desc could not be opened";

    QString ret;

    file.readLine( ret, 128 );
    file.close();

    int pos = ret.find( '\n' );
    if( pos!=-1 )
        ret.truncate( pos );

    return ret;
}

QPixmap AddSpawnerSetAction::dirtPixmap( const QString& dir )
{
    QFile file( dir + "/icon.xpm" );
    QFileInfo finfo( file );

    if( finfo.exists()==false )
    {
        return QPixmap( (const char **)default_plug_xpm );
    }

    return QPixmap( finfo.absFilePath() );
}

void AddSpawnerSetAction::initToolBarButton(TKToolBarButton* b)
{
  b->setPopup( m_pPopupMenu );
}

void AddSpawnerSetAction::slotActivated(int id)
{
  if (id<0 || pathList.at((uint)id) == 0L )
    return;

  QString path = QString(*pathList.at((uint)id));
  pathList.clear();

  emit activated(path);
}
/************************************************************************************************/

AddSpawnerSetDlg::AddSpawnerSetDlg( QWidget *par, const char *name, QString /*rdir*/ )
    : QDialog( par, name, true )
{
    setCaption(i18n("Load Stencil Set"));

    m_rootDir = "/";

//    m_rootDir = rdir;

    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->setMargin(5);
    vbox->setSpacing(10);

    QListView *pListView = new QListView(this, "spawner list");
    QObject::connect( pListView, SIGNAL(selectionChanged(QListViewItem*)),
                    this, SLOT(itemSelected(QListViewItem*)));
    pListView->addColumn( i18n("Set Name"), 250 );
    pListView->setRootIsDecorated(true);


    KStandardDirs *dirs = KGlobal::dirs();
    QStringList dirList = dirs->findDirs("data", "kivio/stencils");
    QString rootDir;
    for( QStringList::Iterator it = dirList.begin(); it != dirList.end(); ++it )
    {
        rootDir = (*it);
        loadCollections( pListView, rootDir );
    }


    vbox->addWidget( pListView );

    pListView->setFocus();

    KButtonBox *bb = new KButtonBox( this );
    bb->addStretch();

    m_ok = bb->addButton( i18n("OK") );
    m_ok->setDefault( TRUE );
    m_ok->setEnabled(false);

    QPushButton *pCancel = bb->addButton( i18n( "Cancel" ) );
    bb->layout();
    vbox->addWidget( bb );
    connect( m_ok, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
    connect( pCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

    resize( 270, 300 );

    m_pListView = pListView;
}

void AddSpawnerSetDlg::loadCollections( QListView *pListView, QString &rootDirStr )
{
    QDir rootDir( rootDirStr );
    QListViewItem *pColItem;

    rootDir.setFilter( QDir::Dirs );
    rootDir.setSorting( QDir::Name );

    const QFileInfoList *colList = rootDir.entryInfoList();
    QFileInfoListIterator colIt( *colList );
    QFileInfo *colFInfo;

    /*
     * We maintain 2 sets of items here.  The items prefixed with 'col' are the collection items and
     * refer to any data used in level 1 nodes.  The items prefixed with 'set' are set items and
     * refer to any data used in level 2 nodes.
     *
     * A collection is a grouping of one or more sets. Some example sets would be "Basic Flowcharting Shapes 1",
     * "Basic Flowcharting Shapes 2", "Auditing 1".  A collection would be "Flowcharting Shapes".
    */

    while( (colFInfo = colIt.current()) )
    {
        if( colFInfo->fileName() != ".." &&
            colFInfo->fileName() != "." )
        {
            pColItem = new QListViewItem( pListView, dirDesc( colFInfo->absFilePath() ), m_rootDir + "/" + colFInfo->fileName() );
            setPixmap( colFInfo->absFilePath(), pColItem );

            pListView->insertItem( pColItem );

            loadSet( pListView, pColItem, rootDirStr + "/" + colFInfo->fileName() );
        }

        ++colIt;
    }
}

void AddSpawnerSetDlg::loadSet( QListView */*pListView*/, QListViewItem *pParentItem, const QString &rootDirStr )
{
    QDir rootDir( rootDirStr );
    QListViewItem *pSetItem;

    rootDir.setFilter( QDir::Dirs );
    rootDir.setSorting( QDir::Name );

    const QFileInfoList *setList = rootDir.entryInfoList();
    QFileInfoListIterator setIt( *setList );
    QFileInfo *setFInfo;

    while( (setFInfo = setIt.current()) )
    {
        if( setFInfo->fileName() != ".." &&
            setFInfo->fileName() != "." )
        {
            pSetItem = new QListViewItem( pParentItem, dirDesc( setFInfo->absFilePath() ), rootDirStr + "/" + setFInfo->fileName() );
            setPixmap( setFInfo->absFilePath(), pSetItem );

            pParentItem->insertItem( pSetItem );
        }

        ++setIt;
    }

}


AddSpawnerSetDlg::~AddSpawnerSetDlg()
{
}

void AddSpawnerSetDlg::slotOk()
{
    accept();
}

QString AddSpawnerSetDlg::dirDesc( QString dir )
{
    int pos;

    QFile file( dir + "/desc" );

    if( file.exists()==false )
        return "Unknown - .desc does not exist";

    if( file.open( IO_ReadOnly )==false )
        return "Unknown - .desc could not be opened";

    QString ret;

    file.readLine( ret, 128 );
    file.close();

    pos = ret.find( '\n' );
    if( pos!=-1 )
        ret.truncate( pos );

    return ret;
}

void AddSpawnerSetDlg::setPixmap( QString dir, QListViewItem *pItem )
{
    QFile file( dir + "/icon.xpm" );
    QFileInfo finfo( file );

    if( finfo.exists()==false )
    {
        pItem->setPixmap( 0, QPixmap( (const char **)default_plug_xpm ) );
        return;
    }

    pItem->setPixmap( 0, QPixmap( finfo.absFilePath() ) );
}

QString AddSpawnerSetDlg::spawnerSetName()
{
    QListViewItem *pItem;

    pItem = m_pListView->selectedItem();

    return pItem->text(1);
}

void AddSpawnerSetDlg::itemSelected( QListViewItem *pItem )
{
    if( pItem->depth() == 1 )
    {
        // Depth = 1 -> Enable OK button
        m_ok->setEnabled(true);
    }
    else
    {
        m_ok->setEnabled(false);
    }
}
#include "add_spawner_set_dlg.moc"
