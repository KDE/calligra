#ifndef FAKE_BROWSERARGUMENTS_H
#define FAKE_BROWSERARGUMENTS_H

#include <QObject>

namespace KParts {

struct BrowserArguments
{
  BrowserArguments() {}

#if 0
  /**
   * This buffer can be used by the part to save and restore its contents.
   * See KHTMLPart for instance.
   */
  QStringList docState;

  /**
   * @p softReload is set when user just hits reload button. It's used
   * currently for two different frameset reload strategies. In case of
   * soft reload individual frames are reloaded instead of reloading whole
   * frameset.
   */
  bool softReload;

  /**
   * KHTML-specific field, contents of the HTTP POST data.
   */
  QByteArray postData;

  /**
   * KHTML-specific field, header defining the type of the POST data.
   */
  void setContentType( const QString & contentType );
  /**
   * KHTML-specific field, header defining the type of the POST data.
   */
  QString contentType() const;
  /**
   * KHTML-specific field, whether to do a POST instead of a GET,
   * for the next openURL.
   */
  void setDoPost( bool enable );

  /**
   * KHTML-specific field, whether to do a POST instead of a GET,
   * for the next openURL.
   */
  bool doPost() const;

  /**
   * Whether to lock the history when opening the next URL.
   * This is used during e.g. a redirection, to avoid a new entry
   * in the history.
   */
  void setLockHistory( bool lock );
  bool lockHistory() const;

  /**
   * Whether the URL should be opened in a new tab instead in a new window.
   */
  void setNewTab( bool newTab );
  bool newTab() const;

  /**
   * The frame in which to open the URL. KHTML/Konqueror-specific.
   */
  QString frameName;

  /**
   * If true, the part who asks for a URL to be opened can be 'trusted'
   * to execute applications. For instance, the directory views can be
   * 'trusted' whereas HTML pages are not trusted in that respect.
   */
  bool trustedSource;

  /**
   * @return true if the request was a result of a META refresh/redirect request or
   * HTTP redirect.
   */
  bool redirectedRequest () const;

  /**
   * Set the redirect flag to indicate URL is a result of either a META redirect
   * or HTTP redirect.
   *
   * @param redirected
   */
  void setRedirectedRequest(bool redirected);

  /**
   * Set whether the URL specifies to be opened in a new window.
   *
   * When openUrlRequest is emitted:
   * <ul>
   *  <li>normally the url would be opened in the current view.</li>
   *  <li>setForcesNewWindow(true) specifies that a new window or tab should be used:
   *  setNewTab(true) requests a tab specifically, otherwise the user-preference is followed.
   *  This is typically used for target="_blank" in web browsers.</li>
   * </ul>
   *
   * When createNewWindow is emitted:
   * <ul>
   *  <li>if setNewTab(true) was called, a tab is created.</li>
   *  <li>otherwise, if setForcesNewWindow(true) was called, a window is created.</li>
   *  <li>otherwise the user preference is followed.</li>
   * </ul>
   */
  void setForcesNewWindow( bool forcesNewWindow );

  /**
   * Whether the URL specifies to be opened in a new window
   */
  bool forcesNewWindow() const;
#endif
};

#if 0
class WindowArgsPrivate;

/**
 * The WindowArgs are used to specify arguments to the "create new window"
 * call (see the createNewWindow variant that uses WindowArgs).
 * The primary reason for this is the javascript window.open function.
 */
class KPARTS_EXPORT WindowArgs
{
public:
    WindowArgs();
    ~WindowArgs();
    WindowArgs( const WindowArgs &args );
    WindowArgs &operator=( const WindowArgs &args );
    WindowArgs( const QRect &_geometry, bool _fullscreen, bool _menuBarVisible,
                bool _toolBarsVisible, bool _statusBarVisible, bool _resizable );
    WindowArgs( int _x, int _y, int _width, int _height, bool _fullscreen,
                bool _menuBarVisible, bool _toolBarsVisible,
                bool _statusBarVisible, bool _resizable );

    void setX(int x);
    int x() const;

    void setY(int y);
    int y() const;

    void setWidth(int w);
    int width() const;

    void setHeight(int h);
    int height() const;

    void setFullScreen(bool fs);
    bool isFullScreen() const;

    void setMenuBarVisible(bool visible);
    bool isMenuBarVisible() const;

