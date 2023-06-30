#include "Controller.h"

bool Controller::init()
{
	processId = getProcessId(moduleName);
	if (!processId)
	{
		std::cout << "Failed to get process id!\n";
		return false;
	}
	std::cout << "Process: " << moduleName << " (" << processId << ")\n";

	baseAddress = loadModuleBaseAddress(moduleName);
	if (!baseAddress)
	{
		std::cout << "Failed to get module base address!\n";
		return false;
	}
	std::cout << "Base address: " << std::hex << baseAddress << "\n";

	return true;
}

std::uint32_t Controller::getProcessId(std::string_view procName)
{
	struct HandleDeleter
	{
		using pointer = HANDLE;

		void operator()(HANDLE handle) const
		{
			if (handle != NULL || handle != INVALID_HANDLE_VALUE)
			{
				CloseHandle(handle);
			}
		}
	};

	using uniqueHandle = std::unique_ptr<HANDLE, HandleDeleter>;

	const uniqueHandle snapshotHandle{ CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL) };
	if (snapshotHandle.get() == INVALID_HANDLE_VALUE)
	{
		std::cout << "Failed to create snapshot!\n";
		return NULL;
	}

	PROCESSENTRY32 processEntry = { 0 };
	processEntry.dwSize = sizeof(MODULEENTRY32);

	while (Process32Next(snapshotHandle.get(), &processEntry))
	{
		if (procName.compare(processEntry.szExeFile) == 0)
		{
			return processEntry.th32ProcessID;
		}
	}

	return NULL;
}

ULONG64 Controller::loadModuleBaseAddress(const char* moduleName)
{
	std::cout << "Loading module base address...\n";
	ULONG64 baseAddress = 0;

	common::PAYLOAD payload;
	payload.operation = common::PayloadOperation::RequestBaseAddress;
	payload.pid = processId;
	payload.input = (void*)moduleName;
	payload.output = &baseAddress;

	callHookedSourceFunc(&payload);

	return (ULONG64)payload.output;
}

bool Controller::writeMemory(uintptr_t address, UINT_PTR value, size_t size)
{
	common::PAYLOAD payload;
	payload.pid = processId;
	payload.targetAddress = address;
	payload.inputSize = size;
	payload.operation = common::PayloadOperation::GenericWriteMemory;
	payload.input = (void*)value;

	callHookedSourceFunc(&payload);

	return true;
}
