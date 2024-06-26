/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2009 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOINLINETEXTOBJECTMANAGER_H
#define KOINLINETEXTOBJECTMANAGER_H

#include "KoInlineObject.h"
#include "KoVariableManager.h"
#include "kotext_export.h"

// Qt
#include <QHash>
#include <QTextBlock>

class KoCanvasBase;
class KoTextLocator;
class KoInlineNote;
class KoInlineCite;

class QTextCharFormat;
class QAction;

/**
 * A container to register all the inlineTextObjects with.
 * Inserting an inline-object in a QTextDocument should be done via this manager which will
 * insert a placeholder in the text and you should add the KoInlineTextObjectManager to the
 * KoTextDocument.
 */
class KOTEXT_EXPORT KoInlineTextObjectManager : public QObject
{
    Q_OBJECT
public:
    enum Properties {
        InlineInstanceId = 577297549 // If you change this, don't forget to change KoCharacterStyle.h
    };

    /// Constructor
    explicit KoInlineTextObjectManager(QObject *parent = nullptr);
    ~KoInlineTextObjectManager() override;

    /**
     * Retrieve a formerly added inline object based on the format.
     * @param format the textCharFormat
     */
    KoInlineObject *inlineTextObject(const QTextCharFormat &format) const;

    /**
     * Retrieve a formerly added inline object based on the cursor position.
     * @param cursor the cursor which position is used. The anchor is ignored.
     */
    KoInlineObject *inlineTextObject(const QTextCursor &cursor) const;

    /**
     * Retrieve a formerly added inline object based on the KoInlineObject::id() of the object.
     * @param id the id assigned to the inline text object when it was added.
     */
    KoInlineObject *inlineTextObject(int id) const;

    QList<KoInlineObject *> inlineTextObjects() const;

    /**
     * Insert a new inline object into the manager as well as the document.
     * This method will cause a placeholder to be inserted into the text at cursor position,
     *  possibly replacing a selection.  The object will then be used as an inline
     * character and painted at the specified location in the text.
     * @param cursor the cursor which indicated the document and the position in that document
     *      where the inline object will be inserted.
     * @param object the inline object to insert.
     */
    void insertInlineObject(QTextCursor &cursor, KoInlineObject *object);

    /**
     * Add inline object into the manager.
     *
     * This methods add the inline object into the manager. This is useful if you have a command
     * that removes and adds a inline object to the manager. If the object already was inserted before
     * (the object id is already set) it keeps the old id, otherwise a new id will be generated.
     *
     * @param object the inline object to insert.
     */
    void addInlineObject(KoInlineObject *object);

    /**
     * Remove an inline object from this manager. The object will also be removed from
     * the bookmarkmanager if it is a bookmark. This is not done smart: you might end up
     * with dangling start or end bookmarks.
     * Should really only be called by KoTextEditor's delete commands
     * @param object the object to be removed
     */
    void removeInlineObject(KoInlineObject *object);

    /**
     * Set a property that may have changed which will be forwarded to all registered textObjects.
     * If the key has changed then all registered InlineObject instances that have stated to want
     * updates will get called with the change.
     * The property will be stored to allow it to be retrieved via the intProperty() and friends.
     * @see KoInlineObject::propertyChangeListener()
     */
    void setProperty(KoInlineObject::Property key, const QVariant &value);

    /// retrieve a property
    QVariant property(KoInlineObject::Property key) const;

    /// retrieve an int property
    int intProperty(KoInlineObject::Property key) const;

    /// retrieve a bool property
    bool boolProperty(KoInlineObject::Property key) const;

    /// retrieve a string property
    QString stringProperty(KoInlineObject::Property key) const;

    /// remove a property from the store.
    void removeProperty(KoInlineObject::Property key);

    /**
     * Return the variableManager.
     */
    const KoVariableManager *variableManager() const;
    /**
     * Return the variableManager.
     */
    KoVariableManager *variableManager();

    /**
     * Create a list of actions that can be used to plug into a menu, for example.
     * This method internally uses KoInlineObjectRegistry::createInsertVariableActions() but extends
     * the list with all registered variable-names.
     * Each of these actions, when executed, will insert the relevant variable in the current text-position.
     * The actions assume that the text tool is selected, if that's not the case then they will silently fail.
     * @param host the canvas for which these actions are created.  Note that the actions will get these
     *  actions as a parent (for memory management purposes) as well.
     * @see KoVariableManager
     */
    QList<QAction *> createInsertVariableActions(KoCanvasBase *host) const;

    QList<KoTextLocator *> textLocators() const;

    /**
     * It returns a list of all end notes in the document
     */
    QList<KoInlineNote *> endNotes() const;

    QMap<QString, KoInlineCite *> citations(bool duplicatesEnabled = true) const;

    QList<KoInlineCite *> citationsSortedByPosition(bool duplicatesEnabled = true, QTextBlock block = QTextBlock()) const;

public Q_SLOTS:
    void documentInformationUpdated(const QString &info, const QString &data);

Q_SIGNALS:
    /**
     * Emitted whenever a property is set and it turns out to be changed.
     */
    void propertyChanged(int, const QVariant &variant);

private:
    void insertObject(KoInlineObject *object);

    QHash<int, KoInlineObject *> m_objects;
    QHash<int, KoInlineObject *> m_deletedObjects;
    QList<KoInlineObject *> m_listeners; // holds objects also in m_objects, but which want propertyChanges
    int m_lastObjectId;
    QHash<int, QVariant> m_properties;

    KoVariableManager m_variableManager;
};

Q_DECLARE_METATYPE(KoInlineTextObjectManager *)
#endif
