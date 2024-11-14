#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <Lmcons.h>
#include <VersionHelpers.h>
#include <Shlwapi.h>
#include <thread>
#pragma comment(lib, "Shlwapi.lib")

// ��������� �������
void PrintOSVersion();
void PrintProcessName();
void PrintCPUUsageTime();
void PrintMemoryUsage();
void ListFilesInDirectory(const TCHAR* directoryPath);
void ShowMainMenu();
void ShowContextMenu(const TCHAR* filePath);
void DeleteFileByPath(const TCHAR* filePath);
void CopyFileByPath(const TCHAR* sourcePath, const TCHAR* newFilePath);
void CopyFileWithUserInput();
void PrintDeveloperInfo();
void PrintHelp();
void ShowSystemUtilitiesMenu();
void StartUtilityInThread(const char* utilityPath);
void LogProcessStart(const char* processName, const char* logFileName);

void Terminal()
{
    TCHAR currentDirectory[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, currentDirectory);

    char command[256];
    printf("\n=== Terminal mode ===\n");
    printf("Type 'exit' to return to the main menu.\n");

    while (true)
    {
        printf("%S> ", currentDirectory);
        std::cin.getline(command, 256);

        // ������� ������
        if (strcmp(command, "exit") == 0)
        {
            printf("Exiting terminal...\n");
            break;
        }
        // ������� ������ ����������� ����������
        else if (strcmp(command, "lol") == 0)
        {
            ListFilesInDirectory(currentDirectory);
        }
        // ������� ����� ����������
        else if (strncmp(command, "kek", 3) == 0)
        {
            char* path = command + 3;
            if (SetCurrentDirectoryA(path))
            {
                GetCurrentDirectory(MAX_PATH, currentDirectory);
                printf("Directory changed to %S\n", currentDirectory);
            }
            else
            {
                printf("Failed to change directory. Check the path.\n");
            }
        }

        // ������� copy - ���������� ����� 
        else if (strncmp(command, "copy", 5) == 0)
        {
            char source[MAX_PATH], destination[MAX_PATH];
            if (sscanf_s(command + 5, "%s %s", source, destination) == 2)
            {
                CopyFileByPath((L"source.txt"), (L"destination.txt"));
            }
            else
            {
                printf("Usage: copy <source> <destination>\n");
            }
        }

        // ������� del - �������� �����
        else if (strncmp(command, "del ", 4) == 0)
        {
            char filePath[MAX_PATH];
            strcpy_s(filePath, command + 4);
            DeleteFileByPath((L"filePath"));
        }
        // ������� run � ������ �������
        else if (strncmp(command, "run ", 4) == 0)
        {
            char utility[MAX_PATH];
            strcpy_s(utility, command + 4);
            StartUtilityInThread(utility);
        }
        // ����������� �������
        else
        {
            printf("Unknown command: %s\n", command);
        }
    }
}

void LogProcessStart(const char* processName, const char* logFileName)
{
    FILE* logFile;
    fopen_s(&logFile, logFileName, "a");
    if (logFile == NULL)
    {
        printf("Failed to open log file: %s\n", logFileName);
        return;
    }

    SYSTEMTIME time;
    GetLocalTime(&time);
    fprintf(logFile, "Process: %s started at %02d:%02d:%02d\n", processName, time.wHour, time.wMinute, time.wSecond);
    fclose(logFile);
}


DWORD WINAPI LaunchUtility(LPVOID lpParameter)
{
    const char* utilityPath = (const char*)lpParameter;

    LogProcessStart(utilityPath, "process_log.txt");

    SHELLEXECUTEINFOA sei = { sizeof(sei) };
    sei.fMask = SEE_MASK_NOCLOSEPROCESS; // ��������� ������� ��������, ����� �������� ����� ��� ���������� ������
    sei.lpVerb = "runas"; // ����������� ������ �� ����� ��������������
    sei.lpFile = utilityPath; // ���� � ������������ �����
    sei.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteExA(&sei))
    {
        DWORD error = GetLastError();
        if (error == ERROR_CANCELLED) {
            printf("User canceled the request.\n");
        }
        else {
            printf("Failed to launch %s. Error: %lu\n", utilityPath, error);
        }
    }
    else
    {
        printf("%s launched successfully.\n", utilityPath);
        if (sei.hProcess != NULL) 
        {
        WaitForSingleObject(sei.hProcess, INFINITE);
        // ��������� ���������� ��������
        CloseHandle(sei.hProcess);
        }
        else 
        {
            printf("Process handle is NULL, skipping CloseHandle.\n");
        }

    }

    return 0;
}

