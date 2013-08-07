#ifndef FAKE_KCALENDARSYSTEM_H
#define FAKE_KCALENDARSYSTEM_H

#include "klocale.h"

#include <QStringList>
#include <QDate>

class KCalendarSystemPrivate;
class KCalendarEra;
class KLocale;

#include "kofake_export.h"

class KOFAKE_EXPORT KCalendarSystem
{
public:
    enum StringFormat { ShortFormat, LongFormat };
    enum MonthNameFormat { ShortName, LongName, ShortNamePossessive, LongNamePossessive, NarrowName };
    enum WeekDayNameFormat { ShortDayName, LongDayName, NarrowDayName };

    //KDE5 add default value to calendarSystem
    /**
     * @since 4.6
     *
     * Creates a KCalendarSystem object for the required Calendar System
     *
     * @param calendarSystem the Calendar System to create, defaults to QDate compatible
     * @param locale locale to use for translations. The global locale is used if null.
     * @return a KCalendarSystem object
     */
    static KCalendarSystem *create(KLocale::CalendarSystem calendarSystem,
                                   const KLocale *locale = 0) {return 0;}

#if 0    /**
     * @since 4.6
     *
     * Creates a KCalendarSystem object for the required Calendar System
     *
     * @param calendarSystem the Calendar System to create
     * @param config a configuration file with a 'KCalendarSystem %calendarType' group detailing
     *               locale-related preferences (such as era options).  The global config is used
                     if null.
     * @param locale locale to use for translations. The global locale is used if null.
     * @return a KCalendarSystem object
     */
    static KCalendarSystem *create(KLocale::CalendarSystem calendarSystem, KSharedConfig::Ptr config,
                                   const KLocale *locale = 0);

    /**
     * @since 4.6
     *
     * Returns the list of currently supported Calendar Systems
     *
     * @return list of Calendar Systems
     */
    static QList<KLocale::CalendarSystem> calendarSystemsList();

    /**
     * @since 4.6
     *
     * Returns a localized label to display for the required Calendar System type.
     *
     * Use with calendarSystemsList() to populate selection lists of available
     * calendar systems.
     *
     * @param calendarSystem the specific calendar type to return the label for
     * @param locale the locale to use for the label, defaults to global
     * @return label for calendar
     */
    static QString calendarLabel(KLocale::CalendarSystem calendarSystem, const KLocale *locale = KGlobal::locale());

    //KDE5 Remove
    /**
     * @since 4.7
     *
     * Returns the Calendar System enum value for a given Calendar Type,
     * e.g. KLocale::QDateCalendar for "gregorian"
     *
     * @param calendarType the calendar type to convert
     * @return calendar system for calendar type
     */
    static KLocale::CalendarSystem calendarSystem(const QString &calendarType);

    //KDE5 remove
    /**
     * @since 4.7
     *
     * Returns the deprecated Calendar Type for a given Calendar System enum value,
     * e.g. "gregorian" for KLocale::QDateCalendar
     *
     * @param calendarSystem the calendar system to convert
     * @return calendar type for calendar system
     */
    static QString calendarType(KLocale::CalendarSystem calendarSystem);
#endif

    KCalendarSystem(const KLocale *locale = 0);

#if 0
    /**
     * Constructor of abstract calendar class. This will be called by derived classes.
     *
     * @param config a configuration file with a 'KCalendarSystem %calendarName' group detailing
     *               locale-related preferences (such as era options).  The global config is used
                     if null.
     * @param locale locale to use for translations. The global locale is used if null.
     */
    explicit KCalendarSystem(const KSharedConfig::Ptr config, const KLocale *locale = 0);
#endif

    virtual ~KCalendarSystem();

#if 0
    //KDE5 make virtual?
    /**
     * @since 4.6
     *
     * Returns the Calendar System type of the KCalendarSystem object
     *
     * @return type of calendar system
     */
    KLocale::CalendarSystem calendarSystem() const;

    //KDE5 make virtual?
    /**
     * @since 4.6
     *
     * Returns a localized label to display for the current Calendar System type.
     *
     * @return localized label for this Calendar System
     */
    QString calendarLabel() const;

