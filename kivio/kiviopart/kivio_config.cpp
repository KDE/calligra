/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
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
#include "kivio_config.h"
#include "kivio_view.h"
#include "kiviooptionsdialog.h"
#include "kivio_page.h"
#include "kivio_command.h"
#include "kivio_doc.h"
#include "kivio_map.h"
#include "kivioglobal.h"

#include <qfile.h>
#include <qtextstream.h>

#include <kiconloader.h>
#include <kdebug.h>
#include <kdeversion.h>
#include <kstandarddirs.h>
#include <koPageLayoutDia.h>
#include <koUnit.h>
#include <klocale.h>

KivioOptions::KivioOptions()
{
  initGlobalConfig();
  initDefaultConfig();
}

KivioOptions::~KivioOptions()
{
  saveGlobalConfig();
}

void KivioOptions::initGlobalConfig()
{
  QDomDocument* doc = new QDomDocument("GlobalConfig");
  QDomElement root;

  QString path = locateLocal("appdata", "globalconfig");
  QFile f(path);
  if ( !f.open(IO_ReadOnly) ) {
    globalDefPageLayout = KoPageLayoutDia::standardLayout();
  } else {
    doc->setContent(&f);
    root = doc->documentElement();

    QDomElement ple = root.namedItem("PaperLayout").toElement();
    globalDefPageLayout = Kivio::loadPageLayout(ple);
  }

  delete doc;
}

void KivioOptions::initDefaultConfig()
{
  defPageLayout = globalDefPageLayout;
}

void KivioOptions::paperLayoutSetup(KivioView* view)
{
  KivioPage* page = view->activePage();
  KoPageLayout l = page->paperLayout();
  KoHeadFoot headfoot;
  int tabs = FORMAT_AND_BORDERS | DISABLE_UNIT;
  KoUnit::Unit unit = view->doc()->units();

  if(KoPageLayoutDia::pageLayout(l, headfoot, tabs, unit))
  {
    KivioDoc* doc = page->doc();
    KivioChangeLayoutCommand * cmd = new KivioChangeLayoutCommand(
      i18n("Change Page Layout"),page ,page->paperLayout(), l);
    doc->addCommand( cmd );
    page->setPaperLayout(l);
  }
}

void KivioOptions::setup(KivioView* view)
{
  KivioOptionsDialog dlg(view, "setupDialog");
  dlg.exec();
}

void KivioOptions::saveGlobalConfig()
{
  QDomDocument* doc = new QDomDocument("GlobalConfig");
  QDomElement root = doc->createElement("GlobalConfig");
  doc->appendChild(root);

  QDomElement ple = doc->createElement("PaperLayout");
  root.appendChild(ple);
  Kivio::savePageLayout(ple, globalDefPageLayout);

  QString path = locateLocal("appdata", "globalconfig");
  QFile f(path);
  QTextStream ts(&f);
  if (f.open(IO_WriteOnly)) {
    ts << *doc;
    f.close();
  }
  delete doc;
}

void KivioOptions::setDefaultPageLayout(const KoPageLayout &pl)
{
  defPageLayout = pl;
}

void KivioOptions::setGlobalDefaultPageLayout(const KoPageLayout &pl)
{
  globalDefPageLayout = pl;
  saveGlobalConfig();
}

void KivioOptions::save(QDomElement& element)
{
  QDomElement e = element.ownerDocument().createElement("DefPaperLayout");
  element.appendChild(e);
  Kivio::savePageLayout(e, defPageLayout);
}

void KivioOptions::load(const QDomElement& element)
{
  QDomElement ple = element.namedItem("DefPaperLayout").toElement();
  defPageLayout = Kivio::loadPageLayout(ple);
}

/**********************************************************************************/
static const char * connectorTarget_xpm[] = {
"7 7 3 1",
"       c None",
".      c #FFFFFF",
"+      c #051EFF",
" .   . ",
".+. .+.",
" .+.+. ",
"  .+.  ",
" .+.+. ",
".+. .+.",
" .   . "};



static const char * lock_xpm[] = {
"10 10 3 1",
" 	c None",
".	c #FFFFFF",
"+	c #000000",
"    ..    ",
"   .++.   ",
"  .+..+.  ",
"  .+..+.  ",
" .++++++. ",
" .+....+. ",
" .+.  .+. ",
" .+....+. ",
" .++++++. ",
"  ......  "};


