/* This file is part of the KDE project

   Copyright (C) 2013 Inge Wallin <inge@lysator.liu.se>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/


// Own
#include "KoXmlStreamReader.h"

// Qt
#include <QString>
#include <QStringList>
#include <QSet>

// KDE
#include <kdebug.h>


// ----------------------------------------------------------------
//                 class KoXmlStreamReader::Private


class KoXmlStreamReader::Private
{
public:
    Private(KoXmlStreamReader *qq);
    ~Private();

    void clear();

    void        checkSoundness();
    QStringRef  buildQName();

    KoXmlStreamReader *q;       // The owner object;

    bool  isSound;              // True if the document is sound (see the class doc for details)
    bool  isChecked;            // True if the soundness is checked

    QHash<QString, QString>  expectedNamespaces;  // nsUri, prefix
    QHash<QString, QString>  extraNamespaces;     // nsUri, prefix

    // This is only used when a document is unsound, but is always created.
    QHash<QString, QString>  prefixes; // nsUri, prefix

    // If the document is unsound, we need to build the qualified
    // names from a prefix that we get from "prefixes" and the actual
    // name.  But we return a QStringRef and not QString so we need to
    // make sure that the stringref is valid until this
    // KoXmlStreamReader is destructed.  So we use this QSet as a
    // cache of all constructed qualifiednames that we ever generate.
    QSet<QString>  qualifiedNamesCache;
};


KoXmlStreamReader::Private::Private(KoXmlStreamReader *qq)
    : q(qq)
{
    clear();
}

KoXmlStreamReader::Private::~Private()
{
}


void KoXmlStreamReader::Private::clear()
{
    // The initial state is Unsound and Unchecked.  The check may
    // switch it to Sound.  See the class documentation for an
    // explanation of those terms.
    isSound = false;
    isChecked = false;

    expectedNamespaces.clear();
    extraNamespaces.clear();

    prefixes.clear();
    qualifiedNamesCache.clear();
}


void KoXmlStreamReader::Private::checkSoundness()
{
    isSound = true;

    // Temp values
    QStringList    namespacesToFix;    // List of namespaces we need to create a unique prefix for
    QSet<QString>  usedPrefixes;

    // Initialize by setting all expected prefixes and all extra ones.
    prefixes.clear();
    foreach(const QString &nsUri, expectedNamespaces.keys()) {
        QString prefix = expectedNamespaces.value(nsUri);

        prefixes.insert(nsUri, prefix);
        usedPrefixes.insert(prefix);
    }
    foreach(const QString &nsUri, extraNamespaces.keys()) {
        QString prefix = extraNamespaces.value(nsUri);

        prefixes.insert(nsUri, prefix);
        usedPrefixes.insert(prefix);
    }

    // The document is "Sound" if for all the declared namespaces in
    // the document, the prefix is the same as the one in the expected
    // namespaces for the same namespace URI.
    //
    // If it is not Sound, then we need to rewrite the prefixes for
    // the qualified names when the caller wantes to access them.
    // Hopefully this only happens once in a million documents (and it
    // would be interesting to know which application created such a
    // strange beast).
    //
    QXmlStreamNamespaceDeclarations  nsDeclarations = q->QXmlStreamReader::namespaceDeclarations();
    foreach(const QXmlStreamNamespaceDeclaration &decl, nsDeclarations) {

        QString nsUri(decl.namespaceUri().toString());
        QString prefix(decl.prefix().toString());

        if (prefixes.contains(nsUri)) {
            if (prefix == prefixes.value(nsUri)) {

                // 1. nsUri = expected nsUri AND prefix = expected prefix:
                //
                // Soundness is not disturbed. Let's continue with the next declaration.
                continue;
            }
            else {
                // 2. nsUri = expected nsUri AND prefix != expected prefix:
                //
                // Document is not sound but we don't need to do
                // anything else; the expected prefix is already in
                // prefixes[] and the prefix will be rewritten to the
                // expected one when used later in the document.
                isSound = false;
                continue;
            }
        }
        else {
            // 3. nsUri is not among the expected nsUri's
            //
            // Let's check if the prefix is unique or if it already
            // exists among the expected ones.  If it is unique the
            // soundness is not affected, otherwise it is unsound.
            prefixes.insert(nsUri, prefix);
            usedPrefixes.insert(prefix);
            bool found = false;
            foreach (const QString &value, prefixes) {
                if (value == prefix) {
                    found = true;
                    break;
                }
            }
            if (found) {
                // Yes, the prefix is used for another namespace among
                // the expected ones.  Let's store this namespace for
                // now and create a unique, non-"expected" prefix
                // later when all namespaces and prefixes are known.
                isSound = false;
                namespacesToFix.append(nsUri);
            }
        }
    }

    // Finally, if necessary, create unique prefixes for namespaces
    // that are found to use one of the expected prefixes.  It doesn't
    // much matter what we come up with here since these will have to
    // be accessed through namespaceUri() anyway.
    int number = 1;
    foreach (const QString &ns, namespacesToFix) {
        bool ok = false;
        QString pfx;
        while (!ok) {
            pfx = QString("pfx%d").arg(number++);
            if (!usedPrefixes.contains(pfx)) {
                ok = true;
            }
        }
        prefixes.insert(ns, pfx);
    }

    isChecked = true;
}

QStringRef KoXmlStreamReader::Private::buildQName()
{
    if (!isChecked) {
        checkSoundness();       // Sets isChecked and isSound;
    }

    if (isSound) {
        return q->qualifiedName();
    }

    // FIXME: Handle undeclared prefixes.  (Is that even legal?)
    //QString nsUri = q->QXmlStreamReader::namespaceUri().toString();
    QString qualifiedName = prefixes.value(q->QXmlStreamReader::namespaceUri().toString())
        + ':' + q->QXmlStreamReader::name().toString();

    // The following code is because qualifiedName() returns a
    // QStringRef, not a QString.  So we need to make sure that the
    // QString that it references stays valid until the end of the
    // document is parsed.  We do this by storing all qualified names
    // that are accessed in a QSet<QString> and return a QStringRef
    // that references the copy in the set.  Ugly bug effective.
#if 1
    if (!qualifiedNamesCache.contains(qualifiedName)) {
        // FIXME: Is there a way to do this at the same time as the
        // check without creating a double copy if it was already inserted?
        qualifiedNamesCache.insert(qualifiedName);
    }

    QSet<QString>::ConstIterator  it = qualifiedNamesCache.find(qualifiedName);
#else
    // This should work too but it's unclear from the documentation
    // what is returned if it was already in the set.  It only
    // mentions "the inserted item"
    QSet<QString>::ConstIterator  it = qualifiedNamesCache.insert(qualifiedName);
#endif
    return (*it).leftRef(-1);  // Will always succeed since we entered it if it didn't exist already.
}


// ----------------------------------------------------------------
//                     class KoXmlStreamReader


KoXmlStreamReader::KoXmlStreamReader()
    : QXmlStreamReader()
    , d(new KoXmlStreamReader::Private(this))
{
}

KoXmlStreamReader::~KoXmlStreamReader()
{
    delete d;
}


void KoXmlStreamReader::clear()
{
    d->clear();

    QXmlStreamReader::clear();
}


void KoXmlStreamReader::addExpectedNamespace(QString prefix, QString namespaceUri)
{
    d->expectedNamespaces.insert(namespaceUri, prefix);

    d->isChecked = false;
    d->isSound = false;
}

void KoXmlStreamReader::addExtraNamespace(QString prefix, QString namespaceUri)
{
    d->extraNamespaces.insert(namespaceUri, prefix);

    d->isChecked = false;
    d->isSound = false;
}


// ----------------------------------------------------------------
//                 Reimplemented from QXmlStreamReader


// Should this be made inline?  that would make it very fast at the
// cost of a possibly unstable API.

QStringRef KoXmlStreamReader::qualifiedName() const
{
    return d->isSound ? QXmlStreamReader::qualifiedName() : d->buildQName();
}
