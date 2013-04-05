#ifndef FAKE_KICONLOADER_H
#define FAKE_KICONLOADER_H

#include <kicon.h>
#include <kcomponentdata.h>

#include <QObject>

class KIconLoader
{
public:
    enum Context { Any, Action, Application, Device, FileSystem, MimeType, Animation, Category, Emblem, Emote, International, Place, StatusIcon };
    enum Type { Fixed, Scalable, Threshold };
    enum MatchType { MatchExact, MatchBest };
    enum Group { NoGroup=-1, Desktop=0, FirstGroup=0, Toolbar, MainToolbar, Small, Panel, Dialog, LastGroup, User };
    enum StdSizes { SizeSmall=16, SizeSmallMedium=22, SizeMedium=32, SizeLarge=48, SizeHuge=64, SizeEnormous=128 };
    enum States { DefaultState, ActiveState, DisabledState, LastState };
    explicit KIconLoader(const QString& appname=QString(), KStandardDirs *dirs = 0, QObject* parent = 0) {}
    explicit KIconLoader(const KComponentData &componentData, QObject* parent = 0) {}

    static KIconLoader* global()
    {
        static KIconLoader* is_instance = 0;
        if (!is_instance)
            is_instance = new KIconLoader();
        return is_instance;
    }

    void addAppDir(const QString& appname) {}

    QPixmap loadIcon(const QString& name, KIconLoader::Group group, int size=0,
                     int state=KIconLoader::DefaultState, const QStringList &overlays = QStringList(),
                     QString *path_store=0L,
                     bool canReturnNull=false) const
    {
        return QPixmap();
    }

    QPixmap loadMimeTypeIcon( const QString& iconName, KIconLoader::Group group, int size=0,
                              int state=KIconLoader::DefaultState, const QStringList &overlays = QStringList(),
                              QString *path_store=0 ) const
    {
        return QPixmap();
    }

#if 0
    /**
     * Creates an icon set, that will do on-demand loading of the icon.
     * Loading itself is done by calling loadIcon .
     *
     * @param name The name of the icon, without extension.
     * @param group The icon group. This will specify the size of and effects to
     * be applied to the icon.
     * @param size If nonzero, this overrides the size specified by @p group.
     *             See KIconLoader::StdSizes.
     * @param canReturnNull Can return a null iconset? If false, iconset
     * containing the "unknown" pixmap is returned when no appropriate icon has
     * been found.
     * @return the icon set. Can be null when not found, depending on
     *          @p canReturnNull.
     *
     * @deprecated use KIcon instead, which uses the iconloader internally
     */
#ifndef KDE_NO_DEPRECATED
    KDE_DEPRECATED QIcon loadIconSet(const QString& name, KIconLoader::Group group, int size = 0,
                                     bool canReturnNull = false);
#endif

#endif

    QString iconPath(const QString& name, int group_or_size, bool canReturnNull=false) const { return QString(); }

#if 0
    /**
     * Loads an animated icon.
     * @param name The name of the icon.
     * @param group The icon group. See loadIcon().
     * @param size Override the default size for @p group.
     *             See KIconLoader::StdSizes.
     * @param parent The parent object of the returned QMovie.
     * @return A QMovie object. Can be null if not found or not valid.
     *         Ownership is passed to the caller.
     */
    QMovie *loadMovie(const QString& name, KIconLoader::Group group, int size=0, QObject *parent=0) const;

    /**
     * Returns the path to an animated icon.
     * @param name The name of the icon.
     * @param group The icon group. See loadIcon().
     * @param size Override the default size for @p group.
     *             See KIconLoader::StdSizes.
     * @return the full path to the movie, ready to be passed to QMovie's constructor.
     * Empty string if not found.
     */
    QString moviePath(const QString& name, KIconLoader::Group group, int size=0) const;

    /**
     * Loads an animated icon as a series of still frames. If you want to load
     * a .mng animation as QMovie instead, please use loadMovie() instead.
     * @param name The name of the icon.
     * @param group The icon group. See loadIcon().
     * @param size Override the default size for @p group.
     *             See KIconLoader::StdSizes.
     * @return A QStringList containing the absolute path of all the frames
     * making up the animation.
     */
    QStringList loadAnimated(const QString& name, KIconLoader::Group group, int size=0) const;

    /**
     * Queries all available icons for a specific group, having a specific
     * context.
     * @param group_or_size If positive, search icons whose size is
     * specified by the icon group @p group_or_size. If negative, search
     * icons whose size is - @p group_or_size.
     *             See KIconLoader::Group and KIconLoader::StdSizes
     * @param context The icon context.
     * @return a list of all icons
     */
    QStringList queryIcons(int group_or_size, KIconLoader::Context context=KIconLoader::Any) const;

