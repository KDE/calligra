#ifndef __kwimage_h__
#define __kwimage_h__

#include <qstring.h>

#include <koImage.h>
#include <koImageCollection.h>

class KWTextDocument;
typedef KoImageCollection<QString>::Image KWImage;

class KWImageCollection : public KoImageCollection<QString>
{
public:

    /**
     * Find or create an image
     */
    KWImage image( const QString & fileName );
};

#include <qrichtext_p.h>
using namespace Qt3;

/*class KWTextCustomItem : public QTextCustomItem
{
public:
    KWTextCustomItem( KWTextDocument *textdoc ) : QTextCustomItem( textdoc ) {}

};*/

/**
 * This class is used by "Insert Picture", i.e. having an image inline in a paragraph.
 */
class KWTextImage : public QTextCustomItem
{
public:
    /**
     * Set filename to load a real file from the disk
     * Otherwise use setImage() - this is what's done on loading
     */
    KWTextImage( KWTextDocument *textdoc, const QString & filename );
    ~KWTextImage()
    {
        // Remove image from collection ?
    }

    Placement placement() const { return place; }
    void adjustToPainter( QPainter* );
    int widthHint() const { return width; }
    int minimumWidth() const { return width; }

    void setImage( const KWImage &image );
    KWImage image() const { return m_image; }

    void draw( QPainter* p, int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg );

private:
    Placement place;
    KWImage m_image;
};

#endif
