/*
 * This file is part of Maemo 5 Office UI for KOffice
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
 * Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>
 *
 * Contact: Manikandaprasad N C <manikandaprasad.chandrasekar@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef APPLICATION_CONTROLLER_H
#define APPLICATION_CONTROLLER_H

#include <KoAbstractApplicationController.h>

/*!
 * @brief implementation of the application's controller
 */
class ApplicationController : public KoAbstractApplicationController
{
    Q_OBJECT

public:
    ApplicationController(QObject *application);
    ~ApplicationController();

    /*!
     * Reimplemented for KoAbstractApplication.
     */
    virtual bool openDocuments(const KoAbstractApplicationOpenDocumentArguments& args);

    //! Implemented for KoAbstractApplicationController
    virtual void showMessage(KoAbstractApplication::MessageType type,
                             const QString& messageText = QString());

    //! Implemented for KoAbstractApplicationController
    virtual QMessageBox::StandardButton askQuestion(QuestionType type,
                                                    const QString& messageText = QString());

    //! Implemented for KoAbstractApplicationController
    virtual bool startNewInstance(const KoAbstractApplicationOpenDocumentArguments& args);

    //! Implemented for KoAbstractApplicationController
    virtual void showUiBeforeDocumentOpening(bool isNewDocument);

    //! Implemented for KoAbstractApplicationController
    virtual QString showGetOpenFileNameDialog(const QString& caption,
                                              const QString& dir, const QString& filter);

    //! Implemented for KoAbstractApplicationController
    virtual QString showGetSaveFileNameDialog(const QString& caption,
                                              const QString& dir, const QString& filter);

    //! Implemented for KoAbstractApplicationController
    virtual void setCentralWidget(QWidget *widget);

    //! Implemented for KoAbstractApplicationController
    virtual void updateActions();

    //! Implemented for KoAbstractApplicationController
    virtual void handleDocumentPageSetupChanged();

    //! Implemented for KoAbstractApplicationController
    virtual void handleCurrentPageChanged(int previousPage);
    
    //! Implemented for KoAbstractApplicationController
    virtual QString applicationName() const;

    //! Implemented for KoAbstractApplicationController
    virtual bool isVirtualKeyboardVisible() const;

    //! Implemented for KoAbstractApplicationController
    KoExternalEditorInterface* createExternalCellEditor(KoCellTool* cellTool) const;

public slots:
    //! Implemented for KoAbstractApplicationController
    virtual void setWindowTitle(const QString& title);

    //! Implemented for KoAbstractApplicationController
    virtual void setProgressIndicatorVisible(bool visible);

    //! Reimplemented for KoAbstractApplicationController
    virtual bool setEditingMode(bool set);

    //! Reimplemented for KoAbstractApplicationController
    virtual void closeDocument();

    //! Implemented for KoAbstractApplicationController
    virtual void setVirtualKeyboardVisible(bool set);

protected slots:
    /*!
     * Reimplemented for KoAbstractApplicationController
     */
    virtual bool openScheduledDocument();

    //! Reimplemented for KoAbstractApplicationController
    virtual void resourceChanged(int key, const QVariant& value);

private:
};

#endif // APPLICATION_CONTROLLER_H
