/* This file is part of the KDE libraries
   Copyright (C) 1999 Simon Hausmann <hausmann@kde.org>
   Copyright (C) 2000 Kurt Granroth <granroth@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KoXMLGuiFactory_h
#define KoXMLGuiFactory_h

#include <kdeui_export.h>
#include <kcomponentdata.h>

#include <QtCore/QObject>

class QAction;
class KoXMLGUIFactoryPrivate;
class KoXMLGUIClient;
class KoXMLGUIBuilder;
class KComponentData;

class QDomAttr;
class QDomDocument;
class QDomElement;
class QDomNode;
class QDomNamedNodeMap;

namespace KoXMLGUI
{
struct MergingIndex;
struct ContainerNode;
struct ContainerClient;
class BuildHelper;
}

/**
 * KoXMLGUIFactory, together with KoXMLGUIClient objects, can be used to create
 * a GUI of container widgets (like menus, toolbars, etc.) and container items
 * (menu items, toolbar buttons, etc.) from an XML document and action objects.
 *
 * Each KoXMLGUIClient represents a part of the GUI, composed from containers and
 * actions. KoXMLGUIFactory takes care of building (with the help of a KoXMLGUIBuilder)
 * and merging the GUI from an unlimited number of clients.
 *
 * Each client provides XML through a QDomDocument and actions through a
 * KActionCollection . The XML document contains the rules for how to merge the
 * GUI.
 *
 * KoXMLGUIFactory processes the DOM tree provided by a client and plugs in the client's actions,
 * according to the XML and the merging rules of previously inserted clients. Container widgets
 * are built via a KoXMLGUIBuilder , which has to be provided with the KoXMLGUIFactory constructor.
 */
class KDEUI_EXPORT KoXMLGUIFactory : public QObject
{
  friend class KoXMLGUI::BuildHelper;
  Q_OBJECT
 public:
  /**
   * Constructs a KoXMLGUIFactory. The provided @p builder KoXMLGUIBuilder will be called
   * for creating and removing container widgets, when clients are added/removed from the GUI.
   *
   * Note that the ownership of the given KoXMLGUIBuilder object won't be transferred to this
   * KoXMLGUIFactory, so you have to take care of deleting it properly.
   */
  explicit KoXMLGUIFactory( KoXMLGUIBuilder *builder, QObject *parent = 0 );

  /**
   * Destructor
   */
  ~KoXMLGUIFactory();

  // XXX move to somewhere else? (Simon)
  /// @internal
  static QString readConfigFile(const QString &filename,
          const KComponentData &componentData = KComponentData());
  /// @internal
  static bool saveConfigFile(const QDomDocument& doc, const QString& filename,
          const KComponentData &componentData = KComponentData());

  /**
   * @internal
   * Find or create the ActionProperties element, used when saving custom action properties
   */
  static QDomElement actionPropertiesElement( QDomDocument& doc );

  /**
   * @internal
   * Find or create the element for a given action, by name.
   * Used when saving custom action properties
   */
  static QDomElement findActionByName( QDomElement& elem, const QString& sName, bool create );

  /**
   * Creates the GUI described by the QDomDocument of the client,
   * using the client's actions, and merges it with the previously
   * created GUI.
   * This also means that the order in which clients are added to the factory
   * is relevant; assuming that your application supports plugins, you should
   * first add your application to the factory and then the plugin, so that the
   * plugin's UI is merged into the UI of your application, and not the other
   * way round.
   */
  void addClient( KoXMLGUIClient *client );

  /**
   * Removes the GUI described by the client, by unplugging all
   * provided actions and removing all owned containers (and storing
   * container state information in the given client)
   */
  void removeClient( KoXMLGUIClient *client );

  void plugActionList( KoXMLGUIClient *client, const QString &name, const QList<QAction*> &actionList );
  void unplugActionList( KoXMLGUIClient *client, const QString &name );

