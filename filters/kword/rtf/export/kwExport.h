#ifndef KWEXPORT_H
#define KWEXPORT_H

#include <qstring.h>
#include <qdom.h>
#include <kdebug.h>
#include <qdir.h>
#include <tagprocessing.h>
#include <qvaluelist.h>

#define KDEBUG_KWFILTER 30505




/***************************************************************************/

// Reportedly, the capability to set tabs will be taken out of kword
// at some time in the future.
// Note from David Faure: of course not ! Just needs to be reimplemented, that's all :) 

class TabularData
   {
   public:
   TabularData() { ptpos = -1; type = -1;} // constructor

   int ptpos;  // position of the tab stop in pts
   int type;  // alignment of data - 0 = left, 1 = center, 2 = right, 3 = decimal
   }; // end TabularD

/***************************************************************************/
// Used to store LEFTBORDER, RIGHTBORDER, TOPBORDER, BOTTOMBORDER  tag data
class BorderStyle
   {
   public:
   // default constructor
   BorderStyle() { red = -1; green = -1; blue = -1; style = -1, width = -1;}

   int red;
   int green;
   int blue;
   int style;  // 0 = solid line, 1 = dash, 2 = dots, 3 = pattern, 4 = pattern
   int width;  // width in points

   }; // end BorderStyle class

/***************************************************************************/
// The following class is used to store PAPER tag data.

class PaperAttributes
   {
   public:
   // default constructor
   PaperAttributes() { format = -1; width = -1; height = -1; orientation = -1;
                       columns = -1; colSpacing = -1; hType = -1; fType = -1;
                       spHeadBody = -1; spFootBody = -1;}

   int format;  // Paper size code A4, Letter, etc.
   int width;  // in pts
   int height;  // in pts
   int orientation; // 0 = portrait, 1 = landscape
   int columns;  // no. of columns
   int colSpacing;  // in mm
   int hType;  // Header type
   int fType;  // Footer type
   int spHeadBody;
   int spFootBody;


   };  // end PaperAttributes class

/***************************************************************************/
// The following class is used to store PAPERBORDERS tag data.

class PaperBorders
   {
   public:

   // default constructor
   PaperBorders() { left = -1; right = -1; bottom = -1; top = -1;}

   int left;
   int right;
   int bottom;
   int top;

   }; // end PaperBorders class

/***************************************************************************/
// The following class is used to store ATTRIBUTES tag data.

class Attributes
   {
   public:

   // default constructor
   Attributes() { processing = -1; standardpage = -1; hasHeader = -1; hasFooter = -1;}

   int processing;  // 0 = word processing doc, 1 = DTP doc
   int standardpage;
   int hasHeader;  // (0/1) for header
   int hasFooter;  // (0/1) for footer
   QString unit;   // basic unit for ruler "mm" etc.

   };  // end Attributes class

/***************************************************************************/

class TextFormatting
// Container for text format portion of format
{
   public:
      TextFormatting () {formatId = 1; pos = -1; len = -1; fontSize = -1; fontWeight = -1;
                         fontName = ""; italic = false; underline = false; strikeout = false;
                         vertalign = -1; red = 0; blue = 0; green = 0;}

      TextFormatting ( int     id,
                       int     p,
                       int     l,
                       int     s,
                       int     w,
                       QString f,
                       bool    i,
                       bool    u,
                       bool    so,
                       int     v,
                       int     r,
                       int     b,
                       int     g ) : formatId (id), pos (p), len (l), fontSize (s), fontWeight (w),
                       fontName (f), italic (i), underline (u), strikeout (so), vertalign(v),
                       red (r), blue (b), green (g) {}

      int     formatId;
      int     pos;
      int     len;
      int     fontSize;
      int     fontWeight;
      QString fontName;
      bool    italic;
      bool    underline;
      bool    strikeout;
      int     vertalign;
      int     red;
      int     blue;
      int     green;
};

/***************************************************************************/

class ColorLayout
   {
   public:

   int red;
   int green;
   int blue;
   };

/***************************************************************************/

class PictureAnchor
// container for picturn anchor tag name and position
{
   public:
      PictureAnchor () {}

      PictureAnchor ( int     p,
                      QString k  ) : pos (p), key (k) {}

      int     pos;
      QString key;
};

