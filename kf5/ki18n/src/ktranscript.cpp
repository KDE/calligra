/*  This file is part of the KDE libraries    Copyright (C) 2007 Chusslove Illich <caslav.ilic@gmx.net>

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

#include <ktranscript_p.h>
#include <common_helpers_p.h>

#include <ki18n_export.h>

//#include <unistd.h>

#include <kjs/value.h>
#include <kjs/object.h>
#include <kjs/lookup.h>
#include <kjs/function.h>
#include <kjs/interpreter.h>
#include <kjs/string_object.h>
#include <kjs/error_object.h>

#include <QVariant>
#include <QStringList>
#include <QList>
#include <QDir>
#include <QHash>
#include <QPair>
#include <QSet>
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QRegExp>
#include <qendian.h>

using namespace KJS;

class KTranscriptImp;
class Scriptface;

typedef QHash<QString, QString> TsConfigGroup;
typedef QHash<QString, TsConfigGroup> TsConfig;

// Transcript implementation (used as singleton).
class KTranscriptImp : public KTranscript
{
    public:

    KTranscriptImp ();
    ~KTranscriptImp ();

    QString eval (const QList<QVariant> &argv,
                  const QString &lang,
                  const QString &ctry,
                  const QString &msgctxt,
                  const QHash<QString, QString> &dynctxt,
                  const QString &msgid,
                  const QStringList &subs,
                  const QList<QVariant> &vals,
                  const QString &ftrans,
                  QList<QStringList> &mods,
                  QString &error,
                  bool &fallback);

    QStringList postCalls (const QString &lang);

    // Lexical path of the module for the executing code.
    QString currentModulePath;

    private:

    void loadModules (const QList<QStringList> &mods, QString &error);
    void setupInterpreter (const QString &lang);

    TsConfig config;

    QHash<QString, Scriptface*> m_sface;
};

// Script-side transcript interface.
class Scriptface : public JSObject
{
    public:
    Scriptface (ExecState *exec, const TsConfigGroup &config);
    ~Scriptface ();

    // Interface functions.
    JSValue *loadf (ExecState *exec, const List &fnames);
    JSValue *setcallf (ExecState *exec, JSValue *name,
                       JSValue *func, JSValue *fval);
    JSValue *hascallf (ExecState *exec, JSValue *name);
    JSValue *acallf (ExecState *exec, const List &argv);
    JSValue *setcallForallf (ExecState *exec, JSValue *name,
                             JSValue *func, JSValue *fval);
    JSValue *fallbackf (ExecState *exec);
    JSValue *nsubsf (ExecState *exec);
    JSValue *subsf (ExecState *exec, JSValue *index);
    JSValue *valsf (ExecState *exec, JSValue *index);
    JSValue *msgctxtf (ExecState *exec);
    JSValue *dynctxtf (ExecState *exec, JSValue *key);
    JSValue *msgidf (ExecState *exec);
    JSValue *msgkeyf (ExecState *exec);
    JSValue *msgstrff (ExecState *exec);
    JSValue *dbgputsf (ExecState *exec, JSValue *str);
    JSValue *warnputsf (ExecState *exec, JSValue *str);
    JSValue *localeCountryf (ExecState *exec);
    JSValue *normKeyf (ExecState *exec, JSValue *phrase);
    JSValue *loadPropsf (ExecState *exec, const List &fnames);
    JSValue *getPropf (ExecState *exec, JSValue *phrase, JSValue *prop);
    JSValue *setPropf (ExecState *exec, JSValue *phrase, JSValue *prop, JSValue *value);
    JSValue *toUpperFirstf (ExecState *exec, JSValue *str, JSValue *nalt);
    JSValue *toLowerFirstf (ExecState *exec, JSValue *str, JSValue *nalt);
    JSValue *getConfStringf (ExecState *exec, JSValue *key, JSValue *dval);
    JSValue *getConfBoolf (ExecState *exec, JSValue *key, JSValue *dval);
    JSValue *getConfNumberf (ExecState *exec, JSValue *key, JSValue *dval);

    enum {
        Load,
        Setcall,
        Hascall,
        Acall,
        SetcallForall,
        Fallback,
        Nsubs,
        Subs,
        Vals,
        Msgctxt,
        Dynctxt,
        Msgid,
        Msgkey,
        Msgstrf,
        Dbgputs,
        Warnputs,
        LocaleCountry,
        NormKey,
        LoadProps,
        GetProp,
        SetProp,
        ToUpperFirst,
        ToLowerFirst,
        GetConfString,
        GetConfBool,
        GetConfNumber
    };

    // Helper methods to interface functions.
    QString loadProps_text (const QString &fpath);
    QString loadProps_bin (const QString &fpath);
    QString loadProps_bin_00 (const QString &fpath);
    QString loadProps_bin_01 (const QString &fpath);

    // Virtual implementations.
    bool getOwnPropertySlot (ExecState *exec, const Identifier& propertyName, PropertySlot& slot);
    JSValue *getValueProperty (ExecState *exec, int token) const;
    void put (ExecState *exec, const Identifier &propertyName, JSValue *value, int attr);
    void putValueProperty (ExecState *exec, int token, JSValue *value, int attr);
    const ClassInfo* classInfo() const { return &info; }

    static const ClassInfo info;

    // Link to its interpreter.
    // FIXME: Probably accessible without the explicit link.
    Interpreter *jsi;

    // Current message data.
    const QString *msgctxt;
    const QHash<QString, QString> *dynctxt;
    const QString *msgid;
    const QStringList *subs;
    const QList<QVariant> *vals;
    const QString *ftrans;
    const QString *ctry;

    // Fallback request handle.
    bool *fallback;

    // Function register.
    QHash<QString, JSObject*> funcs;
    QHash<QString, JSValue*> fvals;
    QHash<QString, QString> fpaths;

    // Ordering of those functions which execute for all messages.
    QList<QString> nameForalls;

    // Property values per phrase (used by *Prop interface calls).
    // Not QStrings, in order to avoid conversion from UTF-8 when
    // loading compiled maps (less latency on startup).
    QHash<QByteArray, QHash<QByteArray, QByteArray> > phraseProps;
    // Unresolved property values per phrase,
    // containing the pointer to compiled pmap file handle and offset in it.
    QHash<QByteArray, QPair<QFile*, quint64> > phraseUnparsedProps;
    QHash<QByteArray, QByteArray> resolveUnparsedProps (const QByteArray &phrase);
    // Set of loaded pmap files by paths and file handle pointers.
    QSet<QString> loadedPmapPaths;
    QSet<QFile*> loadedPmapHandles;

    // User config.
    TsConfigGroup config;
};

// ----------------------------------------------------------------------
// Custom debug and warning output (kdebug not available)
#define DBGP "KTranscript: "
void dbgout (const char*str) {
    #ifndef NDEBUG
    fprintf(stderr, DBGP"%s\n", str);
    #else
    Q_UNUSED(str);
    #endif
}
template <typename T1>
void dbgout (const char* str, const T1 &a1) {
    #ifndef NDEBUG
    fprintf(stderr, DBGP"%s\n", QString::fromUtf8(str).arg(a1).toLocal8Bit().data());
    #else
    Q_UNUSED(str); Q_UNUSED(a1);
    #endif
}
template <typename T1, typename T2>
void dbgout (const char* str, const T1 &a1, const T2 &a2) {
    #ifndef NDEBUG
    fprintf(stderr, DBGP"%s\n", QString::fromUtf8(str).arg(a1).arg(a2).toLocal8Bit().data());
    #else
    Q_UNUSED(str); Q_UNUSED(a1); Q_UNUSED(a2);
    #endif
}
template <typename T1, typename T2, typename T3>
void dbgout (const char* str, const T1 &a1, const T2 &a2, const T3 &a3) {
    #ifndef NDEBUG
    fprintf(stderr, DBGP"%s\n", QString::fromUtf8(str).arg(a1).arg(a2).arg(a3).toLocal8Bit().data());
    #else
    Q_UNUSED(str); Q_UNUSED(a1); Q_UNUSED(a2); Q_UNUSED(a3);
    #endif
}

#define WARNP "KTranscript: "
void warnout (const char*str) {
    fprintf(stderr, WARNP"%s\n", str);
}
template <typename T1>
void warnout (const char* str, const T1 &a1) {
    fprintf(stderr, WARNP"%s\n", QString::fromUtf8(str).arg(a1).toLocal8Bit().data());
}

// ----------------------------------------------------------------------
// Conversions between QString and KJS UString.
// Taken from kate.
UString::UString(const QString &d)
{
    unsigned int len = d.length();
    UChar *dat = static_cast<UChar*>(fastMalloc(sizeof(UChar) * len));
    memcpy(dat, d.unicode(), len * sizeof(UChar));
    m_rep = UString::Rep::create(dat, len);
}
QString UString::qstring() const
{
    return QString((QChar*) data(), size());
}

// ----------------------------------------------------------------------
// Produces a string out of a KJS exception.
QString expt2str (ExecState *exec)
{
    JSValue *expt = exec->exception();
    if (   expt->isObject()
        && expt->getObject()->hasProperty(exec, "message"))
    {
        JSValue *msg = expt->getObject()->get(exec, "message");
        return QString::fromLatin1("Error: %1").arg(msg->getString().qstring());
    }
    else
    {
        QString strexpt = exec->exception()->toString(exec).qstring();
        return QString::fromLatin1("Caught exception: %1").arg(strexpt);
    }
}

// ----------------------------------------------------------------------
// Count number of lines in the string,
// up to and excluding the requested position.
int countLines (const QString &s, int p)
{
    int n = 1;
    int len = s.length();
    for (int i = 0; i < p && i < len; ++i) {
        if (s[i] == QLatin1Char('\n')) {
            ++n;
        }
    }
    return n;
}

// ----------------------------------------------------------------------
// Normalize string key for hash lookups,
QByteArray normKeystr (const QString &raw, bool mayHaveAcc = true)
{
    // NOTE: Regexes should not be used here for performance reasons.
    // This function may potentially be called thousands of times
    // on application startup.

    QString key = raw;

    // Strip all whitespace.
    int len = key.length();
    QString nkey;
    for (int i = 0; i < len; ++i) {
        QChar c = key[i];
        if (!c.isSpace()) {
            nkey.append(c);
        }
    }
    key = nkey;

    // Strip accelerator marker.
    if (mayHaveAcc) {
        key = removeAcceleratorMarker(key);
    }

    // Convert to lower case.
    key = key.toLower();

    return key.toUtf8();
}

// ----------------------------------------------------------------------
// Trim multiline string in a "smart" way:
// Remove leading and trailing whitespace up to and including first
// newline from that side, if there is one; otherwise, don't touch.
QString trimSmart (const QString &raw)
{
    // NOTE: This could be done by a single regex, but is not due to
    // performance reasons.
    // This function may potentially be called thousands of times
    // on application startup.

    int len = raw.length();

    int is = 0;
    while (is < len && raw[is].isSpace() && raw[is] != QLatin1Char('\n')) {
        ++is;
    }
    if (is >= len || raw[is] != QLatin1Char('\n')) {
        is = -1;
    }

    int ie = len - 1;
    while (ie >= 0 && raw[ie].isSpace() && raw[ie] != QLatin1Char('\n')) {
        --ie;
    }
    if (ie < 0 || raw[ie] != QLatin1Char('\n')) {
        ie = len;
    }

    return raw.mid(is + 1, ie - is - 1);
}

// ----------------------------------------------------------------------
// Produce a JavaScript object out of Qt variant.
JSValue *variantToJsValue (const QVariant &val)
{
    QVariant::Type vtype = val.type();
    if (vtype == QVariant::String)
        return jsString(val.toString());
    else if (   vtype == QVariant::Double \
             || vtype == QVariant::Int || vtype == QVariant::UInt \
             || vtype == QVariant::LongLong || vtype == QVariant::ULongLong)
        return jsNumber(val.toDouble());
    else
        return jsUndefined();
}

// ----------------------------------------------------------------------
// Parse ini-style config file,
// returning content as hash of hashes by group and key.
// Parsing is not fussy, it will read what it can.
TsConfig readConfig (const QString &fname)
{
    TsConfig config;
    // Add empty group.
    TsConfig::iterator configGroup;
    configGroup = config.insert(QString(), TsConfigGroup());

    QFile file(fname);
    if (!file.open(QIODevice::ReadOnly)) {
        return config;
    }
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        int p1, p2;

        // Remove comment from the line.
        p1 = line.indexOf(QLatin1Char('#'));
        if (p1 >= 0) {
            line = line.left(p1);
        }
        line = line.trimmed();
        if (line.isEmpty()) {
            continue;
        }

        if (line[0] == QLatin1Char('[')) {
            // Group switch.
            p1 = 0;
            p2 = line.indexOf(QLatin1Char(']'), p1 + 1);
            if (p2 < 0) {
                continue;
            }
            QString group = line.mid(p1 + 1, p2 - p1 - 1).trimmed();
            configGroup = config.find(group);
            if (configGroup == config.end()) {
                // Add new group.
                configGroup = config.insert(group, TsConfigGroup());
            }
        } else {
            // Field.
            p1 = line.indexOf(QLatin1Char('='));
            if (p1 < 0) {
                continue;
            }
            QString field = line.left(p1).trimmed();
            QString value = line.mid(p1 + 1).trimmed();
            if (!field.isEmpty()) {
                (*configGroup)[field] = value;
            }
        }
    }
    file.close();

    return config;
}

// ----------------------------------------------------------------------
// Dynamic loading.
Q_GLOBAL_STATIC(KTranscriptImp, globalKTI)
extern "C"
{
    KI18N_EXPORT KTranscript *load_transcript ()
    {
        return globalKTI();
    }
}

// ----------------------------------------------------------------------
// KTranscript definitions.

KTranscriptImp::KTranscriptImp ()
{
    // Load user configuration.
    const QString tsConfigPath = QDir::homePath() + QLatin1Char('/') + QLatin1String(".transcriptrc");
    config = readConfig(tsConfigPath);
}

KTranscriptImp::~KTranscriptImp ()
{
    // FIXME: vallgrind shows an afwul lot of "invalid read" in WTF:: stuff
    // when deref is called... Are we leaking somewhere?
    //foreach (Scriptface *sface, m_sface.values())
    //    sface->jsi->deref();
}

QString KTranscriptImp::eval (const QList<QVariant> &argv,
                              const QString &lang,
                              const QString &ctry,
                              const QString &msgctxt,
                              const QHash<QString, QString> &dynctxt,
                              const QString &msgid,
                              const QStringList &subs,
                              const QList<QVariant> &vals,
                              const QString &ftrans,
                              QList<QStringList> &mods,
                              QString &error,
                              bool &fallback)
{
    //error = "debug"; return QString();

    error.clear(); // empty error message means successful evaluation
    fallback = false; // fallback not requested

    #if 0
    // FIXME: Maybe not needed, as KJS has no native outside access?
    // Unportable (needs unistd.h)?

    // If effective user id is root and real user id is not root.
    if (geteuid() == 0 && getuid() != 0)
    {
        // Since scripts are user input, and the program is running with
        // root permissions while real user is not root, do not invoke
        // scripting at all, to prevent exploits.
        error = "Security block: trying to execute a script in suid environment.";
        return QString();
    }
    #endif

    // Load any new modules and clear the list.
    if (!mods.isEmpty())
    {
        loadModules(mods, error);
        mods.clear();
        if (!error.isEmpty())
            return QString();
    }

    // Add interpreters for new languages.
    // (though it should never happen here, but earlier when loading modules;
    // this also means there are no calls set, so the unregistered call error
    // below will be reported).
    if (!m_sface.contains(lang))
        setupInterpreter(lang);

    // Shortcuts.
    Scriptface *sface = m_sface[lang];
    ExecState *exec = sface->jsi->globalExec();
    JSObject *gobj = sface->jsi->globalObject();

    // Link current message data for script-side interface.
    sface->msgctxt = &msgctxt;
    sface->dynctxt = &dynctxt;
    sface->msgid = &msgid;
    sface->subs = &subs;
    sface->vals = &vals;
    sface->ftrans = &ftrans;
    sface->fallback = &fallback;
    sface->ctry = &ctry;

    // Find corresponding JS function.
    int argc = argv.size();
    if (argc < 1)
    {
        //error = "At least the call name must be supplied.";
        // Empty interpolation is OK, possibly used just to initialize
        // at a given point (e.g. for Ts.setForall() to start having effect).
        return QString();
    }
    QString funcName = argv[0].toString();
    if (!sface->funcs.contains(funcName))
    {
        error = QString::fromLatin1("Unregistered call to '%1'.").arg(funcName);
        return QString();
    }
    JSObject *func = sface->funcs[funcName];
    JSValue *fval = sface->fvals[funcName];

    // Recover module path from the time of definition of this call,
    // for possible load calls.
    currentModulePath = sface->fpaths[funcName];

    // Execute function.
    List arglist;
    for (int i = 1; i < argc; ++i)
        arglist.append(variantToJsValue(argv[i]));
    JSValue *val;
    if (fval->isObject())
        val = func->callAsFunction(exec, fval->getObject(), arglist);
    else // no object associated to this function, use global
        val = func->callAsFunction(exec, gobj, arglist);

    if (fallback)
    // Fallback to ordinary translation requested.
    {
        // Possibly clear exception state.
        if (exec->hadException())
            exec->clearException();

        return QString();
    }
    else if (!exec->hadException())
    // Evaluation successful.
    {
        if (val->isString())
        // Good to go.
        {
            return val->getString().qstring();
        }
        else
        // Accept only strings.
        {
            QString strval = val->toString(exec).qstring();
            error = QString::fromLatin1("Non-string return value: %1").arg(strval);
            return QString();
        }
    }
    else
    // Exception raised.
    {
        error = expt2str(exec);

        exec->clearException();

        return QString();
    }
}

QStringList KTranscriptImp::postCalls (const QString &lang)
{
    // Return no calls if scripting was not already set up for this language.
    // NOTE: This shouldn't happen, as postCalls cannot be called in such case.
    if (!m_sface.contains(lang))
        return QStringList();

    // Shortcuts.
    Scriptface *sface = m_sface[lang];

    return sface->nameForalls;
}

void KTranscriptImp::loadModules (const QList<QStringList> &mods,
                                  QString &error)
{
    QList<QString> modErrors;

    foreach (const QStringList &mod, mods)
    {
        QString mpath = mod[0];
        QString mlang = mod[1];

        // Add interpreters for new languages.
        if (!m_sface.contains(mlang))
            setupInterpreter(mlang);

        // Setup current module path for loading submodules.
        // (sort of closure over invocations of loadf)
        int posls = mpath.lastIndexOf(QLatin1Char('/'));
        if (posls < 1)
        {
            modErrors.append(QString::fromLatin1(
                "Funny module path '%1', skipping.").arg(mpath));
            continue;
        }
        currentModulePath = mpath.left(posls);
        QString fname = mpath.mid(posls + 1);
        // Scriptface::loadf() wants no extension on the filename
        fname = fname.left(fname.lastIndexOf(QLatin1Char('.')));

        // Load the module.
        ExecState *exec = m_sface[mlang]->jsi->globalExec();
        List alist;
        alist.append(jsString(fname));

        m_sface[mlang]->loadf(exec, alist);

        // Handle any exception.
        if (exec->hadException())
        {
            modErrors.append(expt2str(exec));
            exec->clearException();
        }
    }

    // Unset module path.
    currentModulePath.clear();

    foreach (const QString &merr, modErrors)
        error.append(merr + QLatin1Char('\n'));
}

KJS_QT_UNICODE_IMPL

#define SFNAME "Ts"
void KTranscriptImp::setupInterpreter (const QString &lang)
{
    // Create new interpreter.
    Interpreter *jsi = new Interpreter;
    KJS_QT_UNICODE_SET;
    jsi->initGlobalObject();
    jsi->ref();

    // Add scripting interface into the interpreter.
    // NOTE: Config may not contain an entry for the language, in which case
    // it is automatically constructed as an empty hash. This is intended.
    Scriptface *sface = new Scriptface(jsi->globalExec(), config[lang]);
    jsi->globalObject()->put(jsi->globalExec(), SFNAME, sface,
                             DontDelete|ReadOnly);

    // Store scriptface and link to its interpreter.
    sface->jsi = jsi;
    m_sface[lang] = sface;

    //dbgout("=====> Created interpreter for '%1'", lang);
}

// ----------------------------------------------------------------------
// Scriptface internal mechanics.
#include "ktranscript.lut.h"

/* Source for ScriptfaceProtoTable.
@begin ScriptfaceProtoTable 2
    load            Scriptface::Load            DontDelete|ReadOnly|Function 0
    setcall         Scriptface::Setcall         DontDelete|ReadOnly|Function 3
    hascall         Scriptface::Hascall         DontDelete|ReadOnly|Function 1
    acall           Scriptface::Acall           DontDelete|ReadOnly|Function 0
    setcallForall   Scriptface::SetcallForall   DontDelete|ReadOnly|Function 3
    fallback        Scriptface::Fallback        DontDelete|ReadOnly|Function 0
    nsubs           Scriptface::Nsubs           DontDelete|ReadOnly|Function 0
    subs            Scriptface::Subs            DontDelete|ReadOnly|Function 1
    vals            Scriptface::Vals            DontDelete|ReadOnly|Function 1
    msgctxt         Scriptface::Msgctxt         DontDelete|ReadOnly|Function 0
    dynctxt         Scriptface::Dynctxt         DontDelete|ReadOnly|Function 1
    msgid           Scriptface::Msgid           DontDelete|ReadOnly|Function 0
    msgkey          Scriptface::Msgkey          DontDelete|ReadOnly|Function 0
    msgstrf         Scriptface::Msgstrf         DontDelete|ReadOnly|Function 0
    dbgputs         Scriptface::Dbgputs         DontDelete|ReadOnly|Function 1
    warnputs        Scriptface::Warnputs        DontDelete|ReadOnly|Function 1
    localeCountry   Scriptface::LocaleCountry   DontDelete|ReadOnly|Function 0
    normKey         Scriptface::NormKey         DontDelete|ReadOnly|Function 1
    loadProps       Scriptface::LoadProps       DontDelete|ReadOnly|Function 0
    getProp         Scriptface::GetProp         DontDelete|ReadOnly|Function 2
    setProp         Scriptface::SetProp         DontDelete|ReadOnly|Function 3
    toUpperFirst    Scriptface::ToUpperFirst    DontDelete|ReadOnly|Function 2
    toLowerFirst    Scriptface::ToLowerFirst    DontDelete|ReadOnly|Function 2
    getConfString   Scriptface::GetConfString   DontDelete|ReadOnly|Function 2
    getConfBool     Scriptface::GetConfBool     DontDelete|ReadOnly|Function 2
    getConfNumber   Scriptface::GetConfNumber   DontDelete|ReadOnly|Function 2
@end
*/
/* Source for ScriptfaceTable.
@begin ScriptfaceTable 0
@end
*/

