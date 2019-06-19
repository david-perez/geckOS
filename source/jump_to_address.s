	.text
	.thumb
	.thumb_func
	.align      2
	.globl      jump_to_address
	.type       jump_to_address, %function
jump_to_address:
	push	{lr}
	blx 	r0
	pop 	{pc}
	.size       jump_to_address, . - jump_to_address