    void setToolBarsVisible(bool visible);
    bool toolBarsVisible() const;

    void setStatusBarVisible(bool visible);
    bool isStatusBarVisible() const;

    void setResizable(bool resizable);
    bool isResizable() const;

    void setLowerWindow(bool lower);
    bool lowerWindow() const;

    void setScrollBarsVisible(bool visible);
    bool scrollBarsVisible() const;

private:
    QSharedDataPointer<WindowArgsPrivate> d;
};

/**
 * The KParts::OpenUrlEvent event informs that a given part has opened a given URL.
 * Applications can use this event to send this information to interested plugins.
 *
 * The event should be sent before opening the URL in the part, so that the plugins
 * can use part()->url() to get the old URL.
 */
class KPARTS_EXPORT OpenUrlEvent : public Event
{
public:
  OpenUrlEvent( ReadOnlyPart *part, const KUrl &url,
                const OpenUrlArguments& args = OpenUrlArguments(),
                const BrowserArguments& browserArgs = BrowserArguments() );
  virtual ~OpenUrlEvent();

  ReadOnlyPart *part() const;
  KUrl url() const;
  OpenUrlArguments arguments() const;
  BrowserArguments browserArguments() const;

  static bool test( const QEvent *event );

private:
  class OpenUrlEventPrivate;
  OpenUrlEventPrivate * const d;
};

 /**
  * The Browser Extension is an extension (yes, no kidding) to
  * KParts::ReadOnlyPart, which allows a better integration of parts
  * with browsers (in particular Konqueror).
  * Remember that ReadOnlyPart only has openUrl(KUrl) and a few arguments() but not much more.
  * For full-fledged browsing, we need much more than that, including
  * enabling/disabling of standard actions (print, copy, paste...),
  * allowing parts to save and restore their data into the back/forward history,
  * allowing parts to control the location bar URL, to requests URLs
  * to be opened by the hosting browser, etc.
  *
  * The part developer needs to define its own class derived from BrowserExtension,
  * to implement the virtual methods [and the standard-actions slots, see below].
  *
  * The way to associate the BrowserExtension with the part is to simply
  * create the BrowserExtension as a child of the part (in QObject's terms).
  * The hosting application will look for it automatically.
  *
  * Another aspect of the browser integration is that a set of standard
  * actions are provided by the browser, but implemented by the part
  * (for the actions it supports).
  *
  * The following standard actions are defined by the host of the view:
  *
  * [selection-dependent actions]
  * @li @p cut : Copy selected items to clipboard and store 'not cut' in clipboard.
  * @li @p copy : Copy selected items to clipboard and store 'cut' in clipboard.
  * @li @p paste : Paste clipboard into view URL.
  * @li @p pasteTo(const KUrl &) : Paste clipboard into given URL.
  * @li @p searchProvider : Lookup selected text at default search provider
  *
  * [normal actions]
  * @li None anymore.
  *
  *
  * The view defines a slot with the name of the action in order to implement the action.
  * The browser will detect the slot automatically and connect its action to it when
  * appropriate (i.e. when the view is active).
  *
  *
  * The selection-dependent actions are disabled by default and the view should
  * enable them when the selection changes, emitting enableAction().
  *
  * The normal actions do not depend on the selection.
  *
  * A special case is the configuration slots, not connected to any action directly.
  *
  * [configuration slot]
  * @li @p reparseConfiguration : Re-read configuration and apply it.
  * @li @p disableScrolling: no scrollbars
  */
#endif

class BrowserExtension : public QObject
{
  //Q_OBJECT
  //Q_PROPERTY( bool urlDropHandling READ isURLDropHandlingEnabled WRITE setURLDropHandlingEnabled )
public:

#if 0
  /**
   * Constructor
   *
   * @param parent The KParts::ReadOnlyPart that this extension ... "extends" :)
   */
  explicit BrowserExtension( KParts::ReadOnlyPart *parent );


  virtual ~BrowserExtension();

