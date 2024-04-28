/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013-2014 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOODF_OPC_RELSET_MANAGER_H
#define KOODF_OPC_RELSET_MANAGER_H

#include "koodf_export.h"

class QString;
class KoStore;
class OpcRelSet;

class OpcRelSetManager
{
public:
    OpcRelSetManager();
    ~OpcRelSetManager();

    OpcRelSet *relSet(const QString &path) const;
    void setRelSet(const QString &path, OpcRelSet *relSet);

    OpcRelSet *documentRelSet() const;
    void setDocumentRelSet(OpcRelSet *relSet);

    void clear();

    bool loadRelSets(KoStore *odfStore);
    bool saveRelSets(KoStore *odfStore);

private:
    class Private;
    Private *const d;
};

#endif