    /**
     * Returns a QDate holding the epoch of the calendar system.  Usually YMD
     * of 1/1/1, access the returned QDates method toJulianDay() if you
     * require the actual Julian day number.  Note: a particular calendar
     * system implementation may not include the epoch in its supported range,
     * or the calendar system may be proleptic in which case it supports dates
     * before the epoch.
     *
     * @see KCalendarSystem::earliestValidDate
     * @see KCalendarSystem::latestValidDate
     * @see KCalendarSystem::isProleptic
     * @see KCalendarSystem::isValid
     *
     * @return epoch of calendar system
     */
    virtual QDate epoch() const;

    /**
     * Returns the earliest date valid in this calendar system implementation.
     *
     * If the calendar system is proleptic then this may be before epoch.
     *
     * @see KCalendarSystem::epoch
     * @see KCalendarSystem::latestValidDate
     *
     * @return date the earliest valid date
     */
    virtual QDate earliestValidDate() const;

    /**
     * Returns the latest date valid in this calendar system implementation.
     *
     * @see KCalendarSystem::epoch
     * @see KCalendarSystem::earliestValidDate
     *
     * @return date the latest valid date
     */
    virtual QDate latestValidDate() const;

    /**
     * Returns whether a given date is valid in this calendar system.
     *
     * @param year the year portion of the date to check
     * @param month the month portion of the date to check
     * @param day the day portion of the date to check
     * @return @c true if the date is valid, @c false otherwise
     */
    virtual bool isValid(int year, int month, int day) const = 0;

    //KDE5 make virtual?
    /**
     * @since 4.4
     *
     * Returns whether a given date is valid in this calendar system.
     *
     * @param year the year portion of the date to check
     * @param dayOfYear the day of year portion of the date to check
     * @return @c true if the date is valid, @c false otherwise
     */
    bool isValid(int year, int dayOfYear) const;

    //KDE5 make virtual?
    /**
     * @since 4.5
     *
     * Returns whether a given date is valid in this calendar system.
     *
     * @param eraName the Era Name portion of the date to check
     * @param yearInEra the Year In Era portion of the date to check
     * @param month the Month portion of the date to check
     * @param day the Day portion of the date to check
     * @return @c true if the date is valid, @c false otherwise
     */
    bool isValid(const QString &eraName, int yearInEra, int month, int day) const;

    //KDE5 make virtual?
    /**
     * @since 4.4
     *
     * Returns whether a given date is valid in this calendar system.
     *
     * @param year the year portion of the date to check
     * @param isoWeekNumber the ISO week portion of the date to check
     * @param dayOfIsoWeek the day of week portion of the date to check
     * @return @c true if the date is valid, @c false otherwise
     */
    bool isValidIsoWeekDate(int year, int isoWeekNumber, int dayOfIsoWeek) const;

    /**
     * Returns whether a given date is valid in this calendar system.
     *
     * @param date the date to check
     * @return @c true if the date is valid, @c false otherwise
     */
    virtual bool isValid(const QDate &date) const;

    /**
     * Changes the date's year, month and day. The range of the year, month
     * and day depends on which calendar is being used.  All years entered
     * are treated literally, i.e. no Y2K translation is applied to years
     * entered in the range 00 to 99.  Replaces setYMD.
     *
     * @param date date to change
     * @param year year
     * @param month month number
     * @param day day of month
     * @return @c true if the date is valid, @c false otherwise
     */
    virtual bool setDate(QDate &date, int year, int month, int day) const;

    //KDE5 make virtual?
    /**
     * @since 4.4
     *
     * Set a date using the year number and day of year number only.
     *
     * @param date date to change
     * @param year year
     * @param dayOfYear day of year
     * @return @c true if the date is valid, @c false otherwise
     */
    bool setDate(QDate &date, int year, int dayOfYear) const;

