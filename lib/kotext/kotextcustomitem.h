// -*- c++ -*-
// File included by korichtext.h to add our own methods to KoTextCustomItem

/**
 * KoTextCustomItem is the base class for custom items (i.e. special chars)
 * Custom items include:
 * - variables ( KoVariable, kovariable.h )
 * - in kword: inline images ( KWTextImage, kwtextimage.h ) (to be removed)
 * - in kword: anchors, i.e. floating frames ( KWAnchor, kwanchor.h )
 */
public:
    /** The text document in which this customitem is */
    KoTextDocument * textDocument() const { return parent; }

    /** When the user deletes a custom item, it isn't destroyed but
     * moved into the undo/redo history - setDeleted( true )
     * and it can be then copied back from there into the real world - setDeleted( false ). */
    virtual void setDeleted( bool b ) { m_deleted = b; }

    bool isDeleted() const { return m_deleted; }

    /** Called when the item is created or 'deleted' by the user
     * Most custom items don't need to reimplement those, since
     * the custom item is simply moved into the undo/redo history
     * when deleting (or undoing a creation).
     * It is not deleted and re-created later. */
    virtual KCommand * createCommand() { return 0L; }
    virtual KCommand * deleteCommand() { return 0L; }

    /** Save to XML */
    virtual void save( QDomElement& formatElem ) = 0;
    /** Save to Oasis XML */
    virtual void saveOasis( KoXmlWriter& writer, KoSavingContext& context ) const = 0;
    /** Return type of custom item. See DTD for VARIABLE.id docu. */
    virtual int typeId() const = 0;

    /** Reimplement this to calculate the item width
     * It is important to start with "if ( m_deleted ) return;" */
    virtual void resize() {}

    /** Reimplemented by KoVariable to recalculate the value.
     * It exists at the KoTextCustomItem level so that KoTextParag::setCustomItem
     * can call it to set the initial value.
     * This should call always resize(). */
    virtual void recalc() { resize(); }

    /** The index in paragraph(), where this anchor is
     * Slightly slow (does a linear search in the paragraph) */
    int index() const;

    /** The formatting given to this 'special' character
     * Slightly slow (does a linear search in the paragraph) */
    KoTextFormat * format() const;

    /**
     * All coordinates are in pixels.
     */
    virtual void drawCustomItem(QPainter* p, int x, int y, int wpix, int hpix, int ascentpix, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool selected, int offset,  bool drawingShadow) = 0;

protected:
    bool m_deleted;

