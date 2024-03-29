        AREA |C$$code|, CODE, READONLY
|x$codeseg|

	EXPORT svc_kprintf
	EXPORT svc_createitem
	EXPORT svc_deleteitem
	EXPORT svc_finditem
	EXPORT svc_closeitem
        EXPORT svc_yield
	EXPORT svc_openitem
	EXPORT svc_setitempri
	EXPORT svc_setitemowner
	EXPORT svc_lockitem
	EXPORT svc_unlockitem
	EXPORT svc_findandopenitem
	EXPORT svc_remhead
	EXPORT svc_addhead
	EXPORT svc_remtail
	EXPORT svc_addtail
	EXPORT svc_inserttail
	EXPORT svc_removenode
	EXPORT svc_allocmem
	EXPORT svc_freemem
	EXPORT svc_initlist
	EXPORT svc_findnamednode
	EXPORT svc_scavengemem
	EXPORT svc_locateitem
	EXPORT svc_memset
	EXPORT svc_memcpy
	EXPORT svc_getpagesize

	IMPORT KernelBase

svc_kprintf
	mov r12,#&0e
	b call_swi_in_r12

svc_createitem
	mov r12,#&0
	b call_swi_in_r12

svc_deleteitem
	mov r12,#3
	b call_swi_in_r12
svc_finditem
	mov r12,#4
	b call_swi_in_r12
svc_closeitem
	mov r12,#8
	b call_swi_in_r12
svc_openitem
	mov r12,#5
	b call_swi_in_r12
svc_yield
        mov r12,#9
        b call_swi_in_r12
svc_setitempri
	mov r12,#10
	b call_swi_in_r12
svc_setitemowner
	mov r12,#28
	b call_swi_in_r12
svc_lockitem
	mov r12,#7
	b call_swi_in_r12
svc_unlockitem
	mov r12,#6
	b call_swi_in_r12
svc_findandopenitem
	mov r12,#36
	b call_swi_in_r12
svc_remhead
	mov r12,#-1
	b call_swi_in_r12
svc_addhead
	mov r12,#-2
	b call_swi_in_r12
svc_remtail
	mov r12,#-3
	b call_swi_in_r12
svc_addtail
	mov r12,#-4
	b call_swi_in_r12
svc_inserttail
	mov r12,#-5
	b call_swi_in_r12
svc_removenode
	mov r12,#-6
	b call_swi_in_r12
svc_allocmem
	mov r12,#-7
	b call_swi_in_r12
svc_freemem
	mov r12,#-8
	b call_swi_in_r12
svc_initlist
	mov r12,#-9
	b call_swi_in_r12
svc_findnamednode
	mov r12,#-10
	b call_swi_in_r12
svc_scavengemem
	mov r12,#-11
	b call_swi_in_r12
svc_locateitem
	mov r12,#-12
	b call_swi_in_r12
svc_memset
	mov r12,#-13
	b call_swi_in_r12
svc_memcpy
	mov r12,#-14
	b call_swi_in_r12
svc_getpagesize
	mov r12,#-15
	b call_swi_in_r12


call_swi_in_r12
	stmfd sp!, {r9,lr}
	ldr r9,=KernelBase
	ldr r9, [r9]
	ldrb lr, [r9, #&2a]
	add r12,r12,lr
	add r12,r12,#1
	mov lr,pc
	ldr pc,[r9,-r12,lsl#2]
	ldmfd sp!, {r9,pc}

	END