KJS_DEFINE_PROTOTYPE(ScriptfaceProto)
KJS_IMPLEMENT_PROTOFUNC(ScriptfaceProtoFunc)
KJS_IMPLEMENT_PROTOTYPE("Scriptface", ScriptfaceProto, ScriptfaceProtoFunc, ObjectPrototype)

const ClassInfo Scriptface::info = {"Scriptface", 0, &ScriptfaceTable, 0};

Scriptface::Scriptface (ExecState *exec, const TsConfigGroup &config_)
: JSObject(ScriptfaceProto::self(exec)), fallback(NULL), config(config_)
{}

Scriptface::~Scriptface ()
{
    qDeleteAll(loadedPmapHandles);
}

bool Scriptface::getOwnPropertySlot (ExecState *exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<Scriptface, JSObject>(exec, &ScriptfaceTable, this, propertyName, slot);
}

JSValue *Scriptface::getValueProperty (ExecState * /*exec*/, int token) const
{
    switch (token) {
        default:
            dbgout("Scriptface::getValueProperty: Unknown property id %1", token);
    }
    return jsUndefined();
}

void Scriptface::put (ExecState *exec, const Identifier &propertyName, JSValue *value, int attr)
{
    lookupPut<Scriptface, JSObject>(exec, propertyName, value, attr, &ScriptfaceTable, this);
}

