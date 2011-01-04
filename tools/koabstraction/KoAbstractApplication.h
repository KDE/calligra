/* This file is part of the KDE project
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
 * Copyright (C) 2010 Jarosław Staniek <staniek@kde.org>
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

#ifndef KOABSTRACTAPPLICATION_H
#define KOABSTRACTAPPLICATION_H

#include "KoAbstractApplicationController.h"

//! Intermediate class defining slots for custom office application.
//! Do not inherit it, instead define KoAbstractApplicationBase typedef,
//! e.g. as QMainWindow and include KoAbstractApplication.h header.
class KoAbstractApplication : public KoAbstractApplicationBase, public KoAbstractApplicationController
{
    Q_OBJECT
public:
    KoAbstractApplication(QWidget *parent = 0);
    ~KoAbstractApplication();

public slots:
    /*!
     * Convenience method: opens one document pointed by @a fileName for viewing.
     */
    virtual bool openDocument(const QString &fileName);

    /*!
     * Convenience method: opens one document pointed by @a fileName for editing as template.
     */
    virtual bool openDocumentAsTemplate(const QString &fileName);

    /*!
     * Opens documents as specified by @a args.
     * See documentation of @a KoAbstractApplicationOpenDocumentArgumentsfor explanation
     * of arguments @a args.
     */
    virtual bool openDocuments(const KoAbstractApplicationOpenDocumentArguments& args);

    /*!
     * Shows or hides virtual keyboard.
     */
    void toggleVirtualKeyboardVisibility();

signals:
    /*!
     * Presentation has entered full screen mode.
     */
    void presentationStarted();
    /*!
     * Presentation has exited from full screen mode.
     */
    void presentationStopped();
    /*!
     * Presentation has moved to the next slide.
     */
    void nextSlide();
    /*!
     * Presentation has moved to the previous slide.
     */
    void previousSlide();

protected:
    virtual void closeEvent(QCloseEvent *event);

protected slots:
    // -- slots for possible reimplementation, do not forget to call superclass --

    /*!
     * Goes to particular page @a page.
     */
    void goToPage(int page);

    /*!
     * Goes to previous page.
     */
    void goToPreviousPage();

    /*!
     * Goes to next page.
     */
    void goToNextPage();

    /*!
     * Called whenever page setup for document changes.
     * @see document(), KWDocument::pageSetupChanged()
     */
    void documentPageSetupChanged();

    /*!
     * Closes document.
     */
    virtual void closeDocument();

    /*!
     * Shows open document window. After accepting returns immediately and real opening will be executed in 100 ms.
     * @return true if document is about to be opened.
     */
    virtual bool openDocument();

    /*!
     * Opens document.
     */
    virtual bool doOpenDocument();

    /*!
     * Saves document. Calls saveDocumentAs() if needed.
     * @return true if document has been saved.
     */
    virtual bool saveDocument();

    /*!
     * Saves document under name selected by user.
     * @return true if document has been saved.
     */
    virtual bool saveDocumentAs();

    //! Implemented for KoAbstractApplicationController
    virtual void goToNextSlide();

    //! Implemented for KoAbstractApplicationController
    virtual void goToPreviousSlide();

    //! Implemented for KoAbstractApplicationController
    virtual void resourceChanged(int key, const QVariant &value);

    //! Implemented for KoAbstractApplicationController
    void addSheet();

    //! Implemented for KoAbstractApplicationController
    void removeSheet();

    //! Implemented for KoAbstractApplicationController
    bool setEditingMode(bool set);

    void showEditingMode() { setEditingMode(true); }

    //! Implemented for KoAbstractApplicationController
    virtual void activeToolChanged(KoCanvasController* canvas, int uniqueToolId);
};

#endif