KivioConfig *KivioConfig::s_config=0L;


/**
 * Constructs a config file from a filename
 *
 * @param fileName The file to read from
 *
 * This creates/reads a config file.  It sets default values
 * if certain values aren't found.
 */
KivioConfig::KivioConfig( const QString &fileName )
    : KSimpleConfig( fileName, false )
{
   kdDebug(43000) << "KivioConfig - created" << endl;

    QColor *defColor = new QColor(0x4BD2FF/* 154, 250, 154 */);
    m_stencilBGType  = (KivioConfig::StencilBGType)readNumEntry( "StencilBackgroundType", (int)sbgtColor );
    m_stencilBGFile  = readPathEntry( "StencilBackgroundFile" );
    m_stencilBGColor = readColorEntry( "StencilBackgroundColor", defColor );
    delete defColor;

    m_pStencilBGPixmap = NULL;

    // Load the stencil bg pixmap if applicable.  If it doesn't load properly, then
    // set the type to color and delete the pixmap.
    if( m_stencilBGType == sbgtPixmap )
    {
        m_pStencilBGPixmap = new QPixmap( BarIcon( m_stencilBGFile ) );
        if( !m_pStencilBGPixmap || m_pStencilBGPixmap->isNull() )
        {
            if( m_pStencilBGPixmap )
            {
                delete m_pStencilBGPixmap;
                m_pStencilBGPixmap = NULL;
            }
            m_stencilBGType = sbgtColor;
            kdDebug(43000) << "KivioConfig - Unable to load " << m_stencilBGFile << endl;
        }
        else
        {
	   kdDebug(43000) << "KivioConfig::KivioConfig() - loaded background" << endl;
        }
    }

    m_pConnectorTargetPixmap = new QPixmap( (const char **)connectorTarget_xpm);
    m_pLockPixmap = new QPixmap( (const char **)lock_xpm);

    kdDebug(43000) << "KivioConfig::KivioConfig() - StencilBackgroundType: " <<  m_stencilBGType << endl;
    kdDebug(43000) << "KivioConfig::KivioConfig() - StencilBackgroundFile: " << m_stencilBGFile << endl;
    kdDebug(43000) << "KivioConfig::KivioConfig() - StencilBackgroundColor: " << m_stencilBGColor.red()
	      << " " << m_stencilBGColor.green() << " " <<  m_stencilBGColor.blue() << endl;

    writeConfig();
}


/**
 * Destructor
 *
 * Deletes the background pixmap if applicable.  Also deletes the
 * pixmap which represents the connector targets.
 */
KivioConfig::~KivioConfig()
{
    if( m_pStencilBGPixmap )
    {
        delete m_pStencilBGPixmap;
        m_pStencilBGPixmap = NULL;
    }

    if( m_pConnectorTargetPixmap )
    {
        delete m_pConnectorTargetPixmap;
        m_pConnectorTargetPixmap = NULL;
    }

    if( m_pLockPixmap )
    {
       delete m_pLockPixmap;
       m_pLockPixmap = NULL;
    }

    kdDebug(43000) << "KivioConfig::~KivioConfig() - deleted" << endl;
}


/**
 * Writes the KivioConfig to file.
 */
void KivioConfig::writeConfig()
{
    writeEntry( "StencilBackgroundType", QString::number((int)m_stencilBGType) );
#if KDE_IS_VERSION(3,1,3)
    writePathEntry( "StencilBackgroundFile", m_stencilBGFile );
#else
    writeEntry( "StencilBackgroundFile", m_stencilBGFile );
#endif
    writeEntry( "StencilBackgroundColor", m_stencilBGColor.name() );

    sync();
}


/**
 * Destroys the static KivioConfig object.
 *
 * This function is called when the application is shutting down so
 * that all allocated memory is released.
 */
bool KivioConfig::deleteConfig()
{
    if( !KivioConfig::s_config )
        return false;

    delete KivioConfig::s_config;
    KivioConfig::s_config = NULL;

    return true;
}


/**
 * Reads the configuration into a static variable.
 *
 * This is called by the application during initialization so
 * the static variable s_config gets created.
 */
bool KivioConfig::readUserConfig()
{
    // Avoid double read
    if( s_config )
        return false;

    s_config = new KivioConfig( "kivio.conf" );


    return true;
}
#include "kivio_config.moc"