    //KDE5 make virtual?
    /**
     * @since 4.5
     *
     * Set a date using the era, year in era number, month and day
     *
     * @param date date to change
     * @param eraName Era string
     * @param yearInEra Year In Era number
     * @param month Month number
     * @param day Day Of Month number
     * @return @c true if the date is valid, @c false otherwise
     */
    bool setDate(QDate &date, QString eraName, int yearInEra, int month, int day) const;

    //KDE5 make virtual?
    /**
     * @since 4.4
     *
     * Set a date using the year number, ISO week number and day of week number.
     *
     * @param date date to change
     * @param year year
     * @param isoWeekNumber ISO week of year
     * @param dayOfIsoWeek day of week Mon..Sun (1..7)
     * @return @c true if the date is valid, @c false otherwise
     */
    bool setDateIsoWeek(QDate &date, int year, int isoWeekNumber, int dayOfIsoWeek) const;

    /**
     * @deprecated Use setDate() instead
     *
     * Some implementations reject year range 00 to 99, but extended date
     * ranges now require these to be accepted.  Equivalent in QDate is
     * obsoleted.
     *
     * Changes the date's year, month and day. The range of the year, month
     * and day depends on which calendar is being used.
     *
     * @param date Date to change
     * @param y Year
     * @param m Month number
     * @param d Day of month
     * @return true if the date is valid; otherwise returns false.
     */
    KDE_DEPRECATED virtual bool setYMD(QDate &date, int y, int m, int d) const;

    //KDE5 make virtual?
    /**
     * @since 4.5
     *
     * Returns the year, month and day portion of a given date in the current calendar system
     *
     * @param date date to get year, month and day for
     * @param year year number returned in this variable
     * @param month month number returned in this variable
     * @param day day of month returned in this variable
     */
    void getDate(const QDate date, int *year, int *month, int *day) const;

    /**
     * Returns the year portion of a given date in the current calendar system
     *
     * @param date date to return year for
     * @return year, 0 if input date is invalid
     */
    virtual int year(const QDate &date) const;
#endif
    /**
     * Returns the month portion of a given date in the current calendar system
     *
     * @param date date to return month for
     * @return month of year, 0 if input date is invalid
     */
    int month(const QDate &date) const {return 0;}

    /**
     * Returns the day portion of a given date in the current calendar system
     *
     * @param date date to return day for
     * @return day of the month, 0 if input date is invalid
     */
     int day(const QDate &date) const {return 0;}
#if 0
    //KDE5 make virtual?
    /**
     * @since 4.5
     *
     * Returns the Era Name portion of a given date in the current calendar system,
     * for example "AD" or "Anno Domini" for the Gregorian calendar and Christian Era.
     *
     * @param date date to return Era Name for
     * @param format format to return, either short or long
     * @return era name, empty string if input date is invalid
     */
    QString eraName(const QDate &date, StringFormat format = ShortFormat) const;

    //KDE5 make virtual?
    /**
     * @since 4.5
     *
     * Returns the Era Year portion of a given date in the current
     * calendar system, for example "2000 AD" or "Heisei 22".
     *
     * @param date date to return Era Year for
     * @param format format to return, either short or long
     * @return era name, empty string if input date is invalid
     */
    QString eraYear(const QDate &date, StringFormat format = ShortFormat) const;

    //KDE5 make virtual?
    /**
     * @since 4.5
     *
     * Returns the Year In Era portion of a given date in the current calendar
     * system, for example 1 for "1 BC".
     *
     * @param date date to return Year In Era for
     * @return Year In Era, -1 if input date is invalid
     */
    int yearInEra(const QDate &date) const;
#endif

