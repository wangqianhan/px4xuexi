px4_add_module(
	MODULE examples__mavlink_push
	MAIN mavlink_push
	STACK_MAIN 2000
	SRCS
		mavlink_push.c
	DEPENDS
		platforms__common
	)
