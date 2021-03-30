
#include<physics.h>
#include <metahost.h>
#include<stdio.h>
#include<Windows.h>
//#include <mscoree.h> 

#pragma comment(lib, "mscoree.lib")

PhsicsAPI gPhysics;

#ifdef _DEBUG
const wchar_t PhyDllPath[] = L".\\gsphysics\\bin\\GoldsrcPhysics.dll";
#else// _DEBUG
const wchar_t PhyDllPath[] = L".\\gsphysics\\bin\\GoldsrcPhysics.dll";
#endif

//globle CLR handle
ICLRMetaHost* pMetaHost = nullptr;
ICLRMetaHostPolicy* pMetaHostPolicy = nullptr;
ICLRRuntimeHost* pRuntimeHost = nullptr;
ICLRRuntimeInfo* pRuntimeInfo = nullptr;

void ReleaseCLR()
{
	if (pRuntimeInfo != nullptr)
	{
		pRuntimeInfo->Release();
		pRuntimeInfo = nullptr;
	}

	if (pRuntimeHost != nullptr)
	{
		pRuntimeHost->Release();
		pRuntimeHost = nullptr;
	}

	if (pMetaHost != nullptr)
	{
		pMetaHost->Release();
		pMetaHost = nullptr;
	}
}

int InitCLR()
{
	if (pRuntimeInfo != nullptr)
		return 0;
	HRESULT hr = CLRCreateInstance(CLSID_CLRMetaHost, IID_ICLRMetaHost, (LPVOID*)&pMetaHost);
	hr = pMetaHost->GetRuntime(L"v4.0.30319", IID_PPV_ARGS(&pRuntimeInfo));

	if (FAILED(hr)) {
		ReleaseCLR();
	}

	hr = pRuntimeInfo->GetInterface(CLSID_CLRRuntimeHost, IID_PPV_ARGS(&pRuntimeHost));
	hr = pRuntimeHost->Start();
	return (int)hr;
}

int ExitCLR()
{
	pRuntimeHost->Stop();//return HRESULT
	ReleaseCLR();
	return 0;
}

void* GetFunctionPointer(LPWSTR name)
{
	void* pfn = NULL;

	const int bufsize = 128;
	wchar_t buffer[bufsize];//marshal args to [0xXXXX|MethodName] format
	swprintf(buffer, bufsize, L"%p|%s", &pfn, name);

	DWORD dwRet = 0;
	HRESULT hr = pRuntimeHost->ExecuteInDefaultAppDomain(PhyDllPath,
		L"UsrSoft.ManagedExport.ManagedExporter",
		L"GetFunctionPointer",
		buffer,
		&dwRet);

	if (hr != S_OK)
		exit(12345);
	if ((DWORD)pfn != dwRet)
		exit(54321);

	return pfn;
}

