#ifndef FAKE_KFILEDIALOG_H
#define FAKE_KFILEDIALOG_H

#include <QFileDialog>
#include <QComboBox>
#include <kfile.h>
#include <kurl.h>
#include <kmimetype.h>

class KPreviewWidgetBase;

class KFileDialog : public QFileDialog
{
public:
    KFileDialog( const KUrl& startDir, const QString& filter, QWidget *parent, QWidget* widget = 0 )
        : QFileDialog(parent)
        , m_modes(KFile::File)
    {
        setDirectory(startDir.toLocalFile());
        //setNameFilter(filter);
    }

    void setCaption(const QString &s)
    {
        setWindowTitle(s);
    }

    enum OperationMode { Other = 0, Opening, Saving };

    enum Option { ConfirmOverwrite  = 0x01, ShowInlinePreview = 0x02 };
    Q_DECLARE_FLAGS(Options, Option)

    KUrl selectedUrl() const
    {
        return selectedFiles().isEmpty() ? KUrl() : KUrl(selectedFiles().first());
    }

    KUrl::List selectedUrls() const
    {
        KUrl::List ret;
        Q_FOREACH(const QString &s, selectedFiles())
            ret.append(KUrl(s));
        return ret;
    }

#if 0
    /**
     * @returns the currently shown directory.
     */
    KUrl baseUrl() const;

    /**
     * Returns the full path of the selected file in the local filesystem.
     * (Local files only)
     */
    QString selectedFile() const;

    /**
     * Returns a list of all selected local files.
     */
    QStringList selectedFiles() const;

    /**
     * Sets the directory to view.
     *
     * @param url URL to show.
     * @param clearforward Indicates whether the forward queue
     * should be cleared.
     */
    void setUrl(const KUrl &url, bool clearforward = true);

    /**
     * Sets the file name to preselect to @p name
     *
     * This takes absolute URLs and relative file names.
     */
    void setSelection(const QString& name);

#endif

    void setOperationMode( KFileDialog::OperationMode m )
    {
        if ( m == Saving)
            setAcceptMode(QFileDialog::AcceptSave);
        else
            setAcceptMode(QFileDialog::AcceptOpen);
    }

#if 0
    /**
     * @returns the current operation mode, Opening, Saving or Other. Default
     * is Other.
     *
     * @see operationMode
     * @see KFileDialog::OperationMode
     */
    OperationMode operationMode() const;

    /**
     * Sets whether the filename/url should be kept when changing directories.
     * This is for example useful when having a predefined filename where
     * the full path for that file is searched.
     *
     * This is implicitly set when operationMode() is KFileDialog::Saving
     *
     * getSaveFileName() and getSaveUrl() set this to true by default, so that
     * you can type in the filename and change the directory without having
     * to type the name again.
     */
    void setKeepLocation( bool keep );

    /**
     * @returns whether the contents of the location edit are kept when
     * changing directories.
     */
    bool keepsLocation() const;

    /**
     * Sets the filter to be used to @p filter.
     *
     * The filter can be either set as a space-separated list of
     * mimetypes, which is recommended, or as a list of shell globs
     * separated by @c '\\n'.
     *
     * If the filter contains an unescaped @c '/', a mimetype filter is assumed.
     * If you would like a @c '/' visible in your filter it can be escaped with
     * a @c '\'. You can specify multiple mimetypes like this (separated with
     * space):
     *
     * \code
     * kfile->setFilter( "image/png text/html text/plain" );
     * \endcode
     *
     * When showing the filter to the user, the mimetypes will be automatically
     * translated into their description like `PNG image'. Multiple mimetypes
     * will be automatically summarized to a filter item `All supported files'.
     * To add a filter item for all files matching @c '*', add @c all/allfiles
     * as mimetype.
     *
     * If the filter contains no unescaped @c '/', it is assumed that
     * the filter contains conventional shell globs. Several filter items
     * to select from can be separated by @c '\\n'. Every
     * filter entry is defined through @c namefilter|text to display.
     * If no @c '|' is found in the expression, just the namefilter is
     * shown. Examples:
     *
     * \code
     * kfile->setFilter("*.cpp|C++ Source Files\n*.h|Header files");
     * kfile->setFilter("*.cpp");
     * kfile->setFilter("*.cpp|Sources (*.cpp)");
     * kfile->setFilter("*.cpp|" + i18n("Sources (*.cpp)"));
     * kfile->setFilter("*.cpp *.cc *.C|C++ Source Files\n*.h *.H|Header files");
     * \endcode
     *
     * Note: The text to display is not parsed in any way. So, if you
     * want to show the suffix to select by a specific filter, you must
     * repeat it.
     *
     * For better consistency across applications, it is recommended to use a
     * mimetype filter.
     *
     * @see filterChanged
     * @see setMimeFilter
     */
    void setFilter(const QString& filter);

