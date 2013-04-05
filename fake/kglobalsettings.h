#ifndef FAKE_KGLOBALSETTINGS_H
#define FAKE_KGLOBALSETTINGS_H

#include <QFont>
#include <QObject>
#include <QApplication>
#include <QDebug>

class KGlobalSettings : public QObject
{
public:

    static KGlobalSettings* self()
    {
        static KGlobalSettings* s_instance = 0;
        if (!s_instance)
            s_instance = new KGlobalSettings();
        return s_instance;
    }

    static QFont generalFont() { return QFont(); }
    static QFont smallestReadableFont() { return QFont(); }
    enum GraphicEffect {
        NoEffects = 0x0000,
        GradientEffects         = 0x0001,
        SimpleAnimationEffects  = 0x0002,
        ComplexAnimationEffects = 0x0006
    };
    Q_DECLARE_FLAGS(GraphicEffects, GraphicEffect)
    static GraphicEffects graphicEffectsLevel() { return GraphicEffects(NoEffects); }

    static int dndEventDelay() { return QApplication::startDragDistance(); }
    static bool singleClick() {
#ifdef Q_OS_WIN32
        return false;
#else
        return true;
#endif
    }
    static bool smoothScroll() { return true; }

#if 0
        enum TearOffHandle {
            Disable = 0, ///< disable tear-off handles
            ApplicationLevel, ///< enable on application level
            Enable ///< enable tear-off handles
        };
        static TearOffHandle insertTearOffHandle();
        static bool changeCursorOverIcon();
        static int autoSelectDelay();
        static KDE_DEPRECATED int contextMenuKey ();
\       static bool showContextMenusOnPress ();
        enum Completion {
            CompletionNone=1,
            CompletionAuto,
            CompletionMan,
            CompletionShell,
            CompletionPopup,
            CompletionPopupAuto
        };
        static Completion completionMode();
        struct KMouseSettings
        {
            enum { RightHanded = 0, LeftHanded = 1 };
            int handed; // left or right
        };
        static KMouseSettings & mouseSettings();
        static QString desktopPath();
        static QString autostartPath();
        static QString documentPath();
        static QString musicPath();
        static QString videosPath();
        static QString downloadPath();
        static QString picturesPath();
        static QColor inactiveTitleColor();
        static QColor inactiveTextColor();
        static QColor activeTitleColor();
        static QColor activeTextColor();
        static int contrast();
        static qreal contrastF(const KSharedConfigPtr &config = KSharedConfigPtr());
        static bool shadeSortColumn();
        static bool allowDefaultBackgroundImages();
        static QFont generalFont();
        static QFont fixedFont();
        static QFont toolBarFont();
        static QFont menuFont();
        static QFont windowTitleFont();
        static QFont taskbarFont();
        static QFont largeFont(const QString &text = QString());
        static QFont smallestReadableFont();
        static bool isMultiHead();
        static bool wheelMouseZooms();
        static QRect splashScreenDesktopGeometry();
        static QRect desktopGeometry(const QPoint& point);
        static QRect desktopGeometry(const QWidget* w);
        static bool showIconsOnPushButtons();
        static bool naturalSorting();
        enum GraphicEffect {
            NoEffects               = 0x0000, ///< GUI with no effects at all.
            GradientEffects         = 0x0001, ///< GUI with only gradients enabled.
            SimpleAnimationEffects  = 0x0002, ///< GUI with simple animations enabled.
            ComplexAnimationEffects = 0x0006  ///< GUI with complex animations enabled.
                                              ///< Note that ComplexAnimationsEffects implies SimpleAnimationEffects.
        };
        Q_DECLARE_FLAGS(GraphicEffects, GraphicEffect)
        static GraphicEffects graphicEffectsLevel();
        static GraphicEffects graphicEffectsLevelDefault();
        static bool showFilePreview(const KUrl &);
        static bool opaqueResize();
        static int buttonLayout();
        static QPalette createApplicationPalette(const KSharedConfigPtr &config = KSharedConfigPtr());
        static QPalette createNewApplicationPalette(const KSharedConfigPtr &config = KSharedConfigPtr());
        enum ChangeType { PaletteChanged = 0, FontChanged, StyleChanged,
                          SettingsChanged, IconChanged, CursorChanged,
                          ToolbarStyleChanged, ClipboardConfigChanged,
                          BlockShortcuts, NaturalSortingChanged };
        static void emitChange(ChangeType changeType, int arg = 0);
        static KGlobalSettings* self();
        enum ActivateOption {
            ApplySettings = 0x1, ///< Make all globally applicable settings take effect.
            ListenForChanges = 0x2 ///< Listen for changes to the settings.
        };
        Q_DECLARE_FLAGS(ActivateOptions, ActivateOption)
        void activate(); //KDE5: Merge with the overloaded method below
        void activate(ActivateOptions options);

        enum SettingsCategory { SETTINGS_MOUSE, SETTINGS_COMPLETION, SETTINGS_PATHS,
                                SETTINGS_POPUPMENU, SETTINGS_QT, SETTINGS_SHORTCUTS,
                                SETTINGS_LOCALE, SETTINGS_STYLE };

    Q_SIGNALS:
        void kdisplayPaletteChanged();
        void kdisplayStyleChanged();
        void kdisplayFontChanged();
        void appearanceChanged();
        void toolbarAppearanceChanged(int);
        void settingsChanged(int category);
        void iconChanged(int group);
        void cursorChanged();
        void blockShortcuts(int data);
        void naturalSortingChanged();
#endif


};

#endif
 
 