/***************************************************************************/

class TableAnchor
// container for table anchor tag name and position
{
   public:
      TableAnchor () {pos = -1; name = "";}

      TableAnchor ( int     p,
                    QString n  ) : pos (p), name (n) {}

      int     pos;
      QString name;
};

/***************************************************************************/

struct ParaLayout
// container for paragraph type, flow and counter information
{
      // default constructor
      ParaLayout() {type = -1; depth = -1; start = -1; lefttext = "";
                    righttext = ""; layout = ""; flow = ""; idFirst = -1;
                    idLeft = -1; idRight = -1; lineSpacing = -1; }

      int     type;      // indicator for enum type - numeric, alpha, roman num, etc.
      int     depth;     // numbering depth 1, 1.1, 1.1.1, etc
      int     start;     // starting number
      QString lefttext;  // text left of the enumerator
      QString righttext; // text right of the enumerator
      QString layout;    // Kword paragraph style
      QString flow;      // right, left, center, justify
      int     idFirst;   // amount of the first line indentation
      int     idLeft;    // indentation of the remaining lines
      int     idRight;   // right indentation
      int     lineSpacing; // in points
      BorderStyle leftBorder;  // Paragraph border information
      BorderStyle rightBorder;
      BorderStyle topBorder;
      BorderStyle bottomBorder;
      QValueList < TabularData > tabularData; // Data on tab settings
};  // end struct ParaLayouts


/***************************************************************************/

class FormatData
// Container for the three classes
{
    public:
        FormatData () {}

        FormatData ( const TextFormatting &t ) : id (1), text          (t) {}
        FormatData ( const PictureAnchor  &p ) : id (2), pictureAnchor (p) {}
        FormatData ( const TableAnchor    &t ) : id (6), tableAnchor   (t) {}
        FormatData ( int i ) : id (i) {}

        int              id;

        TextFormatting   text;
        PictureAnchor    pictureAnchor;
        TableAnchor      tableAnchor;
};
/***************************************************************************/

struct Frame
// structure for FRAME tag information.
{
      // default constructor
      Frame() {left = -1; right = -1; top = -1; bottom = -1;
               runaround = -1; runaroundGap = -1; autoCreateNewFrame = -1;
               newFrameBehaviour = -1; sheetSide = -1; }

      int left;
      int right;
      int top;
      int bottom;
      int runaround;
      int runaroundGap;
      int autoCreateNewFrame;
      int newFrameBehaviour;
      int sheetSide;

};  // end struct Frame



/***************************************************************************/

// Pictures and tables first get their anchors, and later reveil their
// actual content. Therefore, all picture and table data must be put
// into a list of anchored inserts. The last anchored insert must
// become the first element in the list so the post-processing of
// inserting the necessary Latex tags for pictures and tables
// doesn't destroy the position indexes stored with every object.

class TableCell
{
   public:
      TableCell () {}

      TableCell ( int     c,
                  int     r,
                  QString t,
                  const BorderStyle &l,
                  const BorderStyle &ri,
                  const BorderStyle &tp,
                  const BorderStyle &b,
                  const Frame &f  ) : col (c), row (r), text (t),
                                     left (l), right (ri), top (tp),
                                     bottom (b), frame (f) {}

      int     col;
      int     row;
      QString text;
      BorderStyle left;
      BorderStyle right;
      BorderStyle top;
      BorderStyle bottom;
      Frame frame;
};  // end TableCell

/***************************************************************************/

class Table
{
   public:
      Table () {}

      Table ( QString n ) : name (n), cols (0) {}

      void addCell ( int     c,
                     int     r,
                     QString t,
                     BorderStyle l,
                     BorderStyle ri,
                     BorderStyle tp,
                     BorderStyle b,
                     Frame f        );

      QString               name;
      int                   cols;
      QValueList<TableCell> cellList;
};  // end Table

/***************************************************************************/

/***************************************************************************/

class Picture
{
    public:
        Picture () {}

        Picture ( QString n,
                  QString f  ) : name (n), koStoreName (f) {}

        Picture ( QString n ) : name (n) {}

        QString name;
        QString koStoreName;
};

/***************************************************************************/

class AnchoredInsert
{
    public:
        AnchoredInsert () {}

