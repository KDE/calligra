/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPRINTJOB_H
#define KOPRINTJOB_H

#include <QAbstractPrintDialog>
#include <QList>
#include <QObject>
#include <QPrinter>

#include "komain_export.h"

class QWidget;

/**
 * A print job is an interface that the KoView uses to create an application-specific
 * class that can take care of printing.
 * The printjob should be able to print again after a print job has been completed,
 * using the same QPrinter to allow the user to alter settings on the QPrinter and
 * call print again.
 * The printjob can thus see startPrinting() called more than once, and the implementation
 * of that signal should honor the removePolicy passed to it.
 */
class KOMAIN_EXPORT KoPrintJob : public QObject
{
    Q_OBJECT
public:
    /**
     * Constructor.
     * @param parent the parent qobject that is passed for memory management purposes.
     */
    explicit KoPrintJob(QObject *parent = nullptr);
    ~KoPrintJob() override;

    /// A policy to allow the printjob to delete itself after its done printing.
    enum RemovePolicy {
        DeleteWhenDone, ///< Delete the job when its done with printing.
        DoNotDelete ///< Keep the job around so it can be started again.
    };

    /// Returns the printer that is used for this print job so others can alter the details of the print-job.
    virtual QPrinter &printer() = 0;
    /// If this print job is used in combination with a printdialog the option widgets this method
    /// returns will be shown in the print dialog.
    virtual QList<QWidget *> createOptionWidgets() const = 0;

    virtual int documentFirstPage() const
    {
        return 1;
    }
    virtual int documentLastPage() const
    {
        return 1;
    }
    virtual int documentCurrentPage() const
    {
        return 1;
    }

    virtual QAbstractPrintDialog::PrintDialogOptions printDialogOptions() const;

    /**
     *@brief Check if the painter can print to the printer
     *@returns true if the print job can print to the given printer
     */
    virtual bool canPrint();

public Q_SLOTS:
    /**
     * This is called every time the job should be executed.
     * When called the document should be printed a new painter using the printer
     * of this printJob in order to honor the settings the user made on the printer.
     * canPrint() should be called before startPrinting to check if the painter can print
     * to the printer
     * @param removePolicy a policy that should be honored so the caller can make sure
     *   this job doesn't leak memory after being used.
     */
    virtual void startPrinting(KoPrintJob::RemovePolicy removePolicy = DoNotDelete);
};

#endif
