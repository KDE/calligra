#include <plugin.h>

#include <klocale.h>
#include <kstddirs.h>
#include <kmessagebox.h>

// kword includes
//#include <kword_doc.h>
//#include <serialletter.h>

/*
 *  use the KWord-class KWSerialLetterDataBase for all actions with serial letters
 *
 *  a pointer to a KWSerialLetterDataBase can be got from KWordDocument::getSerialLetterDataBase()
 *
 *  /me wonders how to get current document
 */

PluginKWordAddresses::PluginKWordAddresses( QObject* parent, const char* name )
  : Plugin( parent, name )
{
  setXMLFile( locate( "data", "kword/kpartplugins/kword_addresses.rc" ) );

  new KAction( i18n( "Import addresses" ), 0, this, SLOT( slotImport() ), actionCollection(), "kword_addresses_import" );
  new KAction( i18n( "Export addresses" ), 0, this, SLOT( slotExport() ), actionCollection(), "kword_addresses_export" );
  new KAction( i18n( "Configure address im-/export" ), 0, this, SLOT( slotConfigure() ), actionCollection(), "kword_addresses_configure" );
}

PluginKWordAddresses::~PluginKWordAddresses()
{
}

void PluginKWordAddresses::slotImport()
{
  KMessageBox::information( 0L, i18n( "Sorry, not implemented yet!" ) );
}

void PluginKWordAddresses::slotExport()
{
  KMessageBox::information( 0L, i18n( "Sorry, not implemented yet!" ) );
}

void PluginKWordAddresses::slotConfigure()
{
  KMessageBox::information( 0L, i18n( "Sorry, not implemented yet!" ) );
}
