#ifndef FAKE_SONNET_BACKGROUNDCHECKER_H
#define FAKE_SONNET_BACKGROUNDCHECKER_H

#include "speller.h"
#include <QObject>

namespace Sonnet
{

class BackgroundChecker : public QObject
{
    Q_OBJECT
public:
    BackgroundChecker(QObject *parent =0) : QObject(parent) {}
    BackgroundChecker(const Speller &speller, QObject *parent =0) : QObject(parent) {}
    void setText(const QString &text) {}
    QString text() const { return QString(); }
    QString currentContext() const { return QString(); }
    Speller speller() const { return m_speller; }
    void setSpeller(const Speller &speller) { m_speller = speller; }
    bool checkWord(const QString &word) { return true; }
    QStringList suggest(const QString &word) const { return QStringList(); }
    bool addWordToPersonal(const QString &word) { return false; }
    void restore(KConfig *config) {}
public Q_SLOTS:
    virtual void start() {}
    virtual void stop() {}
    void replace(int start, const QString &oldText, const QString &newText) {}
    void changeLanguage(const QString &lang) {}
    virtual void continueChecking() {}
Q_SIGNALS:
    void done();
private:
    Speller m_speller;
};

}

#endif

