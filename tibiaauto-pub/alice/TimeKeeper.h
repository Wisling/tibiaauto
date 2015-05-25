#ifndef TIME_KEEPER_H
#define TIME_KEEPER_H

/*

        TimeKeeper

        Author: Jonathan Roewen

        Description: Maintains a priority queue of pending events that are
                     to be triggered at a particular time. Currently, it is
                     quite inaccurate, with a 3-second precision, due to
                     the need of interleaving it within the socket code.

 */

#include <queue>
#include <vector>

using namespace std;

extern long tSecond();
extern long tMinute();
extern long tHour();
extern long tDay();
extern long tWeek();
extern time_t tNow();

class Event
{
public:
	virtual ~Event() {
	}

	virtual void run() = 0;
};

class TimedEvent
{
public:
	TimedEvent(time_t t, Event *e) {
		time  = t;
		event = e;
	}
	time_t getTime() const {
		return time;
	}
	void run() {
		event->run();
	}
private:
	time_t time;
	Event *event;
};

struct tComp {
	bool operator() (const TimedEvent *lhs, const TimedEvent *rhs) {
		return lhs->getTime() > rhs->getTime();
	}
};

typedef priority_queue<TimedEvent *, vector<TimedEvent *>, tComp> TimeQueue;

class Timer
{
public:
	Timer();

	void addEvent(TimedEvent *);
	void addEvent(time_t, Event *);

	void processPending();
private:
	TimeQueue tq;
};

extern Timer *timer;

#endif  //	TIME_KEEPER_H