void StartUtilityInThread(const char* utilityPath)
{
    // ������� �����
    DWORD threadId;
    HANDLE hThread = CreateThread(
        NULL,            // �������� ������������ �� ���������
        0,               // ������ ����� �� ���������
        LaunchUtility,   // ������� ������
        (LPVOID)utilityPath, // �������� ��� ������� ������
        0,               // ������ ������ ����������
        &threadId        // �������� ID ������
    );

    if (hThread == NULL)
    {
        printf("Failed to create thread for %s. Error: %lu\n", utilityPath, GetLastError());
    }
    else
    {
        WaitForSingleObject(hThread, INFINITE);
        // ��������� ���������� ������, ����� �������� ������
        CloseHandle(hThread);
    }
}

void PrintDeveloperInfo()
{
    printf("\n=== System ===\n");
    printf("WIN1_MANAGER_IVANOV_DMITRY_IVTm-41 v1.0\n");
    printf("DEVELOPER: IVANOV_DMITRY_SERGEEVICH \n");
    printf("MAIL: ivanovdmitry530@gmail.com\n");
    printf("DATE_OF_DEVELOPMENT: NOV 2024\n");
    printf("APPOINTMENT: Laboratory work for Makarov I.S.\n");
}

void PrintHelp()
{
    printf("\n=== Help (How to Use the File Manager) ===\n");
    printf("This file manager allows you to perform the following operations:\n");
    printf("1. Show OS Version: Displays the current version of the Windows operating system.\n");
    printf("2. Show Process Name: Displays the name of the current process.\n");
    printf("3. Show CPU Usage: Displays information about CPU usage time.\n");
    printf("4. Show Memory Usage: Shows the percentage of physical and virtual memory in use.\n");
    printf("5. List Files in Directory: Lists files in the specified directory.\n");
    printf("6. About (Developer Info): Displays information about the program's developer.\n");
    printf("7. Help: Displays this help message.\n");
    printf("8. Exit: Closes the file manager.\n");
    printf("\nTo choose an option, enter its number and press Enter.\n");
}

void CopyFileWithUserInput()
{
    TCHAR sourcePath[MAX_PATH];
    TCHAR newFilePath[MAX_PATH];

    printf("Enter the source file path: ");
    if (fgetws(sourcePath, MAX_PATH, stdin) == NULL)
    {
        printf("Error reading source file path.\n");
        return;
    }

    // ������� ������
    int clear;

    printf("Enter the destination file path: ");
    if (fgetws(newFilePath, MAX_PATH, stdin) == NULL)
    {
        printf("Error reading destination file path.\n");
        return;
    }

    while ((clear = getchar()) != '\n' && clear != EOF);

    // �������� ������� ����� ������
    size_t srcLen = wcslen(sourcePath);
    if (srcLen > 0 && sourcePath[srcLen - 1] == '\n')
    {
        sourcePath[srcLen - 1] = '\0';
    }

    size_t destLen = wcslen(newFilePath);
    if (destLen > 0 && newFilePath[destLen - 1] == '\n') {
        newFilePath[destLen - 1] = '\0';
    }

    // ����� ����� ��� �������
    printf("Source path: %S\n", sourcePath);
    printf("Destination path: %S\n", newFilePath);

    // ����� �������  
    CopyFileByPath(sourcePath, newFilePath);
}

void SimulateCPUWork() // �������� ������ ����������
{
    for (volatile long i = 0; i < 1000000000; ++i);
}

void PrintOSVersion() {
    if (IsWindows10OrGreater())
    {
        printf("Windows 10 or greater\n");
    }
    else if (IsWindows8OrGreater())
    {
        printf("Windows 8 or greater\n");
    }
    else if (IsWindows7OrGreater())
    {
        printf("Windows 7 or greater\n");
    }
    else {
        printf("Your operation system may not be Windows");
    }
}

void PrintProcessName()
{
    TCHAR processPATH[MAX_PATH];

    if (GetModuleFileName(NULL, processPATH, MAX_PATH) > 0) // �������� ������ ���� 
    {
        const TCHAR* processName = PathFindFileName(processPATH); // ��������� ������ ��� �����
        printf("Process Name: %S\n", processName);
    }
    else
    {
        DWORD error = GetLastError();
        printf("Failed to get process name. Error code: %lu\n", error);
    }
}

