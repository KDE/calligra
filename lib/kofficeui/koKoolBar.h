#ifndef __ko_koolbar_h__
#define __ko_koolbar_h__

#include <qwidget.h>
#include <qpixmap.h>
#include <qpushbt.h>
#include <qscrbar.h>

#include <map>
#include <memory>
#include <string>

/************************************************************
 * Template-Klasse fuer Referenz-Semantik
 *  - stellt sicher, dass bei Vergleichen die Elemente
 *    und nicht die Zeiger verglichen werden
 ************************************************************/
template <class T>
class KoSmartPtr {
  private:
    T* adr;      // eigentlicher Zeiger auf den Wert

  public:
    /* Konstruktor aus Wert vom Typ T
     *  - merkt sich Adresse des Wertes
     */
    KoSmartPtr() : adr( 0L ) { }
    KoSmartPtr (T& t) : adr(&t) { }
    ~KoSmartPtr()
    {
     if ( adr )
       delete adr;
    } 
  
    // Dereferenzierung
    T& operator*() const {
        return *adr;
    }
    T* operator->() const {
        return adr;
    }
    T* operator=(T* _p)
    {
      adr = _p;
      return adr;
    }
    operator T*() const
    {
      return adr;
    }
  
    // Vergleiche
    friend bool operator== <class T> (const KoSmartPtr<T>& lhs, const KoSmartPtr<T>& rhs);
    friend bool operator< <class T> (const KoSmartPtr<T>& lhs, const KoSmartPtr<T>& rhs);
};

// Implementierung der Vergleiche
template <class T>
inline bool operator== (const KoSmartPtr<T>& lhs, const KoSmartPtr<T>& rhs)
{
    return *lhs.adr == *rhs.adr;
}
template <class T>
inline bool operator< (const KoSmartPtr<T>& lhs, const KoSmartPtr<T>& rhs)
{
    return *lhs.adr < *rhs.adr;
}

class KoKoolBar;
class KoKoolBarGroup;
class KoKoolBarItem : public QObject
{
  private:
    Q_OBJECT
  public:
    KoKoolBarItem( KoKoolBarGroup &_grp, QPixmap& _pix, const char *_text = "" );

    int id() { return m_id; }
    void press();
    bool isEnabled() { return m_bEnabled; }
    void setEnabled( bool _e ) { m_bEnabled = _e; }
    
    int height() { return m_iHeight; }
    QPixmap& pixmap() { return m_pixmap; }
    const char* text() { return m_strText.c_str(); }
  
  signals:
    void pressed( int _group, int _id );
    void pressed();
  protected:
    void calc( QWidget & );
   
    int m_iHeight;
    KoKoolBarGroup& m_group;
    string m_strText;
    QPixmap m_pixmap;
    int m_id;
    bool m_bEnabled;
};
  
class KoKoolBarGroup : public QObject
{
  Q_OBJECT
public:
  typedef KoSmartPtr<KoKoolBarItem> Item_ptr;

protected:
  map<int,Item_ptr> m_mapItems;
  KoKoolBar& m_bar;
  string m_strText;
  int m_id;
  QPushButton m_button;
  bool m_bEnabled;

public:
  KoKoolBarGroup( KoKoolBar &_bar, const char *_text );
  ~KoKoolBarGroup() { }

  void append( KoKoolBarItem *_i ) { m_mapItems[ _i->id() ] = _i; }
  void remove( int _id );
  
  KoKoolBar& bar() { return m_bar; }
  QPushButton& button() { return m_button; }
  int id() { return m_id; }
  bool isEnabled() { return m_bEnabled; }
  void setEnabled( bool _e ) { m_bEnabled = _e; }
  KoKoolBarItem* item( int _id )
  {  map<int,Item_ptr>::iterator pos = m_mapItems.find( _id ); if ( pos == m_mapItems.end() ) return 0L;
     return pos->second; }
  int items() { return m_mapItems.size(); }
  map<int,Item_ptr>::iterator beginIterator() { return m_mapItems.begin(); }
  map<int,Item_ptr>::iterator endIterator() { return m_mapItems.end(); }
    
 public slots:
    void pressed();
};
  
class KoKoolBarBox : public QWidget
{
  Q_OBJECT
public:
  KoKoolBarBox( KoKoolBar &_bar );

  void setActiveGroup( KoKoolBarGroup *_grp );
  int maxHeight();
  void scrollUp();
  void scrollDown();
  bool needsScrolling();
  bool isAtBottom();
  bool isAtTop();
  
protected:
  virtual void paintEvent( QPaintEvent *_ev );
  virtual void mousePressEvent( QMouseEvent *_ev )
  { KoKoolBarItem *item = findByPos( _ev->pos().y() + m_iYOffset ); if ( !item ) return; item->press(); }

  KoKoolBarItem* findByPos( int _abs_y );
  
  KoKoolBar &m_bar;
  int m_iYOffset;
  int m_iYIcon;
  KoKoolBarGroup *m_pGroup;
};

class KoKoolBar : public QWidget
{
  Q_OBJECT
public:
  KoKoolBar( QWidget *_parent = 0L, const char *_name = 0L );
  virtual ~KoKoolBar() { };
  
  virtual int insertGroup( const char *_text );
  virtual int insertItem( int _grp, QPixmap& _pix, const char *_text = "", QObject *_obj = 0L, const char *_slot = 0L );
  virtual void removeGroup( int _grp );
  virtual void removeItem( int _grp, int _id );
  virtual void setActiveGroup( int _grp );
  virtual int activeGroup() { return m_iActiveGroup; }
  virtual void enableItem( int _grp, int _id, bool _enable );
  virtual void enableGroup( int _grp, bool _enable );

protected slots:  
  void slotUp();
  void slotDown();

protected:
  typedef KoSmartPtr<KoKoolBarGroup> Group_ptr;

  virtual void resizeEvent( QResizeEvent *_ev );
  
  void updateScrollButtons();
  
  map<int,Group_ptr> m_mapGroups;
  int m_iActiveGroup;
  KoKoolBarBox* m_pBox;
  QPushButton* m_pButtonUp;
  QPushButton* m_pButtonDown;
};

#endif
