#include "ModuleProject.h"

#include "fmt/format.h"

#include "Application.h"
#include "Modules/ModuleFiles.h"
#include "Modules/ModuleEvents.h"
#include "Utils/Logging.h"
#include "Utils/Buffer.h"
#include "Utils/UID.h"
#include "Utils/FileDialog.h"

#include <Windows.h>
#include <shellapi.h>
#include <ObjIdl.h>

#include "Utils/Leaks.h"

template<class T>
static T struct_cast(void* ptr, LONG offset = 0) {
	return reinterpret_cast<T>(reinterpret_cast<intptr_t>(ptr) + offset);
}

struct RSDSHeader {
	DWORD signature;
	GUID guid;
	long version;
	char filename[1];
};

static std::string& GetLastErrorStdStr() {
	DWORD error = GetLastError();
	if (error) {
		LPVOID lpMsgBuf;
		DWORD bufLen = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			error,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf,
			0,
			NULL);
		if (bufLen) {
			LPCSTR lpMsgStr = (LPCSTR) lpMsgBuf;
			std::string result(lpMsgStr, lpMsgStr + bufLen);

			LocalFree(lpMsgBuf);

			return result;
		}
	}
	return std::string();
}

static bool DebugDirRelativeVirtualAddress(PIMAGE_OPTIONAL_HEADER optionalHeader, DWORD& debugDirRva, DWORD& debugDirSize) {
	if (optionalHeader->Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
		// Represents the optional header format.
		PIMAGE_OPTIONAL_HEADER64 optionalHeader64 = struct_cast<PIMAGE_OPTIONAL_HEADER64>(optionalHeader);
		debugDirRva = optionalHeader64->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress;
		debugDirSize = optionalHeader64->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].Size;
	} else {
		// Represents the optional header format.
		PIMAGE_OPTIONAL_HEADER32 optionalHeader32 = struct_cast<PIMAGE_OPTIONAL_HEADER32>(optionalHeader);
		debugDirRva = optionalHeader32->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress;
		debugDirSize = optionalHeader32->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].Size;
	}

	if (debugDirRva == 0 && debugDirSize == 0) {
		return true;
	} else if (debugDirRva == 0 || debugDirSize == 0) {
		return false;
	}

	return true;
}

static bool FileOffsetRelativeVirtualAdress(PIMAGE_NT_HEADERS ntHeaders, DWORD rva, DWORD& fileOffset) {
	bool found = false;

	// Represents the image section header format.
	IMAGE_SECTION_HEADER* sectionHeader = IMAGE_FIRST_SECTION(ntHeaders);
	for (int i = 0; i < ntHeaders->FileHeader.NumberOfSections && !found; ++i, ++sectionHeader) {
		if (sectionHeader->VirtualAddress <= rva) {
			if (sectionHeader->VirtualAddress + sectionHeader->Misc.VirtualSize > rva) {
				found = true;
				fileOffset = rva - sectionHeader->VirtualAddress + sectionHeader->PointerToRawData;
			}
		}
	}

	if (!found) {
		return false;
	}

	return true;
}

static char* PDBFind(LPBYTE imageBase, PIMAGE_DEBUG_DIRECTORY debugDir) {
	assert(debugDir && imageBase);
	LPBYTE debugInfo = const_cast<LPBYTE>(imageBase + debugDir->PointerToRawData);
	const DWORD debugInfoSize = debugDir->SizeOfData;

	if (debugInfo == 0 || debugInfoSize == 0) {
		return nullptr;
	}

	if (IsBadReadPtr(debugInfo, debugInfoSize)) {
		return nullptr;
	}

	if (debugInfoSize < sizeof(DWORD)) {
		return nullptr;
	}

	if (debugDir->Type == IMAGE_DEBUG_TYPE_CODEVIEW) {
		auto signature = *(DWORD*) debugInfo;
		if (signature == 'SDSR') {
			auto* info = (RSDSHeader*) (debugInfo);
			if (IsBadReadPtr(debugInfo, sizeof(RSDSHeader))) {
				return nullptr;
			}
			if (IsBadStringPtrA((const char*) info->filename, UINT_MAX)) {
				return nullptr;
			}
			return info->filename;
		}
	}

	return nullptr;
}

