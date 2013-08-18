/*  This file is part of the KDE libraries
    Copyright (C) 2006, 2013 Chusslove Illich <caslav.ilic@gmx.net>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include <QDebug>
#include <QMutexLocker>
#include <QStringList>
#include <QByteArray>
#include <QChar>
#include <QHash>
#include <QList>
#include <QVector>
#include <QFile>
#include <QFileInfo>
#include <QLibrary>
#include <QDir>
#include <QCoreApplication>
#include <qstandardpaths.h>

#include <common_helpers_p.h>
#include <kcatalog_p.h>
#include <ktranscript_p.h>
#include <kuitmarkup.h>
#include <kuitmarkup_p.h>
#include <klocalizedstring.h>

// Truncate string, for output of long messages.
static QString shortenMessage(const QString &str)
{
    const int maxlen = 20;
    if (str.length() <= maxlen) {
        return str;
    } else {
        return str.left(maxlen).append(QLatin1String("..."));
    }
}

static void splitLocale(const QString &aLocale,
                        QString &language, QString &country,
                        QString &modifier, QString &charset)
{
    QString locale = aLocale;

    language.clear();
    country.clear();
    modifier.clear();
    charset.clear();

    // In case there are several concatenated locale specifications,
    // truncate all but first.
    int f = locale.indexOf(QLatin1Char(':'));
    if (f >= 0) {
        locale.truncate(f);
    }

    f = locale.indexOf(QLatin1Char('.'));
    if (f >= 0) {
        charset = locale.mid(f + 1);
        locale.truncate(f);
    }

    f = locale.indexOf(QLatin1Char('@'));
    if (f >= 0) {
        modifier = locale.mid(f + 1);
        locale.truncate(f);
    }

    f = locale.indexOf(QLatin1Char('_'));
    if (f >= 0) {
        country = locale.mid(f + 1);
        locale.truncate(f);
    }

    language = locale;
}

static void appendLanguagesFromVariable(QStringList &languages,
                                        const char *envar, bool isList = false)
{
    QByteArray qenvar(qgetenv(envar));
    if (!qenvar.isEmpty()) {
        QString value = QFile::decodeName(qenvar);
        if (isList) {
            languages += value.split(QLatin1Char(':'));
        } else {
            // Process the value to create possible combinations.
            QString language, country, modifier, charset;
            splitLocale(value, language, country, modifier, charset);

            if (!country.isEmpty() && !modifier.isEmpty()) {
                languages +=   language + QLatin1Char('_')
                             + country + QLatin1Char('@')
                             + modifier;
            }
            // NOTE: Priority is unclear in case both the country and
            // the modifier are present. Should really language@modifier be of
            // higher priority than language_country?
            // In at least one case (Serbian language), it is better this way.
            if (!modifier.isEmpty()) {
                languages += language + QLatin1Char('@') + modifier;
            }
            if (!country.isEmpty()) {
                languages += language + QLatin1Char('_') + country;
            }
            languages += language;
        }
    }
}

// Extract the first country code from a list of language_COUNTRY strings.
// Country code is converted to all lower case letters.
static QString extractCountry(const QStringList &languages)
{
    QString country;
    foreach (const QString &language, languages) {
        int pos1 = language.indexOf(QLatin1Char('_'));
        if (pos1 >= 0) {
            ++pos1;
            int pos2 = pos1;
            while (pos2 < language.length() && language[pos2].isLetter()) {
                ++pos2;
            }
            country = language.mid(pos1, pos2 - pos1);
            break;
        }
    }
    country = country.toLower();
    return country;
}

typedef qulonglong pluraln;
typedef qlonglong intn;
typedef qulonglong uintn;
typedef double realn;

class KLocalizedStringPrivate
{
    friend class KLocalizedString;

    QByteArray domain;
    QStringList languages;
    Kuit::VisualFormat format;
    QByteArray context;
    QByteArray text;
    QByteArray plural;
    QStringList arguments;
    QList<QVariant> values;
    QHash<int, KLocalizedString> klsArguments;
    QHash<int, int> klsArgumentFieldWidths;
    QHash<int, QChar> klsArgumentFillChars;
    bool numberSet;
    pluraln number;
    int numberOrdinal;
    QHash<QString, QString> dynamicContext;
    bool markupAware;
    bool relaxedSubs;

    static void translateRaw(const QByteArray &domain,
                             const QStringList &languages,
                             const QByteArray &msgctxt,
                             const QByteArray &msgid,
                             const QByteArray &msgid_plural,
                             qulonglong n,
                             QString &language,
                             QString &translation);

    QString toString(const QByteArray &domain,
                     const QStringList &languages,
                     Kuit::VisualFormat format,
                     bool isArgument = false) const;
    QString substituteSimple(const QString &translation,
                             const QStringList &arguments,
                             QChar plchar = QLatin1Char('%'),
                             bool isPartial = false) const;
    QString formatMarkup(const QByteArray &domain,
                         const QString &language,
                         const QString &context,
                         const QString &text,
                         Kuit::VisualFormat format) const;
    QString substituteTranscript(const QString &scriptedTranslation,
                                 const QString &language,
                                 const QString &country,
                                 const QString &ordinaryTranslation,
                                 const QStringList &arguments,
                                 const QList<QVariant> &values,
                                 bool &fallback) const;
    int resolveInterpolation(const QString &scriptedTranslation, int pos,
                             const QString &language,
                             const QString &country,
                             const QString &ordinaryTranslation,
                             const QStringList &arguments,
                             const QList<QVariant> &values,
                             QString &result,
                             bool &fallback) const;
    QVariant segmentToValue(const QString &segment) const;
    QString postTranscript(const QString &pcall,
                           const QString &language,
                           const QString &country,
                           const QString &finalTranslation,
                           const QStringList &arguments,
                           const QList<QVariant> &values) const;

    static const KCatalog &getCatalog(const QByteArray &domain,
                                      const QString &language);
    static void locateScriptingModule(const QByteArray &domain,
                                      const QString &language);

    static void loadTranscript();
};

typedef QHash<QString, KCatalog*> KCatalogPtrHash;

class KLocalizedStringPrivateStatics
{
    public:

    QHash<QByteArray, KCatalogPtrHash> catalogs;
    QStringList languages;

    QByteArray ourDomain;
    QByteArray applicationDomain;
    QString codeLanguage;
    QStringList localeLanguages;

    const QString theFence;
    const QString startInterp;
    const QString endInterp;
    const QChar scriptPlchar;
    const QChar scriptVachar;

    const QString scriptDir;
    QHash<QString, QStringList> scriptModules;
    QList<QStringList> scriptModulesToLoad;

    bool loadTranscriptCalled;
    KTranscript *ktrs;

    QHash<QString, KuitFormatter*> formatters;

    QList<QByteArray> qtDomains;
    QList<int> qtDomainInsertCount;

    QMutex klspMutex;

    KLocalizedStringPrivateStatics();
    ~KLocalizedStringPrivateStatics();

    void initializeLocaleLanguages();
};

KLocalizedStringPrivateStatics::KLocalizedStringPrivateStatics()
    : catalogs()
    , languages()

    , ourDomain("ki18n")
    , applicationDomain()
    , codeLanguage(QLatin1String("en_US"))
    , localeLanguages()

    , theFence(QLatin1String("|/|"))
    , startInterp(QLatin1String("$["))
    , endInterp(QLatin1String("]"))
    , scriptPlchar(QLatin1Char('%'))
    , scriptVachar(QLatin1Char('^'))

    , scriptDir(QLatin1String("LC_SCRIPTS"))
    , scriptModules()
    , scriptModulesToLoad()

    , loadTranscriptCalled(false)
    , ktrs(NULL)

    , formatters()

    , qtDomains()
    , qtDomainInsertCount()

    , klspMutex(QMutex::Recursive)
{
    initializeLocaleLanguages();
    languages = localeLanguages;
}

KLocalizedStringPrivateStatics::~KLocalizedStringPrivateStatics()
{
    foreach (const KCatalogPtrHash &languageCatalogs, catalogs) {
        qDeleteAll(languageCatalogs);
    }
    // ktrs is handled by QLibrary.
    //delete ktrs;
    qDeleteAll(formatters);
}

Q_GLOBAL_STATIC(KLocalizedStringPrivateStatics, staticsKLSP)

void KLocalizedStringPrivateStatics::initializeLocaleLanguages()
{
    QMutexLocker lock(&klspMutex);

    // Collect languages by same order of priority as for gettext(3).
    // LANGUAGE contains list of language codes, not locale string.
    appendLanguagesFromVariable(localeLanguages, "LANGUAGE", true);
    appendLanguagesFromVariable(localeLanguages, "LC_ALL");
    appendLanguagesFromVariable(localeLanguages, "LC_MESSAGES");
    appendLanguagesFromVariable(localeLanguages, "LANG");
}

KLocalizedString::KLocalizedString()
: d(new KLocalizedStringPrivate)
{
}

KLocalizedString::KLocalizedString(const char *domain, const char *context,
                                   const char *text, const char *plural,
                                   bool markupAware)
: d(new KLocalizedStringPrivate)
{
    d->domain = domain;
    d->languages.clear();
    d->format = Kuit::UndefinedFormat;
    d->context = context;
    d->text = text;
    d->plural = plural;
    d->numberSet = false;
    d->number = 0;
    d->numberOrdinal = 0;
    d->markupAware = markupAware;
    d->relaxedSubs = false;
}

KLocalizedString::KLocalizedString(const KLocalizedString &rhs)
: d(new KLocalizedStringPrivate(*rhs.d))
{
}

KLocalizedString& KLocalizedString::operator=(const KLocalizedString &rhs)
{
    if (&rhs != this) {
        *d = *rhs.d;
    }
    return *this;
}

KLocalizedString::~KLocalizedString()
{
    delete d;
}

bool KLocalizedString::isEmpty() const
{
    return d->text.isEmpty();
}

void KLocalizedStringPrivate::translateRaw(const QByteArray &domain,
                                           const QStringList &languages,
                                           const QByteArray &msgctxt,
                                           const QByteArray &msgid,
                                           const QByteArray &msgid_plural,
                                           qulonglong n,
                                           QString &language,
                                           QString &msgstr)
{
    KLocalizedStringPrivateStatics *s = staticsKLSP();

    // Empty msgid would result in returning the catalog header,
    // which is never intended, so warn and return empty translation.
    if (msgid.isNull() || msgid.isEmpty()) {
        qWarning() << QString::fromLatin1(
            "KLocalizedString: "
            "Trying to look up translation of \"\", fix the code.");
        language.clear();
        msgstr.clear();
        return;
    }
    // Gettext semantics allows empty context, but it is pointless, so warn.
    if (!msgctxt.isNull() && msgctxt.isEmpty()) {
        qWarning() << QString::fromLatin1(
            "KLocalizedString: "
            "Using \"\" as context, fix the code.");
    }
    // Gettext semantics allows empty plural, but it is pointless, so warn.
    if (!msgid_plural.isNull() && msgid_plural.isEmpty()) {
        qWarning() << QString::fromLatin1(
            "KLocalizedString: "
            "Using \"\" as plural text, fix the code.");
    }

    // Set translation to text in code language, in case no translation found.
    msgstr =   msgid_plural.isNull() || n == 1
             ? QString::fromUtf8(msgid)
             : QString::fromUtf8(msgid_plural);
    language = s->codeLanguage;

    if (domain.isEmpty()) {
        return;
    }

    // Languages are ordered from highest to lowest priority.
    foreach (const QString &testLanguage, languages) {
        // If code language reached, no catalog lookup is needed.
        if (testLanguage == s->codeLanguage) {
            return;
        }
        // Skip this language if there is no application catalog for it.
        if (!KLocalizedString::isApplicationTranslatedInto(testLanguage)) {
            continue;
        }
        const KCatalog &catalog = getCatalog(domain, testLanguage);
        QString testMsgstr;
        if (!msgctxt.isNull() && !msgid_plural.isNull()) {
            testMsgstr = catalog.translate(msgctxt, msgid, msgid_plural, n);
        } else if (!msgid_plural.isNull()) {
            testMsgstr = catalog.translate(msgid, msgid_plural, n);
        } else if (!msgctxt.isNull()) {
            testMsgstr = catalog.translate(msgctxt, msgid);
        } else {
            testMsgstr = catalog.translate(msgid);
        }
        if (!testMsgstr.isEmpty()) {
            // Translation found.
            language = testLanguage;
            msgstr = testMsgstr;
            return;
        }
    }
}

QString KLocalizedString::toString() const
{
    return d->toString(d->domain, d->languages, d->format);
}

QString KLocalizedString::toString(const char *domain) const
{
    return d->toString(domain, d->languages, d->format);
}

QString KLocalizedString::toString(const QStringList &languages) const
{
    return d->toString(d->domain, languages, d->format);
}

QString KLocalizedString::toString(Kuit::VisualFormat format) const
{
    return d->toString(d->domain, d->languages, format);
}

QString KLocalizedStringPrivate::toString(const QByteArray &domain,
                                          const QStringList &languages,
                                          Kuit::VisualFormat format,
                                          bool isArgument) const
{
    KLocalizedStringPrivateStatics *s = staticsKLSP();

    QMutexLocker lock(&s->klspMutex);

    // Assure the message has been supplied.
    if (text.isEmpty()) {
        qWarning() << QString::fromLatin1(
            "Trying to convert empty KLocalizedString to QString.");
        #ifndef NDEBUG
        return QLatin1String("(I18N_EMPTY_MESSAGE)");
        #else
        return QString();
        #endif
    }

    // Check whether plural argument has been supplied, if message has plural.
    if (!plural.isEmpty() && !numberSet) {
        qWarning() << QString::fromLatin1(
            "Plural argument to message {%1} not supplied before conversion.")
            .arg(shortenMessage(QString::fromUtf8(text)));
    }

    // Resolve inputs.
    QByteArray resolvedDomain = domain;
    if (resolvedDomain.isEmpty()) {
        resolvedDomain = s->applicationDomain;
    }
    QStringList resolvedLanguages = languages;
    if (resolvedLanguages.isEmpty()) {
        resolvedLanguages = s->languages;
    }
    Kuit::VisualFormat resolvedFormat = format;

    // Get raw translation.
    QString language, rawTranslation;
    translateRaw(resolvedDomain, resolvedLanguages,
                 context, text, plural, number,
                 language, rawTranslation);
    QString country = extractCountry(resolvedLanguages);

    // Set ordinary translation and possibly scripted translation.
    QString translation, scriptedTranslation;
    int fencePos = rawTranslation.indexOf(s->theFence);
    if (fencePos > 0) {
        // Script fence has been found, strip the scripted from the
        // ordinary translation.
        translation = rawTranslation.left(fencePos);

        // Scripted translation.
        scriptedTranslation = rawTranslation.mid(fencePos + s->theFence.length());

        // Try to initialize Transcript if not initialized and script not empty.
        // FIXME: And also if Transcript not disabled: where to configure this?
        if (!s->loadTranscriptCalled && !scriptedTranslation.isEmpty()) {
            loadTranscript();

            // Definitions from this library's scripting module
            // must be available to all other modules.
            // So force creation of this library's catalog here,
            // to make sure the scripting module is loaded.
            getCatalog(s->ourDomain, language);
        }
    } else if (fencePos < 0) {
        // No script fence, use translation as is.
        translation = rawTranslation;
    } else { // fencePos == 0
        // The msgstr starts with the script fence, no ordinary translation.
        // This is not allowed, consider message not translated.
        qWarning() << QString::fromLatin1(
            "Scripted message {%1} without ordinary translation, discarded.")
            .arg(shortenMessage(translation)) ;
        translation =   plural.isEmpty() || number == 1
                      ? QString::fromUtf8(text)
                      : QString::fromUtf8(plural);
    }

    // Resolve substituted KLocalizedString arguments.
    QStringList resolvedArguments;
    QList<QVariant> resolvedValues;
    for (int i = 0; i < arguments.size(); i++) {
        if (klsArguments.contains(i)) {
            const KLocalizedString &kls = klsArguments.value(i);
            int fieldWidth = klsArgumentFieldWidths.value(i);
            QChar fillChar = klsArgumentFillChars.value(i);
            // Override argument's languages and format, but not domain.
            bool isArgumentSub = true;
            QString resdArg = kls.d->toString(kls.d->domain, resolvedLanguages,
                                              resolvedFormat, isArgumentSub);
            resolvedValues.append(resdArg);
            if (markupAware && !kls.d->markupAware) {
                resdArg = Kuit::escape(resdArg);
            }
            resdArg = QString::fromLatin1("%1").arg(resdArg, fieldWidth, fillChar);
            resolvedArguments.append(resdArg);
        } else {
            QString resdArg = arguments[i];
            if (markupAware) {
                resdArg = Kuit::escape(resdArg);
            }
            resolvedArguments.append(resdArg);
            resolvedValues.append(values[i]);
        }
    }

    // Substitute placeholders in ordinary translation.
    QString finalTranslation = substituteSimple(translation, resolvedArguments);
    if (markupAware && !isArgument) {
        // Resolve markup in ordinary translation.
        finalTranslation = formatMarkup(resolvedDomain,
                                        language,
                                        QString::fromUtf8(context),
                                        finalTranslation,
                                        resolvedFormat);
    }

    // If there is also a scripted translation.
    if (!scriptedTranslation.isEmpty()) {
        // Evaluate scripted translation.
        bool fallback;
        scriptedTranslation = substituteTranscript(scriptedTranslation,
                                                   language, country,
                                                   finalTranslation,
                                                   resolvedArguments,
                                                   resolvedValues,
                                                   fallback);

        // If any translation produced and no fallback requested.
        if (!scriptedTranslation.isEmpty() && !fallback) {
            if (markupAware && !isArgument) {
                // Resolve markup in scripted translation.
                scriptedTranslation = formatMarkup(resolvedDomain,
                                                   language,
                                                   QString::fromUtf8(context),
                                                   scriptedTranslation,
                                                   resolvedFormat);
            }
            finalTranslation = scriptedTranslation;
        }
    }

    // Execute any scripted post calls; they cannot modify the final result,
    // but are used to set states.
    if (s->ktrs != NULL) {
        QStringList pcalls = s->ktrs->postCalls(language);
        foreach(const QString &pcall, pcalls) {
            postTranscript(pcall, language, country, finalTranslation,
                           resolvedArguments, resolvedValues);
        }
    }

    return finalTranslation;
}

QString KLocalizedStringPrivate::substituteSimple(const QString &translation,
                                                  const QStringList &arguments,
                                                  QChar plchar,
                                                  bool isPartial) const
{
    #ifdef NDEBUG
    Q_UNUSED(isPartial);
    #endif

    QStringList tsegs; // text segments per placeholder occurrence
    QList<int> plords; // ordinal numbers per placeholder occurrence
    #ifndef NDEBUG
    QVector<int> ords; // indicates which placeholders are present
    #endif
    int slen = translation.length();
    int spos = 0;
    int tpos = translation.indexOf(plchar);
    while (tpos >= 0) {
        int ctpos = tpos;

        ++tpos;
        if (tpos == slen) {
            break;
        }

        if (translation[tpos].digitValue() > 0) {
            // NOTE: %0 is not considered a placeholder.
            // Get the placeholder ordinal.
            int plord = 0;
            while (tpos < slen && translation[tpos].digitValue() >= 0) {
                plord = 10 * plord + translation[tpos].digitValue();
                ++tpos;
            }
            --plord; // ordinals are zero based

            #ifndef NDEBUG
            // Perhaps enlarge storage for indicators.
            // Note that QVector<int> will initialize new elements to 0,
            // as they are supposed to be.
            if (plord >= ords.size()) {
                ords.resize(plord + 1);
            }

            // Indicate that placeholder with computed ordinal is present.
            ords[plord] = 1;
            #endif

            // Store text segment prior to placeholder and placeholder number.
            tsegs.append(translation.mid(spos, ctpos - spos));
            plords.append(plord);

            // Position of next text segment.
            spos = tpos;
        }

        tpos = translation.indexOf(plchar, tpos);
    }
    // Store last text segment.
    tsegs.append(translation.mid(spos));

    #ifndef NDEBUG
    // Perhaps enlarge storage for plural-number ordinal.
    if (!plural.isEmpty() && numberOrdinal >= ords.size()) {
        ords.resize(numberOrdinal + 1);
    }

    // Message might have plural but without plural placeholder, which is an
    // allowed state. To ease further logic, indicate that plural placeholder
    // is present anyway if message has plural.
    if (!plural.isEmpty()) {
        ords[numberOrdinal] = 1;
    }
    #endif

    // Assemble the final string from text segments and arguments.
    QString finalTranslation;
    for (int i = 0; i < plords.size(); i++) {
        finalTranslation.append(tsegs.at(i));
        if (plords.at(i) >= arguments.size()) { // too little arguments
            // put back the placeholder
            finalTranslation.append(QLatin1Char('%') + QString::number(plords.at(i) + 1));
            #ifndef NDEBUG
            if (!isPartial) {
                // spoof the message
                finalTranslation.append(QLatin1String("(I18N_ARGUMENT_MISSING)"));
            }
            #endif
        } else { // just fine
            finalTranslation.append(arguments.at(plords.at(i)));
        }
    }
    finalTranslation.append(tsegs.last());

    #ifndef NDEBUG
    if (!isPartial && !relaxedSubs) {
        // Check that there are no gaps in numbering sequence of placeholders.
        bool gaps = false;
        for (int i = 0; i < ords.size(); i++) {
            if (!ords.at(i)) {
                gaps = true;
                qWarning() << QString::fromLatin1(
                    "Placeholder %%1 skipped in message {%2}.")
                    .arg(QString::number(i + 1), shortenMessage(translation));
            }
        }
        // If no gaps, check for mismatch between the number of
        // unique placeholders and actually supplied arguments.
        if (!gaps && ords.size() != arguments.size()) {
            qWarning() << QString::fromLatin1(
                "%1 instead of %2 arguments to message {%3} "
                "supplied before conversion.")
                .arg(arguments.size()).arg(ords.size())
                .arg(shortenMessage(translation));
        }

        // Some spoofs.
        if (gaps) {
            finalTranslation.append(QLatin1String("(I18N_GAPS_IN_PLACEHOLDER_SEQUENCE)"));
        }
        if (ords.size() < arguments.size()) {
            finalTranslation.append(QLatin1String("(I18N_EXCESS_ARGUMENTS_SUPPLIED)"));
        }
    }
    if (!isPartial) {
        if (!plural.isEmpty() && !numberSet) {
            finalTranslation.append(QLatin1String("(I18N_PLURAL_ARGUMENT_MISSING)"));
        }
    }
    #endif

    return finalTranslation;
}

QString KLocalizedStringPrivate::formatMarkup(const QByteArray &domain,
                                              const QString &language,
                                              const QString &context,
                                              const QString &text,
                                              Kuit::VisualFormat format) const
{
    KLocalizedStringPrivateStatics *s = staticsKLSP();

    QHash<QString, KuitFormatter*>::iterator formatter = s->formatters.find(language);
    if (formatter == s->formatters.end()) {
        formatter = s->formatters.insert(language, new KuitFormatter(language));
    }
    return (*formatter)->format(domain, context, text, format);
}

QString KLocalizedStringPrivate::substituteTranscript(const QString &scriptedTranslation,
                                                      const QString &language,
                                                      const QString &country,
                                                      const QString &ordinaryTranslation,
                                                      const QStringList &arguments,
                                                      const QList<QVariant> &values,
                                                      bool &fallback) const
{
    KLocalizedStringPrivateStatics *s = staticsKLSP();

    if (s->ktrs == NULL) {
        // Scripting engine not available.
        return QString();
    }

    // Iterate by interpolations.
    QString finalTranslation;
    fallback = false;
    int ppos = 0;
    int tpos = scriptedTranslation.indexOf(s->startInterp);
    while (tpos >= 0) {
        // Resolve substitutions in preceding text.
        QString ptext = substituteSimple(scriptedTranslation.mid(ppos, tpos - ppos),
                                         arguments, s->scriptPlchar, true);
        finalTranslation.append(ptext);

        // Resolve interpolation.
        QString result;
        bool fallbackLocal;
        tpos = resolveInterpolation(scriptedTranslation, tpos,
                                    language, country, ordinaryTranslation,
                                    arguments, values,
                                    result, fallbackLocal);

        // If there was a problem in parsing the interpolation, cannot proceed
        // (debug info already reported while parsing).
        if (tpos < 0) {
            return QString();
        }
        // If fallback has been explicitly requested, indicate global fallback
        // but proceed with evaluations (other interpolations may set states).
        if (fallbackLocal) {
            fallback = true;
        }

        // Add evaluated interpolation to the text.
        finalTranslation.append(result);

        // On to next interpolation.
        ppos = tpos;
        tpos = scriptedTranslation.indexOf(s->startInterp, tpos);
    }
    // Last text segment.
    finalTranslation.append(substituteSimple(scriptedTranslation.mid(ppos),
                                             arguments, s->scriptPlchar, true));

    // Return empty string if fallback was requested.
    return fallback ? QString() : finalTranslation;
}

int KLocalizedStringPrivate::resolveInterpolation(const QString &scriptedTranslation,
                                                  int pos,
                                                  const QString &language,
                                                  const QString &country,
                                                  const QString &ordinaryTranslation,
                                                  const QStringList &arguments,
                                                  const QList<QVariant> &values,
                                                  QString &result,
                                                  bool &fallback) const
{
    // pos is the position of opening character sequence.
    // Returns the position of first character after closing sequence,
    // or -1 in case of parsing error.
    // result is set to result of Transcript evaluation.
    // fallback is set to true if Transcript evaluation requested so.

    KLocalizedStringPrivateStatics *s = staticsKLSP();

    result.clear();
    fallback = false;

    // Split interpolation into arguments.
    QList<QVariant> iargs;
    int slen = scriptedTranslation.length();
    int islen = s->startInterp.length();
    int ielen = s->endInterp.length();
    int tpos = pos + s->startInterp.length();
    while (1) {
        // Skip whitespace.
        while (tpos < slen && scriptedTranslation[tpos].isSpace()) {
            ++tpos;
        }
        if (tpos == slen) {
            qWarning() << QString::fromLatin1(
                "Unclosed interpolation {%1} in message {%2}.")
                .arg(scriptedTranslation.mid(pos, tpos - pos),
                     shortenMessage(scriptedTranslation));
            return -1;
        }
        if (scriptedTranslation.mid(tpos, ielen) == s->endInterp) {
            break; // no more arguments
        }

        // Parse argument: may be concatenated from free and quoted text,
        // and sub-interpolations.
        // Free and quoted segments may contain placeholders, substitute them;
        // recurse into sub-interpolations.
        // Free segments may be value references, parse and record for
        // consideration at the end.
        // Mind backslash escapes throughout.
        QStringList segs;
        QVariant vref;
        while (   !scriptedTranslation[tpos].isSpace()
               && scriptedTranslation.mid(tpos, ielen) != s->endInterp) {
            if (scriptedTranslation[tpos] == QLatin1Char('\'')) { // quoted segment
                QString seg;
                ++tpos; // skip opening quote
                // Find closing quote.
                while (tpos < slen && scriptedTranslation[tpos] != QLatin1Char('\'')) {
                    if (scriptedTranslation[tpos] == QLatin1Char('\\'))
                        ++tpos; // escape next character
                    seg.append(scriptedTranslation[tpos]);
                    ++tpos;
                }
                if (tpos == slen) {
                    qWarning() << QString::fromLatin1(
                        "Unclosed quote in interpolation {%1} in message {%2}.")
                        .arg(scriptedTranslation.mid(pos, tpos - pos),
                             shortenMessage(scriptedTranslation));
                    return -1;
                }

                // Append to list of segments, resolving placeholders.
                segs.append(substituteSimple(seg, arguments, s->scriptPlchar, true));

                ++tpos; // skip closing quote
            } else if (scriptedTranslation.mid(tpos, islen) == s->startInterp) { // sub-interpolation
                QString resultLocal;
                bool fallbackLocal;
                tpos = resolveInterpolation(scriptedTranslation, tpos,
                                            language, country, ordinaryTranslation,
                                            arguments, values,
                                            resultLocal, fallbackLocal);
                if (tpos < 0) { // unrecoverable problem in sub-interpolation
                    // Error reported in the subcall.
                    return tpos;
                }
                if (fallbackLocal) { // sub-interpolation requested fallback
                    fallback = true;
                }
                segs.append(resultLocal);
            } else { // free segment
                QString seg;
                // Find whitespace, quote, opening or closing sequence.
                while (   tpos < slen
                       && !scriptedTranslation[tpos].isSpace()
                       && scriptedTranslation[tpos] != QLatin1Char('\'')
                       && scriptedTranslation.mid(tpos, islen) != s->startInterp
                       && scriptedTranslation.mid(tpos, ielen) != s->endInterp) {
                    if (scriptedTranslation[tpos] == QLatin1Char('\\'))
                        ++tpos; // escape next character
                    seg.append(scriptedTranslation[tpos]);
                    ++tpos;
                }
                if (tpos == slen) {
                    qWarning() << QString::fromLatin1(
                        "Non-terminated interpolation {%1} in message {%2}.")
                        .arg(scriptedTranslation.mid(pos, tpos - pos),
                             shortenMessage(scriptedTranslation));
                    return -1;
                }

                // The free segment may look like a value reference;
                // in that case, record which value it would reference,
                // and add verbatim to the segment list.
                // Otherwise, do a normal substitution on the segment.
                vref = segmentToValue(seg);
                if (vref.isValid()) {
                    segs.append(seg);
                } else {
                    segs.append(substituteSimple(seg, arguments, s->scriptPlchar, true));
                }
            }
        }

        // Append this argument to rest of the arguments.
        // If the there was a single text segment and it was a proper value
        // reference, add it instead of the joined segments.
        // Otherwise, add the joined segments.
        if (segs.size() == 1 && vref.isValid()) {
            iargs.append(vref);
        } else {
            iargs.append(segs.join(QString()));
        }
    }
    tpos += ielen; // skip to first character after closing sequence

    // NOTE: Why not substitute placeholders (via substituteSimple) in one
    // global pass, then handle interpolations in second pass? Because then
    // there is the danger of substituted text or sub-interpolations producing
    // quotes and escapes themselves, which would mess up the parsing.

    // Evaluate interpolation.
    QString msgctxt = QString::fromUtf8(context);
    QString msgid = QString::fromUtf8(text);
    QString scriptError;
    bool fallbackLocal;
    result = s->ktrs->eval(iargs, language, country,
                           msgctxt, dynamicContext, msgid,
                           arguments, values, ordinaryTranslation,
                           s->scriptModulesToLoad,
                           scriptError, fallbackLocal);
    // s->scriptModulesToLoad will be cleared during the call.

    if (fallbackLocal) { // evaluation requested fallback
        fallback = true;
    }
    if (!scriptError.isEmpty()) { // problem with evaluation
        fallback = true; // also signal fallback
        if (!scriptError.isEmpty()) {
            qWarning() << QString::fromLatin1(
                "Interpolation {%1} in {%2} failed: %3")
                .arg(scriptedTranslation.mid(pos, tpos - pos),
                     shortenMessage(scriptedTranslation),
                     scriptError);
        }
    }

    return tpos;
}

QVariant KLocalizedStringPrivate::segmentToValue(const QString &segment) const
{
    KLocalizedStringPrivateStatics *s = staticsKLSP();

    // Return invalid variant if segment is either not a proper
    // value reference, or the reference is out of bounds.

    // Value reference must start with a special character.
    if (segment.left(1) != s->scriptVachar) {
        return QVariant();
    }

    // Reference number must start with 1-9.
    // (If numstr is empty, toInt() will return 0.)
    QString numstr = segment.mid(1);
    if (numstr.left(1).toInt() < 1) {
        return QVariant();
    }

    // Number must be valid and in bounds.
    bool ok;
    int index = numstr.toInt(&ok) - 1;
    if (!ok || index >= values.size()) {
        return QVariant();
    }

    // Passed all hoops.
    return values.at(index);
}

QString KLocalizedStringPrivate::postTranscript(const QString &pcall,
                                                const QString &language,
                                                const QString &country,
                                                const QString &finalTranslation,
                                                const QStringList &arguments,
                                                const QList<QVariant> &values) const
{
    KLocalizedStringPrivateStatics *s = staticsKLSP();

    if (s->ktrs == NULL) {
        // Scripting engine not available.
        // (Though this cannot happen, we wouldn't be here then.)
        return QString();
    }

    // Resolve the post call.
    QList<QVariant> iargs;
    iargs.append(pcall);
    QString msgctxt = QString::fromUtf8(context);
    QString msgid = QString::fromUtf8(text);
    QString scriptError;
    bool fallback;
    QString dummy = s->ktrs->eval(iargs, language, country,
                                  msgctxt, dynamicContext, msgid,
                                  arguments, values, finalTranslation,
                                  s->scriptModulesToLoad,
                                  scriptError, fallback);
    // s->scriptModulesToLoad will be cleared during the call.

    // If the evaluation went wrong.
    if (!scriptError.isEmpty()) {
        qWarning() << QString::fromLatin1(
            "Post call {%1} for message {%2} failed: %3")
            .arg(pcall, shortenMessage(msgid), scriptError);
        return QString();
    }

    return finalTranslation;
}

KLocalizedString KLocalizedString::withLanguages(const QStringList &languages) const
{
    KLocalizedString kls(*this);
    kls.d->languages = languages;
    return kls;
}

KLocalizedString KLocalizedString::withDomain(const char *domain) const
{
    KLocalizedString kls(*this);
    kls.d->domain = domain;
    return kls;
}

KLocalizedString KLocalizedString::withFormat(Kuit::VisualFormat format) const
{
    KLocalizedString kls(*this);
    kls.d->format = format;
    return kls;
}

KLocalizedString KLocalizedString::subs(int a, int fieldWidth, int base,
                                        QChar fillChar) const
{
    KLocalizedString kls(*this);
    if (!kls.d->plural.isEmpty() && !kls.d->numberSet) {
        kls.d->number = static_cast<pluraln>(abs(a));
        kls.d->numberSet = true;
        kls.d->numberOrdinal = d->arguments.size();
    }
    kls.d->arguments.append(QString::fromLatin1("%1").arg(a, fieldWidth, base, fillChar));
    kls.d->values.append(static_cast<intn>(a));
    return kls;
}

KLocalizedString KLocalizedString::subs(uint a, int fieldWidth, int base,
                                        QChar fillChar) const
{
    KLocalizedString kls(*this);
    if (!kls.d->plural.isEmpty() && !kls.d->numberSet) {
        kls.d->number = static_cast<pluraln>(a);
        kls.d->numberSet = true;
        kls.d->numberOrdinal = d->arguments.size();
    }
    kls.d->arguments.append(QString::fromLatin1("%1").arg(a, fieldWidth, base, fillChar));
    kls.d->values.append(static_cast<uintn>(a));
    return kls;
}

KLocalizedString KLocalizedString::subs(long a, int fieldWidth, int base,
                                        QChar fillChar) const
{
    KLocalizedString kls(*this);
    if (!kls.d->plural.isEmpty() && !kls.d->numberSet) {
        kls.d->number = static_cast<pluraln>(abs(a));
        kls.d->numberSet = true;
        kls.d->numberOrdinal = d->arguments.size();
    }
    kls.d->arguments.append(QString::fromLatin1("%1").arg(a, fieldWidth, base, fillChar));
    kls.d->values.append(static_cast<intn>(a));
    return kls;
}

KLocalizedString KLocalizedString::subs(ulong a, int fieldWidth, int base,
                                        QChar fillChar) const
{
    KLocalizedString kls(*this);
    if (!kls.d->plural.isEmpty() && !kls.d->numberSet) {
        kls.d->number = static_cast<pluraln>(a);
        kls.d->numberSet = true;
        kls.d->numberOrdinal = d->arguments.size();
    }
    kls.d->arguments.append(QString::fromLatin1("%1").arg(a, fieldWidth, base, fillChar));
    kls.d->values.append(static_cast<uintn>(a));
    return kls;
}

KLocalizedString KLocalizedString::subs(qlonglong a, int fieldWidth, int base,
                                        QChar fillChar) const
{
    KLocalizedString kls(*this);
    if (!kls.d->plural.isEmpty() && !kls.d->numberSet) {
        kls.d->number = static_cast<pluraln>(qAbs(a));
        kls.d->numberSet = true;
        kls.d->numberOrdinal = d->arguments.size();
    }
    kls.d->arguments.append(QString::fromLatin1("%1").arg(a, fieldWidth, base, fillChar));
    kls.d->values.append(static_cast<intn>(a));
    return kls;
}

KLocalizedString KLocalizedString::subs(qulonglong a, int fieldWidth, int base,
                                        QChar fillChar) const
{
    KLocalizedString kls(*this);
    if (!kls.d->plural.isEmpty() && !kls.d->numberSet) {
        kls.d->number = static_cast<pluraln>(a);
        kls.d->numberSet = true;
        kls.d->numberOrdinal = d->arguments.size();
    }
    kls.d->arguments.append(QString::fromLatin1("%1").arg(a, fieldWidth, base, fillChar));
    kls.d->values.append(static_cast<uintn>(a));
    return kls;
}

KLocalizedString KLocalizedString::subs(double a, int fieldWidth,
                                        char format, int precision,
                                        QChar fillChar) const
{
    KLocalizedString kls(*this);
    kls.d->arguments.append(QString::fromLatin1("%1").arg(a, fieldWidth, format, precision, fillChar));
    kls.d->values.append(static_cast<realn>(a));
    return kls;
}

KLocalizedString KLocalizedString::subs(QChar a, int fieldWidth,
                                        QChar fillChar) const
{
    KLocalizedString kls(*this);
    QString baseArg = QString(a);
    QString fmtdArg = QString::fromLatin1("%1").arg(a, fieldWidth, fillChar);
    kls.d->arguments.append(fmtdArg);
    kls.d->values.append(baseArg);
    return kls;
}

KLocalizedString KLocalizedString::subs(const QString &a, int fieldWidth,
                                        QChar fillChar) const
{
    KLocalizedString kls(*this);
    QString baseArg = a;
    QString fmtdArg = QString::fromLatin1("%1").arg(a, fieldWidth, fillChar);
    kls.d->arguments.append(fmtdArg);
    kls.d->values.append(baseArg);
    return kls;
}

KLocalizedString KLocalizedString::subs(const KLocalizedString &a,
                                        int fieldWidth,
                                        QChar fillChar) const
{
    KLocalizedString kls(*this);
    // KLocalizedString arguments must be resolved inside toString
    // when the domain, language, visual format, etc. become known.
    int i = kls.d->arguments.size();
    kls.d->klsArguments[i] = a;
    kls.d->klsArgumentFieldWidths[i] = fieldWidth;
    kls.d->klsArgumentFillChars[i] = fillChar;
    kls.d->arguments.append(QString());
    kls.d->values.append(0);
    return kls;
}

KLocalizedString KLocalizedString::inContext(const QString &key,
                                             const QString &value) const
{
    KLocalizedString kls(*this);
    kls.d->dynamicContext[key] = value;
    return kls;
}

KLocalizedString KLocalizedString::relaxSubs() const
{
    KLocalizedString kls(*this);
    kls.d->relaxedSubs = true;
    return kls;
}

KLocalizedString KLocalizedString::ignoreMarkup() const
{
    KLocalizedString kls(*this);
    kls.d->markupAware = false;
    return kls;
}

void KLocalizedString::setApplicationDomain(const char *domain)
{
    KLocalizedStringPrivateStatics *s = staticsKLSP();

    QMutexLocker lock(&s->klspMutex);

    s->applicationDomain = domain;
}

QByteArray KLocalizedString::applicationDomain()
{
    KLocalizedStringPrivateStatics *s = staticsKLSP();

    return s->applicationDomain;
}

void KLocalizedString::setLanguages(const QStringList &languages)
{
    KLocalizedStringPrivateStatics *s = staticsKLSP();

    QMutexLocker lock(&s->klspMutex);

    s->languages = languages;
}

void KLocalizedString::clearLanguages()
{
    KLocalizedStringPrivateStatics *s = staticsKLSP();

    QMutexLocker lock(&s->klspMutex);

    s->languages = s->localeLanguages;
}

bool KLocalizedString::isApplicationTranslatedInto(const QString &language)
{
    KLocalizedStringPrivateStatics *s = staticsKLSP();

    return    language == s->codeLanguage
           || !KCatalog::catalogLocaleDir(s->applicationDomain, language).isEmpty();
}

const KCatalog &KLocalizedStringPrivate::getCatalog(const QByteArray &domain,
                                                    const QString &language)
{
    KLocalizedStringPrivateStatics *s = staticsKLSP();

    QMutexLocker lock(&s->klspMutex);

    QHash<QByteArray, KCatalogPtrHash>::iterator languageCatalogs = s->catalogs.find(domain);
    if (languageCatalogs == s->catalogs.end()) {
        languageCatalogs = s->catalogs.insert(domain, KCatalogPtrHash());
    }
    KCatalogPtrHash::iterator catalog = languageCatalogs->find(language);
    if (catalog == languageCatalogs->end()) {
        catalog = languageCatalogs->insert(language, new KCatalog(domain, language));
        locateScriptingModule(domain, language);
    }
    return **catalog;
}

void KLocalizedStringPrivate::locateScriptingModule(const QByteArray &domain,
                                                    const QString &language)
{
    KLocalizedStringPrivateStatics *s = staticsKLSP();

    QMutexLocker lock(&s->klspMutex);

    // Assemble module's relative path.
    QString modrpath =   language + QLatin1Char('/')
                       + s->scriptDir + QLatin1Char('/')
                       + QString::fromLatin1(domain) + QLatin1Char('/')
                       + QString::fromLatin1(domain) + QLatin1String(".js");

    // Try to find this module.
    QString modapath = QStandardPaths::locate(
        QStandardPaths::GenericDataLocation,
        QLatin1String("locale") + QLatin1Char('/') + modrpath);

    // If the module exists and hasn't been already included.
    if (   !modapath.isEmpty()
        && !s->scriptModules[language].contains(domain)) {
        // Indicate that the module has been considered.
        s->scriptModules[language].append(domain);

        // Store the absolute path and language of the module,
        // to load on next script evaluation.
        QStringList module;
        module.append(modapath);
        module.append(language);
        s->scriptModulesToLoad.append(module);
    }
}

extern "C"
{
    typedef KTranscript *(*InitFunc)();
}

void KLocalizedStringPrivate::loadTranscript()
{
    KLocalizedStringPrivateStatics *s = staticsKLSP();

    QMutexLocker lock(&s->klspMutex);

    s->loadTranscriptCalled = true;
    s->ktrs = NULL; // null indicates that Transcript is not available

    #if 0
    // FIXME: Automatic plugin path resolution does not work at the moment,
    // so search manually through library paths.
    QString pluginPathNoExt = QLatin1String("kf5/ktranscript");
    #else
    QString pluginPathNoExt;
    QStringList nameFilters;
    QString pluginSubdir = QLatin1String("kf5");
    QString pluginName = QLatin1String("ktranscript");
    nameFilters.append(pluginName + QLatin1String(".*"));
    foreach (const QString &dirPath, QCoreApplication::libraryPaths()) {
        QString dirPathKf = dirPath + QLatin1Char('/') + pluginSubdir;
        if (!QDir(dirPathKf).entryList(nameFilters).isEmpty()) {
            pluginPathNoExt = dirPathKf + QLatin1Char('/') + pluginName;
            break;
        }
    }
    if (pluginPathNoExt.isEmpty()) {
        qWarning() << QString::fromLatin1("Cannot find Transcript plugin.");
        return;
    }
    #endif

    QLibrary lib(pluginPathNoExt);
    if (!lib.load()) {
        qWarning() << QString::fromLatin1("Cannot load Transcript plugin:")
                   << lib.errorString();
        return;
    }

    InitFunc initf = (InitFunc) lib.resolve("load_transcript");
    if (!initf) {
        lib.unload();
        qWarning() << QString::fromLatin1(
            "Cannot find function load_transcript in Transcript plugin.");
        return;
    }

    s->ktrs = initf();
}

QString KLocalizedString::localizedFilePath(const QString &filePath)
{
    KLocalizedStringPrivateStatics *s = staticsKLSP();

    // Check if l10n subdirectory is present, stop if not.
    QFileInfo fileInfo(filePath);
    QString locDirPath =   fileInfo.path() + QLatin1Char('/')
                         + QLatin1String("l10n");
    QFileInfo locDirInfo(locDirPath);
    if (!locDirInfo.isDir()) {
        return filePath;
    }

    // Go through possible localized paths by priority of languages,
    // return first that exists.
    QString fileName = fileInfo.fileName();
    foreach (const QString &lang, s->languages) {
        QString locFilePath =   locDirPath + QLatin1Char('/')
                              + lang + QLatin1Char('/')
                              + fileName;
        QFileInfo locFileInfo(locFilePath);
        if (locFileInfo.isFile() && locFileInfo.isReadable()) {
            return locFilePath;
        }
    }

    return filePath;
}

QString KLocalizedString::removeAcceleratorMarker(const QString &label)
{
    return ::removeAcceleratorMarker(label);
}

QString KLocalizedString::translateQt(const char *context,
                                      const char *sourceText,
                                      const char *comment,
                                      int n)
{
    // NOTE: Qt message semantics.
    //
    // Qt's context is normally the name of the class of the method which makes
    // the tr(sourceText) call. However, it can also be manually supplied via
    // translate(context, sourceText) call.
    //
    // Qt's sourceText is the actual message displayed to the user.
    //
    // Qt's comment is an optional argument of tr() and translate(), like
    // tr(sourceText, comment) and translate(context, sourceText, comment).
    //
    // We handle this in the following way:
    //
    // If the comment is given, then it is considered gettext's msgctxt, so a
    // context call is made.
    //
    // If the comment is not given, but context is given, then we treat it as
    // msgctxt only if it was manually supplied (the one in translate()) -- but
    // we don't know this, so we first try a context call, and if translation
    // is not found, we fallback to ordinary call.
    //
    // If neither comment nor context are given, it's just an ordinary call
    // on sourceText.

    Q_UNUSED(n);

    KLocalizedStringPrivateStatics *s = staticsKLSP();

    QMutexLocker lock(&s->klspMutex);

    if (!sourceText || !sourceText[0]) {
        qWarning() << QString::fromLatin1(
            "KLocalizedString::translateQt: "
            "Trying to look up translation of \"\", fix the code.");
        return QString();
    }

    // NOTE: Condition (language != s->codeLanguage) means that translation
    // was found, otherwise the original text was returned as translation.
    QString translation;
    QString language;
    foreach (const QByteArray &domain, s->qtDomains) {
        if (comment && comment[0]) {
            // Comment given, go for context call.
            KLocalizedStringPrivate::translateRaw(domain, s->languages,
                                                  comment, sourceText, 0, 0,
                                                  language, translation);
        } else {
            // Comment not given, go for try-fallback with context.
            if (context && context[0]) {
                KLocalizedStringPrivate::translateRaw(domain, s->languages,
                                                      context, sourceText, 0, 0,
                                                      language, translation);
            }
            if (language.isEmpty() || language == s->codeLanguage) {
                KLocalizedStringPrivate::translateRaw(domain, s->languages,
                                                      0, sourceText, 0, 0,
                                                      language, translation);
            }
        }
        if (language != s->codeLanguage) {
            return translation;
        }
    }
    // No proper translation found, return empty according to Qt semantics.
    return QString();
}

void KLocalizedString::insertQtDomain(const char *domain)
{
    KLocalizedStringPrivateStatics *s = staticsKLSP();

    QMutexLocker lock(&s->klspMutex);

    int pos = s->qtDomains.indexOf(domain);
    if (pos < 0) {
        // Domain priority is undefined, but to minimize damage
        // due to message conflicts, put later inserted catalogs at front.
        s->qtDomains.prepend(domain);
        s->qtDomainInsertCount.prepend(1);
    } else {
        ++s->qtDomainInsertCount[pos];
    }
}

void KLocalizedString::removeQtDomain(const char *domain)
{
    KLocalizedStringPrivateStatics *s = staticsKLSP();

    QMutexLocker lock(&s->klspMutex);

    int pos = s->qtDomains.indexOf(domain);
    if (pos >= 0 && --s->qtDomainInsertCount[pos] == 0) {
        s->qtDomains.removeAt(pos);
        s->qtDomainInsertCount.removeAt(pos);
    }
}

KLocalizedString ki18n(const char* text)
{
    return KLocalizedString(NULL, NULL, text, NULL, false);
}

KLocalizedString ki18nc(const char* context, const char *text)
{
    return KLocalizedString(NULL, context, text, NULL, false);
}

KLocalizedString ki18np(const char* singular, const char* plural)
{
    return KLocalizedString(NULL, NULL, singular, plural, false);
}

KLocalizedString ki18ncp(const char* context,
                         const char* singular, const char* plural)
{
    return KLocalizedString(NULL, context, singular, plural, false);
}

KLocalizedString ki18nd(const char *domain, const char* text)
{
    return KLocalizedString(domain, NULL, text, NULL, false);
}

KLocalizedString ki18ndc(const char *domain, const char* context,
                         const char *text)
{
    return KLocalizedString(domain, context, text, NULL, false);
}

KLocalizedString ki18ndp(const char *domain,
                         const char* singular, const char* plural)
{
    return KLocalizedString(domain, NULL, singular, plural, false);
}

KLocalizedString ki18ndcp(const char *domain, const char* context,
                          const char* singular, const char* plural)
{
    return KLocalizedString(domain, context, singular, plural, false);
}

KLocalizedString kxi18n(const char* text)
{
    return KLocalizedString(NULL, NULL, text, NULL, true);
}

KLocalizedString kxi18nc(const char* context, const char *text)
{
    return KLocalizedString(NULL, context, text, NULL, true);
}

KLocalizedString kxi18np(const char* singular, const char* plural)
{
    return KLocalizedString(NULL, NULL, singular, plural, true);
}

KLocalizedString kxi18ncp(const char* context,
                          const char* singular, const char* plural)
{
    return KLocalizedString(NULL, context, singular, plural, true);
}

KLocalizedString kxi18nd(const char *domain, const char* text)
{
    return KLocalizedString(domain, NULL, text, NULL, true);
}

KLocalizedString kxi18ndc(const char* domain, const char* context,
                          const char *text)
{
    return KLocalizedString(domain, context, text, NULL, true);
}

KLocalizedString kxi18ndp(const char* domain,
                          const char* singular, const char* plural)
{
    return KLocalizedString(domain, NULL, singular, plural, true);
}

KLocalizedString kxi18ndcp(const char* domain, const char* context,
                           const char* singular, const char* plural)
{
    return KLocalizedString(domain, context, singular, plural, true);
}

