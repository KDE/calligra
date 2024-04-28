/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef MUSIC_SHAPE
#define MUSIC_SHAPE

#include <KoFrameShape.h>
#include <KoShape.h>

#define MusicShapeId "MusicShape"

namespace MusicCore
{
class Sheet;
}

class MusicRenderer;
class MusicStyle;
class Engraver;

class MusicShape : public KoShape, public KoFrameShape
{
public:
    MusicShape();
    ~MusicShape() override;
    /// reimplemented
    void paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintcontext) override;
    void constPaint(QPainter &painter, const KoViewConverter &converter) const;

    /// reimplemented
    void setSize(const QSizeF &newSize) override;

    /// reimplemented
    void saveOdf(KoShapeSavingContext &context) const override;
    // reimplemented
    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;

    MusicCore::Sheet *sheet();
    void setSheet(MusicCore::Sheet *sheet, int firstSystem);
    int firstSystem() const;
    void setFirstSystem(int system);
    int lastSystem() const;
    int firstBar() const;
    int lastBar() const;
    MusicRenderer *renderer();
    MusicStyle *style();
    void engrave(bool engraveBars = true);

    MusicShape *successor()
    {
        return m_successor;
    }
    MusicShape *predecessor()
    {
        return m_predecessor;
    }

protected:
    // reimplemented
    bool loadOdfFrameElement(const KoXmlElement &element, KoShapeLoadingContext &context) override;

private:
    MusicCore::Sheet *m_sheet;
    int m_firstSystem;
    int m_lastSystem;
    MusicStyle *m_style;
    Engraver *m_engraver;
    MusicRenderer *m_renderer;
    MusicShape *m_successor;
    MusicShape *m_predecessor;
};

#endif // MUSIC_SHAPE
