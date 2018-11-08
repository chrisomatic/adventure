#pragma once

/* multimedia functions (such as MIDI) for Windows */

// Plays a specified MIDI file by using MCI_OPEN and MCI_PLAY. Returns 
// as soon as playback begins. The window procedure function for the 
// specified window will be notified when playback is complete. 
// Returns 0L on success; otherwise, it returns an MCI error code.

DWORD playMIDIFile(HWND hWndNotify, LPSTR lpszMIDIFileName)
{

	UINT wDeviceID;
	DWORD dwReturn;
	MCI_OPEN_PARMS mciOpenParms;
	MCI_PLAY_PARMS mciPlayParms;
	MCI_STATUS_PARMS mciStatusParms;

	// Open the device by specifying the device and filename.
	// MCI will attempt to choose the MIDI mapper as the output port.
	mciOpenParms.lpstrDeviceType = "sequencer";
	mciOpenParms.lpstrElementName = lpszMIDIFileName;
	if (dwReturn = mciSendCommand(NULL, MCI_OPEN,
		MCI_OPEN_TYPE | MCI_OPEN_ELEMENT,
		(DWORD)(LPVOID)&mciOpenParms))
	{
		// Failed to open device. Don't close it; just return error.
		return (dwReturn);
	}

	// The device opened successfully; get the device ID.
	wDeviceID = mciOpenParms.wDeviceID;

	// Check if the output port is the MIDI mapper.
	mciStatusParms.dwItem = MCI_SEQ_STATUS_PORT;
	if (dwReturn = mciSendCommand(wDeviceID, MCI_STATUS,
		MCI_STATUS_ITEM, (DWORD)(LPVOID)&mciStatusParms))
	{
		mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);
		return (dwReturn);
	}

	// Begin playback. The window procedure function for the parent 
	// window will be notified with an MM_MCINOTIFY message when 
	// playback is complete. At this time, the window procedure closes 
	// the device.
	mciPlayParms.dwCallback = (DWORD)hWndNotify;
	if (dwReturn = mciSendCommand(wDeviceID, MCI_PLAY, MCI_NOTIFY,
		(DWORD)(LPVOID)&mciPlayParms))
	{
		mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);
		return (dwReturn);
	}

	return (0L);
}

static int get_files_in_directory_with_extension(const char* directory_path, const char* extension,char file_paths[100][MAX_PATH])
{
    WIN32_FIND_DATA ffd;
    TCHAR szDir[MAX_PATH];
    HANDLE hFind = INVALID_HANDLE_VALUE;
    size_t length_of_arg;
    int num_files = 0;

    StringCchLength(directory_path, MAX_PATH, &length_of_arg);

    if (length_of_arg > (MAX_PATH - 3))
    {
        printf(TEXT("\nDirectory path is too long.\n"));
        return (-1);
    }

    StringCchCopy(szDir, MAX_PATH, directory_path);
    StringCchCat(szDir, MAX_PATH, TEXT("\\*")); 
    StringCchCat(szDir, MAX_PATH, TEXT(extension));

    hFind = FindFirstFile(szDir, &ffd);

    if (INVALID_HANDLE_VALUE == hFind) 
    {
        printf(TEXT("\nError getting first file.\n"));
        return -1;
    } 

    char full_file_path[MAX_PATH] = {0};
    StringCchCopy(full_file_path,MAX_PATH,directory_path);
	StringCchCat(full_file_path, MAX_PATH, "\\");
	StringCchCat(full_file_path, MAX_PATH, ffd.cFileName);
    StringCchCopy(file_paths[num_files],MAX_PATH,full_file_path);

	do
    {
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            // directory
        }
        else
        {
            StringCchCopy(full_file_path,MAX_PATH,directory_path);
            StringCchCat(full_file_path, MAX_PATH, "\\");
            StringCchCat(full_file_path, MAX_PATH, ffd.cFileName);
            StringCchCopy(file_paths[num_files++],MAX_PATH,full_file_path);
        }
	} while (FindNextFile(hFind, &ffd) != 0);
    

    FindClose(hFind); 

    return num_files;
}