        AnchoredInsert ( const Picture  &i, int p ) : type (2), picture (i), pos (p) {}
        AnchoredInsert ( const Table    &i, int p ) : type (6), table   (i), pos (p) {}

        int     type;

        Picture picture;
        Table   table;

        int     pos;
};

/***************************************************************************/

struct DocData
{
    bool                       article;
    bool                       head1;
    bool                       head2;
    bool                       head3;
    bool                       bulletList;
    bool                       enumeratedList;
    bool                       romanList;
    bool                       ROMANList;
    bool                       alphabeticalList;
    bool                       ALPHAList;
    bool                       grpMgr;
    QValueList<AnchoredInsert> anchoredInsertList;
    int                        frameInfo;
};

/***************************************************************************/

struct FilterData
{
    QString storeFileName;
    QString exportFileName;
};

/***************************************************************************/


void ProcessPictureData ( );
/* void ProcessPictureData ( Picture  &picture,
                          int       picturePos,
                          QString  &storeFileName,
                          QString  &exportFileName,
                          QString  &outputText  );   */

void ProcessTableData ( Table   &table,
                        int     tablePos,
                        QString &outputText );

void ProcessParagraph ( QString &paraText,
                        QValueList<FormatData> &paraFormatDataList,
                        QValueList<FormatData> &paraFormatDataFormats,
                        QString &outputText,
                        ParaLayout &layout,
                        DocData *docData );

void paperSize( PaperAttributes &paper,
                PaperBorders &paperBorders  );


void ProcessDocTag ( QDomNode   myNode,
                     void      *tagData,
                     QString   &outputText );



void ProcessBordersStyleTag ( QDomNode    myNode,
                        void       *tagData,
                        QString    &         );

void ProcessTabulatorTag ( QDomNode    myNode,
                        void       *tagData,
                        QString    &         );

void ProcessAttributesTag ( QDomNode    myNode,
                        void       *tagData,
                        QString    &         );

void ProcessPaperBordersTag ( QDomNode    myNode,
                        void       *tagData,
                        QString    &         );

void ProcessPaperTag ( QDomNode    myNode,
                        void       *tagData,
                        QString    &         );

void ProcessValueTag ( QDomNode   myNode,
                       void      *tagData,
                       QString   &         );

void ProcessIntValueTag ( QDomNode   myNode,
                       void      *tagData,
                       QString   &         );

void ProcessTextTag ( QDomNode    myNode,
                      void       *tagData,
                      QString    &         );

void ProcessCounterTag ( QDomNode    myNode,
                        void       *tagData,
                        QString    &         );

void ProcessFlowTag ( QDomNode    myNode,
                        void       *tagData,
                        QString    &         );

void ProcessIndentTag ( QDomNode    myNode,
                        void       *tagData,
                        QString    &         );


void ProcessLayoutTag ( QDomNode   myNode,
                        void      *tagData,
                        QString   &outputText );

void ProcessItalicTag ( QDomNode   myNode,
                        void      *tagData,
                        QString   &         );

void ProcessFontTag ( QDomNode    myNode,
                      void       *tagData,
                      QString    &         );

void ProcessAnchorTag ( QDomNode    myNode,
                        void       *tagData,
                        QString    &         );

void ProcessFormatTag ( QDomNode   myNode,
                        void      *tagData,
                        QString   &outputText );

void ProcessFormatsTag ( QDomNode   myNode,
                         void      *tagData,
                         QString   &outputText );


void ProcessParagraphTag ( QDomNode   myNode,
                           void      *tagData,
                           QString   &outputText );  // part virtual

void ProcessFramesetTag ( QDomNode   myNode,
                          void      *tagData,
                          QString   &outputText );

void ProcessFramesetsTag ( QDomNode   myNode,
                          void      *tagData,
                          QString   &outputText );

void ProcessPixmapsKeyTag ( QDomNode   myNode,
                            void      *tagData,
                            QString   &outputText );

void ProcessPixmapsTag ( QDomNode   myNode,
                         void      *tagData,
                         QString   &outputText );



AnchoredInsert *findAnchoredInsert (AnchoredInsert   searchElement,
                                    QValueList<AnchoredInsert> &list);

void ProcessColorTag ( QDomNode    myNode,
                       void       *tagData,
                       QString    &         );

#endif
