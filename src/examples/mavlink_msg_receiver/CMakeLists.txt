px4_add_module(
	MODULE examples__mavlink_msg_receiver
	MAIN mavlink_msg_receiver
	STACK_MAIN 2000
	SRCS
		mavlink_msg_receiver.c
	DEPENDS
		platforms__common
	)