bool ModuleProject::PDBReplace(const std::string& filename, const std::string& namePDB, std::string& originalPDB) {
	HANDLE fp = nullptr;
	HANDLE filemap = nullptr;
	LPVOID mem = nullptr;
	bool result = false;

	DEFER {
		if (mem != nullptr) {
			UnmapViewOfFile(mem);
		}

		if (filemap != nullptr) {
			CloseHandle(filemap);
		}

		if ((fp != nullptr) && (fp != INVALID_HANDLE_VALUE)) {
			CloseHandle(fp);
		}
	};

	fp = CreateFileA(filename.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if ((fp == INVALID_HANDLE_VALUE) || (fp == nullptr)) {
		return false;
	}

	// Creates or opens a named or unnamed file mapping object for a specified file.
	filemap = CreateFileMapping(fp, nullptr, PAGE_READWRITE, 0, 0, nullptr);
	if (filemap == nullptr) {
		return false;
	}

	// Maps a view of a file mapping into the address space of a calling process.
	mem = MapViewOfFile(filemap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (mem == nullptr) {
		return false;
	}

	//  Represents the DOS header format.
	PIMAGE_DOS_HEADER dosHeader = struct_cast<PIMAGE_DOS_HEADER>(mem);
	if (dosHeader == 0) {
		return false;
	}

	// Verifies that the calling process has read access to the specified range of memory.
	if (IsBadReadPtr(dosHeader, sizeof(IMAGE_DOS_HEADER))) {
		return false;
	}

	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		return false;
	}

	// Represents the PE header format.
	PIMAGE_NT_HEADERS ntHeaders = struct_cast<PIMAGE_NT_HEADERS>(dosHeader, dosHeader->e_lfanew);
	if (ntHeaders == 0) {
		return false;
	}

	// Verifies that the calling process has read access to the specified range of memory.
	if (IsBadReadPtr(ntHeaders, sizeof(ntHeaders->Signature))) {
		return false;
	}

	// Check if signature of the NT header is the correct one
	if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) {
		return false;
	}

	// Verifies that the calling process has read access to the specified range of memory.
	if (IsBadReadPtr(&ntHeaders->FileHeader, sizeof(IMAGE_FILE_HEADER))) {
		return false;
	}

	if (IsBadReadPtr(&ntHeaders->OptionalHeader, ntHeaders->FileHeader.SizeOfOptionalHeader)) {
		return false;
	}

	if (ntHeaders->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC && ntHeaders->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
		return false;
	}

	// Retrieve the contents of PE file sections
	auto sectionHeaders = IMAGE_FIRST_SECTION(ntHeaders);
	if (IsBadReadPtr(sectionHeaders, ntHeaders->FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER))) {
		return false;
	}

	DWORD debugDirVRA = 0;
	DWORD debugDirSize = 0;
	if (!DebugDirRelativeVirtualAddress(&ntHeaders->OptionalHeader, debugDirVRA, debugDirSize)) {
		return false;
	}

	if (debugDirVRA == 0 || debugDirSize == 0) {
		return false;
	}

	DWORD offset = 0;
	if (!FileOffsetRelativeVirtualAdress(ntHeaders, debugDirVRA, offset)) {
		return false;
	}

	// Represents the debug directory format.
	PIMAGE_DEBUG_DIRECTORY debugDir = struct_cast<PIMAGE_DEBUG_DIRECTORY>(mem, offset);
	if (debugDir == 0) {
		return false;
	}

	if (IsBadReadPtr(debugDir, debugDirSize)) {
		return false;
	}

	if (debugDirSize < sizeof(IMAGE_DEBUG_DIRECTORY)) {
		return false;
	}

	int numEntries = debugDirSize / sizeof(IMAGE_DEBUG_DIRECTORY);
	if (numEntries == 0) {
		return false;
	}

	for (int i = 1; i <= numEntries; ++i, ++debugDir) {
		char* pdb = PDBFind((LPBYTE) mem, debugDir);
		if (pdb) {
			size_t len = strlen(pdb);
			if (len >= strlen(namePDB.c_str())) {
				originalPDB = pdb;
				memcpy_s(pdb, len, namePDB.c_str(), namePDB.length());
				pdb[namePDB.length()] = 0;
				result = true;
			}
		}
	}

	return result;
}

namespace Tesseract {
	std::istream& getline(std::istream& is, std::string& t) {
		// Clear to add the new value
		t.clear();

		std::istream::sentry se(is, true);
		std::streambuf* sb = is.rdbuf();

		for (;;) {
			int c = sb->sbumpc();
			switch (c) {
			case '\n':
				return is;
			case '\r':
				if (sb->sgetc() == '\n')
					sb->sbumpc();
				return is;
			case std::streambuf::traits_type::eof():
				// Also handle the case when the last line has no line ending
				if (t.empty())
					is.setstate(std::ios::eofbit);
				return is;
			default:
				t += (char) c;
				break;
			}
		}
	}
} // namespace Tesseract

