#include <stdafx.h>

#include "TimeKeeper.h"

#include <iostream>

using namespace std;

#include <time.h>

Timer *timer = new Timer();

long tSecond() {
	time_t t;
	struct tm *now;
	
	time(&t);
	now = localtime(&t);

	now->tm_sec++;
	
	return (long)difftime(mktime(now), t);
}

long tMinute() {
	time_t t;
	struct tm *now;
	
	time(&t);
	now = localtime(&t);

	now->tm_min++;
	
	return (long)difftime(mktime(now), t);
}

long tHour() {
	time_t t;
	struct tm *now;
	
	time(&t);
	now = localtime(&t);

	now->tm_hour++;
	
	return (long)difftime(mktime(now), t);
}

long tDay() {
	return 12 * tHour();
}

long tWeek() {
	return 7 * tDay();
}

time_t tNow() {
	return time(NULL);
}

Timer::Timer() {
	;
}

void Timer::addEvent(TimedEvent *te) {
	tq.push(te);
}

void Timer::addEvent(time_t t, Event *e) {
	tq.push(new TimedEvent(t, e));
	//cout << "Event will run " << ((t - time(NULL)) / tSecond()) << " seconds from now" << endl;
}

void Timer::processPending() {
	time_t t;
	
	t = time(NULL);
	while (!tq.empty() && t >= tq.top()->getTime()) {
		tq.top()->run();
		tq.pop();
	}
}