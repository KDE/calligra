#ifndef FAKE_KCHARSELECT_H
#define FAKE_KCHARSELECT_H

#include <QString>
#include <QStringList>
#include <QFont>
#include <QUrl>
#include <QWidget>
#include <kglobal.h>
#include <kactioncollection.h>

class KCharSelect : public QWidget
{
public:
    enum Control {
        SearchLine = 0x01,
        FontCombo = 0x02,
        FontSize = 0x04,
        BlockCombos = 0x08,
        CharacterTable = 0x10,
        DetailBrowser = 0x20,
        HistoryButtons = 0x40,
        AllGuiElements      = 65535
    };
    Q_DECLARE_FLAGS(Controls, Control)

    explicit KCharSelect(QWidget *parent, KActionCollection *collection, const Controls controls = AllGuiElements) : QWidget(parent) {}
    //virtual QSize sizeHint() const;
    QChar currentChar() const { return QChar(); }
    QFont currentFont() const { return QFont(); }
    QList<QChar> displayedChars() const { return QList<QChar>(); }
    void setCurrentChar(const QChar &c) {}
    void setCurrentFont(const QFont &font) {}
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KCharSelect::Controls)

#endif
