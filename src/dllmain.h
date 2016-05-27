// dllmain.h : Declaration of module class.

class CFlashProtectorModule : public ATL::CAtlDllModuleT< CFlashProtectorModule >
{
public :
	DECLARE_LIBID(LIBID_FlashProtectorLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_FLASHPROTECTOR, "{754A5E43-9F39-4B76-AA90-D7A769BFE9CC}")
};

extern class CFlashProtectorModule _AtlModule;