  /**
   * Set of flags passed via the popupMenu signal, to ask for some items in the popup menu.
   */
  enum PopupFlag {
      DefaultPopupItems=0x0000, /**< default value, no additional menu item */
      ShowNavigationItems=0x0001, /**< show "back" and "forward" (usually done when clicking the background of the view, but not an item) */
      ShowUp=0x0002, /**<  show "up" (same thing, but not over e.g. HTTP). Requires ShowNavigationItems. */
      ShowReload=0x0004, /**< show "reload" (usually done when clicking the background of the view, but not an item) */
      ShowBookmark=0x0008, /**< show "add to bookmarks" (usually not done on the local filesystem) */
      ShowCreateDirectory=0x0010, /**<  show "create directory" (usually only done on the background of the view, or
       *                      in hierarchical views like directory trees, where the new dir would be visible) */
      ShowTextSelectionItems=0x0020, /**< set when selecting text, for a popup that only contains text-related items. */
      NoDeletion=0x0040, /**< deletion, trashing and renaming not allowed (e.g. parent dir not writeable).
       *            (this is only needed if the protocol itself supports deletion, unlike e.g. HTTP) */
      IsLink=0x0080, /**< show "Bookmark This Link" and other link-related actions (linkactions merging group) */
      ShowUrlOperations=0x0100, /**< show copy, paste, as well as cut if NoDeletion is not set. */
      ShowProperties=0x200     /**< show "Properties" action (usually done by directory views) */
  };

  Q_DECLARE_FLAGS( PopupFlags, PopupFlag )

  /**
   * Set the parameters to use for opening the next URL.
   * This is called by the "hosting" application, to pass parameters to the part.
   * @see BrowserArguments
   */
  virtual void setBrowserArguments( const BrowserArguments &args );

  /**
   * Retrieve the set of parameters to use for opening the URL
   * (this must be called from openUrl() in the part).
   * @see BrowserArguments
   */
  BrowserArguments browserArguments() const;

  /**
   * Returns the current x offset.
   *
   * For a scrollview, implement this using contentsX().
   */
  virtual int xOffset();
  /**
   * Returns the current y offset.
   *
   * For a scrollview, implement this using contentsY().
   */
  virtual int yOffset();

  /**
   * Used by the browser to save the current state of the view
   * (in order to restore it if going back in navigation).
   *
   * If you want to save additional properties, reimplement it
   * but don't forget to call the parent method (probably first).
   */
  virtual void saveState( QDataStream &stream );

  /**
   * Used by the browser to restore the view in the state
   * it was when we left it.
   *
   * If you saved additional properties, reimplement it
   * but don't forget to call the parent method (probably first).
   */
  virtual void restoreState( QDataStream &stream );

  /**
   * Returns whether url drop handling is enabled.
   * See setURLDropHandlingEnabled for more information about this
   * property.
   */
  bool isURLDropHandlingEnabled() const;

  /**
   * Enables or disables url drop handling. URL drop handling is a property
   * describing whether the hosting shell component is allowed to install an
   * event filter on the part's widget, to listen for URI drop events.
   * Set it to true if you are exporting a BrowserExtension implementation and
   * do not provide any special URI drop handling. If set to false you can be
   * sure to receive all those URI drop events unfiltered. Also note that the
   * implementation as of Konqueror installs the event filter only on the part's
   * widget itself, not on child widgets.
   */
  void setURLDropHandlingEnabled( bool enable );

  void setBrowserInterface( BrowserInterface *impl );
  BrowserInterface *browserInterface() const;

  /**
   * @return the status (enabled/disabled) of an action.
   * When the enableAction signal is emitted, the browserextension
   * stores the status of the action internally, so that it's possible
   * to query later for the status of the action, using this method.
   */
  bool isActionEnabled( const char * name ) const;

  /**
   * @return the text of an action, if it was set explicitly by the part.
   * When the setActionText signal is emitted, the browserextension
   * stores the text of the action internally, so that it's possible
   * to query later for the text of the action, using this method.
   */
  QString actionText( const char * name ) const;

  typedef QMap<QByteArray,QByteArray> ActionSlotMap;
  /**
   * Returns a map containing the action names as keys and corresponding
   * SLOT()'ified method names as data entries.
   *
   * This is very useful for
   * the host component, when connecting the own signals with the
   * extension's slots.
   * Basically you iterate over the map, check if the extension implements
   * the slot and connect to the slot using the data value of your map
   * iterator.
   * Checking if the extension implements a certain slot can be done like this:
   *
   * \code
   *   extension->metaObject()->slotNames().contains( actionName + "()" )
   * \endcode
   *
   * (note that @p actionName is the iterator's key value if already
   *  iterating over the action slot map, returned by this method)
   *
   * Connecting to the slot can be done like this:
   *
   * \code
   *   connect( yourObject, SIGNAL( yourSignal() ),
   *            extension, mapIterator.data() )
   * \endcode
   *
   * (where "mapIterator" is your QMap<QCString,QCString> iterator)
   */
  static ActionSlotMap actionSlotMap();

