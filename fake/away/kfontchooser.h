#ifndef FAKE_KFONTCHOOSER_H
#define FAKE_KFONTCHOOSER_H

#include <QWidget>
#include <klocale.h>

class KFontChooser : public QWidget
{
public:
    enum FontColumn { FamilyList=0x01, StyleList=0x02, SizeList=0x04};
    enum FontDiff { NoFontDiffFlags = 0, FontDiffFamily = 1, FontDiffStyle = 2, FontDiffSize = 4, AllFontDiffs = FontDiffFamily | FontDiffStyle | FontDiffSize };
    Q_DECLARE_FLAGS( FontDiffFlags, FontDiff )
    enum DisplayFlag { NoDisplayFlags = 0, FixedFontsOnly = 1, DisplayFrame = 2, ShowDifferences = 4 };
    Q_DECLARE_FLAGS( DisplayFlags, DisplayFlag )
    enum FontListCriteria { FixedWidthFonts=0x01, ScalableFonts=0x02, SmoothScalableFonts=0x04 };

    KFontChooser( QWidget *parent = 0L, const DisplayFlags& flags = DisplayFrame, const QStringList &fontList = QStringList(), int visibleListSize = 8, Qt::CheckState *sizeIsRelativeState = 0L ) : QWidget(parent) {}

    void enableColumn( int column, bool state ) {}
    void setFont( const QFont &font, bool onlyFixed = false ) {}
    FontDiffFlags fontDiffFlags() const { return FontDiffFlags(); }
    QFont font() const { return QFont(); }
    void setColor( const QColor & col ) {}
    QColor color() const { return QColor(); }
    void setBackgroundColor( const QColor & col ) {}
    QColor backgroundColor() const { return QColor(); }
    void setSizeIsRelative( Qt::CheckState relative ) {}
    Qt::CheckState sizeIsRelative() const { return Qt::Unchecked; }
    QString sampleText() const { return QString(); }
    void setSampleText( const QString &text ) {}
    void setSampleBoxVisible( bool visible ) {}

    static void getFontList( QStringList &list, uint fontListCriteria) {}
};

#endif
