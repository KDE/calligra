/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "KoInlineTextObjectManager.h"

#include "InsertNamedVariableAction_p.h"
#include "InsertTextLocator_p.h"
#include "InsertTextReferenceAction_p.h"
#include "KoInlineCite.h"
#include "KoInlineNote.h"
#include "KoInlineObjectRegistry.h"
#include "KoTextLocator.h"

#include <QTextCursor>

KoInlineTextObjectManager::KoInlineTextObjectManager(QObject *parent)
    : QObject(parent)
    , m_lastObjectId(0)
    , m_variableManager(this)
{
}

KoInlineTextObjectManager::~KoInlineTextObjectManager() = default;

KoInlineObject *KoInlineTextObjectManager::inlineTextObject(const QTextCharFormat &format) const
{
    int id = format.intProperty(InlineInstanceId);
    if (id <= 0)
        return nullptr;
    return m_objects.value(id, 0);
}

KoInlineObject *KoInlineTextObjectManager::inlineTextObject(const QTextCursor &cursor) const
{
    return inlineTextObject(cursor.charFormat());
}

KoInlineObject *KoInlineTextObjectManager::inlineTextObject(int id) const
{
    return m_objects.value(id);
}

void KoInlineTextObjectManager::insertInlineObject(QTextCursor &cursor, KoInlineObject *object)
{
    QTextCharFormat oldCf = cursor.charFormat();
    // create a new format out of the old so that the current formatting is
    // also used for the inserted object.  KoVariables render text too ;)
    QTextCharFormat cf(oldCf);
    cf.setObjectType(QTextFormat::UserObject + 1);
    cf.setProperty(InlineInstanceId, ++m_lastObjectId);
    cursor.insertText(QString(QChar::ObjectReplacementCharacter), cf);
    object->setId(m_lastObjectId);
    object->setManager(this);
    object->setup();

    insertObject(object);

    // reset to use old format so that the InlineInstanceId is no longer set.
    cursor.setCharFormat(oldCf);
}

void KoInlineTextObjectManager::insertObject(KoInlineObject *object)
{
    m_objects.insert(object->id(), object);
    if (object->propertyChangeListener()) {
        m_listeners.append(object);
        QHash<int, QVariant>::ConstIterator i;
        for (i = m_properties.constBegin(); i != m_properties.constEnd(); ++i)
            object->propertyChanged((KoInlineObject::Property)(i.key()), i.value());
    }

    // reset to use old format so that the InlineInstanceId is no longer set.
}

void KoInlineTextObjectManager::addInlineObject(KoInlineObject *object)
{
    if (!object) {
        return;
    }

    int id = object->id();
    if (id == -1) {
        object->setId(++m_lastObjectId);
        object->setManager(this);
        object->setup();
    } else {
        m_deletedObjects.remove(id);
    }
    insertObject(object);
}

void KoInlineTextObjectManager::removeInlineObject(KoInlineObject *object)
{
    if (!object) {
        return;
    }

    int id = object->id();
    m_objects.remove(id);
    m_deletedObjects[id] = object;
    m_listeners.removeAll(object);
}

void KoInlineTextObjectManager::setProperty(KoInlineObject::Property key, const QVariant &value)
{
    if (m_properties.contains(key)) {
        if (value == m_properties.value(key))
            return;
        m_properties.remove(key);
    }
    m_properties.insert(key, value);
    foreach (KoInlineObject *obj, m_listeners)
        obj->propertyChanged(key, value);
}

QVariant KoInlineTextObjectManager::property(KoInlineObject::Property key) const
{
    return m_properties.value(key);
}

int KoInlineTextObjectManager::intProperty(KoInlineObject::Property key) const
{
    if (!m_properties.contains(key))
        return 0;
    return m_properties.value(key).toInt();
}

bool KoInlineTextObjectManager::boolProperty(KoInlineObject::Property key) const
{
    if (!m_properties.contains(key))
        return false;
    return m_properties.value(key).toBool();
}

QString KoInlineTextObjectManager::stringProperty(KoInlineObject::Property key) const
{
    if (!m_properties.contains(key))
        return QString();
    return qvariant_cast<QString>(m_properties.value(key));
}

const KoVariableManager *KoInlineTextObjectManager::variableManager() const
{
    return &m_variableManager;
}

KoVariableManager *KoInlineTextObjectManager::variableManager()
{
    return &m_variableManager;
}

void KoInlineTextObjectManager::removeProperty(KoInlineObject::Property key)
{
    m_properties.remove(key);
}