    virtual QDate addYears(const QDate &date, int nyears) const { return date.addYears(nyears); }
    virtual QDate addMonths(const QDate &date, int nmonths) const { return date.addMonths(nmonths); }
    virtual QDate addDays(const QDate &date, int ndays) const { return date.addDays(ndays); }

#if 0
    //KDE5 make virtual?
    /**
     * Returns the difference between two dates in years, months and days.
     * The difference is always caculated from the earlier date to the later
     * date in year, month and day order, with the @p direction parameter
     * indicating which direction the difference is applied from the @p toDate.
     *
     * For example, the difference between 2010-06-10 and 2012-09-5 is 2 years,
     * 2 months and 26 days.  Note that the difference between two last days of
     * the month is always 1 month, e.g. 2010-01-31 to 2010-02-28 is 1 month
     * not 28 days.
     *
     * @param fromDate The date to start from
     * @param toDate The date to end at
     * @param yearsDiff Returns number of years difference
     * @param monthsDiff Returns number of months difference
     * @param daysDiff Returns number of days difference
     * @param direction Returns direction of difference, 1 if fromDate <= toDate, -1 otherwise
     */
    void dateDifference(const QDate &fromDate, const QDate &toDate,
                        int *yearsDiff, int *monthsDiff, int *daysDiff, int *direction) const;

    //KDE5 make virtual?
    /**
    * Returns the difference between two dates in completed calendar years.
    * The returned value will be negative if @p fromDate > @p toDate.
    *
    * For example, the difference between 2010-06-10 and 2012-09-5 is 2 years.
    *
    * @param fromDate The date to start from
    * @param toDate The date to end at
    * @return The number of years difference
    */
    int yearsDifference(const QDate &fromDate, const QDate &toDate) const;

    //KDE5 make virtual?
    /**
     * Returns the difference between two dates in completed calendar months
     * The returned value will be negative if @p fromDate > @p toDate.
     *
     * For example, the difference between 2010-06-10 and 2012-09-5 is 26 months.
     * Note that the difference between two last days of the month is always 1
     * month, e.g. 2010-01-31 to 2010-02-28 is 1 month not 28 days.
     *
     * @param fromDate The date to start from
     * @param toDate The date to end at
     * @return The number of months difference
     */
    int monthsDifference(const QDate &fromDate, const QDate &toDate) const;

    //KDE5 make virtual?
    /**
     * Returns the difference between two dates in days
     * The returned value will be negative if @p fromDate > @p toDate.
     *
     * @param fromDate The date to start from
     * @param toDate The date to end at
     * @return The number of days difference
     */
    int daysDifference(const QDate &fromDate, const QDate &toDate) const;

    /**
     * Returns number of months in the given year
     *
     * @param date the date to obtain year from
     * @return number of months in the year, -1 if input date invalid
     */
    virtual int monthsInYear(const QDate &date) const;

    //KDE5 make virtual?
    /**
     * @since 4.5
     *
     * Returns number of months in the given year
     *
     * @param year the required year
     * @return number of months in the year, -1 if input date invalid
     */
    int monthsInYear(int year) const;

    /**
     * Returns the number of localized weeks in the given year.
     *
     * @param date the date to obtain year from
     * @return number of weeks in the year, -1 if input date invalid
     */
    virtual int weeksInYear(const QDate &date) const;

    //KDE5 Merge with virtual weeksInYear with default
    /**
     * @since 4.7
     *
     * Returns the number of Weeks in a year using the required Week Number System.
     *
     * Unless you specifically want a particular Week Number System (e.g. ISO Weeks)
     * you should use the localized number of weeks provided by weeksInYear().
     *
     * @see week()
     * @see formatDate()
     * @param date the date to obtain year from
     * @param weekNumberSystem the week number system to use
     * @return number of weeks in the year, -1 if  date invalid
     */
    int weeksInYear(const QDate &date, KLocale::WeekNumberSystem weekNumberSystem) const;

    /**
     * Returns the number of localized weeks in the given year.
     *
     * @param year the year
     * @return number of weeks in the year, -1 if input date invalid
     */
    virtual int weeksInYear(int year) const;

    //KDE5 Merge with virtual weeksInYear with default
    /**
     * @since 4.7
     *
     * Returns the number of Weeks in a year using the required Week Number System.
     *
     * Unless you specifically want a particular Week Number System (e.g. ISO Weeks)
     * you should use the localized number of weeks provided by weeksInYear().
     *
     * @see week()
     * @see formatDate()
     * @param year the year
     * @param weekNumberSystem the week number system to use
     * @return number of weeks in the year, -1 if  date invalid
     */
    int weeksInYear(int year, KLocale::WeekNumberSystem weekNumberSystem) const;

