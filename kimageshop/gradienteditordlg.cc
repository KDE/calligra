#include "qlayout.h"
#include "qlistbox.h"
#include "qpushbutton.h"

#include "klocale.h"

#include "gradienteditordlg.h"
#include "kimageshop_doc.h"

GradientEditorDialog::GradientEditorDialog( KImageShopDoc *_doc, QWidget *_parent, const char *_name )
  : KFloatingDialog( _parent, _name )
  , m_pDoc( _doc )
{
  setCaption( i18n( "Gradient editor" ) );

  QWidget *area = new QWidget( this, "GradientEditorDialogArea" );
  setBaseWidget( area );

  QGridLayout *layout = new QGridLayout( area, 7, 2 );

  QListBox *listbox = new QListBox( area, "GradientListBox" );
  layout->addMultiCellWidget( listbox, 0, 4, 0, 0 );

  QPushButton *newButton = new QPushButton( i18n( "New" ), area, "NewButton" );
  layout->addWidget( newButton, 0, 1 );

  QPushButton *copyButton = new QPushButton( i18n( "Copy" ), area, "CopyButton" );
  layout->addWidget( copyButton, 1, 1 );

  QPushButton *deleteButton = new QPushButton( i18n( "Delete" ), area, "DeleteButton" );
  layout->addWidget( deleteButton, 2, 1 );

  QPushButton *refreshButton = new QPushButton( i18n( "Refresh" ), area, "RefreshButton" );
  layout->addWidget( refreshButton, 3, 1 );

  QWidget *gradient = new QWidget( area, "GradientView" );
  gradient->setBackgroundColor( red );
  layout->addMultiCellWidget( gradient, 5, 5, 0, 1 );

  QScrollBar *scroll = new QScrollBar( Horizontal, area, "GradientViewScrollBar" );
  layout->addMultiCellWidget( scroll, 6, 6, 0, 1 );

  layout->setColStretch( 1, 0 );

  layout->setRowStretch( 0, 0 );
  layout->setRowStretch( 1, 0 );
  layout->setRowStretch( 2, 0 );
  layout->setRowStretch( 3, 0 );

  // read filnames
  // QStringList
  listbox->insertItem( "Test 1" );
  listbox->insertItem( "Test 2" );
  listbox->insertItem( "Test 3" );
  listbox->insertItem( "Test 4" );
  listbox->insertItem( "Test 5" );
}

GradientEditorDialog::~GradientEditorDialog()
{
}

#include "gradienteditordlg.moc"









