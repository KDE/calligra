#ifndef KWORD_PARAGVISITORS_H
#define KWORD_PARAGVISITORS_H

#include <kotextdocument.h>
class KWFrameSet;
template <class T> class QValueList;

// This paragraph visitor collects the inline framesets (found via KWAnchor)
// found in a text selection.
class KWCollectFramesetsVisitor : public KoParagVisitor // see kotextdocument.h
{
public:
    KWCollectFramesetsVisitor() : KoParagVisitor() {}
    virtual bool visit( KoTextParag *parag, int start, int end );

    const QValueList<KWFrameSet *>& frameSets() const { return m_framesets; }

private:
    QValueList<KWFrameSet *> m_framesets;
};

#endif /* KWORD_PARAGVISITORS_H */
