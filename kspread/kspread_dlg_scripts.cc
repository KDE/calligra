#include "kspread_dlg_scripts.h"

#include <qlabel.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <unistd.h>
#include <kmessagebox.h>
#include <kapp.h>
#include <klocale.h>
#include <kstddirs.h>

#include <stdlib.h>

KSpreadScripts::KSpreadScripts( QWidget* parent, const char* name )
        : QDialog( parent, name ), KSpreadScriptsData( this )
{
  // editor = 0L;

    setCaption( i18n( "KSpread Scripts" ) );
    connect( list, SIGNAL( highlighted( int ) ), this, SLOT( slotHighlighted( int ) ) );
    connect( list, SIGNAL( selected( int ) ), this, SLOT( slotSelected( int ) ) );
    connect( edit, SIGNAL( clicked() ), this, SLOT( slotEdit() ) );
    connect( rename, SIGNAL( clicked() ), this, SLOT( slotRename() ) );
    connect( add, SIGNAL( clicked() ), this, SLOT( slotAdd() ) );
    connect( del, SIGNAL( clicked() ), this, SLOT( slotDelete() ) );

    updateList();
}

void KSpreadScripts::updateList()
{
    list->clear();
    nameList.clear();

    QString path( locate( "data", "kspread/scripts/" ) );

    QDir d( path );
    d.setFilter( QDir::Files );
    d.setSorting( QDir::Name );
    const QFileInfoList *flist = d.entryInfoList();
    QFileInfoListIterator it( *flist );
    QFileInfo *fi;

    while ( ( fi = it.current() ) )
    {
        QString t = fi->fileName();
        if ( t.right(3) == ".py" )
        {
            list->insertItem( t.left( t.length() - 3 ) );
            nameList.append( t );
        }
        ++it;
    }
}

void KSpreadScripts::slotHighlighted( int )
{
    rename2->setText( list->text( list->currentItem() ) );
}

void KSpreadScripts::slotSelected( int )
{
    rename2->setText( list->text( list->currentItem() ) );
    slotEdit();
}

void KSpreadScripts::slotAdd()
{
    QString t = add2->text();
    if ( t.length() == 0 )
    {
        KMessageBox::error(this, i18n("You must enter a name"));
        return;
    }

    QString t2 ( t );
    t2 += ".ks";
    if ( nameList.find( t2 ) != nameList.end() )
    {
        KMessageBox::error(this,i18n( "The file already exists"));
        return;
    }

    QString d( locate( "data", "/kspread/scripts/" ) );
    d += t2;
    FILE *f = fopen( QFile::encodeName(d), "w" );
    if ( f == 0L )
    {
        KMessageBox::error(this, i18n("Could not open file.\nPerhaps access denied"));
        return;
    }
    fclose( f );

    /* nameList.append( t2 );
    list->inSort( t.data() );    */
    updateList();

    add2->setText( "" );
}

void KSpreadScripts::slotDelete()
{
    if ( list->currentItem() == -1 )
        return;

    QString t = i18n("Do you really want to delete the script\n%1?").arg(list->text( list->currentItem() ) );

    if ( KMessageBox::questionYesNo(this, t,i18n("KSpread Question"))==4 )
        return;

    QString t2( list->text( list->currentItem() ) );
    t2 += ".py";

    /* nameList.remove( t2.data() );
    list->removeItem( list->currentItem() ); */

    QString dir( locate( "data", "/kspread/scripts/" ) );
    dir += t2;
    // HACK
    unlink( QFile::encodeName(t2) );

    /* QString t3;
    t3.sprintf("kfmclient move '%s' trash:/", dir.data() );
    system( t3.data() ); */

    updateList();
}

void KSpreadScripts::slotRename()
{
    if ( list->currentItem() == -1 )
        return;

    QString t = rename2->text();
    if ( t.length() == 0 )
    {
        KMessageBox::error(this, i18n("You must enter a name"));
        return;
    }

    QString t2 ( t );
    t2 += ".py";
    if ( nameList.find( t2 ) != nameList.end() )
    {
        KMessageBox::error(this, i18n("The file already exists"));
        return;
    }

    QString t3( list->text( list->currentItem() ) );
    t3 += ".py";
    /* nameList.remove( t3.data() );
    list->removeItem( list->currentItem() );

    nameList.append( t2 );
    list->inSort( t.data() ); */

    QString dir( locate( "data", "/kspread/scripts/" ) );

    QString t4( dir );
    t4 += t3;
    QString t5( dir );
    t5 += t2;

    ::rename( QFile::encodeName(t4), QFile::encodeName(t5) );

    updateList();
}

void KSpreadScripts::slotEdit()
{
    if ( list->currentItem() == -1 )
        return;

    QString t2( list->text( list->currentItem() ) );
    t2 += ".ks";

    /* if ( editor == 0L )
        editor = createEditor();

    QString dir( kapp->kde_datadir().copy() );
    dir += "/kspread/scripts/";
    dir += t2.data();

    editor->show();
    kdDebug(36001) <<  "EDITOR Opening " << dir.data() << endl;
    editor->openFile( dir ); */
}

KSpreadScripts::~KSpreadScripts()
{
  /* if ( editor )
     delete editor; */
}


KSpreadScriptsData::KSpreadScriptsData( QWidget* parent )
{
        list = new QListBox( parent, "ListBox_1" );
        list->setGeometry( 10, 40, 180, 260 );
        list->setFrameStyle( 51 );
        list->setLineWidth( 2 );

        QLabel* tmpQLabel;
        tmpQLabel = new QLabel( parent, "Label_1" );
        tmpQLabel->setGeometry( 10, 10, 100, 30 );
        tmpQLabel->setText( i18n("Scripts") );

        del = new QPushButton( parent, "PushButton_1" );
        del->setGeometry( 210, 40, 100, 30 );
        del->setText( i18n("Delete") );

        add = new QPushButton( parent, "PushButton_2" );
        add->setGeometry( 210, 90, 100, 30 );
        add->setText( i18n("Add") );

        add2 = new QLineEdit( parent, "LineEdit_1" );
        add2->setGeometry( 210, 130, 180, 30 );
        add2->setText( "" );

        edit = new QPushButton( parent, "PushButton_3" );
        edit->setGeometry( 210, 180, 100, 30 );
        edit->setText( i18n("Edit") );

        rename = new QPushButton( parent, "PushButton_4" );
        rename->setGeometry( 210, 230, 100, 30 );
        rename->setText( i18n("Rename") );

        rename2 = new QLineEdit( parent, "LineEdit_2" );
        rename2->setGeometry( 210, 270, 180, 30 );
        rename2->setText( "" );

        parent->resize( 400, 310 );
}

#include "kspread_dlg_scripts.moc"
