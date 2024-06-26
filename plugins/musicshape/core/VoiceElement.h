/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef MUSIC_CORE_VOICEELEMENT_H
#define MUSIC_CORE_VOICEELEMENT_H

#include <QObject>

namespace MusicCore
{

class Staff;
class VoiceBar;

/**
 * This is the base class for all musical elements that can be added to a voice.
 */
class VoiceElement : public QObject
{
    Q_OBJECT
public:
    /**
     * Creates a new VoiceElement.
     */
    explicit VoiceElement(int length = 0);

    /**
     * Destructor.
     */
    ~VoiceElement() override;

    /**
     * Returns the staff this music element should be displayed on. It can also be nullptr, for example if the element
     * should not be visible.
     */
    Staff *staff() const;

    /**
     * Sets the staff this element should be displayed on.
     *
     * @param staff the new staff this element should be displayed on
     */
    void setStaff(Staff *staff);

    VoiceBar *voiceBar() const;
    void setVoiceBar(VoiceBar *voiceBar);

    /**
     * Returns the x position of this musical element. The x position of an element is measured relative to the left
     * barline of the bar the element is in.
     */
    virtual qreal x() const;

    /**
     * Returns the y position of this musical element. The y position of an element is measure relative to the top
     * of the staff it is in.
     */
    virtual qreal y() const;

    /**
     * Returns the width of this musical element.
     */
    virtual qreal width() const;

    /**
     * Returns the height of this musical element.
     */
    virtual qreal height() const;

    /**
     * Returns the duration of this musical elements in ticks.
     */
    int length() const;

    /**
     * Returns the beatline of this element. This is an x position relative to the start of the element.
     */
    virtual qreal beatline() const;
public Q_SLOTS:
    /**
     * Sets the x position of this musical element.
     */
    void setX(qreal x);

    /**
     * Sets the y position of this musical element.
     */
    void setY(qreal y);
protected Q_SLOTS:
    /**
     * Changes the duration of this musical element.
     *
     * @param length the new duration of this musical element
     */
    void setLength(int length);

    /**
     * Sets the width of this musical element.
     *
     * @param width the new width of this musical element
     */
    void setWidth(qreal width);

    /**
     * Sets the height of this musical element.
     *
     * @param height the new height of this musical element
     */
    void setHeight(qreal height);

    void setBeatline(qreal beatline);
Q_SIGNALS:
    void xChanged(qreal x);
    void yChanged(qreal y);
    void lengthChanged(int length);
    void widthChanged(qreal width);
    void heightChanged(qreal height);

private:
    class Private;
    Private *const d;
};

} // namespace MusicCore

#endif // MUSIC_CORE_VOICEELEMENT_H