  /**
   * @return a pointer to the static action-slot map. Preferred method to get it.
   * The map is created if it doesn't exist yet
   */
  static ActionSlotMap * actionSlotMapPtr();

  /**
   * Queries @p obj for a child object which inherits from this
   * BrowserExtension class. Convenience method.
   */
  static BrowserExtension *childObject( QObject *obj );

  /**
   * Asks the hosting browser to perform a paste (using openUrlRequestDelayed())
   */
  void pasteRequest();

    /**
     * Associates a list of actions with a predefined name known by the host's popupmenu:
     * "editactions" for actions related text editing,
     * "linkactions" for actions related to hyperlinks,
     * "partactions" for any other actions provided by the part
     */
    typedef QMap<QString, QList<QAction *> > ActionGroupMap;

Q_SIGNALS:
#if !defined(Q_MOC_RUN) && !defined(DOXYGEN_SHOULD_SKIP_THIS) && !defined(IN_IDE_PARSER)
public:  // yes, those signals are public; don't tell moc, doxygen or kdevelop :)
#endif
  /**
   * Enables or disable a standard action held by the browser.
   *
   * See class documentation for the list of standard actions.
   */
  void enableAction( const char * name, bool enabled );

  /**
   * Change the text of a standard action held by the browser.
   * This can be used to change "Paste" into "Paste Image" for instance.
   *
   * See class documentation for the list of standard actions.
   */
  void setActionText( const char * name, const QString& text );

  /**
   * Asks the host (browser) to open @p url.
   * To set a reload, the x and y offsets, the service type etc., fill in the
   * appropriate fields in the @p args structure.
   * Hosts should not connect to this signal but to openUrlRequestDelayed().
   */
  void openUrlRequest( const KUrl &url,
                       const KParts::OpenUrlArguments& arguments = KParts::OpenUrlArguments(),
                       const KParts::BrowserArguments &browserArguments = KParts::BrowserArguments() );

  /**
   * This signal is emitted when openUrlRequest() is called, after a 0-seconds timer.
   * This allows the caller to terminate what it's doing first, before (usually)
   * being destroyed. Parts should never use this signal, hosts should only connect
   * to this signal.
   */
  void openUrlRequestDelayed( const KUrl &url,
                              const KParts::OpenUrlArguments& arguments,
                              const KParts::BrowserArguments &browserArguments );

  /**
   * Tells the hosting browser that the part opened a new URL (which can be
   * queried via KParts::Part::url().
   *
   * This helps the browser to update/create an entry in the history.
   * The part may @em not emit this signal together with openUrlRequest().
   * Emit openUrlRequest() if you want the browser to handle a URL the user
   * asked to open (from within your part/document). This signal however is
   * useful if you want to handle URLs all yourself internally, while still
   * telling the hosting browser about new opened URLs, in order to provide
   * a proper history functionality to the user.
   * An example of usage is a html rendering component which wants to emit
   * this signal when a child frame document changed its URL.
   * Conclusion: you probably want to use openUrlRequest() instead.
   */
  void openUrlNotify();

  /**
   * Updates the URL shown in the browser's location bar to @p url.
   */
  void setLocationBarUrl( const QString &url );

  /**
   * Sets the URL of an icon for the currently displayed page.
   */
  void setIconUrl( const KUrl &url );

  /**
   * Asks the hosting browser to open a new window for the given @p url
   * and return a reference to the content part.
   *
   * @p arguments is optional additional information about how to open the url,
   * @see KParts::OpenUrlArguments
   *
   * @p browserArguments is optional additional information for web browsers,
   * @see KParts::BrowserArguments
   *
   * The request for a pointer to the part is only fulfilled/processed
   * if the mimeType is set in the @p browserArguments.
   * (otherwise the request cannot be processed synchronously).
   */
    void createNewWindow( const KUrl &url,
                          const KParts::OpenUrlArguments& arguments = KParts::OpenUrlArguments(),
                          const KParts::BrowserArguments &browserArguments = KParts::BrowserArguments(),
                          const KParts::WindowArgs &windowArgs = KParts::WindowArgs(),
                          KParts::ReadOnlyPart** part = 0 ); // TODO consider moving to BrowserHostExtension?

