#ifndef FAKE_KXMLGUIFACTORY_H
#define FAKE_KXMLGUIFACTORY_H

#include <QObject>
#include <QWidget>
#include <kxmlguiclient.h>

class KXMLGUIFactory : public QObject
{
public:

#if 0
  /**
   * Constructs a KXMLGUIFactory. The provided @p builder KXMLGUIBuilder will be called
   * for creating and removing container widgets, when clients are added/removed from the GUI.
   *
   * Note that the ownership of the given KXMLGUIBuilder object won't be transferred to this
   * KXMLGUIFactory, so you have to take care of deleting it properly.
   */
  explicit KXMLGUIFactory( KXMLGUIBuilder *builder, QObject *parent = 0 );

  /**
   * Destructor
   */
  ~KXMLGUIFactory();

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

#endif
  void addClient( KXMLGUIClient *client ) { m_clients.append(client); }
  void removeClient( KXMLGUIClient *client ) { m_clients.removeAll(client); }
  void plugActionList( KXMLGUIClient *client, const QString &name, const QList<QAction*> &actionList ) {}
  void unplugActionList( KXMLGUIClient *client, const QString &name ) {}
  QList<KXMLGUIClient*> clients() const { return m_clients; }
#if 0
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
#endif

  QWidget *container( const QString &containerName, KXMLGUIClient *client, bool useTagName = false )
  {
      return 0;
  }

  QList<QWidget*> containers( const QString &tagName )
  {
      return QList<QWidget*>();
  }

#if 0
  /**
   * Use this method to free all memory allocated by the KXMLGUIFactory. This deletes the internal node
   * tree and therefore resets the internal state of the class. Please note that the actual GUI is
   * NOT touched at all, meaning no containers are deleted nor any actions unplugged. That is
   * something you have to do on your own. So use this method only if you know what you are doing :-)
   *
   * (also note that this will call KXMLGUIClient::setFactory( 0 ) for all inserted clients)
   */
  void reset();

  /**
   * Use this method to free all memory allocated by the KXMLGUIFactory for a specific container,
   * including all child containers and actions. This deletes the internal node subtree for the
   * specified container. The actual GUI is not touched, no containers are deleted or any actions
   * unplugged. Use this method only if you know what you are doing :-)
   *
   * (also note that this will call KXMLGUIClient::setFactory( 0 ) for all clients of the
   * container)
   */
  void resetContainer( const QString &containerName, bool useTagName = false );

  /**
   * Use this method to reset and reread action properties (shortcuts, etc.) for all actions.
   * This is needed, for example, when you change shortcuts scheme at runtime.
   */
  void refreshActionProperties();

 public Q_SLOTS:
#endif

  int configureShortcuts(bool bAllowLetterShortcuts = true, bool bSaveSettings = true) { return 0; }
  void changeShortcutScheme(const QString &scheme) {}

#if 0
Q_SIGNALS:
  void clientAdded( KXMLGUIClient *client );
  void clientRemoved( KXMLGUIClient *client );

    /**
     * Emitted when the factory is currently making changes to the GUI,
     * i.e. adding or removing clients.
     * makingChanges(true) is emitted before any change happens, and
     * makingChanges(false) is emitted after the change is done.
     * This allows e.g. KMainWindow to know that the GUI is
     * being changed programmatically and not by the user (so there is no reason to
     * save toolbar settings afterwards).
     * @since 4.1.3
     */
    void makingChanges(bool);

#endif
private:
    QList<KXMLGUIClient*> m_clients;
};

#endif
 
