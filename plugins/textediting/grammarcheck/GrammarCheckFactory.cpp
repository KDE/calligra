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
    kDebug() << "grammarcheck plugin not created";
    return new GrammarCheck();
}
