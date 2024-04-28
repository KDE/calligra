/*  This file is part of the KDE project

    SPDX-FileCopyrightText: 2014 Dmitry Kazakov <dimula73@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KORESOURCESERVERPOLICIES_H
#define KORESOURCESERVERPOLICIES_H

#include "kowidgets_export.h"

class KoResource;

template<class T>
struct PointerStoragePolicy {
    typedef T *PointerType;
    static inline void deleteResource(PointerType resource)
    {
        delete resource;
    }
    static inline KoResource *toResourcePointer(PointerType resource)
    {
        return resource;
    }
};

template<class SharedPointer>
struct SharedPointerStoragePolicy {
    typedef SharedPointer PointerType;
    static inline void deleteResource(PointerType resource)
    {
        Q_UNUSED(resource);
    }
    static inline KoResource *toResourcePointer(PointerType resource)
    {
        return resource.data();
    }
};

#endif // KORESOURCESERVERPOLICIES_H