  /**
   * Since the part emits the jobid in the started() signal,
   * progress information is automatically displayed.
   *
   * However, if you don't use a KIO::Job in the part,
   * you can use loadingProgress() and speedProgress()
   * to display progress information.
   */
  void loadingProgress( int percent );
  /**
   * @see loadingProgress
   */
  void speedProgress( int bytesPerSecond );

  void infoMessage( const QString & );

  /**
   * Emit this to make the browser show a standard popup menu for the files @p items.
   *
   * @param global global coordinates where the popup should be shown
   * @param items list of file items which the popup applies to
   * @param args OpenUrlArguments, mostly for metadata here
   * @param browserArguments BrowserArguments, mostly for referrer
   * @param flags enables/disables certain builtin actions in the popupmenu
   * @param actionGroups named groups of actions which should be inserted into the popup, see ActionGroupMap
   */
  void popupMenu( const QPoint &global, const KFileItemList &items,
                  const KParts::OpenUrlArguments &args = KParts::OpenUrlArguments(),
                  const KParts::BrowserArguments &browserArgs = KParts::BrowserArguments(),
                  KParts::BrowserExtension::PopupFlags flags = KParts::BrowserExtension::DefaultPopupItems,
                  const KParts::BrowserExtension::ActionGroupMap& actionGroups = ActionGroupMap() );

  /**
   * Emit this to make the browser show a standard popup menu for the given @p url.
   *
   * Give as much information about this URL as possible,
   * like @p args.mimeType and the file type @p mode
   *
   * @param global global coordinates where the popup should be shown
   * @param url the URL this popup applies to
   * @param mode the file type of the url (S_IFREG, S_IFDIR...)
   * @param args OpenUrlArguments, set the mimetype of the URL using setMimeType()
   * @param browserArguments BrowserArguments, mostly for referrer
   * @param flags enables/disables certain builtin actions in the popupmenu
   * @param actionGroups named groups of actions which should be inserted into the popup, see ActionGroupMap
   */
  void popupMenu( const QPoint &global, const KUrl &url,
                  mode_t mode = static_cast<mode_t>(-1),
                  const KParts::OpenUrlArguments &args = KParts::OpenUrlArguments(),
                  const KParts::BrowserArguments &browserArgs = KParts::BrowserArguments(),
                  KParts::BrowserExtension::PopupFlags flags = KParts::BrowserExtension::DefaultPopupItems,
                  const KParts::BrowserExtension::ActionGroupMap& actionGroups = ActionGroupMap() );

  /**
   * Inform the hosting application about the current selection.
   * Used when a set of files/URLs is selected (with full information
   * about those URLs, including size, permissions etc.)
   */
  void selectionInfo( const KFileItemList& items );
  /**
   * Inform the hosting application about the current selection.
   * Used when some text is selected.
   */
  void selectionInfo( const QString &text );
  /**
   * Inform the hosting application about the current selection.
   * Used when a set of URLs is selected.
   */
  void selectionInfo( const KUrl::List &urls );

  /**
   * Inform the hosting application that the user moved the mouse over an item.
   * Used when the mouse is on an URL.
   */
  void mouseOverInfo( const KFileItem& item );

  /**
   * Ask the hosting application to add a new HTML (aka Mozilla/Netscape)
   * SideBar entry.
   */
  void addWebSideBar(const KUrl &url, const QString& name);

  /**
   * Ask the hosting application to move the top level widget.
   */
  void moveTopLevelWidget( int x, int y );

  /**
   * Ask the hosting application to resize the top level widget.
   */
  void resizeTopLevelWidget( int w, int h );

  /**
   * Ask the hosting application to focus @p part.
   */
  void requestFocus(KParts::ReadOnlyPart *part);

  /**
   * Tell the host (browser) about security state of current page
   * enum PageSecurity { NotCrypted, Encrypted, Mixed };
   */
  void setPageSecurity( int );

