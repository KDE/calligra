
#ifndef FONTSTYLE_H
#define FONTSTYLE_H

#include <qstring.h>
#include <qfont.h>

#include "kformuladefs.h"
#include "symboltable.h"


KFORMULA_NAMESPACE_BEGIN

class AlphaTable;
class ContextStyle;
class SymbolTable;


/**
 * Base class for all supported font styles.
 */
class FontStyle {
public:

    virtual ~FontStyle() {}

    /// lazy init support. Needs to be run before anything else.
    virtual bool init( ContextStyle* context ) = 0;

    /// this styles name
    virtual QString name() = 0;

    /// the table for ordinary symbols (those that have a unicode value)
    virtual const SymbolTable* symbolTable() const { return &m_symbolTable; }
    virtual SymbolTable* symbolTable() { return &m_symbolTable; }

    /// the table for special alphabets.
    virtual const AlphaTable* alphaTable() const { return 0; };

protected:

    bool fontAvailable( QString fontName );

    // This is going to disappear in favour of a real macro facility some day.
    void fillNameTable( SymbolTable::NameTable& names );

private:

    SymbolTable m_symbolTable;
};


/**
 * The information our AlphaTable contains.
 */
class AlphaTableEntry {
public:

    bool valid() const { return pos > -1; }

    QFont font;
    short pos;
};


/**
 * The table for special alphabets.
 */
class AlphaTable {
public:

    enum Family { /*normal,*/ script, fraktur, double_struck };

    virtual ~AlphaTable() {}
    virtual AlphaTableEntry entry( char pos, Family family, bool bold, bool italic ) = 0;
};


KFORMULA_NAMESPACE_END

#endif