    /**
     * Returns the number of days in the given year.
     *
     * @param date the date to obtain year from
     * @return number of days in year, -1 if input date invalid
     */
    virtual int daysInYear(const QDate &date) const;

    //KDE5 make virtual?
    /**
     * @since 4.5
     *
     * Returns the number of days in the given year.
     *
     * @param year the year
     * @return number of days in year, -1 if input date invalid
     */
    int daysInYear(int year) const;

    /**
     * Returns the number of days in the given month.
     *
     * @param date the date to obtain month from
     * @return number of days in month, -1 if input date invalid
     */
    virtual int daysInMonth(const QDate &date) const;

    //KDE5 make virtual?
    /**
     * @since 4.5
     *
     * Returns the number of days in the given month.
     *
     * @param year the year the month is in
     * @param month the month
     * @return number of days in month, -1 if input date invalid
     */
    int daysInMonth(int year, int month) const;

    /**
     * Returns the number of days in the given week.
     *
     * @param date the date to obtain week from
     * @return number of days in week, -1 if input date invalid
     */
    virtual int daysInWeek(const QDate &date) const;

    /**
     * Returns the day number of year for the given date
     *
     * The days are numbered 1..daysInYear()
     *
     * @param date the date to obtain day from
     * @return day of year number, -1 if input date not valid
     */
    virtual int dayOfYear(const QDate &date) const;
#endif

    /**
     * Returns the weekday number for the given date
     *
     * The weekdays are numbered 1..7 for Monday..Sunday.
     *
     * This value is @em not affected by the value of weekStartDay()
     *
     * @param date the date to obtain day from
     * @return day of week number, -1 if input date not valid
     */
    int dayOfWeek(const QDate &date) const {return 0;}

    //KDE5 Make virtual?
    /**
     * Returns the localized Week Number for the date.
     *
     * This may be ISO, US, or any other supported week numbering scheme.  If
     * you specifically require the ISO Week or any other scheme, you should use
     * the week(KLocale::WeekNumberSystem) form.
     *
     * If the date falls in the last week of the previous year or the first
     * week of the following year, then the yearNum returned will be set to the
     * appropriate year.
     *
     * @see weeksInYear()
     * @see formatDate()
     * @param date the date to obtain week from
     * @param yearNum returns the year the date belongs to
     * @return localized week number, -1 if input date invalid
     */
    int week(const QDate &date, int *yearNum = 0) const {return 0;}

#if 0
    //KDE5 Make virtual?
    /**
     * Returns the Week Number for the date in the required Week Number System.
     *
     * Unless you want a specific Week Number System (e.g. ISO Week), you should
     * use the localized Week Number form of week().
     *
     * If the date falls in the last week of the previous year or the first
     * week of the following year, then the yearNum returned will be set to the
     * appropriate year.
     *
     * Technically, the ISO Week Number only applies to the ISO/Gregorian Calendar
     * System, but the same rules will be applied to the current Calendar System.
     *
     * @see weeksInYear()
     * @see formatDate()
     * @param date the date to obtain week from
     * @param weekNumberSystem the Week Number System to use
     * @param yearNum returns the year the date belongs to
     * @return week number, -1 if input date invalid
     */
    int week(const QDate &date, KLocale::WeekNumberSystem weekNumberSystem, int *yearNum = 0) const;

    /**
     * Returns whether a given year is a leap year.
     *
     * Input year must be checked for validity in current Calendar System prior to calling, no
     * validity checking performed in this routine, behaviour is undefined in invalid case.
     *
     * @param year the year to check
     * @return @c true if the year is a leap year, @c false otherwise
     */
    virtual bool isLeapYear(int year) const = 0;

    /**
     * Returns whether a given date falls in a leap year.
     *
     * Input date must be checked for validity in current Calendar System prior to calling, no
     * validity checking performed in this routine, behaviour is undefined in invalid case.
     *
     * @param date the date to check
     * @return @c true if the date falls in a leap year, @c false otherwise
     */
    virtual bool isLeapYear(const QDate &date) const;

