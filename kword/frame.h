/******************************************************************/ 
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Frame (header)                                         */
/******************************************************************/

#ifndef frame_h
#define frame_h

#include <krect.h>
#include <kpoint.h>
#include <qlist.h>
#include <qcursor.h>
#include <ksize.h>

#include "image.h"

#include "parag.h"
#include "paraglayout.h"

#include <iostream>
#include <koStream.h>

class KWordDocument;

enum FrameType {FT_BASE = 0,FT_TEXT = 1,FT_PICTURE = 2};
enum RunAround {RA_NO = 0,RA_BOUNDINGRECT = 1,RA_CONTUR = 2};

/******************************************************************/
/* Class: KWFrame                                                 */
/******************************************************************/

class KWFrame : public KRect
{
public:
  KWFrame();
  KWFrame(const KPoint &topleft,const QPoint &bottomright);
  KWFrame(const KPoint &topleft,const KSize &size);
  KWFrame(int left,int top,int width,int height);
  KWFrame(int left,int top,int width,int height,RunAround _ra,int _gap);
  KWFrame(const QRect &_rect);

  void setRunAround(RunAround _ra) { runAround = _ra; }
  RunAround getRunAround() { return runAround; }

  void setSelected(bool _selected)
    { selected = _selected; }
  bool isSelected()
    { return selected; }

  void addIntersect(KRect _r);
  void clearIntersects()
    { intersections.clear(); }
  
  int getLeftIndent(int _y,int _h);
  int getRightIndent(int _y,int _h);

  bool hasIntersections()
    { return !intersections.isEmpty(); }

  QCursor getMouseCursor(int mx,int my);

  int getRunAroundGap()
    { return runAroundGap; }
  void setRunAroundGap(int gap)
    { runAroundGap = gap; }

protected:
  RunAround runAround;
  bool selected;
  int runAroundGap;

  QList<KRect> intersections;

private:
  KWFrame &operator=(KWFrame &_frame)
    { return _frame; }
  KWFrame (const KWFrame &_frame)
    {;}

};

/******************************************************************/
/* Class: KWFrameSet                                              */
/******************************************************************/

class KWFrameSet
{
public:
  KWFrameSet(KWordDocument *_doc);
  virtual ~KWFrameSet() 
    {;}

  virtual FrameType getFrameType()
    { return FT_BASE; }

  virtual void addFrame(KWFrame _frame);
  virtual void addFrame(KWFrame *_frame);
  virtual void delFrame(unsigned int _num);

  virtual int getFrame(int _x,int _y);
  virtual KWFrame *getFrame(unsigned int _num);
  virtual unsigned int getNumFrames()
    { return frames.count(); }

  virtual bool isPTYInFrame(unsigned int _frame,unsigned int _ypos)
    { return true; }

  virtual void update()
    {;}

  virtual void clear()
    { frames.clear(); }

  virtual bool contains(unsigned int mx,unsigned int my);
  /**
   * Return 1, if a frame gets selected which was not selected before,
   * 2, if a frame gets selected which was already selected
   */
  virtual int selectFrame(unsigned int mx,unsigned int my);
  virtual void deSelectFrame(unsigned int mx,unsigned int my);
  virtual QCursor getMouseCursor(unsigned int mx,unsigned int my);

  virtual void save(ostream &out);

protected:
  virtual void init()
    {;}

  // document
  KWordDocument *doc;

  // frames
  QList<KWFrame> frames;

};

/******************************************************************/
/* Class: KWTextFrameSet                                          */
/******************************************************************/

class KWTextFrameSet : public KWFrameSet
{
public:
  KWTextFrameSet(KWordDocument *_doc)
    : KWFrameSet(_doc) 
    {;}
  virtual ~KWTextFrameSet() 
    {;}

  virtual FrameType getFrameType()
    { return FT_TEXT; }

  virtual void update();
  
  /**
   * If another parag becomes the first one it uses this function
   * to tell the document about it.
   */
  void setFirstParag(KWParag *_parag)
    { parags = _parag; }

  KWParag* getFirstParag() { return parags; }

  virtual bool isPTYInFrame(unsigned int _frame,unsigned int _ypos);

  void deleteParag(KWParag *_parag);
  void joinParag(KWParag *_parag1,KWParag *_parag2);
  void insertParag(KWParag *_parag,InsertPos _pos);
  void splitParag(KWParag *_parag,unsigned int _pos);

  virtual void save(ostream &out);
  virtual void load(KOMLParser&,vector<KOMLAttrib>&);

  bool getAutoCreateNewFrame() { return autoCreateNewFrame; }
  void setAutoCreateNewFrame(bool _auto) { autoCreateNewFrame = _auto; }

protected:
  virtual void init();

  // pointer to the first parag of the list of parags
  KWParag *parags;
  KWParagLayout* defaultParagLayout;
  bool autoCreateNewFrame;

};

/******************************************************************/
/* Class: KWPictureFrameSet                                       */
/******************************************************************/

class KWPictureFrameSet : public KWFrameSet
{
public:
  KWPictureFrameSet(KWordDocument *_doc)
    : KWFrameSet(_doc) 
    { image = 0L; }
  virtual ~KWPictureFrameSet() 
    {;}

  virtual FrameType getFrameType()
    { return FT_PICTURE; }

  virtual void setImage(KWImage *_image)
    { image = _image; }
  void setFileName(QString _filename);
  void setFileName(QString _filename,KSize _imgSize);
  void setSize(KSize _imgSize);

  virtual KWImage* getImage()
    { return image; }
  QString getFileName() { return filename; }

  virtual void save(ostream &out);
  virtual void load(KOMLParser&,vector<KOMLAttrib>&);

protected:
  KWImage *image;
  QString filename;

};

#endif
