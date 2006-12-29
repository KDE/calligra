#ifndef kwtextframeset_h
#define kwtextframeset_h

#include "KoRichText.h"
#include "KWFrameSet.h"
#include "KWFrameSetEdit.h"
#include <KoTextView.h>
#include <KoParagLayout.h>
#include <KoChangeCaseDia.h>
#include "KWVariable.h"
//Added by qt3to4:
#include <QDragLeaveEvent>
#include <Q3MemArray>
#include <Q3PtrList>
#include <QKeyEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <Q3ValueList>
#include <QDragEnterEvent>
#include <QMouseEvent>

class KoGenStyles;
class KoParagStyle;
class KWDocument;
class DCOPObject;
#ifndef KWTextFormat
#define KWTextFormat KoTextFormat
#endif
class KWViewMode;
class KWTextDocument;
class KWordFrameSetIface;
class KWFrame;
class KWFrameViewManager;

class KoTextObject;
class KoDataToolInfo;
class KoVariable;

class KAction;
class KNamedCommand;
class KMacroCommand;

class Q3DragObject;
class Q3ProgressDialog;

class KWTextFrameSet : public KWFrameSet, public KoTextFlow, public KoTextFormatInterface
{
public:
    KWTextFrameSet( KWDocument *doc, const QString & name ) : KWFrameSet(doc) {};
    /// Used for OASIS loading
    KWTextFrameSet( KWDocument* doc, const QDomElement& tag, KoOasisContext& context ): KWFrameSet(doc) {};
    /// Destructor
    ~KWTextFrameSet() {};
    QDomElement save( QDomElement &parentElem, bool saveFrames = true ) {};
    void saveOasis( KoXmlWriter& writer, KoSavingContext& context, bool saveFrames ) const {};
    void setProtectContent ( bool protect ) {};
    bool protectContent() const {};
};

/**
 * Object that is created to edit a Text frame set (KWTextFrameSet).
 * It handles all the events for it.
 * In terms of doc/view design, this object is part of the _view_.
 * There can be several KWFrameSetEdit objects for the same frameset,
 * but there is only one KWFrameSetEdit object per view at a given moment.
 */
class KWTextFrameSetEdit : public KoTextView, public KWFrameSetEdit
{
    Q_OBJECT
public:
    KWTextFrameSetEdit( KWTextFrameSet * fs, KWCanvas * canvas ) : KoTextView(0), KWFrameSetEdit(fs, canvas) {};
    virtual ~KWTextFrameSetEdit() {};
};

class KWFootNoteVariable;
class KWFootNoteFrameSet : public KWTextFrameSet
{
public:
    /** constructor */
    KWFootNoteFrameSet( KWDocument *doc, const QString & name )
        : KWTextFrameSet( doc, name ) {}


};
#endif
