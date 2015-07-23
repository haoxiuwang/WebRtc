#include "webrtc/libjingle/examples/call/CommonUtilities.h"
   


 LPWSTR AnsiToWchar(LPCSTR ansi)  
{  
	int WLength = ::MultiByteToWideChar(CP_ACP, 0, ansi, -1, NULL, 0);  
	LPWSTR pszW = (LPWSTR) ::HeapAlloc(::GetProcessHeap(),8,(WLength+1) * sizeof(WCHAR));
	::MultiByteToWideChar(CP_ACP, 0, ansi, -1, pszW, WLength);
	
	return pszW;  
}


 LPWSTR Utf8ToWchar(LPCSTR utf8)  
 {  
	 int wcsLen = ::MultiByteToWideChar(CP_UTF8, NULL, utf8, strlen(utf8), NULL, 0);

	 wchar_t* wszString=(wchar_t*)::HeapAlloc(GetProcessHeap(),8,(wcsLen + 1) * sizeof(wchar_t));
	 ZeroMemory(wszString,(wcsLen+1)*sizeof(wchar_t));
	 ::MultiByteToWideChar(CP_UTF8, NULL, utf8, strlen(utf8), wszString, wcsLen);

	 return wszString;
 } 

 LPSTR WcharToUtf8(LPCWSTR unicode)  
{	
	int u8Len = ::WideCharToMultiByte(CP_UTF8, NULL, unicode, wcslen(unicode), NULL, 0, NULL, NULL);
	LPSTR szU8 = (LPSTR)::HeapAlloc(GetProcessHeap(),8,u8Len + 1);
	::WideCharToMultiByte(CP_UTF8, NULL, unicode, wcslen(unicode), szU8, u8Len, NULL, NULL);

	return szU8;
}  

 LPSTR WcharToAnsi(LPCWSTR unicode){

	int ALength = ::WideCharToMultiByte(CP_ACP, 0, unicode, -1, NULL, 0, NULL, NULL);  
	LPSTR pszA = ( LPSTR )::HeapAlloc(GetProcessHeap(),8,ALength + 1);  
	::WideCharToMultiByte(CP_ACP, 0, unicode, -1, pszA, ALength, NULL, NULL);

	return pszA;
}

//static LPSTR WcharToUtf8(LPCWSTR unicode){}
 void doFileEnumeration(LPWSTR lpPath, UINT nRecursion, EnumerateFunc pFunc, void* pUserData)        
{        
	static BOOL s_bUserBreak = FALSE;        
	//try{        
	//-------------------------------------------------------------------------        
	if(s_bUserBreak) return;        

	int len = wcslen(lpPath);        
	if(lpPath==NULL || len<=0) return;        


	//NotifySys(NRS_DO_EVENTS, 0,0);        


	wchar_t path[MAX_PATH];        
	wcscpy(path, lpPath);        
	if(lpPath[len-1] != '\\')  
	{  
		wcscat(path, L"\\");     
	}  

	wcscat(path, L"*");        


	WIN32_FIND_DATAW fd;        
	HANDLE hFindFile = FindFirstFile(path, &fd);        
	if(hFindFile == INVALID_HANDLE_VALUE)        
	{       
		::FindClose(hFindFile); return;        
	}        

	wchar_t tempPath[MAX_PATH]; BOOL bUserReture=TRUE; BOOL bIsDirectory;        

	BOOL bFinish = FALSE;        
	while(!bFinish)        
	{        
		wcscpy(tempPath, lpPath);        
		if(lpPath[len-1] != '\\') wcscat(tempPath, L"\\");        
		wcscat(tempPath, fd.cFileName);        

		//"目录"属性  
		bIsDirectory = fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;        

		//如果是.或..        
		if( bIsDirectory  && (wcscmp(fd.cFileName, L".")==0 || wcscmp(fd.cFileName, L"..")==0))         
		{               
			bFinish = (FindNextFile(hFindFile, &fd) == FALSE);        
			continue;        
		}        

	if(pFunc)        
		{       
			bUserReture = pFunc(tempPath ,fd, pUserData);        
			if(bUserReture==FALSE)        
			{        
				s_bUserBreak = TRUE; ::FindClose(hFindFile); return;        
			}        
		}     
		//NotifySys(NRS_DO_EVENTS, 0,0);        
		
		
		if(bIsDirectory && nRecursion) //是子目录        
		{        

			doFileEnumeration(tempPath, nRecursion-1, pFunc, pUserData);        
		}        


		bFinish = (FindNextFile(hFindFile, &fd) == FALSE);        
	}  

	::FindClose(hFindFile);        

	//-------------------------------------------------------------------------        
	//}catch(...){ ASSERT(0); return; }       
}