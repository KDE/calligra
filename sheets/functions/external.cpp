#include "ExternalModule.h"

#include "Function.h"
#include "FunctionModuleRegistry.h"
#include "ValueCalc.h"
#include "ValueConverter.h"

#include <Formula.h>

#include <kdebug.h>
#include <KLocale>

// needed for MODE
#include <QList>
#include <QMap>

using namespace Calligra::Sheets;

Value func_hyperlink(valVector args, ValueCalc *calc, FuncExtra *e);

typedef QList<double> List;

CALLIGRA_SHEETS_EXPORT_FUNCTION_MODULE("external", ExternalModule)

ExternalModule::ExternalModule(QObject *parent, const QVariantList &)
    : FunctionModule(parent)
{
    Function *f;

    f = new Function("HYPERLINK", func_hyperlink);
    f->setParamCount(2);
    add(f);
}

QString ExternalModule::descriptionFileName() const
{
    return QString("external.xml");
}

//
// Function: HYPERLINK
//
Value func_hyperlink(valVector args, ValueCalc *calc, FuncExtra *e)
{
    const Value link = calc->conv()->asString(args[0]);
    Value displayName;

    if (args.count() < 2)
        displayName = link;
    else
        displayName = calc->conv()->asString(args[1]);

    Calligra::Sheets::Cell c(e->sheet, e->mycol, e->myrow);
    c.setLink(link.asString());

    return displayName;
}

#include "ExternalModule.moc"