void Scriptface::putValueProperty (ExecState * /*exec*/, int token, JSValue * /*value*/, int /*attr*/)
{
    switch(token) {
        default:
            dbgout("Scriptface::putValueProperty: Unknown property id %1", token);
    }
}

#define CALLARG(i) (args.size() > i ? args[i] : jsNull())
JSValue *ScriptfaceProtoFunc::callAsFunction (ExecState *exec, JSObject *thisObj, const List &args)
{
    if (!thisObj->inherits(&Scriptface::info)) {
        return throwError(exec, TypeError);
    }
    Scriptface *obj = static_cast<Scriptface*>(thisObj);
    switch (id) {
        case Scriptface::Load:
            return obj->loadf(exec, args);
        case Scriptface::Setcall:
            return obj->setcallf(exec, CALLARG(0), CALLARG(1), CALLARG(2));
        case Scriptface::Hascall:
            return obj->hascallf(exec, CALLARG(0));
        case Scriptface::Acall:
            return obj->acallf(exec, args);
        case Scriptface::SetcallForall:
            return obj->setcallForallf(exec, CALLARG(0), CALLARG(1), CALLARG(2));
        case Scriptface::Fallback:
            return obj->fallbackf(exec);
        case Scriptface::Nsubs:
            return obj->nsubsf(exec);
        case Scriptface::Subs:
            return obj->subsf(exec, CALLARG(0));
        case Scriptface::Vals:
            return obj->valsf(exec, CALLARG(0));
        case Scriptface::Msgctxt:
            return obj->msgctxtf(exec);
        case Scriptface::Dynctxt:
            return obj->dynctxtf(exec, CALLARG(0));
        case Scriptface::Msgid:
            return obj->msgidf(exec);
        case Scriptface::Msgkey:
            return obj->msgkeyf(exec);
        case Scriptface::Msgstrf:
            return obj->msgstrff(exec);
        case Scriptface::Dbgputs:
            return obj->dbgputsf(exec, CALLARG(0));
        case Scriptface::Warnputs:
            return obj->warnputsf(exec, CALLARG(0));
        case Scriptface::LocaleCountry:
            return obj->localeCountryf(exec);
        case Scriptface::NormKey:
            return obj->normKeyf(exec, CALLARG(0));
        case Scriptface::LoadProps:
            return obj->loadPropsf(exec, args);
        case Scriptface::GetProp:
            return obj->getPropf(exec, CALLARG(0), CALLARG(1));
        case Scriptface::SetProp:
            return obj->setPropf(exec, CALLARG(0), CALLARG(1), CALLARG(2));
        case Scriptface::ToUpperFirst:
            return obj->toUpperFirstf(exec, CALLARG(0), CALLARG(1));
        case Scriptface::ToLowerFirst:
            return obj->toLowerFirstf(exec, CALLARG(0), CALLARG(1));
        case Scriptface::GetConfString:
            return obj->getConfStringf(exec, CALLARG(0), CALLARG(1));
        case Scriptface::GetConfBool:
            return obj->getConfBoolf(exec, CALLARG(0), CALLARG(1));
        case Scriptface::GetConfNumber:
            return obj->getConfNumberf(exec, CALLARG(0), CALLARG(1));
        default:
            return jsUndefined();
    }
}

