/* This file is part of the KDE project
 * Copyright (C) 2006 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KWSHAPECONFIGFACTORY_H
#define KWSHAPECONFIGFACTORY_H

#include <KoShapeConfigFactory.h>

class KoShape;
class KWFrame;
class KWDocument;

/// \internal
class FrameConfigSharedState {
public:
    FrameConfigSharedState(KWDocument *document);
    ~FrameConfigSharedState();

    void addUser() { m_refcount++; }
    void removeUser();

    KWFrame *frame() const { return m_frame; }
    void setFrame(KWFrame *frame) { m_deleteFrame = true; m_frame = frame; }
    KWFrame *createFrame(KoShape *shape);
    void markFrameUsed() { m_deleteFrame = false; }

    KWDocument *document() const { return m_document; }

private:
    int m_refcount;
    bool m_deleteFrame;
    KWFrame *m_frame;
    KWDocument *m_document;
};

/// factory to create a KWFrameConnectSelector widget
class KWFrameConnectSelectorFactory : public KoShapeConfigFactory {
public:
    /// constructor
    KWFrameConnectSelectorFactory(FrameConfigSharedState *state) : m_state(state) {}
    ~KWFrameConnectSelectorFactory() {}

    /// reimplemented method from superclass
    KoShapeConfigWidgetBase *createConfigWidget(KoShape *shape);
    /// reimplemented method from superclass
    QString name() const;

    /// reimplemented method from superclass
    bool showForShapeId(const QString &id) const;
    /// reimplemented method from superclass
    int sortingOrder() const { return 1; }

private:
    FrameConfigSharedState *m_state;
};

/// factory to create a KWFrameGeometry widget
class KWFrameGeometryFactory : public KoShapeConfigFactory {
public:
    /// constructor
    KWFrameGeometryFactory(FrameConfigSharedState *state) : m_state(state) {}
    ~KWFrameGeometryFactory() {}

    /// reimplemented method from superclass
    KoShapeConfigWidgetBase *createConfigWidget(KoShape *shape);
    /// reimplemented method from superclass
    QString name() const;
    /// reimplemented method from superclass
    int sortingOrder() const { return 0; }

private:
    FrameConfigSharedState *m_state;
};

/// factory to create a KWFrameRunaroundProperties widget
class KWFrameRunaroundPropertiesFactory : public KoShapeConfigFactory {
public:
    /// constructor
    KWFrameRunaroundPropertiesFactory(FrameConfigSharedState *state) : m_state(state) {}
    ~KWFrameRunaroundPropertiesFactory() {}

    /// reimplemented method from superclass
    KoShapeConfigWidgetBase *createConfigWidget(KoShape *shape);
    /// reimplemented method from superclass
    QString name() const;

    /// reimplemented method from superclass
    int sortingOrder() const { return 5; }

private:
    FrameConfigSharedState *m_state;
};

/// factory to create a KWGeneralFrameProperties widget
class KWGeneralFramePropertiesFactory : public KoShapeConfigFactory {
public:
    /// constructor
    KWGeneralFramePropertiesFactory(FrameConfigSharedState *state) : m_state(state) {}
    ~KWGeneralFramePropertiesFactory() {}

    /// reimplemented method from superclass
    KoShapeConfigWidgetBase *createConfigWidget(KoShape *shape);
    /// reimplemented method from superclass
    QString name() const;
    /// reimplemented method from superclass
    int sortingOrder() const { return 10; }

private:
    FrameConfigSharedState *m_state;
};

#endif
