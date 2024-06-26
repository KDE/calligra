/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2010 Benjamin Port <port.benjamin@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRPAGEEFFECTSTRATEGY_H
#define KPRPAGEEFFECTSTRATEGY_H

#include "KPrPageEffectFactory.h"

#include "stage_export.h"

class STAGE_EXPORT KPrPageEffectStrategy
{
public:
    /**
     * Constructor
     *
     * @param subType The sub type used for the UI
     * @param smilType The smil:type used for loading/saving
     * @param smilSubType The smil:subType used for loading/saving
     * @param reverse The flag for the smil:direction used for loading/saving true means reverse false means forward
     * @param graphicsView The flag for graphics view
     */
    KPrPageEffectStrategy(int subType, const char *smilType, const char *smilSubType, bool reverse, bool graphicsView = false);
    virtual ~KPrPageEffectStrategy();

    /**
     * Get the sub type of this strategy
     *
     * This is not the smil:subType but the sub type that is used for displaying a common UI string for the effect.
     */
    int subType() const;

    /**
     * Setup the timeline used by this strategy
     */
    virtual void setup(const KPrPageEffect::Data &data, QTimeLine &timeLine) = 0;

    /**
     * Paint the page effect
     *
     * This should repaint the whole widget. Due to clipping only the
     * relevant parts are repainted.
     *
     * @param p painter used for painting the effect.
     * @param curPos The current position (frame to the current time)
     * @param data The data used for painting the effect.
     *
     * @see next()
     */
    virtual void paintStep(QPainter &p, int curPos, const KPrPageEffect::Data &data) = 0;

    /**
     * Trigger the next paint paint event.
     *
     * Trigger a repaint of the part of the widget that changed since
     * the last time to this call.
     *
     * @param data The data used for the effect.
     */
    virtual void next(const KPrPageEffect::Data &data) = 0;

    /**
     * The default implementation triggers an update of the whole widget. If you only need to
     * update a smaller part of the widget reimplement this function.
     */
    virtual void finish(const KPrPageEffect::Data &data);

    /**
     * Save transitions in an xml writer
     */
    virtual void saveOdfSmilAttributes(KoXmlWriter &xmlWriter) const;

    /**
     * Save transition as part of the style
     */
    virtual void saveOdfSmilAttributes(KoGenStyle &style) const;

    /**
     * Load additional attributes needed by the strategy.
     *
     * The default implementation is empty
     */
    virtual void loadOdfSmilAttributes(const KoXmlElement &element);

    /**
     * Get the smil:type
     *
     * @return the type used for saving
     */
    const QString &smilType() const;

    /**
     * Get the smil:subtype
     *
     * @return the subtype used for saving
     */
    const QString &smilSubType() const;

    /**
     * Get the smil:direction
     *
     * @return true if smil:direction is reverse, false otherwise
     */
    bool reverse() const;

    /**
     * @return true if we use QGraphicsView, false otherwise
     */
    bool useGraphicsView() const;

private:
    struct SmilData {
        SmilData(const char *type, const char *subType, bool reverse)
            : type(type)
            , subType(subType)
            , reverse(reverse)
        {
        }

        QString type;
        QString subType;
        bool reverse;
    };

    int m_subType;
    SmilData m_smilData;
    bool m_graphicsView;
};

#endif /* KPRPAGEEFFECTSTRATEGY_H */
