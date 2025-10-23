#include <cstdio>

#include <windows.h>
#include <d3d.h>

static LPDIRECTDRAW7        lpDD = nullptr;
static LPDIRECT3D7          lpD3D = nullptr;
static LPDIRECT3DDEVICE7    lpD3DDevice = nullptr;
static LPDIRECTDRAWSURFACE7 lpPrimary = nullptr;
static LPDIRECTDRAWSURFACE7 lpBackBuffer = nullptr;
static LPDIRECT3DVERTEXBUFFER7 lpVertexBuffer = nullptr;
static LPDIRECTDRAWCLIPPER  lpClipper = nullptr;
static HWND hWnd;

static bool fullscreen = false;

static const char* dderr2str(HRESULT err)
{
    switch (err)
    {
    case DDERR_DDSCAPSCOMPLEXREQUIRED: return "DDERR_DDSCAPSCOMPLEXREQUIRED";
    case DDERR_DEVICEDOESNTOWNSURFACE: return "DDERR_DEVICEDOESNTOWNSURFACE";
    case DDERR_EXPIRED: return "DDERR_EXPIRED";
    case DDERR_INVALIDSTREAM: return "DDERR_INVALIDSTREAM";
    case DDERR_MOREDATA: return "DDERR_MOREDATA";
    case DDERR_NEWMODE: return "DDERR_NEWMODE";
    case DDERR_NODRIVERSUPPORT: return "DDERR_NODRIVERSUPPORT";
    case DDERR_NOFOCUSWINDOW: return "DDERR_NOFOCUSWINDOW";
    case DDERR_NOMONITORINFORMATION: return "DDERR_NOMONITORINFORMATION";
    case DDERR_NONONLOCALVIDMEM: return "DDERR_NONONLOCALVIDMEM";
    case DDERR_NOOPTIMIZEHW: return "DDERR_NOOPTIMIZEHW";
    case DDERR_NOSTEREOHARDWARE: return "DDERR_NOSTEREOHARDWARE";
    case DDERR_NOSURFACELEFT: return "DDERR_NOSURFACELEFT";
    case DDERR_NOTLOADED: return "DDERR_NOTLOADED";
    case DDERR_OVERLAPPINGRECTS: return "DDERR_OVERLAPPINGRECTS";
    case DDERR_TESTFINISHED: return "DDERR_TESTFINISHED";
    case DDERR_VIDEONOTACTIVE: return "DDERR_VIDEONOTACTIVE";
    case DDERR_ALREADYINITIALIZED: return "DDERR_ALREADYINITIALIZED";
    case DDERR_CANNOTATTACHSURFACE: return "DDERR_CANNOTATTACHSURFACE";
    case DDERR_CANNOTDETACHSURFACE: return "DDERR_CANNOTDETACHSURFACE";
    case DDERR_CURRENTLYNOTAVAIL: return "DDERR_CURRENTLYNOTAVAIL";
    case DDERR_EXCEPTION: return "DDERR_EXCEPTION";
    case DDERR_GENERIC: return "DDERR_GENERIC";
    case DDERR_HEIGHTALIGN: return "DDERR_HEIGHTALIGN";
    case DDERR_INCOMPATIBLEPRIMARY: return "DDERR_INCOMPATIBLEPRIMARY";
    case DDERR_INVALIDCAPS: return "DDERR_INVALIDCAPS";
    case DDERR_INVALIDCLIPLIST: return "DDERR_INVALIDCLIPLIST";
    case DDERR_INVALIDMODE: return "DDERR_INVALIDMODE";
    case DDERR_INVALIDOBJECT: return "DDERR_INVALIDOBJECT";
    case DDERR_INVALIDPARAMS: return "DDERR_INVALIDPARAMS";
    case DDERR_INVALIDPIXELFORMAT: return "DDERR_INVALIDPIXELFORMAT";
    case DDERR_INVALIDRECT: return "DDERR_INVALIDRECT";
    case DDERR_LOCKEDSURFACES: return "DDERR_LOCKEDSURFACES";
    case DDERR_NO3D: return "DDERR_NO3D";
    case DDERR_NOALPHAHW: return "DDERR_NOALPHAHW";
    case DDERR_NOCLIPLIST: return "DDERR_NOCLIPLIST";
    case DDERR_NOCOLORCONVHW: return "DDERR_NOCOLORCONVHW";
    case DDERR_NOCOOPERATIVELEVELSET: return "DDERR_NOCOOPERATIVELEVELSET";
    case DDERR_NOCOLORKEY: return "DDERR_NOCOLORKEY";
    case DDERR_NOCOLORKEYHW: return "DDERR_NOCOLORKEYHW";
    case DDERR_NODIRECTDRAWSUPPORT: return "DDERR_NODIRECTDRAWSUPPORT";
    case DDERR_NOEXCLUSIVEMODE: return "DDERR_NOEXCLUSIVEMODE";
    case DDERR_NOFLIPHW: return "DDERR_NOFLIPHW";
    case DDERR_NOGDI: return "DDERR_NOGDI";
    case DDERR_NOMIRRORHW: return "DDERR_NOMIRRORHW";
    case DDERR_NOTFOUND: return "DDERR_NOTFOUND";
    case DDERR_NOOVERLAYHW: return "DDERR_NOOVERLAYHW";
    case DDERR_NORASTEROPHW: return "DDERR_NORASTEROPHW";
    case DDERR_NOROTATIONHW: return "DDERR_NOROTATIONHW";
    case DDERR_NOSTRETCHHW: return "DDERR_NOSTRETCHHW";
    case DDERR_NOT4BITCOLOR: return "DDERR_NOT4BITCOLOR";
    case DDERR_NOT4BITCOLORINDEX: return "DDERR_NOT4BITCOLORINDEX";
    case DDERR_NOT8BITCOLOR: return "DDERR_NOT8BITCOLOR";
    case DDERR_NOTEXTUREHW: return "DDERR_NOTEXTUREHW";
    case DDERR_NOVSYNCHW: return "DDERR_NOVSYNCHW";
    case DDERR_NOZBUFFERHW: return "DDERR_NOZBUFFERHW";
    case DDERR_NOZOVERLAYHW: return "DDERR_NOZOVERLAYHW";
    case DDERR_OUTOFCAPS: return "DDERR_OUTOFCAPS";
    case DDERR_OUTOFMEMORY: return "DDERR_OUTOFMEMORY";
    case DDERR_OUTOFVIDEOMEMORY: return "DDERR_OUTOFVIDEOMEMORY";
    case DDERR_OVERLAYCANTCLIP: return "DDERR_OVERLAYCANTCLIP";
    case DDERR_OVERLAYCOLORKEYONLYONEACTIVE: return "DDERR_OVERLAYCOLORKEYONLYONEACTIVE";
    case DDERR_PALETTEBUSY: return "DDERR_PALETTEBUSY";
    case DDERR_COLORKEYNOTSET: return "DDERR_COLORKEYNOTSET";
    case DDERR_SURFACEALREADYATTACHED: return "DDERR_SURFACEALREADYATTACHED";
    case DDERR_SURFACEALREADYDEPENDENT: return "DDERR_SURFACEALREADYDEPENDENT";
    case DDERR_SURFACEBUSY: return "DDERR_SURFACEBUSY";
    case DDERR_CANTLOCKSURFACE: return "DDERR_CANTLOCKSURFACE";
    case DDERR_SURFACEISOBSCURED: return "DDERR_SURFACEISOBSCURED";
    case DDERR_SURFACELOST: return "DDERR_SURFACELOST";
    case DDERR_SURFACENOTATTACHED: return "DDERR_SURFACENOTATTACHED";
    case DDERR_TOOBIGHEIGHT: return "DDERR_TOOBIGHEIGHT";
    case DDERR_TOOBIGSIZE: return "DDERR_TOOBIGSIZE";
    case DDERR_TOOBIGWIDTH: return "DDERR_TOOBIGWIDTH";
    case DDERR_UNSUPPORTED: return "DDERR_UNSUPPORTED";
    case DDERR_UNSUPPORTEDFORMAT: return "DDERR_UNSUPPORTEDFORMAT";
    case DDERR_UNSUPPORTEDMASK: return "DDERR_UNSUPPORTEDMASK";
    case DDERR_VERTICALBLANKINPROGRESS: return "DDERR_VERTICALBLANKINPROGRESS";
    case DDERR_WASSTILLDRAWING: return "DDERR_WASSTILLDRAWING";
    case DDERR_XALIGN: return "DDERR_XALIGN";
    case DDERR_INVALIDDIRECTDRAWGUID: return "DDERR_INVALIDDIRECTDRAWGUID";
    case DDERR_DIRECTDRAWALREADYCREATED: return "DDERR_DIRECTDRAWALREADYCREATED";
    case DDERR_NODIRECTDRAWHW: return "DDERR_NODIRECTDRAWHW";
    case DDERR_PRIMARYSURFACEALREADYEXISTS: return "DDERR_PRIMARYSURFACEALREADYEXISTS";
    case DDERR_NOEMULATION: return "DDERR_NOEMULATION";
    case DDERR_REGIONTOOSMALL: return "DDERR_REGIONTOOSMALL";
    case DDERR_CLIPPERISUSINGHWND: return "DDERR_CLIPPERISUSINGHWND";
    case DDERR_NOCLIPPERATTACHED: return "DDERR_NOCLIPPERATTACHED";
    case DDERR_NOHWND: return "DDERR_NOHWND";
    case DDERR_HWNDSUBCLASSED: return "DDERR_HWNDSUBCLASSED";
    case DDERR_HWNDALREADYSET: return "DDERR_HWNDALREADYSET";
    case DDERR_NOPALETTEATTACHED: return "DDERR_NOPALETTEATTACHED";
    case DDERR_NOPALETTEHW: return "DDERR_NOPALETTEHW";
    case DDERR_BLTFASTCANTCLIP: return "DDERR_BLTFASTCANTCLIP";
    case DDERR_NOBLTHW: return "DDERR_NOBLTHW";
    case DDERR_NODDROPSHW: return "DDERR_NODDROPSHW";
    case DDERR_OVERLAYNOTVISIBLE: return "DDERR_OVERLAYNOTVISIBLE";
    case DDERR_NOOVERLAYDEST: return "DDERR_NOOVERLAYDEST";
    case DDERR_INVALIDPOSITION: return "DDERR_INVALIDPOSITION";
    case DDERR_NOTAOVERLAYSURFACE: return "DDERR_NOTAOVERLAYSURFACE";
    case DDERR_EXCLUSIVEMODEALREADYSET: return "DDERR_EXCLUSIVEMODEALREADYSET";
    case DDERR_NOTFLIPPABLE: return "DDERR_NOTFLIPPABLE";
    case DDERR_CANTDUPLICATE: return "DDERR_CANTDUPLICATE";
    case DDERR_NOTLOCKED: return "DDERR_NOTLOCKED";
    case DDERR_CANTCREATEDC: return "DDERR_CANTCREATEDC";
    case DDERR_NODC: return "DDERR_NODC";
    case DDERR_WRONGMODE: return "DDERR_WRONGMODE";
    case DDERR_IMPLICITLYCREATED: return "DDERR_IMPLICITLYCREATED";
    case DDERR_NOTPALETTIZED: return "DDERR_NOTPALETTIZED";
    case DDERR_UNSUPPORTEDMODE: return "DDERR_UNSUPPORTEDMODE";
    case DDERR_NOMIPMAPHW: return "DDERR_NOMIPMAPHW";
    case DDERR_INVALIDSURFACETYPE: return "DDERR_INVALIDSURFACETYPE";
    case DDERR_DCALREADYCREATED: return "DDERR_DCALREADYCREATED";
    case DDERR_CANTPAGELOCK: return "DDERR_CANTPAGELOCK";
    case DDERR_CANTPAGEUNLOCK: return "DDERR_CANTPAGEUNLOCK";
    case DDERR_NOTPAGELOCKED: return "DDERR_NOTPAGELOCKED";
    case DDERR_NOTINITIALIZED: return "DDERR_NOTINITIALIZED";
    case E_NOINTERFACE: return "E_NOINTERFACE";

    default:
        static char buffer[48] = {};
        snprintf(buffer, sizeof(buffer), "Unknown Error: %#lx", err);
        return buffer;
    }
}
static bool printDDError(HRESULT hErr)
{
    printf("DirectDraw Error %s\n", dderr2str(hErr));
    return true;
}
#undef FAILED
#define FAILED(f) auto hr = (f); (HRESULT)hr < 0 && (printf(#f "\n"), printDDError(hr))