  /**
   * Returns a list of all clients currently added to this factory
   */
  QList<KoXMLGUIClient*> clients() const;

  /**
   * Use this method to get access to a container widget with the name specified with @p containerName
   * and which is owned by the @p client. The container name is specified with a "name" attribute in the
   * XML document.
   *
   * This function is particularly useful for getting hold of a popupmenu defined in an XMLUI file.
   * For instance:
   * \code
   * QMenu *popup = static_cast<QMenu*>(guiFactory()->container("my_popup",this));
   * \endcode
   * where @p "my_popup" is the name of the menu in the XMLUI file, and
   * @p "this" is XMLGUIClient which owns the popupmenu (e.g. the mainwindow, or the part, or the plugin...)
   *
   * @param containerName Name of the container widget
   * @param client Owner of the container widget
   * @param useTagName Specifies whether to compare the specified name with the name attribute or
   *        the tag name.
   *
   * This method may return 0 if no container with the given name exists or is not owned by the client.
   */
  QWidget *container( const QString &containerName, KoXMLGUIClient *client, bool useTagName = false );

  QList<QWidget*> containers( const QString &tagName );

  /**
   * Use this method to free all memory allocated by the KoXMLGUIFactory. This deletes the internal node
   * tree and therefore resets the internal state of the class. Please note that the actual GUI is
   * NOT touched at all, meaning no containers are deleted nor any actions unplugged. That is
   * something you have to do on your own. So use this method only if you know what you are doing :-)
   *
   * (also note that this will call KoXMLGUIClient::setFactory( 0 ) for all inserted clients)
   */
  void reset();

  /**
   * Use this method to free all memory allocated by the KoXMLGUIFactory for a specific container,
   * including all child containers and actions. This deletes the internal node subtree for the
   * specified container. The actual GUI is not touched, no containers are deleted or any actions
   * unplugged. Use this method only if you know what you are doing :-)
   *
   * (also note that this will call KoXMLGUIClient::setFactory( 0 ) for all clients of the
   * container)
   */
  void resetContainer( const QString &containerName, bool useTagName = false );

  /**
   * Use this method to reset and reread action properties (shortcuts, etc.) for all actions.
   * This is needed, for example, when you change shortcuts scheme at runtime.
   */
  void refreshActionProperties();

 public Q_SLOTS:
  /**
   * Show a standard configure shortcut for every action in this factory.
   *
   * This slot can be connected directly to the action to configure shortcuts. This is very simple to
   * do that by adding a single line
   * \code
   * KStandardAction::keyBindings( guiFactory(), SLOT( configureShortcuts() ), actionCollection() );
   * \endcode
   *
   * @param bAllowLetterShortcuts Set to false if unmodified alphanumeric
   *      keys ('A', '1', etc.) are not permissible shortcuts.
   * @param bSaveSettings if true, the settings will also be saved back to
   *      the *uirc file which they were intially read from.
   */
  int configureShortcuts(bool bAllowLetterShortcuts = true, bool bSaveSettings = true);

  void changeShortcutScheme(const QString &scheme);

Q_SIGNALS:
  void clientAdded( KoXMLGUIClient *client );
  void clientRemoved( KoXMLGUIClient *client );

    /**
     * Emitted when the factory is currently making changes to the GUI,
     * i.e. adding or removing clients.
     * makingChanges(true) is emitted before any change happens, and
     * makingChanges(false) is emitted after the change is done.
     * This allows e.g. KoMainWindowBase to know that the GUI is
     * being changed programmatically and not by the user (so there is no reason to
     * save toolbar settings afterwards).
     * @since 4.1.3
     */
    void makingChanges(bool);

private:
    friend class KoXMLGUIClient;
    /// Internal, called by KoXMLGUIClient destructor
    void forgetClient(KoXMLGUIClient *client);

    KoXMLGUIFactoryPrivate *const d;
};

#endif
/* vim: et sw=4
 */