bool ModuleProject::Init() {
#if GAME
	UnloadGameCodeDLL();
	if (!LoadLibrary("Penteract.dll")) {
		LOG("%s", GetLastErrorStdStr().c_str());
	}
#else
	LoadProject("Penteract/Penteract.sln");
#endif
	return true;
}

bool ModuleProject::CleanUp() {
	UnloadGameCodeDLL();
	return true;
}

void ModuleProject::LoadProject(const char* path) {
	projectPath = FileDialog::GetFileFolder(path);
	projectName = FileDialog::GetFileNameAndExtension(path);
	App->files->AddSearchPath(projectPath.c_str());

	if (!App->files->Exists(projectName.c_str())) {
		CreateNewProject(projectPath.c_str(), "");
	}

#ifdef _DEBUG
	CompileProject(Configuration::DEBUG_EDITOR);
#else
	CompileProject(Configuration::RELEASE_EDITOR);
#endif // _DEBUG
}

void ModuleProject::CreateScript(std::string& name) {
	Buffer<char> bufferHeader = App->files->Load("Templates/Header");
	Buffer<char> bufferSource = App->files->Load("Templates/Source");

	std::string header = bufferHeader.Data();
	std::string source = bufferSource.Data();

	std::string assetsPath = "Assets/Scripts/";
	std::string realPath = projectPath + "/Source/";

	std::string result;

	result = fmt::format(header, name, "{", "}");
	App->files->Save((assetsPath + name + ".h").c_str(), result.data(), result.size());

	result = fmt::format(source, name, "{", "}");
	App->files->Save((assetsPath + name + ".cpp").c_str(), result.data(), result.size());
}

void ModuleProject::CreateNewProject(const char* name, const char* path) {
	std::string fullPath = std::string(path) + name;
	std::string batchPath = fullPath + "/Batches";

	App->files->CreateFolder(fullPath.c_str());
	App->files->CreateFolder(batchPath.c_str());

	std::string UIDProject("{");
	UIDProject += GenerateUID128();
	UIDProject += "}";

	CreateMSVCSolution((fullPath + "/" + name + ".sln").c_str(), name, UIDProject.c_str());
	CreateMSVCProject((fullPath + "/" + name + ".vcxproj").c_str(), name, UIDProject.c_str());

	CreateBatches();
}

void ModuleProject::CreateMSVCSolution(const char* path, const char* name, const char* UIDProject) {
	Buffer<char> buffer = App->files->Load("Templates/MSVCSolution");

	std::string solution = buffer.Data();

	std::string UIDSolution("{");
	UIDSolution += GenerateUID128();
	UIDSolution += "}";

	std::string result = fmt::format(solution, name, UIDProject, UIDSolution);

	App->files->Save(path, result.data(), result.size());
}

void ModuleProject::CreateMSVCProject(const char* path, const char* name, const char* UIDProject) {
	Buffer<char> buffer = App->files->Load("Templates/MSVCProject");

	std::string project = buffer.Data();
	std::string enginePath = FileDialog::GetFileFolder(FileDialog::GetAbsolutePath("").c_str());

#ifdef _DEBUG
	std::string result = fmt::format(project, name, UIDProject, "../../Project/Source/", "../../Project/Libs/MathGeoLib", "../../Project/Libs/SDL/include", "../../Project/Libs/rapidjson/include", "../../Project/Libs/OpenAL-soft/include", enginePath);
#else
	std::string result = fmt::format(project, name, UIDProject, "../Engine/Source/", "../Engine/Libs/MathGeoLib", "../Engine/Libs/SDL/include", "../Engine/Libs/rapidjson/include", "../Engine/Libs/OpenAL-soft/include", enginePath);
#endif

	App->files->Save(path, result.data(), result.size());
}

