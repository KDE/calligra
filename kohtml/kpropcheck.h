// kpropcheck.h

#if !defined KPROPCHECK_H
#define KPROPCHECK_H

#include <qcheckbox.h>
#include <kconfig.h>

/** This class provides a checkbox with a label.
  * The widget is resized to fit the font.
  */
class KPropCheck : public QCheckBox
{
	Q_OBJECT
	
public:

	/** Constructor. 'text' is the text of the label.
	  */
	KPropCheck( QWidget *parent, const char *text, const char *key=0, const char *group=0, KConfig *config=0 , const char *name=0 );

	/** Destructor.*/
	virtual ~KPropCheck();
	
	/** Sets the state of the checkbox. */
	void setState( bool state);

	/** Get the state of the checkbox. */
	bool getState();

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

public slots:
	/** Write the config key value. . */
	virtual void setConfig();

	/** Read the config key value into the widget. 
	  * . 
	  */
	virtual void getConfig();

protected:
	/** This method is called after a font change.
	  * It will call sizeUpdate() to resize the widget to contain
	  * the new font.
	  */
	virtual void fontChange( const QFont &oldfont );

	/** Sets the minimum size to fit the font. */
	void sizeUpdate();
	KConfig *ConfigObject;
	QString Group, Key;
};

#endif //KPROPCHECK_H

