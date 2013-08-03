#ifndef FAKE_KCOLORSCHEME_H
#define FAKE_KCOLORSCHEME_H

#include <QColor>
#include <QPalette>
#include <QDebug>

#include "kofake_export.h"


class KColorScheme
{
public:
    
    enum ColorSet {
        View,
        Window,
        Button,
        Selection,
        Tooltip
    };

    enum BackgroundRole {
        NormalBackground = 0,
        AlternateBackground = 1,
        ActiveBackground = 2,
        LinkBackground = 3,
        VisitedBackground = 4,
        NegativeBackground = 5,
        NeutralBackground = 6,
        PositiveBackground = 7
    };

    enum ForegroundRole {
        NormalText = 0,
        InactiveText = 1,
        ActiveText = 2,
        LinkText = 3,
        VisitedText = 4,
        NegativeText = 5,
        NeutralText = 6,
        PositiveText = 7
    };

    enum DecorationRole {
        FocusColor,
        HoverColor
    };

    enum ShadeRole {
        LightShade,
        MidlightShade,
        MidShade,
        DarkShade,
        ShadowShade
    };

    KColorScheme() {}
    KColorScheme(QPalette::ColorGroup, ColorSet = View/*, KSharedConfigPtr = KSharedConfigPtr()*/) {}

    QBrush background(BackgroundRole = NormalBackground) const { return QPalette().brush(QPalette::Background); }
    QBrush foreground(ForegroundRole = NormalText) const { return QPalette().brush(QPalette::Foreground); }
    QBrush decoration(DecorationRole) const { return QPalette().brush(QPalette::Highlight); }
    QColor shade(ShadeRole) const { return background().color(); }

    static QColor shade(const QColor&, ShadeRole) { return KColorScheme().background().color(); }
    static QColor shade(const QColor&, ShadeRole, qreal contrast, qreal chromaAdjust = 0.0) { return KColorScheme().background().color(); }
    static void adjustBackground(QPalette &, BackgroundRole newRole = NormalBackground /*, QPalette::ColorRole color = QPalette::Base, ColorSet set = View, KSharedConfigPtr = KSharedConfigPtr()*/) {}
    //static void adjustForeground(QPalette &, ForegroundRole newRole = NormalText, QPalette::ColorRole color = QPalette::Text, ColorSet set = View, KSharedConfigPtr = KSharedConfigPtr()) {}

};

#endif
 