// ----------------------------------------------------------------------
// Scriptface interface functions.
#define SPREF SFNAME"."

JSValue *Scriptface::loadf (ExecState *exec, const List &fnames)
{
    if (globalKTI()->currentModulePath.isEmpty())
        return throwError(exec, GeneralError,
                          SPREF"load: no current module path, aiiie...");

    for (int i = 0; i < fnames.size(); ++i)
        if (!fnames[i]->isString())
            return throwError(exec, TypeError,
                              SPREF"load: expected string as file name");

    for (int i = 0; i < fnames.size(); ++i)
    {
        QString qfname = fnames[i]->getString().qstring();
        QString qfpath = globalKTI()->currentModulePath + QLatin1Char('/') + qfname + QLatin1String(".js");

        QFile file(qfpath);
        if (!file.open(QIODevice::ReadOnly))
            return throwError(exec, GeneralError,
                              QString::fromLatin1(SPREF"load: cannot read file '%1'") \
                                     .arg(qfpath));

        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        QString source = stream.readAll();
        file.close();

        Completion comp = jsi->evaluate(qfpath, 0, source);

        if (comp.complType() == Throw)
        {
            JSValue *exval = comp.value();
            ExecState *exec = jsi->globalExec();
            QString msg = exval->toString(exec).qstring();

            QString line;
            if (exval->type() == ObjectType)
            {
                JSValue *lval = exval->getObject()->get(exec, "line");
                if (lval->type() == NumberType)
                    line = QString::number(lval->toInt32(exec));
            }

            return throwError(exec, TypeError,
                              QString::fromLatin1("at %1:%2: %3")
                                     .arg(qfpath, line, msg));
        }
        dbgout("Loaded module: %1", qfpath);
    }

    return jsUndefined();
}

