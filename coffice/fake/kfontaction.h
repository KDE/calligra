#ifndef FAKE_KFONTACTION_H
#define FAKE_KFONTACTION_H

#include <kselectaction.h>

class KFontAction : public KSelectAction
{
public:
    KFontAction(uint fontListCriteria, QObject *parent) : KSelectAction(parent) {}
    explicit KFontAction(QObject *parent) : KSelectAction(parent) {}
    KFontAction(const QString& text, QObject *parent) : KSelectAction(parent) {}
    KFontAction(const KIcon &icon, const QString &text, QObject *parent) : KSelectAction(parent) {}
    virtual ~KFontAction() {}
    QString font() const { return QString(); }
    void setFont( const QString &family ) {}
    virtual QWidget* createWidget(QWidget* parent) { return 0; }
};

#endif
