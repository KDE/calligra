/*
 *  kimageshop_config.h - part of KImageShop
 *
 *  A global configuration class for KImageShop
 *
 *  Copyright (c) 1999 Carsten Pfeiffer <pfeiffer@kde.org>
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


#ifndef KIMAGESHOP_CONFIG_H
#define KIMAGESHOP_CONFIG_H

#include <qobject.h>


class QFont;

class LayerDlgConfig;
class BrushDlgConfig;
class ColorDlgConfig;
class GradientDlgConfig;
class GradientEditorConfig;

/**
 * Global configuration class for KImageShop
 *
 * There are different types of configuration settings:
 * @li global settings, that are the same for every full @ref KImageShopView
 * @li other settings, specific to one single KImageShopView instance
 *
 * Every KImageShopView instance creates a new object of this class to hold
 * instance specific settings; global settings are shared between all of them
 *
 * Use @see #getNewConfig to create an instance of this class:
 * the first instance will load settings from disk (global settings via
 * @see #loadConfig and instance specific settings via
 * @see loadGlobalSettings), subsequent instances will get the current
 * settings from the first instance.
 *
 * @author Carsten Pfeiffer <pfeiffer@kde.org>
 * @version $Id$
 */
class KImageShopConfig : public QObject
{
  Q_OBJECT

public:

  /**
   * A factory for config objects. Use it to create a new config object for
   * each new KImageShopView instance
   */
  static KImageShopConfig *getNewConfig();

  ~KImageShopConfig();

  /**
   * saves all document specific settings to disk - but for this instance only
   */
  void 	saveConfig();

  /**
   * saves all global settings - called from @see #saveAll and when the last
   * instance is closed
   */
  void 	saveGlobalSettings();

  /**
   * saves all settings for ALL instances - only called from sessionmanagement
   */
  void 	saveAll();

  /**
   * loads all document specific settings for this instance from disk - called
   * from the first constructor and from sessionmanagement (when there are
   * different documents created with different settings)
   */
  void 	loadConfig();


  void 	loadDialogSettings();
  void 	saveDialogSettings();


protected:

  /**
   * loads the global settings - called only once from the constructor of the
   * first instance
   */
  void 	loadGlobalSettings();

  /**
   * This constructor creates the first KImageShopConfig object and loads
   * the settings from disk. Subsequent objects get these settings from this
   * object thru the copy constructor, but transparently (@see #getNewConfig)
   */
  KImageShopConfig();

  /**
   *  A copy constructor for the KImageShopConfig class (doh)
   *
   * Used by @see #getNewConfig to create a new KImageShopConfig object for a
   * new KImageShopView
   */
  KImageShopConfig( const KImageShopConfig& );


signals:
  void 	globalConfigChanged();


private:

  // static objects - global items that are the same for _all_
  // KImageShopDoc objects
  static KConfig 	*kc;
  static bool		doInit;
  static QList<KImageShopConfig> 	instanceList;

  static QFont 		m_smallFont;
  static QFont		m_tinyFont;


  // document specific configuration (non-static)
  LayerDlgConfig 	*m_pLayerDlgConfig;
  BrushDlgConfig 	*m_pBrushDlgConfig;
  ColorDlgConfig 	*m_pColorDlgConfig;
  GradientDlgConfig 	*m_pGradientDlgConfig;
  GradientEditorConfig 	*m_pGradientEditorConfig;
  // ...
};



// separate configuration classes for all dialogs

/**
 * The interface, every configuration class must implement
 * (pure virtual class)
 */
class BaseConfig : public QObject
{
  Q_OBJECT

public:
  virtual void loadConfig( KConfig * ) = 0;
  virtual void saveConfig( KConfig * ) = 0;
};

class BaseKFDConfig : public BaseConfig
{
  Q_OBJECT

public:
  virtual void loadConfig( KConfig * );
  virtual void saveConfig( KConfig * );

private:
  bool m_docked;
  int m_posX;
  int m_posY;
};

class LayerDlgConfig : public BaseKFDConfig
{
  Q_OBJECT

public:
  LayerDlgConfig( QObject *parent=0, const char *name=0 );
  LayerDlgConfig( const LayerDlgConfig& );
  ~LayerDlgConfig() {};

  void loadConfig( KConfig * );
  void saveConfig( KConfig * );

private:

};

class BrushDlgConfig : public BaseKFDConfig
{
  Q_OBJECT

public:
  BrushDlgConfig( QObject *parent=0, const char *name=0 );
  BrushDlgConfig( const BrushDlgConfig& );
  ~BrushDlgConfig() {};

  void loadConfig( KConfig * );
  void saveConfig( KConfig * );


private:

};

class ColorDlgConfig : public BaseKFDConfig
{
  Q_OBJECT

public:
  ColorDlgConfig( QObject *parent=0, const char *name=0 );
  ColorDlgConfig( const ColorDlgConfig& );
  ~ColorDlgConfig() {};

  void loadConfig( KConfig * );
  void saveConfig( KConfig * );


private:

};

class GradientDlgConfig : public BaseKFDConfig
{
  Q_OBJECT

public:
  GradientDlgConfig( QObject *parent=0, const char *name=0 );
  GradientDlgConfig( const GradientDlgConfig& );
  ~GradientDlgConfig() {};

  void loadConfig( KConfig * );
  void saveConfig( KConfig * );


private:

};

class GradientEditorConfig : public BaseKFDConfig
{
  Q_OBJECT

public:
  GradientEditorConfig( QObject *parent=0, const char *name=0 );
  GradientEditorConfig( const GradientEditorConfig& );
  ~GradientEditorConfig() {};

  void loadConfig( KConfig * );
  void saveConfig( KConfig * );


private:

};


#endif // KIMAGESHOP_CONFIG_H