JSValue *Scriptface::setcallf (ExecState *exec, JSValue *name,
                               JSValue *func, JSValue *fval)
{
    if (!name->isString())
        return throwError(exec, TypeError,
                          SPREF"setcall: expected string as first argument");
    if (   !func->isObject()
        || !func->getObject()->implementsCall())
        return throwError(exec, TypeError,
                          SPREF"setcall: expected function as second argument");
    if (!(fval->isObject() || fval->isNull()))
        return throwError(exec, TypeError,
                          SPREF"setcall: expected object or null as third argument");

    QString qname = name->toString(exec).qstring();
    funcs[qname] = func->getObject();
    fvals[qname] = fval;

    // Register values to keep GC from collecting them. Is this needed?
    put(exec, Identifier(QString::fromLatin1("#:f<%1>").arg(qname)), func, Internal);
    put(exec, Identifier(QString::fromLatin1("#:o<%1>").arg(qname)), fval, Internal);

    // Set current module path as module path for this call,
    // in case it contains load subcalls.
    fpaths[qname] = globalKTI()->currentModulePath;

    return jsUndefined();
}

JSValue *Scriptface::hascallf (ExecState *exec, JSValue *name)
{
    if (!name->isString())
        return throwError(exec, TypeError,
                          SPREF"hascall: expected string as first argument");

    QString qname = name->toString(exec).qstring();
    return jsBoolean(funcs.contains(qname));
}

JSValue *Scriptface::acallf (ExecState *exec, const List &argv)
{
    if (argv.size() < 1) {
        return throwError(exec, SyntaxError,
                          SPREF"acall: expected at least one argument (call name)");
    }
    if (!argv[0]->isString()) {
        return throwError(exec, SyntaxError,
                          SPREF"acall: expected string as first argument (call name)");
    }

    // Get the function and its context object.
    QString callname = argv[0]->getString().qstring();
    if (!funcs.contains(callname)) {
        return throwError(exec, EvalError,
                          QString::fromLatin1(SPREF"acall: unregistered call to '%1'").arg(callname));
    }
    JSObject *func = funcs[callname];
    JSValue *fval = fvals[callname];

    // Recover module path from the time of definition of this call,
    // for possible load calls.
    globalKTI()->currentModulePath = fpaths[callname];

    // Execute function.
    List arglist;
    for (int i = 1; i < argv.size(); ++i)
        arglist.append(argv[i]);
    JSValue *val;
    if (fval->isObject()) {
        // Call function with the context object.
        val = func->callAsFunction(exec, fval->getObject(), arglist);
    }
    else {
        // No context object associated to this function, use global.
        val = func->callAsFunction(exec, jsi->globalObject(), arglist);
    }
    return val;
}

JSValue *Scriptface::setcallForallf (ExecState *exec, JSValue *name,
                                     JSValue *func, JSValue *fval)
{
    if (!name->isString())
        return throwError(exec, TypeError,
                          SPREF"setcallForall: expected string as first argument");
    if (   !func->isObject()
        || !func->getObject()->implementsCall())
        return throwError(exec, TypeError,
                          SPREF"setcallForall: expected function as second argument");
    if (!(fval->isObject() || fval->isNull()))
        return throwError(exec, TypeError,
                          SPREF"setcallForall: expected object or null as third argument");

    QString qname = name->toString(exec).qstring();
    funcs[qname] = func->getObject();
    fvals[qname] = fval;

    // Register values to keep GC from collecting them. Is this needed?
    put(exec, Identifier(QString::fromLatin1("#:fall<%1>").arg(qname)), func, Internal);
    put(exec, Identifier(QString::fromLatin1("#:oall<%1>").arg(qname)), fval, Internal);

    // Set current module path as module path for this call,
    // in case it contains load subcalls.
    fpaths[qname] = globalKTI()->currentModulePath;

    // Put in the queue order for execution on all messages.
    nameForalls.append(qname);

    return jsUndefined();
}

JSValue *Scriptface::fallbackf (ExecState *exec)
{
    Q_UNUSED(exec);
    if (fallback != NULL)
        *fallback = true;
    return jsUndefined();
}

JSValue *Scriptface::nsubsf (ExecState *exec)
{
    Q_UNUSED(exec);
    return jsNumber(subs->size());
}

JSValue *Scriptface::subsf (ExecState *exec, JSValue *index)
{
    if (!index->isNumber())
        return throwError(exec, TypeError,
                          SPREF"subs: expected number as first argument");

    int i = qRound(index->getNumber());
    if (i < 0 || i >= subs->size())
        return throwError(exec, RangeError,
                          SPREF"subs: index out of range");

    return jsString(subs->at(i));
}

JSValue *Scriptface::valsf (ExecState *exec, JSValue *index)
{
    if (!index->isNumber())
        return throwError(exec, TypeError,
                          SPREF"vals: expected number as first argument");

    int i = qRound(index->getNumber());
    if (i < 0 || i >= vals->size())
        return throwError(exec, RangeError,
                          SPREF"vals: index out of range");

    return variantToJsValue(vals->at(i));
}

JSValue *Scriptface::msgctxtf (ExecState *exec)
{
    Q_UNUSED(exec);
    return jsString(*msgctxt);
}

JSValue *Scriptface::dynctxtf (ExecState *exec, JSValue *key)
{
    if (!key->isString())
        return throwError(exec, TypeError,
                          SPREF"dynctxt: expected string as first argument");

    QString qkey = key->getString().qstring();
    if (dynctxt->contains(qkey)) {
        return jsString(dynctxt->value(qkey));
    }
    return jsUndefined();
}

JSValue *Scriptface::msgidf (ExecState *exec)
{
    Q_UNUSED(exec);
    return jsString(*msgid);
}

JSValue *Scriptface::msgkeyf (ExecState *exec)
{
    Q_UNUSED(exec);
    return jsString(QString(*msgctxt + QLatin1Char('|') + *msgid));
}

JSValue *Scriptface::msgstrff (ExecState *exec)
{
    Q_UNUSED(exec);
    return jsString(*ftrans);
}

