#include <qlayout.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qstringlist.h>

#include <klocale.h>
#include <kglobal.h>
#include <kstddirs.h>

#include "kis_doc.h"
#include "gradientview.h"
#include "kis_dlg_gradienteditor.h"

GradientEditorDialog::GradientEditorDialog( KisDoc *_doc, 
    QWidget *_parent, const char *_name )
    : KFloatingDialog( _parent, _name )
    , m_pDoc( _doc )
{
    setCaption( i18n( "Gradient editor" ) );

    QWidget *area = new QWidget( this, "GradientEditorDialogArea" );
    setBaseWidget( area );

    QGridLayout *layout = new QGridLayout( area, 7, 2, 5 );

    QListBox *listbox = new QListBox( area, "GradientListBox" );
    layout->addMultiCellWidget( listbox, 0, 4, 0, 0 );

    QPushButton *newButton = new QPushButton( i18n( "New" ), 
        area, "NewButton" );
    layout->addWidget( newButton, 0, 1 );

    QPushButton *copyButton = new QPushButton( i18n( "Copy" ), 
        area, "CopyButton" );
    layout->addWidget( copyButton, 1, 1 );

    QPushButton *deleteButton = new QPushButton( i18n( "Delete" ), 
        area, "DeleteButton" );
    layout->addWidget( deleteButton, 2, 1 );

    QPushButton *refreshButton = new QPushButton( i18n( "Refresh" ), 
        area, "RefreshButton" );
    layout->addWidget( refreshButton, 3, 1 );

    m_pGradient = new GradientView( area, "GradientView" );
    layout->addMultiCellWidget( m_pGradient, 5, 5, 0, 1 );

    QScrollBar *scroll = new QScrollBar( Horizontal, area, 
        "GradientViewScrollBar" );
    layout->addMultiCellWidget( scroll, 6, 6, 0, 1 );

    layout->setColStretch( 1, 0 );
    layout->setRowStretch( 4, 1 );

    // reading GIMP gradient filenames

    KGlobal::dirs()->addResourceDir( "kis_gradients", 
        "/usr/share/gimp/1.1/gradients" );
    QStringList gradientFilenames 
        = KGlobal::dirs()->findAllResources( "kis_gradients" );

    QStringList::Iterator it = gradientFilenames.begin();
    for( ; it != gradientFilenames.end(); it++ )
    {
        listbox->insertItem( *it );
    }
}

GradientEditorDialog::~GradientEditorDialog()
{
}

#include "kis_dlg_gradienteditor.moc"