    /**
     * Returns the current filter as entered by the user or one of the
     * predefined set via setFilter().
     *
     * @see setFilter()
     * @see filterChanged()
     */
    QString currentFilter() const;

    /**
     * Returns the mimetype for the desired output format.
     *
     * This is only valid if setMimeFilter() has been called
     * previously.
     *
     * @see setFilterMimeType()
     */
    KMimeType::Ptr currentFilterMimeType();

    /**
     * Sets the filter up to specify the output type.
     *
     * @param types a list of mimetypes that can be used as output format
     * @param defaultType the default mimetype to use as output format, if any.
     * If @p defaultType is set, it will be set as the current item.
     * Otherwise, a first item showing all the mimetypes will be created.
     * Typically, @p defaultType should be empty for loading and set for saving.
     *
     * Do not use in conjunction with setFilter()
     */
#endif

    void setMimeFilter( const QStringList& types, const QString& defaultType = QString() )
    {
    }

    /**
     * The mimetype for the desired output format.
     *
     * This is only valid if setMimeFilter() has been called
     * previously.
     *
     * @see setMimeFilter()
     */
    QString currentMimeFilter() const
    {
        return QString();
    }

    void clearFilter() {}
    void setPreviewWidget(KPreviewWidgetBase *w) {}
    void setInlinePreviewShown(bool show) {}
    void setConfirmOverwrite(bool enable) {}

    static QString getOpenFileName( const KUrl& startDir= KUrl(), const QString& filter= QString(), QWidget *parent= 0, const QString& caption = QString() )
    {
        return QFileDialog::getOpenFileName(parent, caption, startDir.url(), filter);
    }

#if 0
   static QString getOpenFileNameWId( const KUrl& startDir,
                                      const QString& filter,
                                      WId parent_id, const QString& caption );

#endif

    static QStringList getOpenFileNames( const KUrl& startDir= KUrl(), const QString& filter = QString(), QWidget *parent = 0, const QString& caption= QString() )
    {
        return QFileDialog::getOpenFileNames(parent, caption, startDir.url(), filter);
    }

    static KUrl getOpenUrl( const KUrl& startDir = KUrl(), const QString& filter = QString(), QWidget *parent= 0, const QString& caption = QString() )
    {
        return KUrl(getOpenFileName(startDir, filter, parent, caption));
    }

    static KUrl::List getOpenUrls( const KUrl& startDir = KUrl(), const QString& filter = QString(), QWidget *parent = 0, const QString& caption = QString() )
    {
        KUrl::List list;
        Q_FOREACH(const QString &s, getOpenFileNames(startDir, filter, parent, caption))
            list.append(KUrl(s));
        return list;
    }

    static QString getSaveFileName( const KUrl& startDir = KUrl(), const QString& filter = QString(), QWidget *parent = 0, const QString& caption = QString() )
    {
        return QFileDialog::getSaveFileName(parent, caption, startDir.url(), filter);
    }

    static QString getSaveFileName( const KUrl& startDir, const QString& filter, QWidget *parent, const QString& caption, Options options )
    {
        return QFileDialog::getSaveFileName(parent, caption, startDir.url(), filter);
    }

#if 0
    /**
     * This function accepts the window id of the parent window, instead
     * of QWidget*. It should be used only when necessary.
     */
    static QString getSaveFileNameWId( const KUrl &startDir, const QString& filter,
                                       WId parent_id,
                                       const QString& caption );