//auto generated
extern "C" void InitPhysicsInterface(char* msg)
{
	InitCLR();
	gPhysics.Set = (void(_stdcall*)(const char* key, const char* value))GetFunctionPointer(L"GoldsrcPhysics.ExportAPIs.PhysicsMain.Set");
	gPhysics.Test = (void(_stdcall*)())GetFunctionPointer(L"GoldsrcPhysics.ExportAPIs.PhysicsMain.Test");
	gPhysics.InitSystem = (void(_stdcall*)(const char* modFolder, void* pEngineStudioAPI))GetFunctionPointer(L"GoldsrcPhysics.ExportAPIs.PhysicsMain.InitSystem");
	gPhysics.ChangeLevel = (void(_stdcall*)(const char* mapName))GetFunctionPointer(L"GoldsrcPhysics.ExportAPIs.PhysicsMain.ChangeLevel");
	gPhysics.LevelReset = (void(_stdcall*)())GetFunctionPointer(L"GoldsrcPhysics.ExportAPIs.PhysicsMain.LevelReset");
	gPhysics.Update = (void(_stdcall*)(float delta))GetFunctionPointer(L"GoldsrcPhysics.ExportAPIs.PhysicsMain.Update");
	gPhysics.Pause = (void(_stdcall*)())GetFunctionPointer(L"GoldsrcPhysics.ExportAPIs.PhysicsMain.Pause");
	gPhysics.Resume = (void(_stdcall*)())GetFunctionPointer(L"GoldsrcPhysics.ExportAPIs.PhysicsMain.Resume");
	gPhysics.ShotDown = (void(_stdcall*)())GetFunctionPointer(L"GoldsrcPhysics.ExportAPIs.PhysicsMain.ShotDown");
	gPhysics.ShowConfigForm = (void(_stdcall*)())GetFunctionPointer(L"GoldsrcPhysics.ExportAPIs.PhysicsMain.ShowConfigForm");
	gPhysics.CreateRagdollController = (void(_stdcall*)(int entityId, char* modelName))GetFunctionPointer(L"GoldsrcPhysics.ExportAPIs.PhysicsMain.CreateRagdollController");
	gPhysics.CreateRagdollControllerIndex = (void(_stdcall*)(int entityId, int index))GetFunctionPointer(L"GoldsrcPhysics.ExportAPIs.PhysicsMain.CreateRagdollControllerIndex");
	gPhysics.CreateRagdollControllerModel = (void(_stdcall*)(int entityId, void * model))GetFunctionPointer(L"GoldsrcPhysics.ExportAPIs.PhysicsMain.CreateRagdollControllerModel");
	gPhysics.StartRagdoll = (void(_stdcall*)(int entityId))GetFunctionPointer(L"GoldsrcPhysics.ExportAPIs.PhysicsMain.StartRagdoll");
	gPhysics.StopRagdoll = (void(_stdcall*)(int entityId))GetFunctionPointer(L"GoldsrcPhysics.ExportAPIs.PhysicsMain.StopRagdoll");
	gPhysics.SetupBonesPhysically = (void(_stdcall*)(int entityId))GetFunctionPointer(L"GoldsrcPhysics.ExportAPIs.PhysicsMain.SetupBonesPhysically");
	gPhysics.ChangeOwner = (void(_stdcall*)(int oldEntity, int newEntity))GetFunctionPointer(L"GoldsrcPhysics.ExportAPIs.PhysicsMain.ChangeOwner");
	gPhysics.SetVelocity = (void(_stdcall*)(int entityId, Vector3 * v))GetFunctionPointer(L"GoldsrcPhysics.ExportAPIs.PhysicsMain.SetVelocity");
	gPhysics.DisposeRagdollController = (void(_stdcall*)(int entityId))GetFunctionPointer(L"GoldsrcPhysics.ExportAPIs.PhysicsMain.DisposeRagdollController");
	gPhysics.ImpulseBone = (void(_stdcall*)(int entityId, int boneId, Vector3 * force))GetFunctionPointer(L"GoldsrcPhysics.ExportAPIs.PhysicsMain.ImpulseBone");
	gPhysics.ClearRagdoll = (void(_stdcall*)())GetFunctionPointer(L"GoldsrcPhysics.ExportAPIs.PhysicsMain.ClearRagdoll");
	gPhysics.HeadShootRagdoll = (void(_stdcall*)(int entityId, Vector3 * force))GetFunctionPointer(L"GoldsrcPhysics.ExportAPIs.PhysicsMain.HeadShootRagdoll");
	gPhysics.Explosion = (void(_stdcall*)(Vector3 * pos, float intensity))GetFunctionPointer(L"GoldsrcPhysics.ExportAPIs.PhysicsMain.Explosion");
	gPhysics.Shoot = (void(_stdcall*)(Vector3 * from, Vector3 * force))GetFunctionPointer(L"GoldsrcPhysics.ExportAPIs.PhysicsMain.Shoot");
	gPhysics.PickBodyLocal = (void(_stdcall*)(Vector3 from, Vector3 to))GetFunctionPointer(L"GoldsrcPhysics.ExportAPIs.PhysicsMain.PickBodyLocal");
	gPhysics.ReleaseBodyLocal = (void(_stdcall*)())GetFunctionPointer(L"GoldsrcPhysics.ExportAPIs.PhysicsMain.ReleaseBodyLocal");
	gPhysics.MoveBodyLocal = (void(_stdcall*)(Vector3 from, Vector3 to))GetFunctionPointer(L"GoldsrcPhysics.ExportAPIs.PhysicsMain.MoveBodyLocal");
	gPhysics.SetPose = (void(_stdcall*)(int entityId, float* pBoneWorldTransform))GetFunctionPointer(L"GoldsrcPhysics.ExportAPIs.PhysicsMain.SetPose");
	gPhysics.AddCollider = (void(_stdcall*)(void*))GetFunctionPointer(L"GoldsrcPhysics.ExportAPIs.PhysicsMain.AddCollider");
}