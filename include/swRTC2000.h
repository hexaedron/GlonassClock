#pragma once
#include <swRTC.h>

class swRTC2000 : public swRTC
{
    public:
    using swRTC::swRTC;
    unsigned long getTimestamp2000(void);
};

// Мы напишем новую функцию, которая вернёт Timestamp с 2000 
// для совместимости с другими либами.
// Взято из swRTC с небольшими изменениями.
unsigned long swRTC2000::getTimestamp2000(void)
{
    unsigned long time = 0;
    const int yearT = 2000;

	//One revolution of the Earth is not 365 days but accurately 365.2422 days.
	//It is leap year that adjusts this decimal fraction. But...
	time += (getYear() - yearT) * 365.2422;
	for (byte i = 0; i < getMonth() - 1; i++)
    {
		time += daysPerMonth[i]; //find day from month
	}
	time = (time + getDay()) * 24UL; //find hours from day
	time = (time + getHours()) * 60UL; //find minutes from hours
	time = (time + getMinutes()) * 60UL; //find seconds from minute
	time += getSeconds(); // add seconds
    time += 86400UL; //year 2000 is a special leap year, so 1 day must be added if date is greater than 29/02/2000
	//the code below checks if, in case of a leap year, the date is before or past the 29th of februray:
	//if no, the leap day hasn't been yet reached so we have to subtract a day
	if (isLeapYear(getYear())) 
    {
	    if (getMonth() <= 2 ) 
        {
	        time -= 86400UL;
	    }
	}
	return (time - 86400UL); //because years start at day 0.0, not day 1.
}