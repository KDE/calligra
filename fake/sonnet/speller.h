#ifndef FAKE_SONNET_SPELLER_H
#define FAKE_SONNET_SPELLER_H

#include <QStringList>
#include <QString>
#include <kconfig.h>

namespace Sonnet
{

class Speller
{
public:
    Speller(const QString &lang=QString()) {}
    bool isValid() const { return false; }
    void setLanguage(const QString &lang) {}
    QString language() const { return QString(); }
    bool isCorrect(const QString &word) const { return true; }
    bool isMisspelled(const QString &word) const { return false; }
    QStringList suggest(const QString &word) const { return QStringList(); }
    bool checkAndSuggest(const QString &word, QStringList &suggestions) const { return false; }
    bool storeReplacement(const QString &bad, const QString &good) { return false; }
    bool addToPersonal(const QString &word) { return false; }
    bool addToSession(const QString &word) { return false; }

    enum Attribute { CheckUppercase, SkipRunTogether };
    void save(KConfig *config) {}
    void restore(KConfig *config) {}

    QStringList availableBackends() const { return QStringList(); }
    QStringList availableLanguages() const { return QStringList(); }
    QStringList availableLanguageNames() const { return QStringList(); }
    QMap<QString, QString> availableDictionaries() const { return QMap<QString, QString>(); }

    void setDefaultLanguage(const QString &lang) {}
    QString defaultLanguage() const  { return QString(); }
    void setDefaultClient(const QString &client) {}
    QString defaultClient() const { return QString(); }
    void setAttribute(Attribute attr, bool b = true) {}
    bool testAttribute(Attribute attr) const { return true; }
};

}

#endif

