#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#include <math.h> // sin, cos
#include "xube.h" // 3d model

///////////////////////////////////////////////////////////////////////////////////////////////////

#define TITLE "Minimal D3D11 by d7samurai"

///////////////////////////////////////////////////////////////////////////////////////////////////

struct float3 { float x, y, z; };
struct matrix { float m[4][4]; };

matrix operator*(const matrix& m1, const matrix& m2);

///////////////////////////////////////////////////////////////////////////////////////////////////

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    WNDCLASSA wndclass = { 0, DefWindowProcA, 0, 0, 0, 0, 0, 0, 0, TITLE };

    RegisterClassA(&wndclass);

    HWND window = CreateWindowExA(0, TITLE, TITLE, WS_POPUP | WS_MAXIMIZE | WS_VISIBLE, 0, 0, 0, 0, nullptr, nullptr, nullptr, nullptr);

    ///////////////////////////////////////////////////////////////////////////////////////////////

    D3D_FEATURE_LEVEL featurelevels[] = { D3D_FEATURE_LEVEL_11_0 };

    DXGI_SWAP_CHAIN_DESC swapchaindesc = {};
    swapchaindesc.BufferDesc.Width  = 0; // use window width
    swapchaindesc.BufferDesc.Height = 0; // use window height
    swapchaindesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // can't specify SRGB framebuffer directly when using FLIP model swap effect. see lines 49, 66
    swapchaindesc.SampleDesc.Count  = 1;
    swapchaindesc.BufferUsage       = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapchaindesc.BufferCount       = 2;
    swapchaindesc.OutputWindow      = window;
    swapchaindesc.Windowed          = TRUE;
    swapchaindesc.SwapEffect        = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    IDXGISwapChain* swapchain;

    ID3D11Device* device;
    ID3D11DeviceContext* devicecontext;

    D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG, featurelevels, ARRAYSIZE(featurelevels), D3D11_SDK_VERSION, &swapchaindesc, &swapchain, &device, nullptr, &devicecontext); // D3D11_CREATE_DEVICE_DEBUG is optional, but provides useful d3d11 debug output

    swapchain->GetDesc(&swapchaindesc); // update swapchaindesc with actual window size

    ///////////////////////////////////////////////////////////////////////////////////////////////

    ID3D11Texture2D* framebuffer;

    swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&framebuffer); // grab framebuffer from swapchain

    D3D11_RENDER_TARGET_VIEW_DESC framebufferRTVdesc = {}; // needed for SRGB framebuffer when using FLIP model swap effect
    framebufferRTVdesc.Format        = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
    framebufferRTVdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    ID3D11RenderTargetView* framebufferRTV;

    device->CreateRenderTargetView(framebuffer, &framebufferRTVdesc, &framebufferRTV);

    ///////////////////////////////////////////////////////////////////////////////////////////////

    D3D11_TEXTURE2D_DESC depthbufferdesc;

    framebuffer->GetDesc(&depthbufferdesc); // copy framebuffer properties; they're mostly the same

    depthbufferdesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthbufferdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    ID3D11Texture2D* depthbuffer;

    device->CreateTexture2D(&depthbufferdesc, nullptr, &depthbuffer);

    ID3D11DepthStencilView* depthbufferDSV;

    device->CreateDepthStencilView(depthbuffer, nullptr, &depthbufferDSV);

    ///////////////////////////////////////////////////////////////////////////////////////////////

    ID3DBlob* vertexshaderCSO;

    D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr, "VertexShaderMain", "vs_5_0", 0, 0, &vertexshaderCSO, nullptr);

    ID3D11VertexShader* vertexshader;

    device->CreateVertexShader(vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), nullptr, &vertexshader);

    D3D11_INPUT_ELEMENT_DESC inputelementdesc[] = // maps to vertexdesc struct in gpu.hlsl via semantic names ("POS", "NOR", "TEX", "COL")
    {
        { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // float3 position
        { "NOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // float3 normal
        { "TEX", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // float2 texcoord
        { "COL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // float3 color
    };

    ID3D11InputLayout* inputlayout;

    device->CreateInputLayout(inputelementdesc, ARRAYSIZE(inputelementdesc), vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), &inputlayout);

    ///////////////////////////////////////////////////////////////////////////////////////////////

    ID3DBlob* pixelshaderCSO;

    D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr, "PixelShaderMain", "ps_5_0", 0, 0, &pixelshaderCSO, nullptr);

    ID3D11PixelShader* pixelshader;

    device->CreatePixelShader(pixelshaderCSO->GetBufferPointer(), pixelshaderCSO->GetBufferSize(), nullptr, &pixelshader);

    ///////////////////////////////////////////////////////////////////////////////////////////////

    D3D11_RASTERIZER_DESC rasterizerdesc = {};
    rasterizerdesc.FillMode = D3D11_FILL_SOLID;
    rasterizerdesc.CullMode = D3D11_CULL_BACK;

    ID3D11RasterizerState* rasterizerstate;

    device->CreateRasterizerState(&rasterizerdesc, &rasterizerstate);

    ///////////////////////////////////////////////////////////////////////////////////////////////

    D3D11_SAMPLER_DESC samplerdesc = {};
    samplerdesc.Filter         = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samplerdesc.AddressU       = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerdesc.AddressV       = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerdesc.AddressW       = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerdesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

    ID3D11SamplerState* samplerstate;

    device->CreateSamplerState(&samplerdesc, &samplerstate);

    ///////////////////////////////////////////////////////////////////////////////////////////////

    D3D11_DEPTH_STENCIL_DESC depthstencildesc = {};
    depthstencildesc.DepthEnable    = TRUE;
    depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthstencildesc.DepthFunc      = D3D11_COMPARISON_LESS;

    ID3D11DepthStencilState* depthstencilstate;

    device->CreateDepthStencilState(&depthstencildesc, &depthstencilstate);

    ///////////////////////////////////////////////////////////////////////////////////////////////

    struct Constants { matrix transform, projection; float3 lightvector; };

    D3D11_BUFFER_DESC constantbufferdesc = {};
    constantbufferdesc.ByteWidth      = sizeof(Constants) + 0xf & 0xfffffff0; // ensure constant buffer size is multiple of 16 bytes
    constantbufferdesc.Usage          = D3D11_USAGE_DYNAMIC; // will be updated from CPU every frame
    constantbufferdesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    constantbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    ID3D11Buffer* constantbuffer;

    device->CreateBuffer(&constantbufferdesc, nullptr, &constantbuffer);

    ///////////////////////////////////////////////////////////////////////////////////////////////

    D3D11_TEXTURE2D_DESC texturedesc = {};
    texturedesc.Width              = TEXTURE_WIDTH;  // in xube.h
    texturedesc.Height             = TEXTURE_HEIGHT; // in xube.h
    texturedesc.MipLevels          = 1;
    texturedesc.ArraySize          = 1;
    texturedesc.Format             = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB; // same as framebuffer(view)
    texturedesc.SampleDesc.Count   = 1;
    texturedesc.Usage              = D3D11_USAGE_IMMUTABLE; // will never be updated
    texturedesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA textureSRD = {};
    textureSRD.pSysMem     = texturedata; // in xube.h
    textureSRD.SysMemPitch = TEXTURE_WIDTH * sizeof(UINT); // 1 UINT = 4 bytes per pixel, 0xAARRGGBB

    ID3D11Texture2D* texture;

    device->CreateTexture2D(&texturedesc, &textureSRD, &texture);

    ID3D11ShaderResourceView* textureSRV;

    device->CreateShaderResourceView(texture, nullptr, &textureSRV);

    ///////////////////////////////////////////////////////////////////////////////////////////////

    D3D11_BUFFER_DESC vertexbufferdesc = {};
    vertexbufferdesc.ByteWidth = sizeof(vertexdata);
    vertexbufferdesc.Usage     = D3D11_USAGE_IMMUTABLE; // will never be updated 
    vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vertexbufferSRD = { vertexdata }; // in xube.h

    ID3D11Buffer* vertexbuffer;

    device->CreateBuffer(&vertexbufferdesc, &vertexbufferSRD, &vertexbuffer);

    ///////////////////////////////////////////////////////////////////////////////////////////////

    D3D11_BUFFER_DESC indexbufferdesc = {};
    indexbufferdesc.ByteWidth = sizeof(indexdata);
    indexbufferdesc.Usage     = D3D11_USAGE_IMMUTABLE; // will never be updated
    indexbufferdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA indexbufferSRD = { indexdata }; // in xube.h

    ID3D11Buffer* indexbuffer;

    device->CreateBuffer(&indexbufferdesc, &indexbufferSRD, &indexbuffer);

    ///////////////////////////////////////////////////////////////////////////////////////////////

    FLOAT clearcolor[4] = { 0.025f, 0.025f, 0.025f, 1.0f };

    UINT stride = 11 * sizeof(float); // vertex size (11 floats: float3 position, float3 normal, float2 texcoord, float3 color)
    UINT offset = 0;

    D3D11_VIEWPORT viewport = { 0.0f, 0.0f, (float)swapchaindesc.BufferDesc.Width, (float)swapchaindesc.BufferDesc.Height, 0.0f, 1.0f };
    
    ///////////////////////////////////////////////////////////////////////////////////////////////

    float w = viewport.Width / viewport.Height; // width (aspect ratio)
    float h = 1.0f;                             // height
    float n = 1.0f;                             // near
    float f = 9.0f;                             // far

    float3 modelrotation    = { 0.0f, 0.0f, 0.0f };
    float3 modelscale       = { 1.0f, 1.0f, 1.0f };
    float3 modeltranslation = { 0.0f, 0.0f, 4.0f };

    ///////////////////////////////////////////////////////////////////////////////////////////////

    while (true)
    {
        MSG msg;

        while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_KEYDOWN) return 0; // PRESS ANY KEY TO EXIT
            DispatchMessageA(&msg);
        }

        ///////////////////////////////////////////////////////////////////////////////////////////

        matrix rotatex   = { 1, 0, 0, 0, 0, (float)cos(modelrotation.x), -(float)sin(modelrotation.x), 0, 0, (float)sin(modelrotation.x), (float)cos(modelrotation.x), 0, 0, 0, 0, 1 };
        matrix rotatey   = { (float)cos(modelrotation.y), 0, (float)sin(modelrotation.y), 0, 0, 1, 0, 0, -(float)sin(modelrotation.y), 0, (float)cos(modelrotation.y), 0, 0, 0, 0, 1 };
        matrix rotatez   = { (float)cos(modelrotation.z), -(float)sin(modelrotation.z), 0, 0, (float)sin(modelrotation.z), (float)cos(modelrotation.z), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
        matrix scale     = { modelscale.x, 0, 0, 0, 0, modelscale.y, 0, 0, 0, 0, modelscale.z, 0, 0, 0, 0, 1 };
        matrix translate = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, modeltranslation.x, modeltranslation.y, modeltranslation.z, 1 };

        modelrotation.x += 0.005f;
        modelrotation.y += 0.009f;
        modelrotation.z += 0.001f;

        ///////////////////////////////////////////////////////////////////////////////////////////

        D3D11_MAPPED_SUBRESOURCE constantbufferMSR;

        devicecontext->Map(constantbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantbufferMSR); // update constant buffer every frame
        {
            Constants* constants = (Constants*)constantbufferMSR.pData;

            constants->transform   = rotatex * rotatey * rotatez * scale * translate;
            constants->projection  = { 2 * n / w, 0, 0, 0, 0, 2 * n / h, 0, 0, 0, 0, f / (f - n), 1, 0, 0, n * f / (n - f), 0 };
            constants->lightvector = { 1.0f, -1.0f, 1.0f };
        }
        devicecontext->Unmap(constantbuffer, 0);

        ///////////////////////////////////////////////////////////////////////////////////////////

        devicecontext->ClearRenderTargetView(framebufferRTV, clearcolor);
        devicecontext->ClearDepthStencilView(depthbufferDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

        devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        devicecontext->IASetInputLayout(inputlayout);
        devicecontext->IASetVertexBuffers(0, 1, &vertexbuffer, &stride, &offset);
        devicecontext->IASetIndexBuffer(indexbuffer, DXGI_FORMAT_R32_UINT, 0);

        devicecontext->VSSetShader(vertexshader, nullptr, 0);
        devicecontext->VSSetConstantBuffers(0, 1, &constantbuffer);

        devicecontext->RSSetViewports(1, &viewport);
        devicecontext->RSSetState(rasterizerstate);

        devicecontext->PSSetShader(pixelshader, nullptr, 0);
        devicecontext->PSSetShaderResources(0, 1, &textureSRV);
        devicecontext->PSSetSamplers(0, 1, &samplerstate);

        devicecontext->OMSetRenderTargets(1, &framebufferRTV, depthbufferDSV);
        devicecontext->OMSetDepthStencilState(depthstencilstate, 0);
        devicecontext->OMSetBlendState(nullptr, nullptr, 0xffffffff); // use default blend mode (i.e. no blending)

        ///////////////////////////////////////////////////////////////////////////////////////////

        devicecontext->DrawIndexed(ARRAYSIZE(indexdata), 0, 0);

        ///////////////////////////////////////////////////////////////////////////////////////////

        swapchain->Present(1, 0);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

matrix operator*(const matrix& m1, const matrix& m2)
{
    return
    {
        m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0],
        m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1],
        m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2],
        m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3],
        m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0],
        m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1],
        m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2],
        m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3],
        m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0],
        m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1],
        m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2],
        m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3],
        m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0],
        m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1],
        m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2],
        m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3],
    };
}
