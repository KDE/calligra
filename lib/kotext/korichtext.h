// -*- c++ -*-
// File included by qrichtext_p.h to add some small classes
// Method implementations are in kotextdocument.cc

/**
 * Base class for "visitors". Visitors are a well-designed way to
 * apply a given operation to all the paragraphs in a selection, or
 * in a document. The visitor needs to inherit KoParagVisitor, and implement visit().
 */
class KoParagVisitor
{
protected:
    /** protected since this is an abstract base class */
    KoParagVisitor() {}
    virtual ~KoParagVisitor() {}
public:
    /** Visit the paragraph @p parag, from index @p start to index @p end */
    virtual bool visit( KoTextParag *parag, int start, int end ) = 0;
};

class KCommand;
class QDomElement;
class KMacroCommand;

/** A CustomItemsMap associates a custom item to an index
 * Used in the undo/redo info for insert/delete text. */
class CustomItemsMap : public QMap<int, KoTextCustomItem *>
{
public:

    /** Insert all the items from the map, into the existing text */
    void insertItems( const KoTextCursor & startCursor, int size );

    /** Delete all the items from the map, adding their commands into macroCmd */
    void deleteAll( KMacroCommand *macroCmd );
};