void PrintCPUUsageTime()
{
    HANDLE hProcess = GetCurrentProcess();
    FILETIME creationTime, exitTime, kernelTime, userTime;

    if (GetProcessTimes(hProcess, &creationTime, &exitTime, &kernelTime, &userTime)) {
        ULARGE_INTEGER ulKernelTime, ulUserTime;

        ulKernelTime.LowPart = kernelTime.dwLowDateTime;
        ulKernelTime.HighPart = kernelTime.dwHighDateTime;

        ulUserTime.LowPart = userTime.dwLowDateTime;
        ulUserTime.HighPart = userTime.dwHighDateTime;

        double kernelTimeInSeconds = ulKernelTime.QuadPart / 10000000.0;
        double userTimeInSeconds = ulUserTime.QuadPart / 10000000.0;

        printf("User CPU Time: %.2f seconds\n", userTimeInSeconds);
        printf("System CPU Time: %.2f seconds\n", kernelTimeInSeconds);
    }
    else {
        DWORD error = GetLastError();
        printf("Failed to get process times. Error code: %lu\n", error);
    }
}

void PrintMemoryUsage()
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);

    if (GlobalMemoryStatusEx(&statex))
    {
        DWORDLONG totalPhysicalMemory = statex.ullTotalPhys;
        DWORDLONG usedPhysicallMemory = totalPhysicalMemory - statex.ullAvailPhys;
        double physicalMemoryUsagePercent = (double)usedPhysicallMemory / totalPhysicalMemory * 100;

        DWORDLONG totalVirtualMemory = statex.ullTotalVirtual;
        DWORDLONG usedVirtualMemory = totalVirtualMemory - statex.ullAvailVirtual;
        double virtualMemoryUsagePercent = (double)usedVirtualMemory / totalVirtualMemory * 100;

        printf("Physical Memory Usage: %.2f%%\n", physicalMemoryUsagePercent);
        printf("Virtual Memory Usage: %.2f%%\n", virtualMemoryUsagePercent);
    }
    else {
        DWORD error = GetLastError();
        printf("Failed to retrieve memory status. Error code: %lu\n", error);
    }
}

// ������� ��� �������� �����
void DeleteFileByPath(const TCHAR* filePath)
{
    if (DeleteFile(filePath))
    {
        printf("File deleted successfully: %S\n", filePath);
    }
    else
    {
        DWORD error = GetLastError();
        printf("Failed to delete file: %S.Error code : %lu\n", filePath, error);
    }
}

// ������� ��� ����������� �����
void CopyFileByPath(const TCHAR* sourcePath, const TCHAR* newFilePath)
{
    // �������� �� ������ ���� �����
    if (sourcePath == NULL || sourcePath[0] == '\0')
    {
        printf("Source file path is empty.\n");
        return;
    }

    // ��������, ��� ���� ���������� �� ������
    if (newFilePath == NULL || newFilePath[0] == '\0') {
        printf("Destination path is empty.\n");
        return;
    }

    printf("Attempting to access source path: %S\n", sourcePath);

    DWORD fileAttributes = GetFileAttributes(sourcePath);
    if (fileAttributes == INVALID_FILE_ATTRIBUTES) {
        DWORD error = GetLastError();
        if (error == ERROR_FILE_NOT_FOUND) {
            printf("Source file does not exist: %S\n", sourcePath);
        }
        else {
            printf("Error accessing source file attributes. Error code: %lu\n", error);
        }
        return;
    }
    else {
        printf("Source file found, proceeding with copy...\n");
    }

    if (CopyFile(sourcePath, newFilePath, FALSE))
    {
        printf("File copied successfully to %S\n", newFilePath);
    }
    else
    {
        DWORD error = GetLastError();
        printf("Failed to copy file to %S. Error code: %lu\n", newFilePath, error);

        switch (error) {
        case ERROR_FILE_NOT_FOUND:
            printf("Error: Source file not found.\n");
            break;
        case ERROR_ACCESS_DENIED:
            printf("Error: Access denied. Please check permissions.\n");
            break;
        case ERROR_PATH_NOT_FOUND:
            printf("Error: Destination path not found.\n");
            break;
        case ERROR_ALREADY_EXISTS:
            printf("Error: File already exists at the destination.\n");
            break;
        default:
            printf("An unknown error occurred.\n");
            break;
        }
    }
}

// ������� ��� ������ ������ � ����������
void ListFilesInDirectory(const TCHAR* directoryPath)
{
    WIN32_FIND_DATA fd;
    TCHAR searchPath[MAX_PATH];
    lstrcpy(searchPath, directoryPath);
    lstrcat(searchPath, TEXT("\\*"));  // ����� ���� ������ � ����������

    HANDLE dh = FindFirstFile(searchPath, &fd);

    if (dh != INVALID_HANDLE_VALUE)
    {
        do
        {
            TCHAR filePath[MAX_PATH];
            lstrcpy(filePath, directoryPath);  // �������� ���� ����������
            lstrcat(filePath, TEXT("\\"));     // ��������� ����������� ����
            lstrcat(filePath, fd.cFileName);   // ��������� ��� �����

            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                printf("%25S  <DIR>\n", fd.cFileName);
            }
            else
            {
                printf("%25S\n", fd.cFileName);
                // ����� ������������ ���� ��� ������� �����
                ShowContextMenu(filePath);
            }
        } while (FindNextFile(dh, &fd));

        FindClose(dh);
    }
    else {
        DWORD error = GetLastError();
        printf("Directory not found or cannot be opened. Error code: %lu\n", error);
    }
}