    //KDE5 Make virtual?
    /**
     * @since 4.6
     *
     * Returns a QDate containing the first day of the year
     *
     * @param year The year to return the date for
     * @return The first day of the year
     */
    QDate firstDayOfYear(int year) const;

    //KDE5 Make virtual?
    /**
     * @since 4.6
     *
     * Returns a QDate containing the last day of the year
     *
     * @param year The year to return the date for
     * @return The last day of the year
     */
    QDate lastDayOfYear(int year) const;

    //KDE5 Make virtual?
    /**
     * @since 4.6
     *
     * Returns a QDate containing the first day of the year
     *
     * @param date The year to return the date for, defaults to today
     * @return The first day of the year
     */
    QDate firstDayOfYear(const QDate &date = QDate::currentDate()) const;

    //KDE5 Make virtual?
    /**
     * @since 4.6
     *
     * Returns a QDate containing the last day of the year
     *
     * @param date The year to return the date for, defaults to today
     * @return The last day of the year
     */
    QDate lastDayOfYear(const QDate &date = QDate::currentDate()) const;

    //KDE5 Make virtual?
    /**
     * @since 4.6
     *
     * Returns a QDate containing the first day of the month
     *
     * @param year The year to return the date for
     * @param month The month to return the date for
     * @return The first day of the month
     */
    QDate firstDayOfMonth(int year, int month) const;

    //KDE5 Make virtual?
    /**
     * @since 4.6
     *
     * Returns a QDate containing the last day of the month
     *
     * @param year The year to return the date for
     * @param month The month to return the date for
     * @return The last day of the month
     */
    QDate lastDayOfMonth(int year, int month) const;

    //KDE5 Make virtual?
    /**
     * @since 4.6
     *
     * Returns a QDate containing the first day of the month
     *
     * @param date The month to return the date for, defaults to today
     * @return The first day of the month
     */
    QDate firstDayOfMonth(const QDate &date = QDate::currentDate()) const;

    //KDE5 Make virtual?
    /**
     * @since 4.6
     *
     * Returns a QDate containing the last day of the month
     *
     * @param date The month to return the date for, defaults to today
     * @return The last day of the month
     */
    QDate lastDayOfMonth(const QDate &date = QDate::currentDate()) const;
#endif

    virtual QString monthName(int month, int year, MonthNameFormat format = LongName) const { return monthName(QDate(year, month, 1), format); }
    virtual QString monthName(const QDate &date, MonthNameFormat format = LongName) const { return m_locale->formatDate(date, KLocale::MonthName); }

    virtual QString weekDayName(int weekDay, WeekDayNameFormat format = LongDayName) const { return QString::number(weekDay); }
    virtual QString weekDayName(const QDate &date, WeekDayNameFormat format = LongDayName) const { return weekDayName(date.weekNumber(), format); }

#if 0
    //KDE5 make protected or remove?
    /**
     * @deprecated for internal use only
     *
     * Converts a year literal of a part of a string into a integer starting at the beginning of the string
     *
     * @param sNum The string to parse
     * @param iLength The number of QChars used, and 0 if no valid symbols was found in the string
     * @return An integer corresponding to the year
     */
    virtual int yearStringToInteger(const QString &sNum, int &iLength) const;

    //KDE5 make protected or remove?
    /**
     * @deprecated for internal use only
     *
     * Converts a month literal of a part of a string into a integer starting at the beginning of the string
     *
     * @param sNum The string to parse
     * @param iLength The number of QChars used, and 0 if no valid symbols was found in the string
     * @return An integer corresponding to the month
     */
    virtual int monthStringToInteger(const QString &sNum, int &iLength) const;

    //KDE5 make protected or remove?
    /**
     * @deprecated for internal use only
     *
     * Converts a day literal of a part of a string into a integer starting at the beginning of the string
     *
     * @param sNum The string to parse
     * @param iLength The number of QChars used, and 0 if no valid symbols was found in the string
     * @return An integer corresponding to the day
     */
    virtual int dayStringToInteger(const QString &sNum, int &iLength) const;
#endif

