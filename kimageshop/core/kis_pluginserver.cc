/*
 *  kis_pluginserver.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter <elter@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <qdir.h>
#include <qfileinfo.h>
#include <qstringlist.h>

#include <ksimpleconfig.h>
#include <klocale.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kinstance.h>

#include "kis_factory.h"
#include "kis_pluginserver.h"

KisPluginServer::KisPluginServer()
{
  m_plugins.setAutoDelete(true);

  /*
   * Find plugin dirs. For example ~/.kde/share/apps/kimageshop/plugins or
   * $KDEDIR/share/apps/kimageshop/plugins
   */
  QStringList pluginDirs = KisFactory::global()->dirs()->resourceDirs("kis_plugins");

  if (!pluginDirs.isEmpty())
    {
      QStringList::Iterator it;

      for ( it = pluginDirs.begin(); it != pluginDirs.end(); ++it )
	{
	  qDebug("Searching plugins in: %s", (*it).latin1());
	  findPlugins(*it);
	}
    }
  else
    qDebug("Warning: No plugin directories found.");


  // build plugin database
  //findPlugins  ( locate("kis", "kimageshop.rc", KisFactory::global()) )
}

KisPluginServer::~KisPluginServer()
{
  m_plugins.clear();
}

void KisPluginServer::findPlugins( const QString &directory )
{
  QString pname, pcomment, pdir, plib, ptype;
  PluginType type = PLUGIN_FILTER;

  QDir dir(directory, "*.kisplugin");
  if (!dir.exists())
    return;
  
  const QFileInfoList *list = dir.entryInfoList();
  QFileInfoListIterator it(*list);
  QFileInfo *fi;

  while ((fi = it.current()))
    {
      KSimpleConfig config(fi->absFilePath(), true);

      config.setGroup("General");
      pname = config.readEntry("Name", fi->baseName());
      pcomment = config.readEntry("Comment", i18n("No description available."));
      pdir = directory + config.readEntry("Subdir", fi->baseName());
      plib = config.readEntry("Library", QString("libkisp_") + fi->baseName());
      ptype = config.readEntry("Type", "Filter");

      if ( ptype == "Filter" )
	type = PLUGIN_FILTER;
      else if (ptype == "Tool" )
	type = PLUGIN_TOOL;
      else
	qDebug("Warning: %s is not a valid KImageShop plugin type.", ptype.latin1()); 
      
      PluginInfo *pi = new PluginInfo(pname, pcomment, pdir, plib, type);
      m_plugins.append(pi);
     
      ++it;
    }
}
