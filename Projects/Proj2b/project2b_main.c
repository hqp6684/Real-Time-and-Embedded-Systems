/*
 * Demonstrate how to set up a timer that, on expiry,
 * sends us a pulse.  This example sets the first
 * expiry to i.5 seconds and the repetition interval
 * to i.5 seconds.
 */

#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>
int position=0;
int period=0;
#define MY_PULSE_CODE   _PULSE_CODE_MINAVAIL

typedef union {
        struct _pulse   pulse;
        /* your other message structures would go
           here too */
} my_message_t;

void* UI(void *vargp){
	//Fetch user input to determine - recipe/pulse
	while (position==0){
		period=388000;//nano - .388ms for 0 degree
	}
	while (position==1){
		period=1264000;//nano - 1.264ms for 90 degree
	}
	while (position==2){
		period=2140000;//nano - 2.14ms for 180 degree
	}
}


int main(void){
	int i=0;
	struct sigevent         event;
	struct itimerspec       itime;
	timer_t                 timer_id;
	int                     chid, rcvid;
	my_message_t            msg;
	pthread_t tid0;

	pthread_create(&tid0, NULL, UI, NULL);

	chid = ChannelCreate(0);

	event.sigev_notify = SIGEV_PULSE;
	event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chid,_NTO_SIDE_CHANNEL, 0);
	event.sigev_priority = getprio(0);
	event.sigev_code = MY_PULSE_CODE;
	timer_create(CLOCK_REALTIME, &event, &timer_id);

	/*
	 * As of the timer_settime(), we will receive our pulse
	 * in 1.5 seconds (the itime.it_value) and every 1.5
	 * seconds thereafter (the itime.it_interval)
	 */
	for (;;) {
		if (i==50){
			position=1; //change period after 50 pulses at .388ms
		}
		itime.it_value.tv_sec = 0;
		itime.it_value.tv_nsec = period;
		itime.it_interval.tv_sec = 0;
		itime.it_interval.tv_nsec = period;

		timer_settime(timer_id, 0, &itime, NULL);

		rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);
		if (rcvid == 0) { /* we got a pulse */
        	if (msg.pulse.code == MY_PULSE_CODE) {
            	printf("we got a pulse from our timer - %d\n",period);
            } /* else other pulses ... */
		} /* else other messages ... */
		i+=1;
	}
	return 0;
}
