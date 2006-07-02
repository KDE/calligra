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

class KoCanvasBase;
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
    void setFrame(KWFrame *frame) { m_frame = frame; }
    KWFrame *createFrame(KoShape *shape);

    KWDocument *document() const { return m_document; }

private:
    int m_refcount;
    KWFrame *m_frame;
    KWDocument *m_document;
};

class KWFrameConnectSelectorFactory : public KoShapeConfigFactory {
public:
    KWFrameConnectSelectorFactory(FrameConfigSharedState *state) : m_state(state) {}
    ~KWFrameConnectSelectorFactory() {}

    KoShapeConfigWidgetBase *createConfigWidget(KoCanvasBase *canvas, KoShape *shape);
    QString name() const;

    bool showForShapeId(const QString &id) const;
private:
    FrameConfigSharedState *m_state;
};

class KWFrameGeometryFactory : public KoShapeConfigFactory {
public:
    KWFrameGeometryFactory(FrameConfigSharedState *state) : m_state(state) {}
    ~KWFrameGeometryFactory() {}

    KoShapeConfigWidgetBase *createConfigWidget(KoCanvasBase *canvas, KoShape *shape);
    QString name() const;
private:
    FrameConfigSharedState *m_state;
};

class KWFrameRunaroundPropertiesFactory : public KoShapeConfigFactory {
public:
    KWFrameRunaroundPropertiesFactory(FrameConfigSharedState *state) : m_state(state) {}
    ~KWFrameRunaroundPropertiesFactory() {}

    KoShapeConfigWidgetBase *createConfigWidget(KoCanvasBase *canvas, KoShape *shape);
    QString name() const;
private:
    FrameConfigSharedState *m_state;
};

class KWGeneralFramePropertiesFactory : public KoShapeConfigFactory {
public:
    KWGeneralFramePropertiesFactory(FrameConfigSharedState *state) : m_state(state) {}
    ~KWGeneralFramePropertiesFactory() {}

    KoShapeConfigWidgetBase *createConfigWidget(KoCanvasBase *canvas, KoShape *shape);
    QString name() const;
private:
    FrameConfigSharedState *m_state;
};

#endif
