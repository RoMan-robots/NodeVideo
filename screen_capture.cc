#include <iostream>
#include <napi.h>
#include <windows.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <thread>
#include <chrono>
#pragma comment(lib, "d3d11.lib")

IDXGIOutputDuplication* duplication = nullptr;
ID3D11Device* device = nullptr;
ID3D11DeviceContext* context = nullptr;
bool streaming = false;

bool Init() {
    IDXGIAdapter1* adapter;
    IDXGIFactory1* factory;
    if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&factory))) return false;
    if (FAILED(factory->EnumAdapters1(0, &adapter))) return false;

    if (FAILED(D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr, 0,
        nullptr, 0, D3D11_SDK_VERSION, &device, nullptr, &context))) return false;

    IDXGIOutput* output;
    if (FAILED(adapter->EnumOutputs(0, &output))) return false;

    IDXGIOutput1* output1;
    if (FAILED(output->QueryInterface(__uuidof(IDXGIOutput1), (void**)&output1))) return false;
    if (FAILED(output1->DuplicateOutput(device, &duplication))) return false;

    return true;
}

void StreamLoop(Napi::ThreadSafeFunction tsfn) {
    while (streaming) {
        DXGI_OUTDUPL_FRAME_INFO infoFrame;
        IDXGIResource* resource = nullptr;

        if (FAILED(duplication->AcquireNextFrame(100, &infoFrame, &resource))) continue;

        ID3D11Texture2D* frame = nullptr;
        if (SUCCEEDED(resource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&frame))) {
            D3D11_TEXTURE2D_DESC desc;
            frame->GetDesc(&desc);

            desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            desc.Usage = D3D11_USAGE_STAGING;
            desc.BindFlags = 0;
            desc.MiscFlags = 0;

            ID3D11Texture2D* stagingTex = nullptr;
            if (SUCCEEDED(device->CreateTexture2D(&desc, nullptr, &stagingTex))) {
                context->CopyResource(stagingTex, frame);

                D3D11_MAPPED_SUBRESOURCE mapped;
                if (SUCCEEDED(context->Map(stagingTex, 0, D3D11_MAP_READ, 0, &mapped))) {
                    size_t rowSize = desc.Width * 4;
                    size_t imageSize = rowSize * desc.Height;
                    uint8_t* bufferCopy = new uint8_t[imageSize];

                    for (UINT y = 0; y < desc.Height; y++) {
                        memcpy(bufferCopy + y * rowSize,
                               static_cast<uint8_t*>(mapped.pData) + y * mapped.RowPitch,
                               rowSize);
                    }

                    tsfn.BlockingCall([desc, bufferCopy, imageSize](Napi::Env env, Napi::Function jsCallback) {
                        Napi::Object obj = Napi::Object::New(env);
                        obj.Set("width", desc.Width);
                        obj.Set("height", desc.Height);
                        obj.Set("data", Napi::Buffer<uint8_t>::Copy(env, bufferCopy, imageSize));
                        jsCallback.Call({ obj });
                        delete[] bufferCopy;
                    });

                    context->Unmap(stagingTex, 0);
                }
                stagingTex->Release();
            }
            frame->Release();
        }

        if (resource) resource->Release();
        duplication->ReleaseFrame();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 5));
    }
    tsfn.Release();
}

Napi::Value StartStream(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (!duplication && !Init()) return env.Null();
    if (!info[0].IsFunction()) return env.Null();

    streaming = true;

    Napi::Function cb = info[0].As<Napi::Function>();
    Napi::ThreadSafeFunction tsfn = Napi::ThreadSafeFunction::New(
        env, cb, "StreamCallback", 0, 1
    );
    std::thread(StreamLoop, tsfn).detach();

    return env.Undefined();
}

Napi::Value StopStream(const Napi::CallbackInfo& info) {
    streaming = false;
    return info.Env().Undefined();
}

Napi::Object InitModule(Napi::Env env, Napi::Object exports) {
    exports.Set("start", Napi::Function::New(env, StartStream));
    exports.Set("stop", Napi::Function::New(env, StopStream));
    return exports;
}

NODE_API_MODULE(screen_capture, InitModule)