  /**
   * Inform the host about items that have been removed.
   */
  void itemsRemoved( const KFileItemList &items );

private Q_SLOTS:
  void slotCompleted();
  void slotOpenUrlRequest( const KUrl &url,
                           const KParts::OpenUrlArguments& arguments = KParts::OpenUrlArguments(),
                           const KParts::BrowserArguments &browserArguments = KParts::BrowserArguments() );

  void slotEmitOpenUrlRequestDelayed();
  void slotEnableAction( const char *, bool );
  void slotSetActionText( const char*, const QString& );

public:
  typedef QMap<QByteArray,int> ActionNumberMap;

private:
  class BrowserExtensionPrivate;
  BrowserExtensionPrivate * const d;
#endif
};

#if 0
/**
 * An extension class for container parts, i.e. parts that contain
 * other parts.
 * For instance a KHTMLPart hosts one part per frame.
 */
class KPARTS_EXPORT BrowserHostExtension : public QObject
{
  Q_OBJECT
public:
  BrowserHostExtension( KParts::ReadOnlyPart *parent );

  virtual ~BrowserHostExtension();

  /**
   * Returns a list of the names of all hosted child objects.
   *
   * Note that this method does not query the child objects recursively.
   */
  virtual QStringList frameNames() const;

  /**
   * Returns a list of pointers to all hosted child objects.
   *
   * Note that this method does not query the child objects recursively.
   */
  virtual const QList<KParts::ReadOnlyPart*> frames() const;

  /**
   * Returns the part that contains @p frame and that may be accessed
   * by @p callingPart
   */
  virtual BrowserHostExtension *findFrameParent(KParts::ReadOnlyPart *callingPart, const QString &frame);

  /**
   * Opens the given url in a hosted child frame. The frame name is specified in the
   * frameName variable in the @p browserArguments parameter (see KParts::BrowserArguments ) .
   */
  virtual bool openUrlInFrame( const KUrl &url,
                               const KParts::OpenUrlArguments& arguments,
                               const KParts::BrowserArguments &browserArguments );

  /**
   * Queries @p obj for a child object which inherits from this
   * BrowserHostExtension class. Convenience method.
   */
  static BrowserHostExtension *childObject( QObject *obj );

private:
  class BrowserHostExtensionPrivate;
  BrowserHostExtensionPrivate * const d;
};

/**
 * An extension class for LiveConnect, i.e. a call from JavaScript
 * from a HTML page which embeds this part.
 * A part can have an object hierarchy by using objid as a reference
 * to an object.
 */
class KPARTS_EXPORT LiveConnectExtension : public QObject
{
  Q_OBJECT
public:
  enum Type {
      TypeVoid=0, TypeBool, TypeFunction, TypeNumber, TypeObject, TypeString
  };
  typedef QList<QPair<Type, QString> > ArgList;

  LiveConnectExtension( KParts::ReadOnlyPart *parent );

  virtual ~LiveConnectExtension();
  /**
   * get a field value from objid, return true on success
   */
  virtual bool get( const unsigned long objid, const QString & field, Type & type, unsigned long & retobjid, QString & value );
  /**
   * put a field value in objid, return true on success
   */
  virtual bool put( const unsigned long objid, const QString & field, const QString & value );
  /**
   * calls a function of objid, return true on success
   */
  virtual bool call( const unsigned long objid, const QString & func, const QStringList & args, Type & type, unsigned long & retobjid, QString & value );
  /**
   * notifies the part that there is no reference anymore to objid
   */
  virtual void unregister( const unsigned long objid );

  static LiveConnectExtension *childObject( QObject *obj );
Q_SIGNALS:
#if !defined(Q_MOC_RUN) && !defined(DOXYGEN_SHOULD_SKIP_THIS) && !defined(IN_IDE_PARSER)
public:  // yes, those signals are public; don't tell moc, doxygen or kdevelop :)
#endif
  /**
   * notify a event from the part of object objid
   */
  void partEvent( const unsigned long objid, const QString & event, const KParts::LiveConnectExtension::ArgList & args );

private:
  class LiveConnectExtensionPrivate;
  LiveConnectExtensionPrivate * const d;
};
#endif

}

#if 0
Q_DECLARE_OPERATORS_FOR_FLAGS( KParts::BrowserExtension::PopupFlags )
#endif

#endif
