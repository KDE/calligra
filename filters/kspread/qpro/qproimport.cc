
/* This file is part of the KDE project
   Copyright (C) 2001 Graham Short <grahshrt@netscape.net>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <strstream.h>

#include <qmessagebox.h>
#include <kmessagebox.h>

#include <kspread_doc.h>
#include <kspread_table.h>
#include <kspread_cell.h>

#include <qproimport.h>

#include <qproformula.h>
#include <qpro/stream.h>
#include <qpro/record_factory.h>

// ---------------------------------------------------------------

QpTableList::QpTableList()
{
   for( int lIdx=0; lIdx<cNameCnt; ++lIdx )
   {
      cTable[lIdx] = 0;
   }
}

QpTableList::~QpTableList()
{
   // don't delete the list of tables
}


void
QpTableList::table(unsigned pIdx, KSpreadTable* pTable)
{
   if(pIdx < cNameCnt)
   {
      cTable[pIdx] = pTable;
   }
}

KSpreadTable*
QpTableList::table(unsigned pIdx)
{
   return (pIdx < cNameCnt ? cTable[pIdx] : 0);
}


// ---------------------------------------------------------------

QpImport::QpImport( KoFilter*   pParent
                  , const char* pName
                  )
 : KoFilter(pParent, pName)
{
//cout << "Hooray - in QpImport::QpImport" << endl; // ???
}

void
QpImport::InitTableName(int pIdx, QString& pResult)
{
   if( pIdx < 26 )
   {
      pResult = (char)('A' + pIdx);
   }
   else
   {
      pResult = (char)('A' -1 + pIdx / 26);
      pResult += (char)('A' + pIdx % 26);
   }
}

bool QpImport::filterImport(const QString &file
                           ,KoDocument *document
                           ,const QString &from
                           ,const QString &to
                           ,const QString &/*config*/
                           )
{
    bool bSuccess=true;

    kdDebug(30501) << "here we go... " << document->className() << endl;

    if(strcmp(document->className(), "KSpreadDoc")!=0)  // it's safer that way :)
    {
        kdWarning(30501) << "document isn't a KSpreadDoc but a " << document->className() << endl;
        return false;
    }
    if(from!="application/x-quattropro" || to!="application/x-kspread")
    {
        kdWarning(30501) << "Invalid mimetypes " << from << " " << to << endl;
        return false;
    }

    kdDebug(30501) << "...still here..." << endl;

    // No need for a dynamic cast here, since we use Qt's moc magic
    KSpreadDoc *ksdoc=(KSpreadDoc*)document;

    if(ksdoc->mimeType()!="application/x-kspread")
    {
        kdWarning(30501) << "Invalid document mimetype " << ksdoc->mimeType() << endl;
        return false;
    }

    QpIStream lIn( (const char*)file.utf8() );

    if( !lIn )
    {
        KMessageBox::sorry( 0L, i18n("QPRO filter can't open input file - please report.") );
        return false;
    }

    KSpreadTable *table=0;
//    KSpreadTable *table=ksdoc->createTable();
//    ksdoc->addTable(table);

    QString field = "";
    int value=0;
    emit sigProgress(value);

   QpRecFactory            lFactory(lIn);
   QpTableList             lTableNames;
   QP_UINT8                lPageIdx = 0;

   QpRec*                  lRec = 0;
   QpRecBop*               lRecBop = 0;
   QpRecIntegerCell*       lRecInt = 0;
   QpRecFloatingPointCell* lRecFloat = 0;
   QpRecFormulaCell*       lRecFormula = 0;
   QpRecLabelCell*         lRecLabel = 0;
   QpRecPageName*          lRecPageName = 0;

   do
   {
      field = "";
      lRec  = lFactory.nextRecord();

      switch( lRec->type() )
      {
      case QpBop:
         lRecBop = (QpRecBop*)lRec;
         lPageIdx = lRecBop->pageIndex();

         // find out if we know about this table already, if not create it
         table=lTableNames.table(lPageIdx);

         if( table == 0 )
         {
            table=ksdoc->createTable();
            // set up a default name for the table
            table->setTableName( lTableNames.name(lPageIdx)
                               , TRUE
                               );
            lTableNames.table(lPageIdx, table);
            ksdoc->addTable(table);
         }
         break;

      case QpIntegerCell:
         lRecInt = (QpRecIntegerCell*)lRec;
         field.setNum( lRecInt->integer() );
//cout << "Setting R " << lRecInt->row()+1 << ", C " << ((unsigned)lRecInt->column()) << endl;
         table->setText( lRecInt->row()+1, ((unsigned)lRecInt->column())+1, field, false );
         break;

      case QpFormulaCell:
         lRecFormula = (QpRecFormulaCell*)lRec;
         {
            KSpreadFormula lAnswer(*lRecFormula, lTableNames);

            char*     lFormula = lAnswer.formula();

            field = lFormula;

            delete [] lFormula;
         }

         // check for referenced tables that haven't been created yet
         for(unsigned lIdx=0; lIdx<lTableNames.cNameCnt; ++lIdx)
         {
            if(lTableNames.allocated(lIdx) && (lTableNames.table(lIdx) == 0) )
            {
               // we're about to reference a table that hasn't been created yet.
               // setText gets upset about this, so create a blank table

               KSpreadTable* lNewTable=ksdoc->createTable();

               // set up a default name for the table
               lNewTable->setTableName( lTableNames.name(lIdx)
                                      , TRUE
                                      );
               lTableNames.table(lIdx, lNewTable);
               ksdoc->addTable(lNewTable);
            }
         }

         table->setText( lRecFormula->row()+1, lRecFormula->column()+1, field, false );
         break;

      case QpFloatingPointCell:
         lRecFloat = (QpRecFloatingPointCell*)lRec;
         field.setNum( lRecFloat->value() );
         table->setText( lRecFloat->row()+1, lRecFloat->column()+1, field, false );
         break;

      case QpLabelCell:
         lRecLabel = (QpRecLabelCell*)lRec;
         field = "'";
         field += lRecLabel->label();
         table->setText( lRecLabel->row()+1, lRecLabel->column()+1, field, false );
         break;

      case QpPageName:
         lRecPageName = (QpRecPageName*)lRec;

         if( lTableNames.allocated(lPageIdx) && lTableNames.table(lPageIdx) )
         {
            lTableNames.table(lPageIdx)->setTableName( lRecPageName->pageName()
//                                                     , TRUE
                                                     );
            lTableNames.name(lPageIdx, lRecPageName->pageName());
         }
         break;

      case QpPassword:
        KMessageBox::sorry( 0L, i18n("Sorry, can't open password protected files.\n"
                                     "The password algorithm has not been published")
                          );
        return false;
      }

      delete lRec;
      lRec = 0;
   } while( lIn );

    emit sigProgress(100);
    return bSuccess;
}

#include <qproimport.moc>
