#include "napi/native_api.h"

#include "./vk.cpp"


OH_NativeXComponent_Callback callback;
EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        { "run", nullptr, &createInstance, nullptr, nullptr, nullptr, napi_default, nullptr }//,
        //{"onSurfaceCreated",nullptr, HelloTriangleApp::OnSurfaceCreated, nullptr, nullptr, nullptr, napi_default, nullptr }
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    
    napi_value exportInstance = nullptr;
    // 用来解析出被wrap了NativeXComponent指针的属性
    napi_get_named_property(env, exports, OH_NATIVE_XCOMPONENT_OBJ, &exportInstance);
    OH_NativeXComponent *nativeXComponent = nullptr;
    // 通过napi_unwrap接口，解析出NativeXComponent的实例指针
    napi_unwrap(env, exportInstance, reinterpret_cast<void**>(&nativeXComponent));
    // 获取XComponentId
    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    OH_NativeXComponent_GetXComponentId(nativeXComponent, idStr, &idSize);
    
    callback.OnSurfaceCreated = HelloTriangleApp::OnSurfaceCreatedCB;
    callback.OnSurfaceDestroyed = HelloTriangleApp::OnDestroyCB;
    OH_NativeXComponent_RegisterCallback(nativeXComponent, &callback);
    return exports;
}
EXTERN_C_END

static napi_module demoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "entry",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};



extern "C" __attribute__((constructor)) void RegisterEntryModule(void)
{
    napi_module_register(&demoModule);
}