JSValue *Scriptface::dbgputsf (ExecState *exec, JSValue *str)
{
    if (!str->isString())
        return throwError(exec, TypeError,
                          SPREF"dbgputs: expected string as first argument");

    QString qstr = str->getString().qstring();

    dbgout("[JS-debug] %1", qstr);

    return jsUndefined();
}

JSValue *Scriptface::warnputsf (ExecState *exec, JSValue *str)
{
    if (!str->isString())
        return throwError(exec, TypeError,
                          SPREF"warnputs: expected string as first argument");

    QString qstr = str->getString().qstring();

    warnout("[JS-warning] %1", qstr);

    return jsUndefined();
}

JSValue *Scriptface::localeCountryf (ExecState *exec)
{
    Q_UNUSED(exec);
    return jsString(*ctry);
}

JSValue *Scriptface::normKeyf (ExecState *exec, JSValue *phrase)
{
    if (!phrase->isString()) {
        return throwError(exec, TypeError,
                          SPREF"normKey: expected string as argument");
    }

    QByteArray nqphrase = normKeystr(phrase->toString(exec).qstring());
    return jsString(QString::fromUtf8(nqphrase));
}

JSValue *Scriptface::loadPropsf (ExecState *exec, const List &fnames)
{
    if (globalKTI()->currentModulePath.isEmpty()) {
        return throwError(exec, GeneralError,
                          SPREF"loadProps: no current module path, aiiie...");
    }

    for (int i = 0; i < fnames.size(); ++i) {
        if (!fnames[i]->isString()) {
            return throwError(exec, TypeError,
                              SPREF"loadProps: expected string as file name");
        }
    }

    for (int i = 0; i < fnames.size(); ++i)
    {
        QString qfname = fnames[i]->getString().qstring();
        QString qfpath_base = globalKTI()->currentModulePath + QLatin1Char('/') + qfname;

        // Determine which kind of map is available.
        // Give preference to compiled map.
        QString qfpath = qfpath_base + QLatin1String(".pmapc");
        bool haveCompiled = true;
        QFile file_check(qfpath);
        if (!file_check.open(QIODevice::ReadOnly)) {
            haveCompiled = false;
            qfpath = qfpath_base + QLatin1String(".pmap");
            QFile file_check(qfpath);
            if (!file_check.open(QIODevice::ReadOnly)) {
                return throwError(exec, GeneralError,
                                  QString::fromLatin1(SPREF"loadProps: cannot read map '%1'")
                                     .arg(qfpath_base));
            }
        }
        file_check.close();

        // Load from appropriate type of map.
        if (!loadedPmapPaths.contains(qfpath)) {
            QString errorString;
            if (haveCompiled) {
                errorString = loadProps_bin(qfpath);
            }
            else {
                errorString = loadProps_text(qfpath);
            }
            if (!errorString.isEmpty()) {
                return throwError(exec, SyntaxError, errorString);
            }
            dbgout("Loaded property map: %1", qfpath);
            loadedPmapPaths.insert(qfpath);
        }
    }

    return jsUndefined();
}

JSValue *Scriptface::getPropf (ExecState *exec, JSValue *phrase, JSValue *prop)
{
    if (!phrase->isString()) {
        return throwError(exec, TypeError,
                          SPREF"getProp: expected string as first argument");
    }
    if (!prop->isString()) {
        return throwError(exec, TypeError,
                          SPREF"getProp: expected string as second argument");
    }

    QByteArray qphrase = normKeystr(phrase->toString(exec).qstring());
    QHash<QByteArray, QByteArray> props = phraseProps.value(qphrase);
    if (props.isEmpty()) {
        props = resolveUnparsedProps(qphrase);
    }
    if (!props.isEmpty()) {
        QByteArray qprop = normKeystr(prop->toString(exec).qstring());
        QByteArray qval = props.value(qprop);
        if (!qval.isEmpty()) {
            return jsString(QString::fromUtf8(qval));
        }
    }
    return jsUndefined();
}

JSValue *Scriptface::setPropf (ExecState *exec, JSValue *phrase, JSValue *prop, JSValue *value)
{
    if (!phrase->isString()) {
        return throwError(exec, TypeError,
                          SPREF"setProp: expected string as first argument");
    }
    if (!prop->isString()) {
        return throwError(exec, TypeError,
                          SPREF"setProp: expected string as second argument");
    }
    if (!value->isString()) {
        return throwError(exec, TypeError,
                          SPREF"setProp: expected string as third argument");
    }

    QByteArray qphrase = normKeystr(phrase->toString(exec).qstring());
    QByteArray qprop = normKeystr(prop->toString(exec).qstring());
    QByteArray qvalue = value->toString(exec).qstring().toUtf8();
    // Any non-existent key in first or second-level hash will be created.
    phraseProps[qphrase][qprop] = qvalue;
    return jsUndefined();
}

static QString toCaseFirst (const QString &qstr, int qnalt, bool toupper)
{
    static const QLatin1String head("~@");
    static const int hlen = 2; //head.length()

    // If the first letter is found within an alternatives directive,
    // change case of the first letter in each of the alternatives.
    QString qstrcc = qstr;
    int len = qstr.length();
    QChar altSep;
    int remainingAlts = 0;
    bool checkCase = true;
    int numChcased = 0;
    int i = 0;
    while (i < len) {
        QChar c = qstr[i];

        if (qnalt && !remainingAlts && qstr.mid(i, hlen) == head) {
            // An alternatives directive is just starting.
            i += 2;
            if (i >= len) break; // malformed directive, bail out
            // Record alternatives separator, set number of remaining
            // alternatives, reactivate case checking.
            altSep = qstrcc[i];
            remainingAlts = qnalt;
            checkCase = true;
        }
        else if (remainingAlts && c == altSep) {
            // Alternative separator found, reduce number of remaining
            // alternatives and reactivate case checking.
            --remainingAlts;
            checkCase = true;
        }
        else if (checkCase && c.isLetter()) {
            // Case check is active and the character is a letter; change case.
            if (toupper) {
                qstrcc[i] = c.toUpper();
            } else {
                qstrcc[i] = c.toLower();
            }
            ++numChcased;
            // No more case checks until next alternatives separator.
            checkCase = false;
        }

        // If any letter has been changed, and there are no more alternatives
        // to be processed, we're done.
        if (numChcased > 0 && remainingAlts == 0) {
            break;
        }

        // Go to next character.
        ++i;
    }

    return qstrcc;
}

JSValue *Scriptface::toUpperFirstf (ExecState *exec,
                                    JSValue *str, JSValue *nalt)
{
    if (!str->isString()) {
        return throwError(exec, TypeError,
                          SPREF"toUpperFirst: expected string as first argument");
    }
    if (!(nalt->isNumber() || nalt->isNull())) {
        return throwError(exec, TypeError,
                          SPREF"toUpperFirst: expected number as second argument");
    }

    QString qstr = str->toString(exec).qstring();
    int qnalt = nalt->isNull() ? 0 : nalt->toInteger(exec);

    QString qstruc = toCaseFirst(qstr, qnalt, true);

    return jsString(qstruc);
}