    virtual QString formatDate(const QDate &fromDate, KLocale::DateFormat toFormat = KLocale::LongDate) const;
    virtual QString formatDate(const QDate &fromDate, const QString &toFormat, KLocale::DateTimeFormatStandard formatStandard = KLocale::KdeFormat) const;
    virtual QString formatDate(const QDate &fromDate, const QString &toFormat, KLocale::DigitSet digitSet, KLocale::DateTimeFormatStandard formatStandard = KLocale::KdeFormat) const;
    virtual QString formatDate(const QDate &date, KLocale::DateTimeComponent component, KLocale::DateTimeComponentFormat format = KLocale::DefaultComponentFormat, KLocale::WeekNumberSystem weekNumberSystem = KLocale::DefaultWeekNumber) const;

    virtual QDate readDate(const QString &str, bool *ok = 0) const;
    virtual QDate readDate(const QString &str, KLocale::ReadDateFlags flags, bool *ok = 0) const;
    virtual QDate readDate(const QString &dateString, const QString &dateFormat, bool *ok = 0) const;
    virtual QDate readDate(const QString &dateString, const QString &dateFormat, bool *ok, KLocale::DateTimeFormatStandard formatStandard) const;

#if 0
    //KDE5 Make virtual
    /**
     * @since 4.6
     *
     * Returns the Short Year Window Start Year for the current Calendar System.
     *
     * Use this function to get the Start Year for the Short Year Window to be
     * applied when 2 digit years are entered for a Short Year input format,
     * e.g. if the Short Year Window Start Year is 1930, then the input Short
     * Year value of 40 is interpreted as 1940 and the input Short Year value
     * of 10 is interpreted as 2010.
     *
     * The Short Year Window is only ever applied when reading the Short Year
     * format and not the Long Year format, i.e. KLocale::ShortFormat or '%y'
     * only and not KLocale::LongFormat or '%Y'.
     *
     * The Start Year 0 effectively means not to use a Short Year Window
     *
     * Each Calendar System requires a different Short Year Window as they have
     * different epochs. The Gregorian Short Year Window usually pivots around
     * the year 2000, whereas the Hebrew Short Year Window usually pivots around
     * the year 5000.
     *
     * This value must always be used when evaluating user input Short Year
     * strings.
     *
     * @see KLocale::shortYearWindowStartYear
     * @see KLocale::applyShortYearWindow
     * @return the short year window start year
     */
    int shortYearWindowStartYear() const;

    //KDE5 Make virtual
    /**
     * @since 4.6
     *
     * Returns the Year Number after applying the Year Window.
     *
     * If the @p inputYear is between 0 and 99, then apply the Year Window and
     * return the calculated Year Number.
     *
     * If the @p inputYear is not between 0 and 99, then the original Year Number
     * is returned.
     *
     * @see KLocale::setYearWindowOffset
     * @see KLocale::yearWindowOffset
     * @param inputYear the year number to apply the year window to
     * @return the year number after applying the year window
     */
    int applyShortYearWindow(int inputYear) const;

    /**
     * Use this to determine which day is the first day of the week.
     *
     * Uses the calendar system's internal locale set when the instance was
     * created, which ensures that the correct calendar system and locale
     * settings are respected, which would not occur in some cases if using
     * the global locale.  Defaults to global locale.
     *
     * @see KLocale::weekStartDay
     *
     * @return an integer (Monday = 1, ..., Sunday = 7)
     */
    virtual int weekStartDay() const;

    /**
     * @deprecated use KLocale::weekDayOfPray() instead
     *
     * Returns the day of the week traditionally associated with religious
     * observance for this calendar system.  Note this may not be accurate
     * for the users locale, e.g. Gregorian calendar used in non-Christian
     * countries, in use cases where this could be an issue it is recommended
     * to use KLocale::weekDayOfPray() instead.
     *
     * @return day number (None = 0, Monday = 1, ..., Sunday = 7)
     */
    KDE_DEPRECATED virtual int weekDayOfPray() const = 0;

    /**
     * Returns whether the calendar is lunar based.
     *
     * @return @c true if the calendar is lunar based, @c false if not
     */
    virtual bool isLunar() const = 0;

