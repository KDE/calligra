#ifndef FAKE_KFONTCHOOSER_H
#define FAKE_KFONTCHOOSER_H

#include <QWidget>
#include <klocale.h>

class KFontChooser : public QWidget
{
public:
    enum DisplayFlag { NoDisplayFlags = 0, FixedFontsOnly = 1, DisplayFrame = 2, ShowDifferences = 4 };
    Q_DECLARE_FLAGS( DisplayFlags, DisplayFlag )
    enum FontListCriteria { FixedWidthFonts=0x01, ScalableFonts=0x02, SmoothScalableFonts=0x04 };
    KFontChooser( QWidget *parent = 0L, const DisplayFlags& flags = DisplayFrame, const QStringList &fontList = QStringList(), int visibleListSize = 8, Qt::CheckState *sizeIsRelativeState = 0L ) : QWidget(parent) {}
    static void getFontList( QStringList &list, uint fontListCriteria) {}
};

#endif
