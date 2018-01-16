.text
.globl    ExitProcess
.globl    GetCurrentCoreNumber
.globl    OutputDebugString
.globl    ConnectToPort

ExitProcess:
	svc #0x07
	ret

GetCurrentCoreNumber:
	svc #0x10
	ret

ConnectToPort:
	svc #0x1F
	ret

OutputDebugString:
	svc #0x27
	ret