QList<QAction *> KoInlineTextObjectManager::createInsertVariableActions(KoCanvasBase *host) const
{
    QList<QAction *> answer = KoInlineObjectRegistry::instance()->createInsertVariableActions(host);
    int i = 0;
    foreach (const QString &name, m_variableManager.variables()) {
        answer.insert(i++, new InsertNamedVariableAction(host, this, name));
    }

    answer.append(new InsertTextLocator(host));
    answer.append(new InsertTextReferenceAction(host, this));
    return answer;
}

QList<KoTextLocator *> KoInlineTextObjectManager::textLocators() const
{
    QList<KoTextLocator *> answers;
    foreach (KoInlineObject *object, m_objects) {
        KoTextLocator *tl = qobject_cast<KoTextLocator *>(object);
        if (tl)
            answers.append(tl);
    }
    return answers;
}

QList<KoInlineNote *> KoInlineTextObjectManager::endNotes() const
{
    QList<KoInlineNote *> answers;
    foreach (KoInlineObject *object, m_objects) {
        KoInlineNote *note = qobject_cast<KoInlineNote *>(object);
        if (note && note->type() == KoInlineNote::Endnote) {
            answers.append(note);
        }
    }
    return answers;
}

QMap<QString, KoInlineCite *> KoInlineTextObjectManager::citations(bool duplicatesEnabled) const
{
    QMap<QString, KoInlineCite *> answers;
    foreach (KoInlineObject *object, m_objects) {
        KoInlineCite *cite = qobject_cast<KoInlineCite *>(object);
        if (cite && (cite->type() == KoInlineCite::Citation || (duplicatesEnabled && cite->type() == KoInlineCite::ClonedCitation))) {
            answers.insert(cite->identifier(), cite);
        }
    }
    return answers;
}

QList<KoInlineCite *> KoInlineTextObjectManager::citationsSortedByPosition(bool duplicatesEnabled, QTextBlock block) const
{
    QList<KoInlineCite *> answers;

    while (block.isValid()) {
        QString text = block.text();
        int pos = text.indexOf(QChar::ObjectReplacementCharacter);

        while (pos >= 0 && pos <= block.length()) {
            QTextCursor cursor(block);
            cursor.setPosition(block.position() + pos);
            cursor.setPosition(cursor.position() + 1, QTextCursor::KeepAnchor);

            KoInlineCite *cite = qobject_cast<KoInlineCite *>(this->inlineTextObject(cursor));

            if (cite && (cite->type() == KoInlineCite::Citation || (duplicatesEnabled && cite->type() == KoInlineCite::ClonedCitation))) {
                answers.append(cite);
            }
            pos = text.indexOf(QChar::ObjectReplacementCharacter, pos + 1);
        }
        block = block.next();
    }

    return answers;
}

void KoInlineTextObjectManager::documentInformationUpdated(const QString &info, const QString &data)
{
    if (info == "title")
        setProperty(KoInlineObject::Title, data);
    else if (info == "description")
        setProperty(KoInlineObject::Description, data);
    else if (info == "comments")
        setProperty(KoInlineObject::Comments, data);
    else if (info == "subject")
        setProperty(KoInlineObject::Subject, data);
    else if (info == "keyword")
        setProperty(KoInlineObject::Keywords, data);
    else if (info == "creator")
        setProperty(KoInlineObject::AuthorName, data);
    else if (info == "initial")
        setProperty(KoInlineObject::AuthorInitials, data);
    else if (info == "title")
        setProperty(KoInlineObject::SenderTitle, data);
    else if (info == "email")
        setProperty(KoInlineObject::SenderEmail, data);
    else if (info == "telephone")
        setProperty(KoInlineObject::SenderPhonePrivate, data);
    else if (info == "telephone-work")
        setProperty(KoInlineObject::SenderPhoneWork, data);
    else if (info == "fax")
        setProperty(KoInlineObject::SenderFax, data);
    else if (info == "country")
        setProperty(KoInlineObject::SenderCountry, data);
    else if (info == "postal-code")
        setProperty(KoInlineObject::SenderPostalCode, data);
    else if (info == "city")
        setProperty(KoInlineObject::SenderCity, data);
    else if (info == "street")
        setProperty(KoInlineObject::SenderStreet, data);
    else if (info == "position")
        setProperty(KoInlineObject::SenderPosition, data);
    else if (info == "company")
        setProperty(KoInlineObject::SenderCompany, data);
}

QList<KoInlineObject *> KoInlineTextObjectManager::inlineTextObjects() const
{
    return m_objects.values();
}
