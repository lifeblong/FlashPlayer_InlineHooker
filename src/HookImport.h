#ifndef HOOK_IMPORT_H_
#define HOOK_IMPORT_H_

//#include <stdio.h>

typedef DWORD (WINAPI *UnDecorateSymbolName_FPTYPE)(PCTSTR DecoratedName,
													PTSTR UnDecoratedName,
													DWORD UndecoratedLength,
													DWORD Flags);

/*
 * More detailed information about IMAGE_DELAY_IMPORT_DESCRIPTOR,
 * please refer to http://msdn.microsoft.com/en-us/magazine/bb985996.aspx
 */
typedef struct _IMAGE_DELAY_IMPORT_DESCRIPTOR
{
	DWORD grAttrs; //The attributes for this structure. Currently, the only flag defined is dlattrRva (1), indicating that the address fields in the structure should be treated as RVAs, rather than virtual addresses.
	DWORD Name; //An RVA to a string with the name of the imported DLL. This string is passed to LoadLibrary.
	DWORD phmod; //An RVA to an HMODULE-sized memory location. When the Delayloaded DLL is brought into memory, its HMODULE is stored at this location.
	DWORD FirstThunk; //An RVA to the Import Address Table for this DLL. This is the same format as a regular IAT.
	DWORD OriginalFirstThunk; //An RVA to the Import Name Table for this DLL. This is the same format as a regular INT.
	DWORD pBoundIAT; //An RVA of the optional bound IAT. An RVA to a bound copy of an Import Address Table for this DLL. This is the same format as a regular IAT. Currently, this copy of the IAT is not actually bound, but this feature may be added in future versions of the BIND program.
	DWORD pUnloadIAT; //An RVA of the optional copy of the original IAT. An RVA to an unbound copy of an Import Address Table for this DLL. This is the same format as a regular IAT. Currently always set to 0.
	DWORD dwTimeStamp; //The date/time stamp of the delayload imported DLL. Normally set to 0.
}IMAGE_DELAY_IMPORT_DESCRIPTOR, *PIMAGE_DELAY_IMPORT_DESCRIPTOR;

template<class T>
PROC* GetAddr(T pImportDesc, HMODULE hCallerMod, PCSTR pszCalleeModName, PCSTR pszAPIName, VOID* pReserverd) {
    if (pImportDesc == NULL) {
        return NULL;
    }
    for (; pImportDesc->Name; pImportDesc++) {
        PSTR pszModName = (PSTR)((PBYTE)hCallerMod + pImportDesc->Name);
        if (lstrcmpiA(pszModName, pszCalleeModName) == 0 || strlen(pszCalleeModName) == 0) {
            PIMAGE_THUNK_DATA pThunkName = (PIMAGE_THUNK_DATA)((PBYTE)hCallerMod + pImportDesc->OriginalFirstThunk);
            PIMAGE_THUNK_DATA pThunkAddr = (PIMAGE_THUNK_DATA)((PBYTE)hCallerMod + pImportDesc->FirstThunk);
            for (; pThunkName->u1.Function, pThunkAddr->u1.Function; pThunkName++, pThunkAddr++) {
                if (pThunkName->u1.AddressOfData > 0x80000000) { //import by ordinal
                    continue;
                }
                // get function name
                char* pszFunName = NULL;
                pszFunName = (char *)((BYTE *)hCallerMod + (DWORD)pThunkName->u1.AddressOfData + 2);
                if (NULL != pszFunName) {
                    if (lstrcmpiA(pszFunName, pszAPIName) == 0) {
                        // get the address of the function address
                        PROC* ppfn = (PROC*) &pThunkAddr->u1.Function;
                        return ppfn;
                    }
                } // end if
            } // end for
        } // end if
    } // end for
    return NULL;
}


template<class T>
PROC* GetAddr(T pImportDesc, HMODULE hCallerMod, PCSTR pszCalleeModName, PCSTR pszAPIName) 
{
    if (pImportDesc == NULL) {
        return NULL;
    }

	UnDecorateSymbolName_FPTYPE fpUnDecorateSymbolName = NULL;
	HMODULE hModule;
	if ((hModule = ::GetModuleHandleA("imagehlp.dll")) || (hModule = ::LoadLibraryA("imagehlp.dll"))) {
		fpUnDecorateSymbolName = (UnDecorateSymbolName_FPTYPE)GetProcAddress(hModule, "UnDecorateSymbolName");
	}
	
	for (; pImportDesc->Name; pImportDesc++) {
        PSTR pszModName = (PSTR)((PBYTE)hCallerMod + pImportDesc->Name);
        if (lstrcmpiA(pszModName, pszCalleeModName) == 0 || strlen(pszCalleeModName) == 0) {
            PIMAGE_THUNK_DATA pThunkName = (PIMAGE_THUNK_DATA)((PBYTE)hCallerMod + pImportDesc->OriginalFirstThunk);
            PIMAGE_THUNK_DATA pThunkAddr = (PIMAGE_THUNK_DATA)((PBYTE)hCallerMod + pImportDesc->FirstThunk);
            for (; pThunkName->u1.Function, pThunkAddr->u1.Function; pThunkName++, pThunkAddr++) {
                if (pThunkName->u1.AddressOfData > 0x80000000) { //import by ordinal
                    continue;
                }
                // get function name
                char* pszFunName = NULL;
                pszFunName = (char *)((BYTE *)hCallerMod + (DWORD)pThunkName->u1.AddressOfData + 2);
                if (NULL != pszFunName) {
					char* pFunctionName = NULL;

                    // get undecorated function name
					char pszUnDecoratedName[512] = {0};
					if ('?' == pszFunName[0] && fpUnDecorateSymbolName && fpUnDecorateSymbolName((PCTSTR)pszFunName,
						(PTSTR)pszUnDecoratedName, sizeof(pszUnDecoratedName), UNDNAME_COMPLETE)) {
							pFunctionName = pszUnDecoratedName;
					} else {
						pFunctionName = pszFunName;
					}

					//// NOTE: For output debugging info, don't remove!
					//char msg[1024] = {0};
					//sprintf(msg, "Function Name = %s, UnDecorated Name = %s\n", pszFunName, pFunctionName);
					//OutputDebugStringA(msg);


					if (strstr(pFunctionName, pszAPIName) != NULL)
					{
                        // get the address of the function address
                        PROC* ppfn = (PROC*) &pThunkAddr->u1.Function;
                        return ppfn;
                    }
                } // end if
            } // end for
        } // end if
    } // end for
    return NULL;
}

PROC* GetIATAPIAddr(HMODULE hCallerMod, PCSTR pszCalleeModName, PCSTR pszAPIName, BOOL bDelayIAT);
PROC* GetIATAPIAddr(HMODULE hCallerMod, PCSTR pszCalleeModName, PCSTR pszAPIName, BOOL bDelayIAT, VOID* pReserverd);

#endif // HOOK_IMPORT_H_