void ModuleProject::CreateBatches() {
	Buffer<char> buffer;
	std::string file;
	std::string result;

	// Debug (.exe)
	buffer = App->files->Load("Templates/BuildDebug");
	file = buffer.Data();
	result = fmt::format(file, projectName);
	App->files->Save((projectPath + "/Batches/BuildDebug.bat").c_str(), result.data(), result.size());

	// Debug Editor (.dll)
	buffer = App->files->Load("Templates/BuildDebugEditor");
	file = buffer.Data();
	result = fmt::format(file, projectName);
	App->files->Save((projectPath + "/Batches/BuildDebugEditor.bat").c_str(), result.data(), result.size());

	// Release (.exe)
	buffer = App->files->Load("Templates/BuildRelease");
	file = buffer.Data();
	result = fmt::format(file, projectName);
	App->files->Save((projectPath + "/Batches/BuildRelease.bat").c_str(), result.data(), result.size());

	// Release Editor (.dll)
	buffer = App->files->Load("Templates/BuildReleaseEditor");
	file = buffer.Data();
	result = fmt::format(file, projectName);
	App->files->Save((projectPath + "/Batches/BuildReleaseEditor.bat").c_str(), result.data(), result.size());
}

void ModuleProject::CompileProject(Configuration config) {
	UnloadGameCodeDLL();

	std::string batchDir = projectPath + "/Batches";

	SHELLEXECUTEINFO sei = {0};
	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.fMask = SEE_MASK_NOCLOSEPROCESS;
	sei.hwnd = NULL;
	sei.lpVerb = "open";
	sei.lpParameters = NULL;
	sei.lpDirectory = batchDir.c_str();
	sei.nShow = NULL;
	sei.hInstApp = NULL;

	switch (config) {
	case Configuration::RELEASE:
		sei.lpFile = "BuildRelease.bat";
		break;
	case Configuration::RELEASE_EDITOR:
		sei.lpFile = "BuildReleaseEditor.bat";
		break;
	case Configuration::DEBUG:
		sei.lpFile = "BuildDebug.bat";
		break;
	case Configuration::DEBUG_EDITOR:
		sei.lpFile = "BuildDebugEditor.bat";
		break;
	default:
		sei.lpFile = "";
		break;
	}

	ShellExecuteEx(&sei);
	WaitForSingleObject(sei.hProcess, INFINITE);
	CloseHandle(sei.hProcess);

	std::string buildPath = projectPath;
	std::string name = FileDialog::GetFileName(projectName.c_str());

	switch (config) {
	case Configuration::RELEASE:
		buildPath += "/Build/Release/";
		break;
	case Configuration::RELEASE_EDITOR:
		buildPath += "/Build/ReleaseEditor/";
		break;
	case Configuration::DEBUG:
		buildPath += "/Build/Debug/";
		break;
	case Configuration::DEBUG_EDITOR:
		buildPath += "/Build/DebugEditor/";
		break;
	}

	std::string originalPDB;
	std::string dllPath = buildPath + name + ".dll";
	std::string auxName = name;
	auxName[auxName.size() - 1] = '_';
	auxName += ".pdb";
	PDBReplace(dllPath, auxName, originalPDB);
	CopyFile(originalPDB.c_str(), auxName.c_str(), FALSE);
	std::string logFile = buildPath + name + ".log";

	Buffer<char> buffer = App->files->Load(logFile.c_str());
	std::string logContent = "";
	if (buffer.Size() > 0) {
		logContent = buffer.Data();
	}

	std::istringstream f(logContent);
	std::string line;
	LOG("---------------------GAME COMPILATION---------------------");
	LOG("%s", logContent.c_str());
	while (Tesseract::getline(f, line)) {
		LOG("%s", line.c_str());
	}
	LOG("----------------------------------------------------------");

	buildPath += name;
	std::string newPath(name);
	newPath[newPath.size() - 1] = '_';
	newPath += ".dll";
	buildPath += ".dll";

	if (!CopyFileA(buildPath.c_str(), newPath.c_str(), FALSE)) {
		LOG("Copy fails");
	}

	if (!LoadGameCodeDLL(buildPath.c_str())) {
		LOG("DLL NOT LOADED: %s", GetLastErrorStdStr().c_str());
	}

	App->events->AddEvent(TesseractEventType::COMPILATION_FINISHED);
}

bool ModuleProject::LoadGameCodeDLL(const char* path) {
	if (gameCodeDLL) {
		LOG("DLL already loaded.");
		return false;
	}

	gameCodeDLL = LoadLibraryA(path);

	return gameCodeDLL ? true : false;
}

bool ModuleProject::UnloadGameCodeDLL() {
	if (!gameCodeDLL) {
		LOG("No DLL to unload.");
		return false;
	}

	FreeLibrary(gameCodeDLL);

	gameCodeDLL = nullptr;

	return true;
}