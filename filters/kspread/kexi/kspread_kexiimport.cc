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

#include "kspread_kexiimport.h"

#include "kspread_kexiimportdialog.h"

//KSpread Includes
#include <kspread_view.h>
#include <kspread_doc.h>
//#include <selection.h>
#include <kspread_sheet.h>
#include <kspread_map.h>
#include <kspread_style.h>

//Koffice Includes
#include <KoFilterChain.h>
#include <KoFilterManager.h>
#include <kofficeversion.h>

//Kexi Includes
#include <kexidb/cursor.h>
#include <kexidb/connection.h>
#include <kexidb/parser/parser.h>

//KDE Includes
#include <kgenericfactory.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <klistview.h>
#include <qcolor.h>

typedef KGenericFactory<KSpreadKexiImport, KoFilter> KSpreadKexiImportFactory;
K_EXPORT_COMPONENT_FACTORY( libkspreadkexiimport, KSpreadKexiImportFactory( "kofficefilters" ) )


//=============================================================================

KSpreadKexiImport::KSpreadKexiImport(KoFilter *parent, const char *name, const QStringList&)
{
}

//=============================================================================

KSpreadKexiImport::~KSpreadKexiImport()
{
    delete m_dialog;
}

//=============================================================================

KoFilter::ConversionStatus KSpreadKexiImport::convert( const QCString& from, const QCString& to )
{
    QPtrList<QListViewItem> objects;
    QString file( m_chain->inputFile() );

    //Create dialog
    m_dialog = new KSpreadKexiImportDialog();

    m_dialog->openDatabase( file, 0);
    if (!m_dialog->exec())
    {
        return KoFilter::UserCancelled;
    }

    objects = m_dialog->selectedItems();

    kdDebug() << "Getting Documents..." << endl;

    KoDocument* document = m_chain->outputDocument();

    if ( !document )
        return KoFilter::StupidError;

    kdDebug() << "here we go... " << document->className() << endl;

    if ( !::qt_cast<const KSpread::Doc *>( document ) )
    {
        kdWarning() << "document isn't a KSpread::Doc but a " << document->className() << endl;
        return KoFilter::NotImplemented;
    }
    if(from!="application/x-kexiproject-sqlite3" || to!="application/x-kspread")
    {
        kdWarning() << "Invalid mimetypes " << from << " " << to << endl;
        return KoFilter::NotImplemented;
    }

    ksdoc = static_cast<KSpread::Doc *>( document ); // type checked above

    if(ksdoc->mimeType()!="application/x-kspread")
    {
        kdWarning() << "Invalid document mimetype " << ksdoc->mimeType() << endl;
        return KoFilter::NotImplemented;
    }
    
    document->emitBeginOperation();
    
    QListViewItem *itm;
    for(itm = objects.first(); itm ; itm = objects.next())
    {
        if (!insertObject(itm->text(1), itm->text(0)))
        {
            return KoFilter::StupidError;
        }
    }
    if (m_dialog->customQuery())
    {
        if (!insertObject(m_dialog->customQueryString(), "Custom"))
        {
            return KoFilter::StupidError;
        }
    }

    document->emitEndOperation();
    kdDebug() << "inserting kexi data completed" << endl;
    return KoFilter::OK;
}

bool KSpreadKexiImport::insertObject(const QString& object, const QString& type)
{
    QStringList fieldNames;
    KSpread::Sheet *sheet;
    KexiDB::Parser *parser;
    KexiDB::QuerySchema *query;
    
    sheet = ksdoc->map()->addNewSheet();
    if (!sheet)
    {
        KMessageBox::error(NULL, i18n("Cant find sheet"), i18n("Error"));
        return false;
    }

    if (type == "Custom")
    {
        sheet->setSheetName("Custom");
    }
    else
    {
        sheet->setSheetName(object);
    }

    //Get the field names
    if (type == "Table")
    {
        fieldNames = m_dialog->connection()->tableSchema(object)->names();
    }
    else if (type == "Query")
    {
        //Note....for queries such as select * from....field names are not available
        fieldNames = m_dialog->connection()->querySchema(object)->names();
    }
    else if (type == "Custom") //Custom Query
    {
        parser = new KexiDB::Parser(m_dialog->connection());
        parser->parse( object );
        
        if (parser->operation() != KexiDB::Parser::OP_Select)
        {
            KMessageBox::error(NULL, i18n("Your query was invalid or not a SELECT query"), i18n("Error"));
            return false;
        }
        query = parser->query();
        fieldNames = query->names();
    }

    //Insert the field headings
    QStringList::iterator it;
    int i = 1;
    for (it = fieldNames.begin(); it != fieldNames.end(); ++it, ++i)
    {
        KSpread::Cell *c = sheet->nonDefaultCell(i ,1);
        c->setCellText(*it, true);
        c->format()->setBgColor(QColor(200,200,200));
        c->format()->setTextFontBold(true);
    }

    //Insert the data
    KexiDB::Cursor *cur;
    if (type == "Table") //Table
    {
        cur = m_dialog->connection()->executeQuery( *(m_dialog->connection()->tableSchema(object)));
    }
    else if (type == "Query") //Query
    {
        cur = m_dialog->connection()->executeQuery( *(m_dialog->connection()->querySchema(object)));
    }
    else if (type == "Custom") //Custom Query
    {
        cur = m_dialog->connection()->executeQuery( *query );
    }
    else
    {
        cur = 0;
    }

    if (!cur)
    {
        KMessageBox::error(NULL, i18n("Error executing query"), i18n("Error"));
        return false;
    }

    cur->moveFirst();
    int j = cur->fieldCount();

    unsigned int k = 2; //start on row 2
    bool asText = false;

    if(m_dialog->conversion() == 2)
        asText = true;

    while (!cur->eof())
    {
        for (i = 0; i < j; ++i)
        {
            sheet->nonDefaultCell(i+1 ,k)->setCellText(cur->value(i).toString(), asText);
        }

        kdDebug() << k << endl;
        cur->moveNext();
        ++k;
    }
    return true;
}
#include "kspread_kexiimport.moc"
