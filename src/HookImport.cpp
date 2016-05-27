#include "stdafx.h"
#include <Windows.h>
#include <TlHelp32.h>
#include <imagehlp.h>
#include "HookImport.h"
#include "HookCommonType.h"

PROC* GetIATAPIAddr(HMODULE hCallerMod, PCSTR pszCalleeModName, PCSTR pszAPIName, BOOL bDelayIAT)
{
    if (NULL == hCallerMod) {
        return NULL;
    }
    ULONG ulSize = 0;
    if (bDelayIAT) {
        PIMAGE_DELAY_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_DELAY_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(\
            hCallerMod, TRUE, IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT, &ulSize);
        return GetAddr(pImportDesc, hCallerMod, pszCalleeModName, pszAPIName);
    } else {
        PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(\
            hCallerMod, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);
        return GetAddr(pImportDesc, hCallerMod, pszCalleeModName, pszAPIName);
    }
}

PROC* GetIATAPIAddr(HMODULE hCallerMod, PCSTR pszCalleeModName, PCSTR pszAPIName, BOOL bDelayIAT, VOID* pReserverd) {
    if (NULL == hCallerMod) {
        return NULL;
    }
    ULONG ulSize = 0;
    if (bDelayIAT) {
        PIMAGE_DELAY_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_DELAY_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(\
            hCallerMod, TRUE, IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT, &ulSize);
        return GetAddr(pImportDesc, hCallerMod, pszCalleeModName, pszAPIName, pReserverd);
    } else {
        PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(\
            hCallerMod, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);
        return GetAddr(pImportDesc, hCallerMod, pszCalleeModName, pszAPIName, pReserverd);
    }
}