    /**
     * This function accepts the window id of the parent window, instead
     * of QWidget*. It should be used only when necessary.
     *
     * @since 4.4
     */
    static QString getSaveFileNameWId( const KUrl &startDir, const QString& filter,
                                       WId parent_id,
                                       const QString& caption,
                                       Options options );

#endif

    static KUrl getSaveUrl( const KUrl& startDir = KUrl(), const QString& filter = QString(), QWidget *parent = 0, const QString& caption = QString() )
    {
        return KUrl(QFileDialog::getSaveFileName(parent, caption, startDir.url(), filter));
    }

#if 0
    /**
     * Creates a modal file dialog and returns the selected
     * filename or an empty string if none was chosen.
     *
     * Note that with this
     * method the user need not select an existing filename.
     *
     * @param startDir Starting directory or @c kfiledialog:/// URL.
     *                 Refer to the KFileWidget documentation for more information
     *                 on this parameter.
     * @param filter A shell glob or a mimetype filter that specifies which files to display.
     *    The preferred option is to set a list of mimetype names, see setMimeFilter() for details.
     *    Otherwise you can set the text to be displayed for the each glob, and
     *    provide multiple globs, see setFilter() for details.
     * @param parent The widget the dialog will be centered on initially.
     * @param caption The name of the dialog widget.
     * @param options Dialog options.
     *
     * @see KFileWidget::KFileWidget()
     *
     * @since 4.4
     */
    static KUrl getSaveUrl( const KUrl& startDir,
                            const QString& filter,
                            QWidget *parent,
                            const QString& caption,
                            Options options );


    /**
     * Creates a modal directory-selection dialog and returns the selected
     * directory (local only) or an empty string if none was chosen.
     *
     * @param startDir Starting directory or @c kfiledialog:/// URL.
     *                 Refer to the KFileWidget documentation for more information
     *                 on this parameter.
     * @param parent The widget the dialog will be centered on initially.
     * @param caption The name of the dialog widget.
     * @return the path to an existing local directory.
     *
     * @see KFileWidget::KFileWidget()
     */
    static QString getExistingDirectory( const KUrl& startDir = KUrl(),
                                         QWidget * parent = 0,
                                         const QString& caption= QString() );

    /**
     * Creates a modal directory-selection dialog and returns the selected
     * directory or an empty string if none was chosen.
     * This version supports remote urls.
     *
     * @param startDir Starting directory or @c kfiledialog:/// URL.
     *                 Refer to the KFileWidget documentation for more information
     *                 on this parameter.
     * @param parent The widget the dialog will be centered on initially.
     * @param caption The name of the dialog widget.
     * @return the url to an existing directory (local or remote).
     *
     * @see KFileWidget::KFileWidget()
     */
    static KUrl getExistingDirectoryUrl( const KUrl& startDir = KUrl(),
                                         QWidget * parent = 0,
                                         const QString& caption= QString() );

    /**
     * Creates a modal file dialog with an image previewer and returns the
     * selected url or an empty string if none was chosen.
     *
     * @param startDir Starting directory or @c kfiledialog:/// URL.
     *                 Refer to the KFileWidget documentation for more information
     *                 on this parameter.
     * @param parent The widget the dialog will be centered on initially.
     * @param caption The name of the dialog widget.
     *
     * @see KFileWidget::KFileWidget()
     */
    static KUrl getImageOpenUrl( const KUrl& startDir = KUrl(),
                                 QWidget *parent = 0,
                                 const QString& caption = QString() );

    /**
     * Sets the mode of the dialog.
     *
     * The mode is defined as (in kfile.h):
     * \code
     *    enum Mode {
     *         File         = 1,
     *         Directory    = 2,
     *         Files        = 4,
     *         ExistingOnly = 8,
     *         LocalOnly    = 16
     *    };
     * \endcode
     * You can OR the values, e.g.
     * \code
     * KFile::Modes mode = KFile::Files |
     *                     KFile::ExistingOnly |
     *                     KFile::LocalOnly );
     * setMode( mode );
     * \endcode
     */
#endif

    void setMode( KFile::Modes m ) { m_modes = m; }
    KFile::Modes mode() const { return m_modes; }

