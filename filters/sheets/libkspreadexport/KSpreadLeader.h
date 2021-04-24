/*
This file is part of the KDE project
SPDX-FileCopyrightText: 2002 Fred Malabre <fmalabre@yahoo.com>

SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KSPREAD_LEADER_H
#define KSPREAD_LEADER_H

#include <kspreadbaseworker.h.h>
#include <KoFilterChain.h>
#include <kdebug.h>
#include <kspread_doc.h>
#include <KoDocumentInfo.h>
#include <kspread_map.h>

namespace KSpread
{

/**
* This class is a leader which will call the callbacks
* to the worker with your own implementation for your export filter.
*
* To write an export filter, just instantiate a leader and
* implement you own worker based on the BaseWorker.
*
* @short A Leader for an KSpread export filter
* @author Fred Malabre
* @see KSpreadBaseWorker
* @see KoFilter
*/
class Leader
{
public:
    /**
    * Default constructor.
    * Call setWorker() before starting to convert the KSpread document.
    *
    * @param filterChain the filter chain of the filter using the lib.
    */
    explicit Leader(KoFilterChain *filterChain);

    /**
    * This constructor set up the Worker at initialization.
    * convert can be called right away after instantiation of the Leader.
    *
    * @param filterChain the filter chain of the filter using the lib.
    * @param newWorker implementation of a class derived from KSpreadWorker.
    */
    Leader(KoFilterChain *filterChain, KSpreadBaseWorker *newWorker);

    /**
    * Default destructor.
    * It does nothing.
    */
    virtual ~Leader();

private:
    KSpreadBaseWorker *m_worker;
    KoFilterChain *m_filterChain;
    int m_maxCellColumn;
    int m_maxCellRow;
    void updateMaxCells(KSpreadSheet *spreadSheet);

protected:
    KSpreadBaseWorker *getWorker() const;
    KoFilter::ConversionStatus doSpreadCell(const Cell& spreadCell, int column, int row);
    KoFilter::ConversionStatus doSpreadSheet(Sheet *spreadSheet);
    KoFilter::ConversionStatus doSpreadBook(Doc *document);
    KoFilter::ConversionStatus doInfo(KoDocumentInfo *info);

public:
    /**
    * Set your own implementation of the Worker.
    *
    * @param newWorker implementation of a class derived from KSpreadWorker.
    */
    void setWorker(KSpreadBaseWorker *newWorker);

    /**
    * Start the conversion process.
    * The callbacks of your Worker will be called at this time.
    *
    * @return status of the conversion.
    */
    KoFilter::ConversionStatus convert();
};

} // namespace KSpread

#endif /* KSPREAD_LEADER_H */
