
#ifndef SYMBOLFONTSTYLE_H
#define SYMBOLFONTSTYLE_H

#include "fontstyle.h"

KFORMULA_NAMESPACE_BEGIN


/**
 * The style of the standard symbol font.
 */
class SymbolFontStyle : public FontStyle {

    /// lazy init support. Needs to be run before anything else.
    virtual bool init( ContextStyle* context );

    /// this styles name
    virtual QString name();

};

KFORMULA_NAMESPACE_END

#endif
