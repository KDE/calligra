// kpropfont.h

#if !defined KPROPCOLOR_H
#define KPROPCOLOR_H

#include <qlabel.h>
#include <qpushbutton.h>
#include <kcolordlg.h>
#include <kconfig.h>
#include <qpainter.h>

/** A color display with a colro dialog popup button.
  * The widget is resized when the button font changes.
  * 
  */
class KPropColor : public QFrame
{
	Q_OBJECT
	
public:
	/** Constructor. 
	  * 'perc' is the percentage of space reserved for the button,
	  * 'buttontext' is the text on the button,
	  */
	KPropColor( QWidget *parent, const char *buttontext, int perc, 
			QColor mydefault, const char *key=0, const char *group=0, KConfig *config=0, const char *name=0 );

	/** Destructor. */
	virtual ~KPropColor();
	
	/** Set the color of the test label. */
	void setLabelColor( QColor color );

	/** Returns the color of the test label text. */
	QColor getLabelColor();

public slots:
	/** Write the config key value. */
	virtual void setConfig();

	/** Read the config key value into the widget. */
	virtual void getConfig();

	/** set the group to modify */
	void setGroup( const char *group );

	/** Returns the group to be modified */
	const char *getGroup();

	/** set the key to modify */
	void setKey( const char * );

	/** returns the Key to be modified */
	const char *getKey();

	/** Set the KConfig object to operate upon. */
	void setKConfig( KConfig * );

	/** Returns the current KConfig object */
	KConfig *getKConfig();

protected:
	/** Handles resize events for the widget. */
	virtual void resizeEvent( QResizeEvent *rev );

	/** Handles paint events for the widget. */
	virtual void paintEvent( QPaintEvent *pev );

	/** This method is called after a font change.
	  * It will call sizeUpdate() to resize the widget to contain
	  * the new font.
	  */
	virtual void fontChange( const QFont &oldfont );

	/** Sets the minimum size to fit the font. */
	void sizeUpdate();

	QPushButton *button;
	int percentage;
	KConfig *ConfigObject;
	QString Group, Key;
	QColor LabelColor, defaultColor;

protected slots:
	/** popup the color dialog and put the color in the Label */
	void getColorDialog();
	
};

#endif //KPROPCOLOR_H

