#ifndef FAKE_KPROPERTIESDIALOG_H
#define FAKE_KPROPERTIESDIALOG_H

#include <kpagedialog.h>
#include <kurl.h>
#include <QVariant>

class KPropertiesDialog : public KPageDialog
{
public:
    KPropertiesDialog(QWidget *parent = 0) : KPageDialog(parent) {}
#if 0
  /**
   * Determine whether there are any property pages available for the
   * given file items.
   * @param _items the list of items to check.
   * @return true if there are any property pages, otherwise false.
   */
  static bool canDisplay( const KFileItemList& _items );

  /**
   * Brings up a Properties dialog, as shown above.
   * This is the normal constructor for
   * file-manager type applications, where you have a KFileItem instance
   * to work with.  Normally you will use this
   * method rather than the one below.
   *
   * @param item file item whose properties should be displayed.
   * @param parent is the parent of the dialog widget.
   * @param name is the internal name.
   */
  explicit KPropertiesDialog( const KFileItem& item,
                              QWidget* parent = 0 );

  /**
   * \overload
   *
   * You use this constructor for cases where you have a number of items,
   * rather than a single item. Be careful which methods you use
   * when passing a list of files or URLs, since some of them will only
   * work on the first item in a list.
   *
   * @param _items list of file items whose properties should be displayed.
   * @param parent is the parent of the dialog widget.
   * @param name is the internal name.
   */
  explicit KPropertiesDialog( const KFileItemList& _items,
                              QWidget *parent = 0 );

  /**
   * Brings up a Properties dialog. Convenience constructor for
   * non-file-manager applications, where you have a KUrl rather than a
   * KFileItem or KFileItemList.
   *
   * @param _url the URL whose properties should be displayed
   * @param parent is the parent of the dialog widget.
   * @param name is the internal name.
   *
   * IMPORTANT: This constructor, together with exec(), leads to a grave
   * display bug (due to KIO::stat() being run before the dialog has all the
   * necessary information). Do not use this combination for now.
   * TODO: Check if the above is still true with Qt4.
   * For local files with a known mimetype, simply create a KFileItem and pass
   * it to the other constructor.
   */
  explicit KPropertiesDialog( const KUrl& _url,
                              QWidget* parent = 0 );

  /**
   * Creates a properties dialog for a new .desktop file (whose name
   * is not known yet), based on a template. Special constructor for
   * "File / New" in file-manager type applications.
   *
   * @param _tempUrl template used for reading only
   * @param _currentDir directory where the file will be written to
   * @param _defaultName something to put in the name field,
   * like mimetype.desktop
   * @param parent is the parent of the dialog widget.
   * @param name is the internal name.
   */
  KPropertiesDialog( const KUrl& _tempUrl, const KUrl& _currentDir,
                     const QString& _defaultName,
                     QWidget* parent = 0 );

  /**
   * Creates an empty properties dialog (for applications that want use
   * a standard dialog, but for things not doable via the plugin-mechanism).
   *
   * @param title is the string display as the "filename" in the caption of the dialog.
   * @param parent is the parent of the dialog widget.
   * @param name is the internal name.
   * @param modal tells the dialog whether it should be modal.
   */
  explicit KPropertiesDialog(const QString& title,
                             QWidget* parent = 0);

  /**
   * Cleans up the properties dialog and frees any associated resources,
   * including the dialog itself. Note that when a properties dialog is
   * closed it cleans up and deletes itself.
   */
  virtual ~KPropertiesDialog();

  /**
   * Immediately displays a Properties dialog using constructor with
   * the same parameters.
   * On MS Windows, if @p item points to a local file, native (non modal) property
   * dialog is displayed (@p parent and @p modal are ignored in this case).
   *
   * @return true on successful dialog displaying (can be false on win32).
   */
  static bool showDialog(const KFileItem& item, QWidget* parent = 0,
                         bool modal = true);

  /**
   * Immediately displays a Properties dialog using constructor with
   * the same parameters.
   * On MS Windows, if @p _url points to a local file, native (non modal) property
   * dialog is displayed (@p parent and @p modal are ignored in this case).
   *
   * @return true on successful dialog displaying (can be false on win32).
   */
  static bool showDialog(const KUrl& _url, QWidget* parent = 0,
                         bool modal = true);

  /**
   * Immediately displays a Properties dialog using constructor with
   * the same parameters.
   * On MS Windows, if @p _items has one element and this element points
   * to a local file, native (non modal) property dialog is displayed
   * (@p parent and @p modal are ignored in this case).
   *
   * @return true on successful dialog displaying (can be false on win32).
   */
  static bool showDialog(const KFileItemList& _items, QWidget* parent = 0,
                         bool modal = true);

