/***************************************************************************
 *   Copyright (C) 2006 by Adam Pigg                                       *
 *   adam@piggz.co.uk                                                      *
 *                                                                         *
 *   Based on insert calendar code:                                        *
 *   Copyright (C) 2005 by Raphael Langerhorst                             *
 *   raphael-langerhorst@gmx.at                                            *
 *                                                                         *
 *   Permission is hereby granted, free of charge, to any person obtaining *
 *   a copy of this software and associated documentation files (the       *
 *   "Software"), to deal in the Software without restriction, including   *
 *   without limitation the rights to use, copy, modify, merge, publish,   *
 *   distribute, sublicense, and/or sell copies of the Software, and to    *
 *   permit persons to whom the Software is furnished to do so, subject to *
 *   the following conditions:                                             *
 *                                                                         *
 *   The above copyright notice and this permission notice shall be        *
 *   included in all copies or substantial portions of the Software.       *
 *                                                                         *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       *
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    *
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR     *
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR *
 *   OTHER DEALINGS IN THE SOFTWARE.                                       *
 ***************************************************************************/

#ifndef KSPREAD_KEXIIMPORT_H
#define KSPREAD_KEXIIMPORT_H

#include <KoFilter.h>

class KAboutData;

namespace KexiDB
{
    class Connection;
}

namespace KSpread
{
    class Doc;    
}

class KSpreadKexiImportDialog;

/**
 * \class PluginInsertKexi kspread_plugininsertkexi.h
 * \brief Plugin for inserting a kexi table/query into a spreadsheet.
 * @author Adam Pigg
 *
 * The plugin class for the Insert Kexi plugin.
 * This plugin is designed to work in KSpread and
 * makes it possible to insert kexi tables/queries into
 * the spreadsheet.
 */
class KSpreadKexiImport : public KoFilter
{
  Q_OBJECT

  protected:
      
    KSpreadKexiImportDialog* m_dialog;

  public:

    /**
     * Constructor. This constructor is usable with KGenericFactory.
     */
      KSpreadKexiImport(KoFilter *parent, const char *name, const QStringList&);

    /**
     * Virtual destructor.
     */
      virtual ~KSpreadKexiImport();

    virtual KoFilter::ConversionStatus convert( const QCString& from, const QCString& to );
    
    private:
        KSpread::Doc *ksdoc;
        bool insertObject(const QString&,const QString&);
};

#endif