// �������� ����
void ShowMainMenu()
{
    int choice;
    do {
        printf("\n==== Main Menu ====\n");
        printf("1. Show OS Version\n");
        printf("2. Show Process Name\n");
        printf("3. Show CPU Usage Time\n");
        printf("4. Show Memory Usage\n");
        printf("5. List Files in Directory\n");
        printf("6. About (Developer Info)\n");
        printf("7. Help\n");
        printf("8. Open System Utilities\n");
        printf("9. Exiting\n");
        printf("10. Open Terminal\n");
        printf("Enter your choice: ");
        std::cin >> choice;

        switch (choice)
        {
        case 1:
            PrintOSVersion();
            break;
        case 2:
            PrintProcessName();
            break;
        case 3:
            PrintCPUUsageTime();
            break;
        case 4:
            PrintMemoryUsage();
            break;
        case 5:
            // ListFilesInDirectory(TEXT("C:\\Users\\Huawei\\source\\repos"));
            ListFilesInDirectory(TEXT("C:\\Users\\Huawei\\Documents\\OS_LABS"));
            break;
        case 6:
            PrintDeveloperInfo();
            break;
        case 7:
            PrintHelp();
            break;
        case 8:
            ShowSystemUtilitiesMenu();
            break;
        case 9:
            printf("Exiting...\n");
            break;
        case 10:
            printf("Open Terminal\n");
            Terminal();
            break;
        default:
            printf("Invalid choice, try again.\n");
        }
    } while (choice != 10);
}

// ����������� ���� ��� ������ � ���������� ������
void ShowContextMenu(const TCHAR* filePath)
{
    int choice;
    printf("\n-- Context Menu for File: %S --\n", filePath);
    printf("1. Show File Info\n");
    printf("2. Delete File\n");
    printf("3. Copy File\n");
    printf("4. Skip\n");
    printf("Enter your choice: ");
    std::cin >> choice;

    switch (choice)
    {
    case 1:
    {
        HANDLE hf = CreateFile(filePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hf != INVALID_HANDLE_VALUE)
        {
            BY_HANDLE_FILE_INFORMATION fi;
            GetFileInformationByHandle(hf, &fi);

            SYSTEMTIME st;
            FileTimeToSystemTime(&(fi.ftCreationTime), &st);

            printf("File created: %02d-%02d-%04d %02d:%02d\n", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute);
            CloseHandle(hf);
        }
    }
    break;
    case 2:
        DeleteFileByPath(filePath);
        break;
    case 3:
        CopyFileWithUserInput();
        break;
    case 4:
        printf("Skipping file.\n");
        break;
    default:
        printf("Invalid choice, returning to main menu.\n");
    }
}

void ShowSystemUtilitiesMenu()
{
    int choice;
    printf("\n-- System Utilities Menu --\n");
    printf("1. Open Task Manager\n");
    printf("2. Open Command Prompt\n");
    printf("3. Open Control Panel\n");
    printf("4. Open Services\n");
    printf("5. Open Device Manager\n");
    printf("6. Open Registry Editor\n");
    printf("7. Return to Previous Menu\n");
    printf("Enter your choice: ");
    std::cin >> choice;

    switch (choice)
    {
    case 1:
        StartUtilityInThread("taskmgr.exe"); // ������ ���������� �����
        break;
    case 2:
        StartUtilityInThread("cmd"); // ������ ��������� ������
        break;
    case 3:
        StartUtilityInThread("control"); // ������ ������ ����������
        break;
    case 4:
        StartUtilityInThread("services.msc");  // ������ ���� ������
        break;
    case 5:
        StartUtilityInThread("devmgmt.msc");  // ������ ���������� ���������
        break;
    case 6:
        StartUtilityInThread("regedit");  // ������ ��������� �������
        break;
    case 7:
        printf("Returning to previous menu.\n");
        break;
    default:
        printf("Invalid choice, returning to previous menu.\n");
    }
}

int main()
{
    // ������ ������� ������
    ShowMainMenu();
    return 0;
}