JSValue *Scriptface::toLowerFirstf (ExecState *exec,
                                    JSValue *str, JSValue *nalt)
{
    if (!str->isString()) {
        return throwError(exec, TypeError,
                          SPREF"toLowerFirst: expected string as first argument");
    }
    if (!(nalt->isNumber() || nalt->isNull())) {
        return throwError(exec, TypeError,
                          SPREF"toLowerFirst: expected number as second argument");
    }

    QString qstr = str->toString(exec).qstring();
    int qnalt = nalt->isNull() ? 0 : nalt->toInteger(exec);

    QString qstrlc = toCaseFirst(qstr, qnalt, false);

    return jsString(qstrlc);
}

JSValue *Scriptface::getConfStringf (ExecState *exec,
                                     JSValue *key, JSValue *dval)
{
    if (!key->isString()) {
        return throwError(exec, TypeError,
                          SPREF"getConfString: expected string "
                          "as first argument");
    }
    if (!(dval->isString() || dval->isNull())) {
        return throwError(exec, TypeError,
                          SPREF"getConfString: expected string "
                          "as second argument (when given)");
    }

    if (dval->isNull()) {
        dval = jsUndefined();
    }

    QString qkey = key->getString().qstring();
    if (config.contains(qkey)) {
        return jsString(config.value(qkey));
    }

    return dval;
}

JSValue *Scriptface::getConfBoolf (ExecState *exec,
                                   JSValue *key, JSValue *dval)
{
    if (!key->isString()) {
        return throwError(exec, TypeError,
                          SPREF"getConfBool: expected string as "
                          "first argument");
    }
    if (!(dval->isBoolean() || dval->isNull())) {
        return throwError(exec, TypeError,
                          SPREF"getConfBool: expected boolean "
                          "as second argument (when given)");
    }

    static QStringList falsities;
    if (falsities.isEmpty()) {
        falsities.append(QString(QLatin1Char('0')));
        falsities.append(QString::fromLatin1("no"));
        falsities.append(QString::fromLatin1("false"));
    }

    if (dval->isNull()) {
        dval = jsUndefined();
    }

    QString qkey = key->getString().qstring();
    if (config.contains(qkey)) {
        QString qval = config.value(qkey).toLower();
        return jsBoolean(!falsities.contains(qval));
    }

    return dval;
}

JSValue *Scriptface::getConfNumberf (ExecState *exec,
                                     JSValue *key, JSValue *dval)
{
    if (!key->isString()) {
        return throwError(exec, TypeError,
                          SPREF"getConfNumber: expected string "
                          "as first argument");
    }
    if (!(dval->isNumber() || dval->isNull())) {
        return throwError(exec, TypeError,
                          SPREF"getConfNumber: expected number "
                          "as second argument (when given)");
    }

    if (dval->isNull()) {
        dval = jsUndefined();
    }

    QString qkey = key->getString().qstring();
    if (config.contains(qkey)) {
        QString qval = config.value(qkey);
        bool convOk;
        double qnum = qval.toDouble(&convOk);
        if (convOk) {
            return jsNumber(qnum);
        }
    }

    return dval;
}

// ----------------------------------------------------------------------
// Scriptface helpers to interface functions.

QString Scriptface::loadProps_text (const QString &fpath)
{
    QFile file(fpath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QString::fromLatin1(SPREF"loadProps_text: cannot read file '%1'")
                      .arg(fpath);
    }
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    QString s = stream.readAll();
    file.close();

    // Parse the map.
    // Should care about performance: possibly executed on each KDE
    // app startup and reading houndreds of thousands of characters.
    enum {s_nextEntry, s_nextKey, s_nextValue};
    QList<QByteArray> ekeys; // holds keys for current entry
    QHash<QByteArray, QByteArray> props; // holds properties for current entry
    int slen = s.length();
    int state = s_nextEntry;
    QByteArray pkey;
    QChar prop_sep, key_sep;
    int i = 0;
    while (1) {
        int i_checkpoint = i;

        if (state == s_nextEntry) {
            while (s[i].isSpace()) {
                ++i;
                if (i >= slen) goto END_PROP_PARSE;
            }
            if (i + 1 >= slen) {
                return QString::fromLatin1(SPREF"loadProps_text: unexpected end "
                               "of file in %1").arg(fpath);
            }
            if (s[i] != QLatin1Char('#')) {
                // Separator characters for this entry.
                key_sep = s[i];
                prop_sep = s[i + 1];
                if (key_sep.isLetter() || prop_sep.isLetter()) {
                    return  QString::fromLatin1(SPREF"loadProps_text: separator "
                                    "characters must not be letters at %1:%2")
                                   .arg(fpath).arg(countLines(s, i));
                }

                // Reset all data for current entry.
                ekeys.clear();
                props.clear();
                pkey.clear();

                i += 2;
                state = s_nextKey;
            }
            else {
                // This is a comment, skip to EOL, don't change state.
                while (s[i] != QLatin1Char('\n')) {
                    ++i;
                    if (i >= slen) goto END_PROP_PARSE;
                }
            }
        }
        else if (state == s_nextKey) {
            int ip = i;
            // Proceed up to next key or property separator.
            while (s[i] != key_sep && s[i] != prop_sep) {
                ++i;
                if (i >= slen) goto END_PROP_PARSE;
            }
            if (s[i] == key_sep) {
                // This is a property key,
                // record for when the value gets parsed.
                pkey = normKeystr(s.mid(ip, i - ip), false);

                i += 1;
                state = s_nextValue;
            }
            else { // if (s[i] == prop_sep) {
                // This is an entry key, or end of entry.
                QByteArray ekey = normKeystr(s.mid(ip, i - ip), false);
                if (!ekey.isEmpty()) {
                    // An entry key.
                    ekeys.append(ekey);

                    i += 1;
                    state = s_nextKey;
                }
                else {
                    // End of entry.
                    if (ekeys.size() < 1) {
                        return QString::fromLatin1(SPREF"loadProps_text: no entry key "
                                       "for entry ending at %1:%2")
                                       .arg(fpath).arg(countLines(s, i));
                    }

                    // Add collected entry into global store,
                    // once for each entry key (QHash implicitly shared).
                    foreach (const QByteArray &ekey, ekeys) {
                        phraseProps[ekey] = props;
                    }

                    i += 1;
                    state = s_nextEntry;
                }
            }
        }
        else if (state == s_nextValue) {
            int ip = i;
            // Proceed up to next property separator.
            while (s[i] != prop_sep) {
                ++i;
                if (i >= slen) goto END_PROP_PARSE;
                if (s[i] == key_sep) {
                    return QString::fromLatin1(SPREF"loadProps_text: property separator "
                                   "inside property value at %1:%2")
                                  .arg(fpath).arg(countLines(s, i));
                }
            }
            // Extract the property value and store the property.
            QByteArray pval = trimSmart(s.mid(ip, i - ip)).toUtf8();
            props[pkey] = pval;

            i += 1;
            state = s_nextKey;
        }
        else {
            return QString::fromLatin1(SPREF"loadProps: internal error 10 at %1:%2")
                          .arg(fpath).arg(countLines(s, i));
        }

        // To avoid infinite looping and stepping out.
        if (i == i_checkpoint || i >= slen) {
            return QString::fromLatin1(SPREF"loadProps: internal error 20 at %1:%2")
                          .arg(fpath).arg(countLines(s, i));
        }
    }

    END_PROP_PARSE:

    if (state != s_nextEntry) {
        return QString::fromLatin1(SPREF"loadProps: unexpected end of file in %1")
                      .arg(fpath);
    }

    return QString();
}

