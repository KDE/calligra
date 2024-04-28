/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2020 Dag Andersen <danders@get2net.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KPRPAGEAPPLICATIONDATA_H
#define KPRPAGEAPPLICATIONDATA_H

#include "stage_export.h"

#include "KPrPageTransition.h"

#include <KoShapeApplicationData.h>

#include <QDebug>

class KoGenStyle;
class KoShapeLoadingContext;
class KoXmlElement;

class KPrPageEffect;

class STAGE_EXPORT KPrPageApplicationData : public KoShapeApplicationData
{
public:
    KPrPageApplicationData();
    ~KPrPageApplicationData() override;

    /**
     * Get the page effect used to go to this page
     */
    KPrPageEffect *pageEffect();

    /**
     * Set the page effect used for this page
     */
    void setPageEffect(KPrPageEffect *effect);

    /**
     * Get the page transition
     */
    KPrPageTransition &pageTransition();
    /**
     * Set the page transition
     */
    void setPageTransition(const KPrPageTransition &transition);

private:
    KPrPageEffect *m_pageEffect;
    KPrPageTransition m_pageTransition;
};

#endif // KPRPAGEAPPLICATIONDATA_H
