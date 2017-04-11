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
#include <uORB/topics/sensor_combined.h>
#include <uORB/topics/ca_trajectory.h>

#include <systemlib/systemlib.h>
#include <systemlib/err.h>

static bool thread_should_exit = false;	/* px4_simple_app exit flag */
static bool thread_running = false;	/* px4_simple_app status flag */
static int daemon_task;	/* Handle of px4_simple_app task / thread */

/*
 * Thread management program
 */
__EXPORT int mavlink_msg_receiver_main(int argc, char *argv[]);

/*
 * Main loop of px4_simple_app
 * user thread, which is used to execute user code
 */
int mavlink_daemon_thread_main(int argc, char *argv[]);

/*
 * print the correct usage for px4_simple_app operating
 */
static void usage(const char *reason);
static void usage(const char *reason)
{
	if (reason){
		warnx("%s\n", reason);
	}

	warnx("usage: daemon {start|stop|status} [-p <additional params>]\n\n");
}

/*
 * px4_simple_app模块的前台管理程序，由shell启动，可以控制模块的线程的启动和停止
 */
int mavlink_msg_receiver_main(int argc, char *argv[])
{
	if (argc < 2){
		usage("missing command");
		return 1;
	}

	if (!strcmp(argv[1], "start")){		//shell start command
		if (thread_running){	//if the shell is already started
			warnx("daemon already running\n");
		}

		thread_should_exit = false;	//set the thread status bite is false
		daemon_task = px4_task_spawn_cmd("daemon", //the process name, commander
									     SCHED_DEFAULT,	//the scheduling type
										 SCHED_PRIORITY_DEFAULT, //the scheduling priority
										 2000, //the stack size of the new process or thread
										 mavlink_daemon_thread_main, //the task / thread main function
										 (argv) ? (char *const *)&argv[2] : (char *const *)NULL); // a void pointer to pass to the new task, in case holding
		//the commandline arguments
		return 0;
	}

	if (!strcmp(argv[1], "stop")){ //shell stop command
		thread_should_exit = true;
		return 0;
	}


	if (!strcmp(argv[1], "status")) { //shell inquire command
		if (thread_running) {
			warnx("\trunning\n");

		} else {
			warnx("\tnot started\n");
		}

		return 0;
	}

	usage("unrecognized command");
	return 1;

}

int mavlink_daemon_thread_main(int argc, char *argv[])
{

	warnx("[daemon] starting\n");

	thread_running = true;

	while(!thread_should_exit)
	{
		warnx("Hello Sky!");
		PX4_INFO("Hello Sky!");

		/* subscribe to sensor_combined topic */
		int sensor_sub_fd = orb_subscribe(ORB_ID(ca_trajectory));
		/* limit the update rate to 5 Hz */
		orb_set_interval(sensor_sub_fd, 200);

		/* advertise attitude topic */


		/* one could wait for multiple topics with this technique, just using one here */
		struct pollfd fds[] = {
			{ .fd = sensor_sub_fd,   .events = POLLIN },
			/* there could be more file descriptors here, in the form like:
			 * { .fd = other_sub_fd,   .events = POLLIN },
			 */
		};

		int error_counter = 0;

		while(1) {
			/* wait for sensor update of 1 file descriptor for 1000 ms (1 second) */
			int poll_ret = px4_poll(fds, 1, 10000);

			/* handle the poll result */
			if (poll_ret == 0) {
				/* this means none of our providers is giving us data */
				PX4_ERR("Got no data within a second");

			} else if (poll_ret < 0) {
				/* this is seriously bad - should be an emergency */
				if (error_counter < 10 || error_counter % 50 == 0) {
					/* use a counter to prevent flooding (and slowing us down) */
					PX4_ERR("ERROR return value from poll(): %d", poll_ret);
				}

				error_counter++;

			} else {

				if (fds[0].revents & POLLIN) {
					/* obtained data for the first file descriptor */
					struct ca_trajectory_s raw;
					/* copy sensors raw data into local buffer */
					orb_copy(ORB_ID(ca_trajectory), sensor_sub_fd, &raw);
					PX4_INFO("ca_trajectory:\t%8.4f\t%8.4f\t%8.4f",
							(double)raw.time_start_usec,
							(double)raw.time_stop_usec,
							(double)raw.seq_id);


				}

				/* there could be more file descriptors here, in the form like:
				 * if (fds[1..n].revents & POLLIN) {}
				 */
			}
		}

		sleep(30);
	}

    warnx("[daemon] exiting.\n");

    PX4_INFO("exiting");

    thread_running = false;

    return 0;
}