  /**
   * Adds a "3rd party" properties plugin to the dialog.  Useful
   * for extending the properties mechanism.
   *
   * To create a new plugin type, inherit from the base class KPropertiesDialogPlugin
   * and implement all the methods. If you define a service .desktop file
   * for your plugin, you do not need to call insertPlugin().
   *
   * @param plugin is a pointer to the KPropertiesDialogPlugin. The Properties
   *        dialog will do destruction for you. The KPropertiesDialogPlugin \b must
   *        have been created with the KPropertiesDialog as its parent.
   * @see KPropertiesDialogPlugin
   */
  void insertPlugin (KPropertiesDialogPlugin *plugin);
#endif

  KUrl kurl() const { return m_url; }
  KFileItem& item() { return m_items.isEmpty() ? m_nullItem : m_items.first(); }
  KFileItemList items() const { return m_items; }

#if 0
  /**
   * If the dialog is being built from a template, this method
   * returns the current directory. If no template, it returns QString().
   * See the template form of the constructor.
   *
   * @return the current directory or QString()
   */
  KUrl currentDir() const;

  /**
   * If the dialog is being built from a template, this method
   * returns the default name. If no template, it returns QString().
   * See the template form of the constructor.
   * @return the default name or QString()
   */
  QString defaultName() const;

  /**
   * Updates the item URL (either called by rename or because
   * a global apps/mimelnk desktop file is being saved)
   * Can only be called if the dialog applies to a single file or URL.
   * @param _newUrl the new URL
   */
  void updateUrl( const KUrl& _newUrl );

  /**
   * Renames the item to the specified name. This can only be called if
   * the dialog applies to a single file or URL.
   * @param _name new filename, encoded.
   * \see FilePropsDialogPlugin::applyChanges
   */
  void rename( const QString& _name );

  /**
   * To abort applying changes.
   */
  void abortApplying();

  /**
   * Shows the page that was previously set by
   * setFileSharingPage(), or does nothing if no page
   * was set yet.
   * \see setFileSharingPage
   */
  void showFileSharingPage();

  /**
   * Sets the file sharing page.
   * This page is shown when calling showFileSharingPage().
   *
   * @param page the page to set
   * \see showFileSharingPage
   */
  void setFileSharingPage(QWidget* page);

   /**
    * Call this to make the filename lineedit readonly, to prevent the user
    * from renaming the file.
    * \param ro true if the lineedit should be read only
    */
  void setFileNameReadOnly( bool ro );

public Q_SLOTS:
  /**
   * Called when the user presses 'Ok'.
   */
  virtual void slotOk();      // Deletes the PropertiesDialog instance
  /**
   * Called when the user presses 'Cancel'.
   */
  virtual void slotCancel();     // Deletes the PropertiesDialog instance

Q_SIGNALS:
  /**
   * This signal is emitted when the Properties Dialog is closed (for
   * example, with OK or Cancel buttons)
   */
  void propertiesClosed();

  /**
   * This signal is emitted when the properties changes are applied (for
   * example, with the OK button)
   */
  void applied();

  /**
   * This signal is emitted when the properties changes are aborted (for
   * example, with the Cancel button)
   */

  void canceled();
  /**
   * Emitted before changes to @p oldUrl are saved as @p newUrl.
   * The receiver may change @p newUrl to point to an alternative
   * save location.
   */
  void saveAs(const KUrl &oldUrl, KUrl &newUrl);

Q_SIGNALS:
  void leaveModality();
#endif

private:
    KUrl m_url;
    KFileItem m_nullItem;
    KFileItemList m_items;
};

class KPropertiesDialogPlugin : public QObject
{
public:
    KPropertiesDialogPlugin(KPropertiesDialog *props, const QVariantList & = QVariantList()) : QObject(props) {}

    #if 0
  /**
   * Constructor
   * To insert tabs into the properties dialog, use the add methods provided by
   * KPageDialog (the properties dialog is a KPageDialog).
   */
  KPropertiesDialogPlugin( KPropertiesDialog *_props );
  virtual ~KPropertiesDialogPlugin();

  /**
   * Applies all changes to the file.
   * This function is called when the user presses 'Ok'. The last plugin inserted
   * is called first.
   */
  virtual void applyChanges();

  /**
   * Convenience method for most ::supports methods
   * @return true if the file is a local, regular, readable, desktop file
   * @deprecated use KFileItem::isDesktopFile
   */
#ifndef KDE_NO_DEPRECATED
  static KDE_DEPRECATED bool isDesktopFile( const KFileItem& _item );
#endif

  void setDirty( bool b );
  bool isDirty() const;

public Q_SLOTS:
  void setDirty(); // same as setDirty( true ). TODO KDE5: void setDirty(bool dirty=true);

Q_SIGNALS:
  /**
   * Emit this signal when the user changed anything in the plugin's tabs.
   * The hosting PropertiesDialog will call applyChanges only if the
   * PropsPlugin has emitted this signal or if you have called setDirty() before.
   */
  void changed();

#endif

};

#endif
 
