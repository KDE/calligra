/* This file is part of the KDE project
   Copyright 2004 Ariya Hidayat <ariya@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "tester.h"
#include "stylecluster_tester.h"
#include "stylecluster.h"

#include "kspread_doc.h"
#include "kspread_map.h"
#include "kspread_style_manager.h"
#include "kspread_style.h"
#include "kspread_sheet.h"

#include <kspread_value.h>



#define CHECK_STYLE(x,y)  check_ptr(__FILE__,__LINE__,#x,x,y)
#define CHECK_QUAD(x,y)  check_value(__FILE__,__LINE__,#x,x,y)
#define CHECK_FAILS_QUAD(x,y)  check_fails_value(__FILE__,__LINE__,#x,x,y)


using namespace KSpread;

StyleClusterTester::StyleClusterTester(): Tester()
{
}

QString StyleClusterTester::name()
{
  return QString("Style Cluster");
}

void StyleClusterTester::check_value( const char *file, int line, const char* msg, void * result, 
  void * expected )
{
  testCount++;
  if( result != expected )
  {
    QString message;
    QTextStream ts( &message, IO_WriteOnly );
    ts << msg;
    ts << "  Result:";
    ts << result;
    ts << ", ";
    ts << "Expected:";
    ts << expected;
    kdDebug() << message << endl;
    fail( file, line, message );
  }
}

void StyleClusterTester::check_fails_value( const char *file, int line, const char* msg, void * result, 
  void * expected )
{
  testCount++;
  if( result == expected )
  {
    QString message;
    QTextStream ts( &message, IO_WriteOnly );
    ts << msg;
    ts << "  Result shouldn't be:";
    ts << result;
    kdDebug() << message << endl;
    fail( file, line, message );
  }
}

template<typename T>
void StyleClusterTester::check_ptr( const char *file, int line, const char* msg, const T &result, 
  const T &expected )
{
  testCount++;
  if( &result != &expected )
  {
    QString message;
    QTextStream ts( &message, IO_WriteOnly );
    ts << msg;
    ts << "  Result:";
    ts << &result;
    ts << ", ";
    ts << "Expected:";
    ts << &expected;
    fail( file, line, message );
  }
}

void StyleClusterTester::run()
{
  testCount = 0;
  errorList.clear();
  
  KSpreadDoc* doc = new KSpreadDoc();
  KSpreadMap* workbook = doc->map();
  KSpreadSheet* sheet = workbook->addNewSheet();
  
  KSpreadStyle* defaultStyle = doc->styleManager()->defaultStyle();

  StyleCluster stylecluster(sheet);
  CHECK_STYLE(stylecluster.lookup(0,0), static_cast< const KSpreadStyle& > (*defaultStyle));
  CHECK_STYLE(stylecluster.lookup(1000,2000), static_cast< const KSpreadStyle& > (*defaultStyle));
  KSpreadStyle *style1 = new KSpreadStyle();
  style1->addRef();
  stylecluster.setStyle(1000,2000, style1);
  CHECK_STYLE(stylecluster.lookup(1000,2000), *style1); 
  CHECK_STYLE(stylecluster.lookup(1001,2000), static_cast< const KSpreadStyle& > (*defaultStyle));
  CHECK_STYLE(stylecluster.lookup(1001,2001), static_cast< const KSpreadStyle& > (*defaultStyle));
  CHECK_STYLE(stylecluster.lookup(1000,2001), static_cast< const KSpreadStyle& > (*defaultStyle));
  CHECK_STYLE(stylecluster.lookup(0,0), static_cast< const KSpreadStyle& > (*defaultStyle));
  
  void *quad1 = stylecluster.lookupNode(0,0); 
  CHECK_QUAD(stylecluster.lookupNode(0,0), quad1);
  CHECK_FAILS_QUAD(stylecluster.lookupNode(0,0), stylecluster.lookupNode(1000,2000)); 
  stylecluster.setStyle(1000,2000, sheet->doc()->styleManager()->defaultStyle());
  CHECK_QUAD(stylecluster.lookupNode(0,0), stylecluster.lookupNode(1000,2000)); 
  
 
  stylecluster.setStyle(0,0, style1);
  stylecluster.setStyle(1,0, style1);
  stylecluster.setStyle(0,1, style1);
  CHECK_FAILS_QUAD(stylecluster.lookupNode(0,0), stylecluster.lookupNode(0,1));
  
  stylecluster.setStyle(1,1, style1);
  
  CHECK_QUAD(stylecluster.lookupNode(0,0), stylecluster.lookupNode(0,1));
  CHECK_QUAD(stylecluster.lookupNode(0,0), stylecluster.lookupNode(1,0));
  CHECK_QUAD(stylecluster.lookupNode(1,0), stylecluster.lookupNode(1,1));

  CHECK_STYLE(stylecluster.lookup(0,0), *style1);
  CHECK_STYLE(stylecluster.lookup(0,1), *style1);
  CHECK_STYLE(stylecluster.lookup(1,0), *style1);
  CHECK_STYLE(stylecluster.lookup(1,1), *style1);
  CHECK_STYLE(stylecluster.lookup(0,2), static_cast< const KSpreadStyle& > (*defaultStyle));

  stylecluster.setStyle(0,0, sheet->doc()->styleManager()->defaultStyle());
  CHECK_STYLE(stylecluster.lookup(0,0), static_cast< const KSpreadStyle& > (*defaultStyle));
  CHECK_STYLE(stylecluster.lookup(0,1), stylecluster.lookup(1,0));
  CHECK_STYLE(stylecluster.lookup(0,1), stylecluster.lookup(1,1));
  CHECK_STYLE(stylecluster.lookup(0,1), *style1);
  
  stylecluster.setStyle(0,1, sheet->doc()->styleManager()->defaultStyle());
  stylecluster.setStyle(1,0, sheet->doc()->styleManager()->defaultStyle());
  stylecluster.setStyle(1,1, sheet->doc()->styleManager()->defaultStyle());
  
//Quad tree should be totally empty now
  
  CHECK_STYLE(stylecluster.lookup(0,0), static_cast< const KSpreadStyle& > (*defaultStyle));
  CHECK_STYLE(stylecluster.lookup(1,1), static_cast< const KSpreadStyle& > (*defaultStyle));
  
  CHECK_QUAD(stylecluster.lookupNode(0,0), stylecluster.lookupNode(1,0));
  CHECK_QUAD(stylecluster.lookupNode(1,1), stylecluster.lookupNode(1005,20320));

//Quad tree is empty.  Add a big 4x4 group, and check that
  

  for(int y = 0; y < 64; y++) {
    for(int x = 0; x < 64; x++) {
      stylecluster.setStyle(x,y, style1);
    }
  }
  
  CHECK_QUAD(stylecluster.lookupNode(0,0), stylecluster.lookupNode(1,1));
  CHECK_QUAD(stylecluster.lookupNode(0,0), stylecluster.lookupNode(2,2));
  CHECK_QUAD(stylecluster.lookupNode(0,0), stylecluster.lookupNode(7,7));
  CHECK_QUAD(stylecluster.lookupNode(0,0), stylecluster.lookupNode(60,60));
 
  //Undo what we did
  for(int y = 0; y < 64; y++) {
    for(int x = 0; x < 64; x++) {
      stylecluster.setStyle(x,y, sheet->doc()->styleManager()->defaultStyle());
    }
  }

  
  if(style1->release()) {
    delete style1;
    style1 = 0;
    testCount++;
  } else {
    testCount++;
    fail( __FILE__, __LINE__, "Style was not correctly freed" );
  }


}


