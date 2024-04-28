/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRPAGEEFFECTRUNNER_H
#define KPRPAGEEFFECTRUNNER_H

#include "KPrPageEffect.h"

class QPixmap;
class QPainter;
class QWidget;

class KPrPageEffectRunner
{
public:
    /**
     * Run the given effect
     *
     * @param oldPage Pixmap of the old page
     * @param newPage Pimap of the new page
     * @param w Widget on which the effect will be displayed
     * @param effect The effect which will be used for displaying the effect
     */
    KPrPageEffectRunner(const QPixmap &oldPage, const QPixmap &newPage, QWidget *w, KPrPageEffect *effect);

    /**
     * Destructor
     */
    ~KPrPageEffectRunner();

    /**
     * Paint the page effect
     *
     * @param painter painter used for painting the effect.
     * @return true if the effect is finished, false otherwise
     */
    bool paint(QPainter &painter);

    /**
     * Trigger the next paint paint event.
     *
     * @param currentTime The current time.
     */
    void next(int currentTime);

    /**
     * Finish the page effect.
     *
     * This only set the m_finish flag to true and triggers an update of the widget.
     */
    void finish();

    /**
     * Check if the page effect is finished.
     *
     * @return true when the page effect is finished
     */
    bool isFinished();

    /**
     * Get the pixmap of the old page
     */
    const QPixmap &oldPage() const;

    /**
     * Get the pixmap of the new page
     */
    const QPixmap &newPage() const;

    /**
     * Set the pixmap of the old page
     */
    void setOldPage(const QPixmap &oldPage);

    /**
     * Set the pixmap of the new page
     */
    void setNewPage(const QPixmap &newPage);

    /**
     * Get the wiget where the animation is shown
     */
    QWidget *widget() const;

protected:
    KPrPageEffect *m_effect;
    KPrPageEffect::Data m_data;
};

#endif /* KPRPAGEEFFECTRUNNER_H */