    /**
     * Returns whether the calendar is lunisolar based.
     *
     * @return @c true if the calendar is lunisolar based, @c false if not
     */
    virtual bool isLunisolar() const = 0;

    /**
     * Returns whether the calendar is solar based.
     *
     * @return @c true if the calendar is solar based, @c false if not
     */
    virtual bool isSolar() const = 0;

    /**
     * Returns whether the calendar system is proleptic, i.e. whether dates
     * before the epoch are supported.
     *
     * @see KCalendarSystem::epoch
     *
     * @return @c true if the calendar system is proleptic, @c false if not
     */
    virtual bool isProleptic() const = 0;

protected:

    /**
     * Internal method to convert a Julian Day number into the YMD values for
     * this calendar system.
     *
     * All calendar system implementations MUST implement julianDayToDate and
     * dateToJulianDay methods as all other methods can be expressed as
     * functions of these.  Does no internal validity checking.
     *
     * @see KCalendarSystem::dateToJulianDay
     *
     * @param jd Julian day number to convert to date
     * @param year year number returned in this variable
     * @param month month number returned in this variable
     * @param day day of month returned in this variable
     * @return @c true if the date is valid, @c false otherwise
     */
    virtual bool julianDayToDate(int jd, int &year, int &month, int &day) const = 0;

    /**
     * Internal method to convert YMD values for this calendar system into a
     * Julian Day number.
     *
     * All calendar system implementations MUST implement julianDayToDate and
     * dateToJulianDay methods as all other methods can be expressed as
     * functions of these.  Does no internal validity checking.
     *
     * @see KCalendarSystem::julianDayToDate
     *
     * @param year year number
     * @param month month number
     * @param day day of month
     * @param jd Julian day number returned in this variable
     * @return @c true if the date is valid, @c false otherwise
     */
    virtual bool dateToJulianDay(int year, int month, int day, int &jd) const = 0;

    /**
     * Returns the locale used for translations and formats for this
     * calendar system instance.  This allows a calendar system instance to be
     * independent of the global translations and formats if required.  All
     * implementations must refer to this locale.
     *
     * Only for internal calendar system use; if public access is required then
     * provide public methods only for those methods actually required.  Any
     * app that creates an instance with its own locale overriding global will
     * have the original handle to the locale and can manipulate it that way if
     * required, e.g. to change default date format.  Only expose those methods
     * that library widgets require access to internally.
     *
     * @see KCalendarSystem::formatDate
     * @see KLocale::formatDate
     * @see KCalendarSystem::weekStartDay
     * @see KLocale::weekStartDay
     * @see KCalendarSystem::readDate
     * @see KLocale::readDate
     *
     * @return locale to use
     */
    const KLocale *locale() const;

    /**
     * @deprecated for internal use only
     *
     * Sets the maximum number of months in a year
     *
     * Only for internal calendar system use
     */
    KDE_DEPRECATED void setMaxMonthsInYear(int maxMonths);

    /**
     * @deprecated for internal use only
     *
     * Sets the maximum number of days in a week
     *
     * Only for internal calendar system use
     */
    KDE_DEPRECATED void setMaxDaysInWeek(int maxDays);

    /**
     * @deprecated for internal use only
     *
     * @since 4.4
     *
     * Sets if Calendar System has Year 0 or not
     *
     * Only for internal calendar system use
     */
    KDE_DEPRECATED void setHasYear0(bool hasYear0);

    /**
     * Constructor of abstract calendar class. This will be called by derived classes.
     *
     * @param dd derived private d-pointer.
     * @param config a configuration file with a 'KCalendarSystem %calendarName' group detailing
     *               locale-related preferences (such as era options).  The global config is used
                     if null.
     * @param locale locale to use for translations. The global locale is used if null.
     */
    KCalendarSystem(KCalendarSystemPrivate &dd,
                    const KSharedConfig::Ptr config = KSharedConfig::Ptr(),
                    const KLocale *locale = 0);
#endif
private:
    const KLocale *m_locale;
};


#endif
