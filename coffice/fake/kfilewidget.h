#ifndef FAKE_KFILEWIDGET_H
#define FAKE_KFILEWIDGET_H

#include <QWidget>
#include <kfile.h>
#include <kurl.h>
#include <kmimetype.h>
#include <kdiroperator.h>
#include <kactioncollection.h>
#include <ktoolbar.h>
#include <kpushbutton.h>
#include <kurlcombobox.h>

class KFileFilterCombo;

class KFileWidget : public QWidget
{
    Q_OBJECT
public:
    enum OperationMode { Other = 0, Opening, Saving };

    KFileWidget(const KUrl& startDir, QWidget *parent)
        : QWidget(parent), m_opMode(Opening), m_modes(KFile::File | KFile::Directory)
    {
    }

#if 0
    void setConfirmOverwrite(bool enable){  // KDE5 TODO: make this virtual
        virtual_hook(0, static_cast<void*>(&enable));
    }
    void setInlinePreviewShown(bool show) { // KDE5 TODO: make this virtual
        virtual_hook(1, static_cast<void*>(&show));
    }
#endif

    virtual KUrl selectedUrl() const { return m_selectedUrls.isEmpty() ? KUrl() : m_selectedUrls.first(); }
    virtual KUrl::List selectedUrls() const { return m_selectedUrls; }
    virtual KUrl baseUrl() const { return KUrl(); }
    virtual QString selectedFile() const { return m_selectedUrls.isEmpty() ? QString() : m_selectedUrls.first().toLocalFile(); }
    virtual QStringList selectedFiles() const
    {
        QStringList files;
        Q_FOREACH(const KUrl &u, m_selectedUrls)
            files.append(u.toLocalFile());
        return files;
    }

    virtual void setUrl(const KUrl &url, bool clearforward = true) {}
    virtual void setSelection(const QString& name) {}

    virtual void setOperationMode( OperationMode m ) { m_opMode = m; }
    virtual OperationMode operationMode() const { return m_opMode; }

    virtual void setKeepLocation( bool keep ) {}
    virtual bool keepsLocation() const { return false; }

    virtual void setFilter(const QString&) {}
    virtual QString currentFilter() const { return QString(); }
    virtual KMimeType::Ptr currentFilterMimeType() { return KMimeType::Ptr(); }
    virtual void setMimeFilter( const QStringList& types, const QString& defaultType = QString() ) {}
    virtual QString currentMimeFilter() const { return QString(); }
    virtual void clearFilter() {}

#if 0
    virtual void setPreviewWidget(KPreviewWidgetBase *w);
#endif

    virtual void setMode( KFile::Modes m ) { m_modes = m; }
    virtual KFile::Modes mode() const { return  m_modes; }

    virtual void setLocationLabel(const QString& text) {}
    KToolBar *toolBar() const { return 0; }
    KPushButton *okButton() const { return 0; }
    KPushButton *cancelButton() const { return 0; }
    KUrlComboBox *locationEdit() const { return 0; }
    KFileFilterCombo *filterWidget() const { return 0; }
    KActionCollection *actionCollection() const { return 0; }

#if 0
    /**
     * This method implements the logic to determine the user's default directory
     * to be listed. E.g. the documents directory, home directory or a recently
     * used directory.
     * @param startDir A URL specifying the initial directory, or using the
     *                 @c kfiledialog:/// syntax to specify a last used
     *                 directory.  If this URL specifies a file name, it is
     *                 ignored.  Refer to the KFileWidget::KFileWidget()
     *                 documentation for the @c kfiledialog:/// URL syntax.
     * @param recentDirClass If the @c kfiledialog:/// syntax is used, this
     *        will return the string to be passed to KRecentDirs::dir() and
     *        KRecentDirs::add().
     * @return The URL that should be listed by default (e.g. by KFileDialog or
     *         KDirSelectDialog).
     * @see KFileWidget::KFileWidget()
     */
    static KUrl getStartUrl( const KUrl& startDir, QString& recentDirClass );

    /**
     * Similar to getStartUrl(const KUrl& startDir,QString& recentDirClass),
     * but allows both the recent start directory keyword and a suggested file name
     * to be returned.
     * @param startDir A URL specifying the initial directory and/or filename,
     *                 or using the @c kfiledialog:/// syntax to specify a
     *                 last used location.
     *                 Refer to the KFileWidget::KFileWidget()
     *                 documentation for the @c kfiledialog:/// URL syntax.
     * @param recentDirClass If the @c kfiledialog:/// syntax is used, this
     *        will return the string to be passed to KRecentDirs::dir() and
     *        KRecentDirs::add().
     * @param fileName The suggested file name, if specified as part of the
     *        @p StartDir URL.
     * @return The URL that should be listed by default (e.g. by KFileDialog or
     *         KDirSelectDialog).
     *
     * @see KFileWidget::KFileWidget()
     * @since 4.3
     */
    static KUrl getStartUrl( const KUrl& startDir, QString& recentDirClass, QString& fileName );

    /**
     * @internal
     * Used by KDirSelectDialog to share the dialog's start directory.
     */
    static void setStartDir( const KUrl& directory );

    virtual void setCustomWidget(QWidget* widget);
    virtual void setCustomWidget(const QString& text, QWidget* widget);
#endif

    /**
     * @returns the KDirOperator used to navigate the filesystem
     * @since 4.3
     */
    KDirOperator* dirOperator() { return 0; }

#if 0
    /**
     * reads the configuration for this widget from the given config group
     * @param group the KConfigGroup to read from
     * @since 4.4
     */
    void readConfig( KConfigGroup& group );
#endif

public Q_SLOTS:
    virtual void slotOk() {}
    virtual void accept() {}
    virtual void slotCancel() {}

#if 0
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

    /**
     * Emitted by slotOk() (directly or asynchronously) once everything has
     * been done. Should be used by the caller to call accept().
     */
    void accepted();
#endif

private:
    OperationMode m_opMode;
    KFile::Modes m_modes;
    KUrl::List m_selectedUrls;
};

#endif
 
