
#ifndef ESSTIXFONTSTYLE_H
#define ESSTIXFONTSTYLE_H

#include "fontstyle.h"

KFORMULA_NAMESPACE_BEGIN


class EsstixAlphaTable : public AlphaTable {
public:

    EsstixAlphaTable();

    virtual AlphaTableEntry entry( char pos, CharFamily family, CharStyle style ) const;

private:

    QFont script_font;
    QFont fraktur_font;
    QFont double_struck_font;
};


class EsstixFontStyle : public FontStyle {

    /// lazy init support. Needs to be run before anything else.
    virtual bool init( ContextStyle* context );

    /// this styles name
    virtual QString name();

    /// the table for special alphabets.
    virtual const AlphaTable* alphaTable() const;

private:

    EsstixAlphaTable m_alphaTable;
};


KFORMULA_NAMESPACE_END

#endif
