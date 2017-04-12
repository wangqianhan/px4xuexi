px4_add_module(
	MODULE examples__mavlink_poll
	MAIN mavlink_poll
	STACK_MAIN 2000
	SRCS
		mavlink_poll.c
	DEPENDS
		platforms__common
	)
