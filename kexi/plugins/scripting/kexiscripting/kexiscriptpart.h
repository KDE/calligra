/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2005 Sebastian Sauer <mail@dipe.org>

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

#ifndef KEXISCRIPTPART_H
#define KEXISCRIPTPART_H

#include <qdom.h>

#include <kexi.h>
#include <kexipart.h>
//#include <kexidialogbase.h>

/**
 * Kexi Scripting Plugin.
 */
class KexiScriptPart : public KexiPart::Part
{
    Q_OBJECT

public:

    /**
     * Constructor.
     *
     * \param parent The parent QObject this part is child of.
     * \param name The name this part has.
     * \param args Optional list of arguments passed to this part.
     */
    KexiScriptPart(QObject *parent, const QStringList& args);

    /**
     * Destructor.
     */
    virtual ~KexiScriptPart();

    /**
     * Implementation of the \a KexiPart::Part::execute method used to
     * execute the passed \p item instance.
     */
    virtual bool execute(KexiPart::Item* item, QObject* sender = 0);

    /**
     * \return the i18n message for the passed \p englishMessage string.
     */
    virtual KLocalizedString i18nMessage(const QString& englishMessage,
                                         KexiWindow* window) const;

protected:

    /**
     * Create a new view.
     *
     * \param parent The parent QWidget the new view is displayed in.
     * \param window The \a KexiWindow the view is child of.
     * \param item The \a KexiPart::Item this view is for.
     * \param viewMode The viewmode we like to have a view for.
     * \param staticObjectArgs Optional list of arguments.
     */
    virtual KexiView* createView(QWidget *parent,
                                 KexiWindow *window,
                                 KexiPart::Item &item,
                                 Kexi::ViewMode viewMode = Kexi::DataViewMode,
                                 QMap<QString, QVariant>* staticObjectArgs = 0);
    /*
             * \param parent The parent QWidget the new view is displayed in.
             * \param dialog The \a KexiDialogBase the view is child of.
             * \param item The \a KexiPart::Item this view is for.
             * \param viewMode The viewmode we like to have a view for.
                                             QWidget *parent,
                                             KexiDialogBase* dialog,
                                             KexiPart::Item& item,
                                             Kexi::ViewMode viewMode = Kexi::DesignViewMode,
                                             QMap<QString,QVariant>* staticObjectArgs = 0);
    */

    /**
     * Initialize the part's actions.
     */
    virtual void initPartActions();

    /**
     * Initialize the instance actions.
     */
    virtual void initInstanceActions();

private:
    /// \internal d-pointer class.
    class Private;
    /// \internal d-pointer instance.
    Private* const d;
};

#endif

