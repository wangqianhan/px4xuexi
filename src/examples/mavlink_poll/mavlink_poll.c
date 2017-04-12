#include <px4_tasks.h>//demon预约
#include <px4_posix.h>
#include <unistd.h>
#include <stdio.h>
#include<px4_log.h>

#include <uORB/uORB.h>
#include<uORB/topics/mavlink_test.h>
__EXPORT int mavlink_poll_main(int argc, char *argv[]);
int mavlink_poll_main(int argc, char *argv[] )
{
   int handle=orb_subscribe(ORB_ID(mavlink_test));
struct mavlink_test_s wang;
while(1) 
 {
orb_copy(ORB_ID(mavlink_test),handle,&wang);
PX4_INFO("%f,%f,%f",(double)wang.x,(double)wang.y,(double)wang.z);
sleep(1);
 }
return 0;
}