// Read big-endian integer of nbytes length at position pos
// in character array fc of length len.
// Update position to point after the number.
// In case of error, pos is set to -1.
template <typename T>
static int bin_read_int_nbytes (const char *fc, qlonglong len, qlonglong &pos, int nbytes)
{
    if (pos + nbytes > len) {
        pos = -1;
        return 0;
    }
    T num = qFromBigEndian<T>((uchar*) fc + pos);
    pos += nbytes;
    return num;
}

// Read 64-bit big-endian integer.
static quint64 bin_read_int64 (const char *fc, qlonglong len, qlonglong &pos)
{
    return bin_read_int_nbytes<quint64>(fc, len, pos, 8);
}

// Read 32-bit big-endian integer.
static quint32 bin_read_int (const char *fc, qlonglong len, qlonglong &pos)
{
    return bin_read_int_nbytes<quint32>(fc, len, pos, 4);
}

// Read string at position pos of character array fc of length n.
// String is represented as 32-bit big-endian byte length followed by bytes.
// Update position to point after the string.
// In case of error, pos is set to -1.
static QByteArray bin_read_string (const char *fc, qlonglong len, qlonglong &pos)
{
    // Binary format stores strings as length followed by byte sequence.
    // No null-termination.
    int nbytes = bin_read_int(fc, len, pos);
    if (pos < 0) {
        return QByteArray();
    }
    if (nbytes < 0 || pos + nbytes > len) {
        pos = -1;
        return QByteArray();
    }
    QByteArray s(fc + pos, nbytes);
    pos += nbytes;
    return s;
}

QString Scriptface::loadProps_bin (const QString &fpath)
{
    QFile file(fpath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QString::fromLatin1(SPREF"loadProps: cannot read file '%1'")
                      .arg(fpath);
    }
    // Collect header.
    QByteArray head(8, '0');
    file.read(head.data(), head.size());
    file.close();

    // Choose pmap loader based on header.
    if (head == "TSPMAP00") {
        return loadProps_bin_00(fpath);
    } else if (head == "TSPMAP01") {
        return loadProps_bin_01(fpath);
    }
    else {
        return QString::fromLatin1(SPREF"loadProps: unknown version of compiled map '%1'")
                      .arg(fpath);
    }
}

QString Scriptface::loadProps_bin_00 (const QString &fpath)
{
    QFile file(fpath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QString::fromLatin1(SPREF"loadProps: cannot read file '%1'")
                      .arg(fpath);
    }
    QByteArray fctmp = file.readAll();
    file.close();
    const char *fc = fctmp.data();
    const int fclen = fctmp.size();

    // Indicates stream state.
    qlonglong pos = 0;

    // Match header.
    QByteArray head(fc, 8);
    pos += 8;
    if (head != "TSPMAP00") goto END_PROP_PARSE;

    // Read total number of entries.
    int nentries;
    nentries = bin_read_int(fc, fclen, pos);
    if (pos < 0) goto END_PROP_PARSE;

    // Read all entries.
    for (int i = 0; i < nentries; ++i) {

        // Read number of entry keys and all entry keys.
        QList<QByteArray> ekeys;
        int nekeys = bin_read_int(fc, fclen, pos);
        if (pos < 0) goto END_PROP_PARSE;
        for (int j = 0; j < nekeys; ++j) {
            QByteArray ekey = bin_read_string(fc, fclen, pos);
            if (pos < 0) goto END_PROP_PARSE;
            ekeys.append(ekey);
        }
        //dbgout("--------> ekey[0]={%1}", QString::fromUtf8(ekeys[0]));

        // Read number of properties and all properties.
        QHash<QByteArray, QByteArray> props;
        int nprops = bin_read_int(fc, fclen, pos);
        if (pos < 0) goto END_PROP_PARSE;
        for (int j = 0; j < nprops; ++j) {
            QByteArray pkey = bin_read_string(fc, fclen, pos);
            if (pos < 0) goto END_PROP_PARSE;
            QByteArray pval = bin_read_string(fc, fclen, pos);
            if (pos < 0) goto END_PROP_PARSE;
            props[pkey] = pval;
        }

        // Add collected entry into global store,
        // once for each entry key (QHash implicitly shared).
        foreach (const QByteArray &ekey, ekeys) {
            phraseProps[ekey] = props;
        }
    }

    END_PROP_PARSE:

    if (pos < 0) {
        return QString::fromLatin1(SPREF"loadProps: corrupt compiled map '%1'")
                      .arg(fpath);
    }

    return QString();
}

QString Scriptface::loadProps_bin_01 (const QString &fpath)
{
    QFile *file = new QFile(fpath);
    if (!file->open(QIODevice::ReadOnly)) {
        return QString::fromLatin1(SPREF"loadProps: cannot read file '%1'")
                      .arg(fpath);
    }

    QByteArray fstr;
    qlonglong pos;

    // Read the header and number and length of entry keys.
    fstr = file->read(8 + 4 + 8);
    pos = 0;
    QByteArray head = fstr.left(8);
    pos += 8;
    if (head != "TSPMAP01") {
        return QString::fromLatin1(SPREF"loadProps: corrupt compiled map '%1'")
                      .arg(fpath);
    }
    quint32 numekeys = bin_read_int(fstr, fstr.size(), pos);
    quint64 lenekeys = bin_read_int64(fstr, fstr.size(), pos);

    // Read entry keys.
    fstr = file->read(lenekeys);
    pos = 0;
    for (quint32 i = 0; i < numekeys; ++i) {
        QByteArray ekey = bin_read_string(fstr, lenekeys, pos);
        quint64 offset = bin_read_int64(fstr, lenekeys, pos);
        phraseUnparsedProps[ekey] = QPair<QFile*, quint64>(file, offset);
    }

    // // Read property keys.
    // ...when it becomes necessary

    loadedPmapHandles.insert(file);
    return QString();
}

QHash<QByteArray, QByteArray> Scriptface::resolveUnparsedProps (const QByteArray &phrase)
{
    QPair<QFile*, quint64> ref = phraseUnparsedProps.value(phrase);
    QFile *file = ref.first;
    quint64 offset = ref.second;
    QHash<QByteArray, QByteArray> props;
    if (file != NULL && file->seek(offset)) {
        QByteArray fstr = file->read(4 + 4);
        qlonglong pos = 0;
        quint32 numpkeys = bin_read_int(fstr, fstr.size(), pos);
        quint32 lenpkeys = bin_read_int(fstr, fstr.size(), pos);
        fstr = file->read(lenpkeys);
        pos = 0;
        for (quint32 i = 0; i < numpkeys; ++i) {
            QByteArray pkey = bin_read_string(fstr, lenpkeys, pos);
            QByteArray pval = bin_read_string(fstr, lenpkeys, pos);
            props[pkey] = pval;
        }
        phraseProps[phrase] = props;
        phraseUnparsedProps.remove(phrase);
    }
    return props;
}
