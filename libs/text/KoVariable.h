/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef __KOVARIABLE_H__
#define __KOVARIABLE_H__

// Calligra libs
#include "KoInlineObject.h"
#include "kotext_export.h"

class KoProperties;
class QWidget;
class KoVariableManager;
class KoVariablePrivate;

/**
 * Base class for in-text variables.
 *
 * A variable is a field inserted into the text and the content is set to a specific value that
 * is used as text.  This class is pretty boring in that it has just a setValue() to alter the
 * text shown; we depend on plugin writers to create more exciting ways to update variables.
 */
class KOTEXT_EXPORT KoVariable : public KoInlineObject
{
    Q_OBJECT
public:
    /**
     * Constructor.
     */
    explicit KoVariable(bool propertyChangeListener = false);
    ~KoVariable() override;

    /**
     * The new value this variable will show.
     * Will be used at the next repaint.
     * @param value the new value this variable shows.
     */
    void setValue(const QString &value);

    /// @return the current value of this variable.
    QString value() const;

    /**
     * Shortly after instantiating this variable the factory should set the
     * properties using this method.
     * Note that the loading mechanism will fill this properties object with the
     * attributes from the ODF file (if applicable), so it would be useful to synchronize
     * the property names based on that.
     */
    virtual void setProperties(const KoProperties *props)
    {
        Q_UNUSED(props);
    }

    /**
     * If this variable has user-editable options it should provide a widget that is capable
     * of manipulating these options so the text-tool can use it to show that to the user.
     * Note that all manipulations should have a direct effect on the variable itself.
     */
    virtual QWidget *createOptionsWidget()
    {
        return nullptr;
    }

protected:
    /**
     * This hook is called whenever the variable gets a new position.
     * If this is a type of variable that needs to change its value based on that
     * you should implement this method and act on it.
     */
    virtual void variableMoved(const QTextDocument *document, int posInDocument);

    friend class KoVariableManager;
    /**
     * return the last known position in the document. Note that if the variable has not yet been layouted,
     * it does not know the position.
     */
    int positionInDocument() const;

    /// reimplemented
    void resize(const QTextDocument *document, QTextInlineObject &object, int posInDocument, const QTextCharFormat &format, QPaintDevice *pd) override;

private:
    void updatePosition(const QTextDocument *document, int posInDocument, const QTextCharFormat &format) override;
    void paint(QPainter &painter,
               QPaintDevice *pd,
               const QTextDocument *document,
               const QRectF &rect,
               const QTextInlineObject &object,
               int posInDocument,
               const QTextCharFormat &format) override;

private Q_SLOTS:
    void documentDestroyed();

private:
    Q_DECLARE_PRIVATE(KoVariable)
};

#endif
