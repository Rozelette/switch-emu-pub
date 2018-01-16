.text
.globl    ExitProcess
.globl    GetCurrentCoreNumber
.globl    OutputDebugString
.globl    ConnectToPort
.globl    SendSyncRequest
.globl    CreateThread
.globl    StartThread
.globl    ExitThread
.globl    SleepThread

.globl    getTLS

.type    ExitProcess,@function
.type    GetCurrentCoreNumber,@function
.type    OutputDebugString,@function
.type    ConnectToPort,@function
.type    SendSyncRequest,@function
.type    CreateThread,@function
.type    StartThread,@function
.type    ExitThread,@function
.type    SleepThread,@function

.type    getTLS,@function

ExitProcess:
	svc #0x07
	ret

CreateThread:
	svc #0x08
	mov	X0,	X1
	ret

StartThread:
	svc #0x09
	ret

ExitThread:
	svc #0x0A
	ret

SleepThread:
	svc #0x0B
	ret

GetCurrentCoreNumber:
	svc #0x10
	ret

ConnectToPort:
	svc #0x1F
	ret

SendSyncRequest:
	svc #0x21
	ret

OutputDebugString:
	svc #0x27
	ret

getTLS:
	MRS X0,	TPIDR_EL0
	ret