// This file is part of the KDE project
// SPDX-FileCopyrightText: 1998-2002 The KSpread Team <calligra-devel@kde.org>
// SPDX-FileCopyrightText: 2005 Tomas Mecir <mecirt@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only

// built-in information functions

#include "InformationModule.h"

#ifdef Q_OS_WIN
#include <windows.h>
#else
#include <sys/utsname.h>
#endif

#include "../../calligra-version.h"

#include <KLocalizedString>
#include <QDir>

#include "engine/CalculationSettings.h"
#include "engine/CellBase.h"
#include "engine/CellBaseStorage.h"
#include "engine/Formula.h"
#include "engine/Function.h"
#include "engine/SheetBase.h"
#include "engine/ValueCalc.h"
#include "engine/ValueConverter.h"

using namespace Calligra::Sheets;

// prototypes (sorted alphabetically)
Value func_errortype(valVector args, ValueCalc *calc, FuncExtra *);
Value func_filename(valVector args, ValueCalc *calc, FuncExtra *);
Value func_formula(valVector args, ValueCalc *calc, FuncExtra *);
Value func_info(valVector args, ValueCalc *calc, FuncExtra *);
Value func_isblank(valVector args, ValueCalc *calc, FuncExtra *);
Value func_isdate(valVector args, ValueCalc *calc, FuncExtra *);
Value func_iserr(valVector args, ValueCalc *calc, FuncExtra *);
Value func_iserror(valVector args, ValueCalc *calc, FuncExtra *);
Value func_iseven(valVector args, ValueCalc *calc, FuncExtra *);
Value func_isformula(valVector args, ValueCalc *calc, FuncExtra *);
Value func_islogical(valVector args, ValueCalc *calc, FuncExtra *);
Value func_isna(valVector args, ValueCalc *calc, FuncExtra *);
Value func_isnottext(valVector args, ValueCalc *calc, FuncExtra *);
Value func_isnum(valVector args, ValueCalc *calc, FuncExtra *);
Value func_isodd(valVector args, ValueCalc *calc, FuncExtra *);
Value func_isref(valVector args, ValueCalc *calc, FuncExtra *);
Value func_istext(valVector args, ValueCalc *calc, FuncExtra *);
Value func_istime(valVector args, ValueCalc *calc, FuncExtra *);
Value func_n(valVector args, ValueCalc *calc, FuncExtra *);
Value func_na(valVector args, ValueCalc *calc, FuncExtra *);
Value func_type(valVector args, ValueCalc *calc, FuncExtra *);
Value func_version(valVector args, ValueCalc *calc, FuncExtra *);

CALLIGRA_SHEETS_EXPORT_FUNCTION_MODULE("kspreadinformationmodule.json", InformationModule)

InformationModule::InformationModule(QObject *parent, const QVariantList &)
    : FunctionModule(parent)
{
    Function *f;

    f = new Function("ERRORTYPE", func_errortype);
    add(f);
    f = new Function("FILENAME", func_filename);
    f->setParamCount(0);
    add(f);
    f = new Function("FORMULA", func_formula);
    f->setParamCount(1);
    f->setNeedsExtra(true);
    add(f);
    f = new Function("INFO", func_info);
    add(f);
    f = new Function("ISBLANK", func_isblank);
    add(f);
    f = new Function("ISDATE", func_isdate);
    add(f);
    f = new Function("ISERR", func_iserr);
    add(f);
    f = new Function("ISERROR", func_iserror);
    add(f);
    f = new Function("ISEVEN", func_iseven);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETISEVEN");
    add(f);
    f = new Function("ISFORMULA", func_isformula);
    f->setNeedsExtra(true);
    add(f);
    f = new Function("ISLOGICAL", func_islogical);
    add(f);
    f = new Function("ISNA", func_isna);
    add(f);
    f = new Function("ISNONTEXT", func_isnottext);
    add(f);
    f = new Function("ISNOTTEXT", func_isnottext);
    add(f);
    f = new Function("ISNUM", func_isnum);
    add(f);
    f = new Function("ISNUMBER", func_isnum);
    add(f);
    f = new Function("ISODD", func_isodd);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETISODD");
    add(f);
    f = new Function("ISREF", func_isref);
    f->setNeedsExtra(true);
    f->setAcceptArray();
    add(f);
    f = new Function("ISTEXT", func_istext);
    add(f);
    f = new Function("ISTIME", func_istime);
    add(f);
    f = new Function("N", func_n);
    add(f);
    f = new Function("NA", func_na);
    f->setParamCount(0);
    add(f);
    f = new Function("TYPE", func_type);
    f->setAcceptArray();
    add(f);
}

