

#pragma once

typedef struct
{
    float x, y, z;
}Vector3;



typedef struct PhsicsAPI_s
{
    /*

    */
    void(_stdcall* Set)(const char* key, const char* value);

    /*

    */
    void(_stdcall* Test)();

    /*

    */
    void(_stdcall* InitSystem)(const char* modFolder, void* pEngineStudioAPI);

    /*
    <summary>
                Load map geomitry collider.
                </summary><param name="mapName"></param>
    */
    void(_stdcall* ChangeLevel)(const char* mapName);

    /*
    <summary>
                地图不变，内容重置，清理在游戏中动态创建的各种CollisionObjects
                cs的每一局结束可以调用
                </summary>
    */
    void(_stdcall* LevelReset)();

    /*
    <summary>
                Physics world update
                </summary><param name="delta"></param>
    */
    void(_stdcall* Update)(float delta);

    /*

    */
    void(_stdcall* Pause)();

    /*

    */
    void(_stdcall* Resume)();

    /*
    <summary>
                Close physics system and release physics resources.
                </summary>
    */
    void(_stdcall* ShotDown)();

    /*
    <summary>
                Show configration form.
                Using cvar to call this is recommended.
                </summary>
    */
    void(_stdcall* ShowConfigForm)();

    /*

    */
    void(_stdcall* CreateRagdollController)(int entityId, char* modelName);

    /*

    */
    void(_stdcall* CreateRagdollControllerIndex)(int entityId, int index);

    /*
    model_t* model
    */
    void(_stdcall* CreateRagdollControllerModel)(int entityId, void* model);

    /*

    */
    void(_stdcall* StartRagdoll)(int entityId);

    /*

    */
    void(_stdcall* StopRagdoll)(int entityId);

    /*

    */
    void(_stdcall* SetupBonesPhysically)(int entityId);

    /*

    */
    void(_stdcall* ChangeOwner)(int oldEntity, int newEntity);

    /*

    */
    void(_stdcall* SetVelocity)(int entityId, Vector3* v);

    /*

    */
    void(_stdcall* DisposeRagdollController)(int entityId);

    /*

    */
    void(_stdcall* ImpulseBone)(int entityId, int boneId, Vector3* force);

    /*

    */
    void(_stdcall* ClearRagdoll)();

    /*

    */
    void(_stdcall* HeadShootRagdoll)(int entityId, Vector3* force);

    /*
    <summary>
                Set an explosion on the specified position.
                The impact range is calculated automatically via intensity.
                </summary><param name="pos"></param><param name="intensity"></param>
    */
    void(_stdcall* Explosion)(Vector3* pos, float intensity);

    /*
    <summary>
                Shoot an invisable bullet to apply impulse to the rigidbody it hits.
                </summary><param name="from">eye pos.</param><param name="force">contains direction and intensity.</param>
    */
    void(_stdcall* Shoot)(Vector3* from, Vector3* force);

    /*
    <summary></summary><param name="from">Eye pos or camera origin</param><param name="to">Camera origin + direction</param>
    */
    void(_stdcall* PickBodyLocal)(Vector3 from, Vector3 to);

    /*
    <summary>
                Release picked body
                </summary>
    */
    void(_stdcall* ReleaseBodyLocal)();

    /*
    <summary></summary><param name="from">Eye pos or camera origin</param><param name="to">Camera origin + direction</param>
    */
    void(_stdcall* MoveBodyLocal)(Vector3 from, Vector3 to);

    void(_stdcall*SetPose)(int entityId, float* pBoneWorldTransform);

    void(_stdcall* AddCollider)(void* pEntity);

}PhsicsAPI;


// Containts all the physics system API
// Call [InitPhysicsInterface] before using these API
extern PhsicsAPI gPhysics;

// Call this function to initialize [gPhysics].
extern "C" void InitPhysicsInterface(char* msg);