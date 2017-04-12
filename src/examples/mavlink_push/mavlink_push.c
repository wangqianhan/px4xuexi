#include <px4_config.h>
#include <px4_tasks.h>
#include <px4_posix.h>
#include <unistd.h>
#include <stdio.h>
#include <poll.h>
#include <string.h>

#include <nuttx/sched.h>
#include <drivers/drv_hrt.h>
#include <arch/board/board.h>

#include <uORB/uORB.h>

#include <uORB/topics/mavlink_test.h>

#include <systemlib/systemlib.h>

static unsigned long x=2;
static unsigned long y=2;
static unsigned long z=2;

int mavlink_push_deamon_main(int argc, char *argv[]);
__EXPORT int mavlink_push_main(int argc, char *argv[]);

int mavlink_push_main(int argc, char *argv[])
{
px4_task_spawn_cmd("deamon",SCHED_DEFAULT,SCHED_PRIORITY_DEFAULT,3600,mavlink_push_deamon_main,(char*const*)NULL);
return 0;

}
int mavlink_push_deamon_main(int argc, char*argv[])
{
while(1){
struct mavlink_test_s a;
a.x=x;
a.y=y;
a.z=z;
orb_advert_t a_pub = orb_advertise(ORB_ID(mavlink_test), &a);
orb_publish(ORB_ID(mavlink_test),a_pub,&a);
PX4_INFO("ca_trajectory:\t%8.4f\t%8.4f\t%8.4f",
							(double)a.x,
							(double)a.y,
							(double)a.z);
x++;
y++;
z++;
sleep(1);


}

return 0;

}


