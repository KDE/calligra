/* This file is part of the Calligra project
 * SPDX-FileCopyrightText: 2006 Sebastian Sauer <mail@dipe.org>
 * SPDX-FileCopyrightText: 2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SCRIPTING_FRAMESET_H
#define SCRIPTING_FRAMESET_H

#include <QObject>
#include <QWeakPointer>
#include "Module.h"
#include "TextDocument.h"
#include "Frame.h"

#include <KoStyleManager.h>

#include <KWTextFrame.h>
#include <KWFrameSet.h>
#include <KWTextFrameSet.h>

namespace Scripting
{

/**
* A frameset holds a number of \a Frame (zero or more) objects where
* each frame holds the content that is displayed on the screen.
*
* The following python sample script does use the FrameSet class;
* \code
* import Words
* # Print the name of the main frameset.
* fs = Words.mainFrameSesettextaroundt()
* if fs:
*     print fs.name()
* # Iterate over all framesets.
* for i in range( Words.frameSetCount() ):
*     fs = Words.frameSet(i)
*     # iterate over all frames of the frameset.
*     for k in fs.frameCount():
*         print fs.frame(k).shapeId()
* print name()
* \endcode
*/
class FrameSet : public QObject
{
    Q_OBJECT
public:
    FrameSet(Module* module, KWFrameSet* frameset)
            : QObject(module), m_frameset(frameset) {}
    virtual ~FrameSet() {}

public Q_SLOTS:

    /** Return this framesets name. */
    const QString name() {
        KWFrameSet *fs = m_frameset.data();
        if (fs)
            return fs->name();
        return QString();
    }
    /** Set the framesets name. */
    void setName(const QString &name) {
        KWFrameSet *fs = m_frameset.data();
        if (fs)
            fs->setName(name);
    }

    /** Return the number of frames this frameset has. */
    int frameCount() {
        KWFrameSet *fs = m_frameset.data();
        if (fs)
            return fs->frames().count();
        return 0;
    }
    /** Return the \a Frame object with index \p frameNr or NULL if there
    exists no \a Frame with such a index. */
    QObject* frame(int frameNr) {
        KWFrameSet *fs = m_frameset.data();
        if (fs && frameNr >= 0 && frameNr < fs->frames().count())
            return new Frame(this, fs->frames().at(frameNr));
        return 0;
    }

    /** Return the \a TextDocument object or NULL if this frameset does not
    have a \a TextDocument object. */
    QObject* document() {
        KWFrameSet *fs = m_frameset.data();
        KWTextFrameSet *textframeset = dynamic_cast<KWTextFrameSet*>(fs);
        return textframeset ? new TextDocument(this, textframeset->document()) : 0;
    }

private:
    QWeakPointer<KWFrameSet> m_frameset;
};

}

#endif
