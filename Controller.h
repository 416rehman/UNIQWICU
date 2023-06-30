#pragma once
#include <cstdint>
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include "..\common.h"

class Controller
{
public:
	uintptr_t baseAddress = 0;
	std::uint32_t processId = 0;
	const char* moduleName = common::targetProcessModuleName;

	bool init();

	template <typename ... Arg>
	uint64_t callHookedSourceFunc(const Arg ... args)
	{
		LoadLibrary("user32.dll");
		void* hookedFunc = GetProcAddress(LoadLibrary(common::hookedLibrary), common::hookedRoutineName);
		
		auto func = static_cast<uint64_t(_stdcall*)(Arg...)>(hookedFunc);
		
		return func(args...);
	}


	std::uint32_t getProcessId(std::string_view procName);
	ULONG64 loadModuleBaseAddress(const char* moduleName);
	bool writeMemory(uintptr_t address, UINT_PTR value, size_t size);

	template <typename T>
	T readMemory(uintptr_t address)
	{
		T response{};

		common::PAYLOAD payload;
		payload.pid = processId;
		payload.targetAddress = address;
		payload.outputSize = sizeof(T);
		payload.operation = common::PayloadOperation::GenericReadMemory;
		payload.output = &response;

		callHookedSourceFunc(&payload);

		return response;
	}

	template <typename T>
	bool writeMemory(uintptr_t address, const T& value)
	{
		return writeMemory(address, (UINT_PTR)value, sizeof(T));
	}
};
