/*
 * This file is part of KSpread
 *
 * Copyright (c) 2005 Cyrille Berger <cberger@cberger.net>
 * Copyright (c) 2006 Isaac Clerencia <isaac@warp.es>
 * Copyright (c) 2006 Sebastian Sauer <mail@dipe.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef SCRIPTINGMODULE_H
#define SCRIPTINGMODULE_H

#include <QString>
#include <QStringList>
#include <QObject>
#include <KoScriptingModule.h>

namespace KSpread
{
class Doc;
class View;
class ViewAdaptor;
//class Sheet;
class SheetAdaptor;
}

/**
* The ScriptingModule class enables access to the KSpread
* functionality from within the scripting backends.
*/
class ScriptingModule : public KoScriptingModule
{
    Q_OBJECT
public:

    /**
    * Constructor.
    * \param parent The optional parent QObject instance
    * this QObject is child of.
    */
    ScriptingModule(QObject* parent = 0);

    /**
    * Destructor.
    */
    virtual ~ScriptingModule();

    /**
    * \return The \a KSpread::View instance this module provides access to or
    * NULL if this module just doesn't know anything about a view. This can
    * be for example the case if the KSpread scripting functionality is used
    * without running a KSpread instance.
    */
    KSpread::View* kspreadView();

    /**
    * \return The \a KSpread::Doc instance that provides access to a
    * document. This method always returns a valid \a KSpread::Doc instance.
    */
    KSpread::Doc* kspreadDoc();

    /**
    * \return The \a KoDocument instance that provides access to a
    * document. This method is essential the same as the kspreadDoc()
    * method above since a \a KSpread::Doc just extends a \a KoDocument .
    *
    * This method overwrites the one from the \a KoScriptingModule class
    * to allow us to return our \a KSpread::Doc here.
    */
    virtual KoDocument* doc();

public slots:

    /**
    * Returns the \a KSpread::MapAdaptor object.
    */
    QObject* map();

    /**
    * Returns the \a KSpread::ViewAdaptor object in which the document is
    * displayed. Such a ViewAdaptor is only available if the script runs
    * embedded in a running KSpread instance. If the script runs for example
    * from within the commandline by using the kross-application there is no
    * View and therefore no ViewAdaptor and this method returns NULL.
    */
    QObject* view();

    /**
    * Returns the \a KSpread::SheetAdaptor object currently active in the
    * document.
    */
    QObject* currentSheet();

    /**
    * Returns a \a KSpread::SheetAdaptor object by the name \p name . The name
    * should be listened in the list returned by the \a sheetNames() method.
    * If there exists no sheet with such a name NULL is returned.
    */
    QObject* sheetByName(const QString& name);

    /**
    * Returns a list of the sheet names. The \a sheetByName method could then
    * be used to access the sheet object who's name is in the list.
    */
    QStringList sheetNames();

    /**
    * Returns true if there is a \a ScriptingFunction object
    * known under the identifier \p name .
    */
    bool hasFunction(const QString& name);

    /**
    * Returns the \a ScriptingFunction object with the identifier \p name . The
    * \a ScriptingFunction provides access to the KSpread formula function
    * functionality. If there is no \a ScriptingFunction known yet with the
    * identifier \p name then a new one is created, remembered and returned.
    */
    QObject* function(const QString& name);

    /**
    * Returns a \a ScriptingCellListener object which provides us some kind of
    * listener to changes in cells.
    */
    QObject* createListener(const QString& sheetname, const QString& range = QString());

    /**
    * Set the document-content to the as argument passed XML string.
    */
    bool fromXML(const QString& xml);

    /**
    * Return the document-content as XML string.
    */
    QString toXML();

    /**
    * Open a document from the defined \p url .
    */
    bool openUrl(const QString& url);

    /**
    * Save the current document to the defined \p url .
    */
    bool saveUrl(const QString& url);

    /**
    * Import a document from the defined \p url .
    */
    bool importUrl(const QString& url);

    /**
    * Export the current document to the defined \p url .
    */
    bool exportUrl(const QString& url);

    /**
    * Create and return a new \a ScriptingReader object that provides us an
    * abstract high-level API to read content from KSpread sheets.
    */
    QObject* reader();

    /**
    * Create and return a new \a ScriptingWriter object that provides us an
    * abstract high-level API to write content to KSpread sheets.
    */
    QObject* writer();

    /**
    * Create and return a new \a ScriptingSheetsListView widget instance which
    * could be used to select 0..n sheets from a list of all available sheets.
    */
    QWidget* createSheetsListView(QWidget* parent);

private:
    Q_DISABLE_COPY(ScriptingModule)

    /// \internal d-pointer class.
    class Private;
    /// \internal d-pointer instance.
    Private* const d;
};

#endif
