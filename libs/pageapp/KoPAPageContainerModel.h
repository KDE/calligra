/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOPAPAGECONTAINERMODEL_H
#define KOPAPAGECONTAINERMODEL_H

#include <KoShapeContainerDefaultModel.h>

class KoPAPageContainerModel : public KoShapeContainerDefaultModel
{
public:
    KoPAPageContainerModel();
    ~KoPAPageContainerModel() override;

    void childChanged(KoShape *child, KoShape::ChangeType type) override;
};

#endif /* KOPAPAGECONTAINERMODEL_H */