bool InitD3D(HWND hWnd)
{
    printf("Setup DDraw\n");
    if (FAILED(DirectDrawCreateEx(nullptr, (void**)&lpDD, IID_IDirectDraw7, nullptr)))
        return false;

    if (fullscreen)
    {
        if (FAILED(lpDD->SetCooperativeLevel(hWnd, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE)))
            return false;
        if (FAILED(lpDD->SetDisplayMode(640, 480, 32, 0, 0)))
            return false;
    }
    else
    {
        if (FAILED(lpDD->SetCooperativeLevel(hWnd, DDSCL_NORMAL)))
            return false;
    }

    DDSURFACEDESC2 ddsd = { sizeof(ddsd) };
    ddsd.dwFlags = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    if (fullscreen)
    {
        ddsd.ddsCaps.dwCaps |= DDSCAPS_FLIP | DDSCAPS_COMPLEX;
        ddsd.dwFlags |= DDSD_BACKBUFFERCOUNT;
        ddsd.dwBackBufferCount = 1;
    }

    printf("Get Primary Surface\n");
    if (FAILED(lpDD->CreateSurface(&ddsd, &lpPrimary, nullptr)))
        return false;

    printf("Get Back Buffer\n");
    if (fullscreen)
    {
        DDSCAPS2 ddscaps = { DDSCAPS_BACKBUFFER };
        if (FAILED(lpPrimary->GetAttachedSurface(&ddscaps, &lpBackBuffer)))
            return false;
    }
    else
    {
        // Retrieve pixel format from primary surface
        DDPIXELFORMAT pf = { sizeof(pf) };
        if (FAILED(lpPrimary->GetPixelFormat(&pf)))
            return false;
        printf("Pixel Format: %#lx, %#lx, %lu, %#lx, %#lx, %#lx, %#lx\n",
               pf.dwFlags, pf.dwFourCC,
               pf.dwRGBBitCount, pf.dwRBitMask,
               pf.dwGBitMask, pf.dwBBitMask,
               pf.dwRGBAlphaBitMask);

        // Create a back buffer with matching pixel format
        DDSURFACEDESC2 backDesc = { sizeof(backDesc) };
        backDesc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PIXELFORMAT;
        backDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE | DDSCAPS_VIDEOMEMORY;
        backDesc.dwWidth = 640;
        backDesc.dwHeight = 480;
        backDesc.ddpfPixelFormat = pf;

        if (FAILED(lpDD->CreateSurface(&backDesc, &lpBackBuffer, nullptr)))
            return false;

        // Create and set up the clipper for windowed mode
        if (FAILED(lpDD->CreateClipper(0, &lpClipper, nullptr)))
            return false;
        if (FAILED(lpClipper->SetHWnd(0, hWnd)))
            return false;
        if (FAILED(lpPrimary->SetClipper(lpClipper)))
            return false;
    }

    printf("Setup D3D\n");
    if (FAILED(lpDD->QueryInterface(IID_IDirect3D7, (void**)&lpD3D)))
        return false;

    printf("Create Device\n");
    if (FAILED(lpD3D->CreateDevice(IID_IDirect3DHALDevice, lpBackBuffer, &lpD3DDevice)))
        if (FAILED(lpD3D->CreateDevice(IID_IDirect3DRGBDevice, lpBackBuffer, &lpD3DDevice)))
            return false;

    lpD3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CCW);

    // Define triangle vertices
    struct CUSTOMVERTEX {
      FLOAT x, y, z, rhw;
      DWORD color;
    };
    static constexpr CUSTOMVERTEX vertices[] = {
        { 320.0f,  50.0f, 0.5f, 1.0f, 0xFFFF0000 },  // Top (red)
        { 420.0f, 400.0f, 0.5f, 1.0f, 0xFF0000FF },  // Bottom-right (blue)
        { 220.0f, 400.0f, 0.5f, 1.0f, 0xFF00FF00 },  // Bottom-left (green)
    };

    printf("Create Vertex Buffer\n");
    D3DVERTEXBUFFERDESC vdesc = {sizeof(vdesc), D3DVBCAPS_WRITEONLY,
                                 D3DFVF_XYZRHW | D3DFVF_DIFFUSE,
                                 sizeof(vertices) / sizeof(vertices[0])};
    if (FAILED(lpD3D->CreateVertexBuffer(&vdesc, &lpVertexBuffer, 0)))
        return false;

    CUSTOMVERTEX *pVertices;
    if (FAILED(lpVertexBuffer->Lock(DDLOCK_WRITEONLY, (void**)&pVertices, nullptr)))
        return false;
    memcpy(pVertices, vertices, sizeof(vertices));
    if (FAILED(lpVertexBuffer->Unlock()))
        return false;

    return true;
}