    void setLocationLabel(const QString& text) {}

#if 0
    /**
     * Returns the KFileWidget that implements most of this file dialog.
     * If you link to libkfile you can cast this to a KFileWidget*.
     */
    KAbstractFileWidget* fileWidget();

    /**
     * Returns a pointer to the toolbar.
     *
     * You can use this to insert custom
     * items into it, e.g.:
     * \code
     *      yourAction = new KAction( i18n("Your Action"), 0,
     *                                this, SLOT( yourSlot() ),
     *                                this, "action name" );
     *      yourAction->plug( kfileDialog->toolBar() );
     * \endcode
     */
    KToolBar *toolBar() const;

    /**
     * @returns a pointer to the OK-Button in the filedialog. You may use it
     * e.g. to set a custom text to it.
     */
    KPushButton *okButton() const;

    /**
     * @returns a pointer to the Cancel-Button in the filedialog. You may use
     * it e.g. to set a custom text to it.
     */
    KPushButton *cancelButton() const;

    /**
     * @returns the combobox used to type the filename or full location of the file.
     * You need to link to libkfile to use this widget.
     */
    KUrlComboBox *locationEdit() const;
#endif

#if 0
    KFileFilterCombo *filterWidget() const;
#else
    QComboBox *filterWidget() const { return 0; }
#endif

#if 0
    /**
     * @returns a pointer to the action collection, holding all the used KActions.
     */
    KActionCollection *actionCollection() const;

    /**
     * This method implements the logic to determine the user's default directory
     * to be listed. E.g. the documents directory, home directory or a recently
     * used directory.
     *
     * @param startDir Starting directory or @c kfiledialog:/// URL.
     *                 Refer to the KFileWidget documentation for more information
     *                 on this parameter.
     * @param recentDirClass If the @c kfiledialog:/// syntax is used, this
     *        will return the string to be passed to KRecentDirs::dir() and
     *        KRecentDirs::add().
     * @return The URL that should be listed by default (e.g. by KFileDialog or
     *         KDirSelectDialog).
     *
     * @see KFileWidget::KFileWidget()
     * @see KFileWidget::getStartUrl( const KUrl& startDir, QString& recentDirClass );
     */
    static KUrl getStartUrl( const KUrl& startDir, QString& recentDirClass );

    /**
     * @internal
     * Used by KDirSelectDialog to share the dialog's start directory.
     */
    static void setStartDir( const KUrl& directory );

#ifdef Q_WS_WIN
public Q_SLOTS:
    int exec();
#endif

Q_SIGNALS:
    /**
      * Emitted when the user selects a file. It is only emitted in single-
      * selection mode. The best way to get notified about selected file(s)
      * is to connect to the okClicked() signal inherited from KDialog
      * and call selectedFile(), selectedFiles(),
      * selectedUrl() or selectedUrls().
      *
      * \since 4.4
      */
    void fileSelected(const KUrl&);
    /**
     * @deprecated, connect to fileSelected(const KUrl&) instead
     */
    QT_MOC_COMPAT void fileSelected(const QString&); // TODO KDE5: remove

    /**
      * Emitted when the user highlights a file.
      *
      * \since 4.4
      */
    void fileHighlighted(const KUrl&);
    /**
     * @deprecated, connect to fileSelected(const KUrl&) instead
     */
    QT_MOC_COMPAT void fileHighlighted(const QString&); // TODO KDE5: remove

    /**
     * Emitted when the user hilights one or more files in multiselection mode.
     *
     * Note: fileHighlighted() or fileSelected() are @em not
     * emitted in multiselection mode. You may use selectedItems() to
     * ask for the current highlighted items.
     * @see fileSelected
     */
    void selectionChanged();

    /**
     * Emitted when the filter changed, i.e. the user entered an own filter
     * or chose one of the predefined set via setFilter().
     *
     * @param filter contains the new filter (only the extension part,
     * not the explanation), i.e. "*.cpp" or "*.cpp *.cc".
     *
     * @see setFilter()
     * @see currentFilter()
     */
    void filterChanged( const QString& filter );
#endif

private:
    KFile::Modes m_modes;
};

#endif
 
