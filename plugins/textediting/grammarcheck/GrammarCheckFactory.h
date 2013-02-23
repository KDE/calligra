#ifndef GRAMMARCHECK_FACTORY
#define GRAMMARCHECK_FACTORY

#include <KoTextEditingFactory.h>

class KoTextEditingPlugin;

class GrammarCheckFactory : public KoTextEditingFactory
{
public:
    explicit GrammarCheckFactory();

    KoTextEditingPlugin *create() const;
};

#endif