QString InformationModule::descriptionFileName() const
{
    return QString("information.xml");
}

// Function: ERROR.TYPE
Value func_errortype(valVector args, ValueCalc *, FuncExtra *)
{
    if (!args[0].isError()) {
        // its an error if the argument isn't an error...
        return Value::errorVALUE();
    }

    if (args[0] == Value::errorNULL()) {
        return Value(1);
    } else if (args[0] == Value::errorDIV0()) {
        return Value(2);
    } else if (args[0] == Value::errorVALUE()) {
        return Value(3);
    } else if (args[0] == Value::errorREF()) {
        return Value(4);
    } else if (args[0] == Value::errorNAME()) {
        return Value(5);
    } else if (args[0] == Value::errorNUM()) {
        return Value(6);
    } else if (args[0] == Value::errorNA()) {
        return Value(7);
    } else if (args[0] == Value::errorCIRCLE()) {
        // non-standard error type
        return Value(101);
    } else if (args[0] == Value::errorDEPEND()) {
        // non-standard error type
        return Value(102);
    } else if (args[0] == Value::errorPARSE()) {
        // non-standard error type
        return Value(103);
    } else {
        // something I didn't think of...
        debugSheets << "Unexpected error type";
        return Value(0);
    }
}
// Function: INFO
Value func_info(valVector args, ValueCalc *calc, FuncExtra *)
{
    QString type = calc->conv()->asString(args[0]).asString().toLower();

    if (type == "directory")
        return Value(QDir::currentPath());

    if (type == "release") {
        return Value(CALLIGRA_VERSION_STRING);
    }

    if (type == "numfile") {
        /*
                KoApplication *app = qobject_cast<KoApplication*>(qApp);
                if(! app) {
                   return Value(0);
                } else {

                    QSet<QString> nameList;
                    QList<KoPart*> parts = app->partList();
                    for(KoPart* part : parts) {
                        nameList.insert(part->document()->objectName());
                    }
                    return Value(nameList.size());
                }
        */
        return Value::errorVALUE(); // let's just declare this unsupported for now
    }

    if (type == "recalc") {
        QString result;
        if (!calc->settings()->isAutoCalculationEnabled())
            result = i18n("Manual");
        else
            result = i18n("Automatic");
        return Value(result);
    }

    if (type == "memavail")
        // not supported
        return Value::errorVALUE();
    if (type == "memused")
        // not supported
        return Value::errorVALUE();
    if (type == "origin")
        // not supported
        return Value::errorVALUE();

    if (type == "system") {
#ifndef Q_OS_WIN
        struct utsname name;
        if (uname(&name) >= 0)
            return Value(QString(name.sysname));
#else
        return Value(QString("Windows"));
#endif
    }

    if (type == "totmem")
        // not supported
        return Value::errorVALUE();

    if (type == "osversion") {
#ifndef Q_OS_WIN
        struct utsname name;
        if (uname(&name) >= 0) {
            QString os = QString("%1 %2 (%3)").arg(name.sysname, name.release, name.machine);
            return Value(os);
        }
#else
        OSVERSIONINFO versionInfo;
        SYSTEM_INFO sysInfo;
        QString architecture;

        versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

        GetVersionEx(&versionInfo);
        GetSystemInfo(&sysInfo);

        switch (sysInfo.wProcessorArchitecture) {
        case PROCESSOR_ARCHITECTURE_AMD64:
            architecture = QString("x86_64");
            break;
        case PROCESSOR_ARCHITECTURE_IA64:
            architecture = QString("ia64");
            break;
        case PROCESSOR_ARCHITECTURE_INTEL:
            architecture = QString("x86");
            break;
        default:
            architecture = QString("unknown");
        }

        QString os = QString("Windows %1.%2 (%3)").arg(versionInfo.dwMajorVersion).arg(versionInfo.dwMinorVersion).arg(architecture);

        return Value(os);
#endif
    }

    return Value::errorVALUE();
}

