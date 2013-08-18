#ifndef FAKE_KFONTSIZEACTION_H
#define FAKE_KFONTSIZEACTION_H

#include <kselectaction.h>

class KFontSizeAction : public KSelectAction
{
public:
    explicit KFontSizeAction(QObject *parent) : KSelectAction(parent) {}
    KFontSizeAction(const QString &text, QObject *parent) : KSelectAction(parent) {}
    KFontSizeAction(const KIcon &icon, const QString &text, QObject *parent) : KSelectAction(parent) {}
    virtual ~KFontSizeAction() {}
    int fontSize() const { return QFont().pixelSize(); }
    void setFontSize( int size ) {}
};

#endif