void Cleanup() {
    if (lpClipper) lpClipper->Release();
    if (lpVertexBuffer) lpVertexBuffer->Release();
    if (lpD3DDevice) lpD3DDevice->Release();
    if (lpD3D) lpD3D->Release();
    if (lpBackBuffer) lpBackBuffer->Release();
    if (lpPrimary) lpPrimary->Release();
    if (lpDD) lpDD->Release();
}

void Render()
{
    if (!lpD3DDevice)
      return;
    lpD3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET, 0xFF000000, 1.0f, 0);
    lpD3DDevice->BeginScene();

    lpD3DDevice->DrawPrimitiveVB(D3DPT_TRIANGLELIST, lpVertexBuffer, 0, 3, D3DDP_DONOTCLIP);

    lpD3DDevice->EndScene();

    if (fullscreen) {
        lpPrimary->Flip(nullptr, DDFLIP_WAIT);
    } else {
        // Blit back buffer to primary surface at the window location
        POINT p = {};
        RECT rect;
        ClientToScreen(hWnd, &p);
        GetClientRect(hWnd, &rect);
        OffsetRect(&rect, p.x, p.y);
        lpPrimary->Blt(&rect, lpBackBuffer, nullptr, DDBLT_WAIT, nullptr);
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR lpCmdLine, int /*nCmdShow*/)
{
    fullscreen = (strstr(lpCmdLine, "-fullscreen") != nullptr);
    printf("Fullscreen: %d\n", fullscreen);

    const WNDCLASS wc = { CS_CLASSDC, WndProc, 0, 0, hInstance, nullptr, nullptr, nullptr, nullptr, "D3D7Triangle" };
    RegisterClass(&wc);

    DWORD style = fullscreen ? WS_POPUP : WS_OVERLAPPED;
    hWnd = CreateWindow("D3D7Triangle", "Direct3D 7 Triangle", style, 100, 100, 640, 480, nullptr, nullptr, hInstance, nullptr);

    if (!InitD3D(hWnd))
        return 0;

    ShowWindow(hWnd, SW_SHOWDEFAULT);
    UpdateWindow(hWnd);

     MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            Render();
        }
    }

    Cleanup();
    return (int)msg.wParam;
}
