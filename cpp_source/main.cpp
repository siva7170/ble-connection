#include <napi.h>
#include <thread>
#include "BLEWrapper.h"
#include <iostream>

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
    return BLEWrapper::Init(env, exports);
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, InitAll)