    /**
     * Queries all available icons for a specific context.
     * @param group_or_size The icon preferred group or size. If available
     * at this group or size, those icons will be returned, in other case,
     * icons of undefined size will be returned. Positive numbers are groups,
     * negative numbers are negated sizes. See KIconLoader::Group and
     * KIconLoader::StdSizes
     * @param context The icon context.
     * @return A QStringList containing the icon names
     * available for that context
     */
    QStringList queryIconsByContext(int group_or_size,
                                    KIconLoader::Context context=KIconLoader::Any) const;

    /**
     * @internal
     */
    bool hasContext( KIconLoader::Context context ) const;

    /**
     * Returns a list of all icons (*.png or *.xpm extension) in the
     * given directory.
     * @param iconsDir the directory to search in
     * @return A QStringList containing the icon paths
     */
    QStringList queryIconsByDir( const QString& iconsDir ) const;

    /**
     * Returns the current size of the icon group.
     * Using e.g. KIconLoader::SmallIcon will retrieve the icon size
     * that is currently set from System Settings->Appearance->Icon
     * sizes. SmallIcon for instance, would typically be 16x16, but
     * the user could increase it and this setting would change as well.
     * @param group the group to check.
     * @return the current size for an icon group.
     */
    int currentSize(KIconLoader::Group group) const;

    /**
     * Returns a pointer to the current theme. Can be used to query
     * available and default sizes for groups.
     * @note The KIconTheme will change if reconfigure() is called and
     * therefore it's not recommended to store the pointer anywhere.
     * @return a pointer to the current theme. 0 if no theme set.
     */
    KIconTheme *theme() const;

    /**
     * Returns a pointer to the KIconEffect object used by the icon loader.
     * @return the KIconEffect.
     */
    KIconEffect *iconEffect() const;

    /**
     * Called by KComponentData::newIconLoader to reconfigure the icon loader.
     * @param _appname the new application name
     * @param _dirs the new standard directories. If 0, the directories
     *              from KGlobal will be taken.
     */
    void reconfigure( const QString& _appname, KStandardDirs *_dirs );

    /**
     * Returns the unknown icon. An icon that is used when no other icon
     * can be found.
     * @return the unknown pixmap
     */
    static QPixmap unknown();

    /**
     * Checks whether the user wants to blend the icons with the background
     *  using the alpha channel information for a given group.
     * @param group the group to check
     * @return true if alpha blending is desired
     * @obsolete
     */
    bool alphaBlending( KIconLoader::Group group ) const;

    /**
     * Adds all the default themes from other desktops at the end of
     * the list of icon themes.
     */
    void addExtraDesktopThemes();

    /**
     * Returns if the default icon themes of other desktops have been added
     * to the list of icon themes where icons are searched.
     */
    bool extraDesktopThemesAdded() const;

    /**
     * Draws overlays on the specified pixmap, it takes the width and height
     * of the pixmap into consideration
     * @param overlays to draw
     * @param pixmap to draw on
     * @since 4.7
     */
    void drawOverlays(const QStringList &overlays, QPixmap &pixmap, KIconLoader::Group group, int state = KIconLoader::DefaultState) const;

 public Q_SLOTS:
   /**
    * Re-initialize the global icon loader
    */
    void newIconLoader();

Q_SIGNALS:
    /**
     * Emitted by newIconLoader once the new settings have been loaded
     */
    void iconLoaderSettingsChanged();
#endif
};

static QPixmap DesktopIcon(const QString& name, int size=0, int state=KIconLoader::DefaultState, const QStringList& overlays = QStringList())
{
    return QPixmap();
}

static QPixmap BarIcon(const QString& name, int size=0, int state=KIconLoader::DefaultState, const QStringList& overlays = QStringList())
{
    return QPixmap();
}

static QPixmap SmallIcon(const QString& name, int size=0, int state=KIconLoader::DefaultState, const QStringList &overlays = QStringList())
{
    return QPixmap();
}

static QPixmap MainBarIcon(const QString& name, int size=0, int state=KIconLoader::DefaultState, const QStringList &overlays = QStringList())
{
    return QPixmap();
}

static QPixmap UserIcon(const QString& name, int state=KIconLoader::DefaultState, const QStringList &overlays = QStringList())
{
    return QPixmap();
}

static int IconSize(KIconLoader::Group group)
{
    return 32;
}

#endif