// Function: ISBLANK
Value func_isblank(valVector args, ValueCalc *, FuncExtra *)
{
    return Value(args[0].isEmpty());
}

// Function: ISLOGICAL
Value func_islogical(valVector args, ValueCalc *, FuncExtra *)
{
    return Value(args[0].isBoolean());
}

// Function: ISTEXT
Value func_istext(valVector args, ValueCalc *, FuncExtra *)
{
    return Value(args[0].isString());
}

// Function: ISREF
Value func_isref(valVector args, ValueCalc * /*calc*/, FuncExtra *e)
{
    if (args[0].isError())
        return args[0]; // errors pass through
    // no reference ?
    if ((e == nullptr) || (e->ranges[0].col1 == -1) || (e->ranges[0].row1 == -1))
        return Value(false);
    // if we are here, it is a reference (cell/range)
    return Value(true);
}

// Function: ISNOTTEXT
Value func_isnottext(valVector args, ValueCalc *, FuncExtra *)
{
    return Value(args[0].isString() ? false : true);
}

// Function: ISNUM
Value func_isnum(valVector args, ValueCalc *, FuncExtra *)
{
    return Value(args[0].isNumber());
}

// Function: ISTIME
Value func_istime(valVector args, ValueCalc *, FuncExtra *)
{
    return Value((args[0].format() == Value::fmt_Time) || (args[0].format() == Value::fmt_DateTime));
}

// Function: ISDATE
Value func_isdate(valVector args, ValueCalc *, FuncExtra *)
{
    return Value((args[0].format() == Value::fmt_Date) || (args[0].format() == Value::fmt_DateTime));
}

// Function: ISODD
Value func_isodd(valVector args, ValueCalc *calc, FuncExtra *)
{
    return Value(calc->isEven(args[0]) ? false : true);
}

// Function: ISEVEN
Value func_iseven(valVector args, ValueCalc *calc, FuncExtra *)
{
    if (args[0].isError())
        return args[0];
    return Value(calc->isEven(args[0]));
}

// Function: ISFORMULA
Value func_isformula(valVector args, ValueCalc *calc, FuncExtra *e)
{
    Q_UNUSED(args)
    Q_UNUSED(calc)
    const Calligra::Sheets::Region &region = e->regions[0];
    QPoint p = region.firstRange().topLeft();
    CellBaseStorage *s = region.firstSheet()->cellStorage();
    Formula formula = s->formula(p.x(), p.y());
    return Value(formula.isValid());
}

// Function: ISERR
Value func_iserr(valVector args, ValueCalc *, FuncExtra *)
{
    return Value((args[0].isError() && (args[0].errorMessage() != Value::errorNA().errorMessage())));
}

// Function: ISERROR
Value func_iserror(valVector args, ValueCalc *, FuncExtra *)
{
    return Value(args[0].isError());
}

// Function: ISNA
Value func_isna(valVector args, ValueCalc *, FuncExtra *)
{
    return Value((args[0].isError() && (args[0].errorMessage() == Value::errorNA().errorMessage())));
}

// Function: TYPE
Value func_type(valVector args, ValueCalc *, FuncExtra *)
{
    // Returns 1 for numbers, 2 for text, 4 for boolean, 16 for error,
    // 64 for arrays
    if (args[0].isArray())
        return Value(64);
    if (args[0].isNumber())
        return Value(1);
    if (args[0].isString())
        return Value(2);
    if (args[0].isBoolean())
        return Value(4);
    if (args[0].isError())
        return Value(16);

    // something else ?
    return Value(0);
}

Value func_filename(valVector, ValueCalc *calc, FuncExtra *)
{
    return Value(calc->settings()->fileName());
}

Value func_formula(valVector, ValueCalc *, FuncExtra *e)
{
    if (e->ranges[0].col1 < 1 || e->ranges[0].row1 < 1)
        return Value::errorVALUE();
    const Calligra::Sheets::CellBase c(e->sheet, e->ranges[0].col1, e->ranges[0].row1);
    if (c.isNull())
        return Value::errorVALUE();
    if (!c.isFormula())
        return Value::errorNA();
    return Value(c.formula().expression());
}

// Function: N
Value func_n(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->conv()->asFloat(args[0]);
}

// Function: NA
Value func_na(valVector, ValueCalc *, FuncExtra *)
{
    return Value::errorNA();
}

#include "information.moc"
