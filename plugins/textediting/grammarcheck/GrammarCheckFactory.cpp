#include "GrammarCheckFactory.h"
#include "GrammarCheck.h"

#include <KDebug>
#include <KLocale>

GrammarCheckFactory::GrammarCheckFactory()
    : KoTextEditingFactory("grammarcheck")
{
    setShowInMenu(true);
    setTitle(i18n("Check Grammar"));
}

KoTextEditingPlugin *GrammarCheckFactory::create() const
{
    kDebug(31000) << "grammarcheck factory created";
    return new GrammarCheck();
}
