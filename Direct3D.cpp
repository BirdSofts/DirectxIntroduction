﻿// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,19.07.2019</created>
/// <changed>ʆϒʅ,10.08.2019</changed>
// ********************************************************************************

#include "Direct3D.h"
#include "Shared.h"


Direct3D::Direct3D ( TheCore* coreObj ) : core ( coreObj ),
// reserve 8 bits for red, green, blue and transparency each in unsigned normalized integer
colourFormat ( DXGI_FORMAT_B8G8R8A8_UNORM ),
initialized ( false ), created ( false ), resized ( false )

{
  try
  {
    HRESULT hResult;

    // flag: needed to get Direct2D interoperability with Direct3D resources
    unsigned int deviceFlags = D3D10_CREATE_DEVICE_BGRA_SUPPORT |
      D3D10_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS;

#ifndef _NOT_DEBUGGING
    deviceFlags |= D3D10_CREATE_DEVICE_DEBUG; // creation with debug layer
#endif // !_NOT_DEBUGGING

    // creation of the device and its context
    // first parameter: pointer to the present adapter on system
    D3D10_FEATURE_LEVEL1 featureLevel { D3D10_FEATURE_LEVEL_10_1 };
    hResult = D3D10CreateDevice1 ( nullptr, D3D10_DRIVER_TYPE_HARDWARE, NULL,
                                   deviceFlags, featureLevel, D3D10_1_SDK_VERSION, &device );



    //// create the device an d swap chain:
    //// filling a swap chain description structure (the type of swap chain)
    //DXGI_SWAP_CHAIN_DESC descSwapC;
    //descSwapC.BufferDesc.Width = 0; // back buffer size, 0: automatic adjustment
    //descSwapC.BufferDesc.Height = 0; // the same
    //descSwapC.BufferDesc.RefreshRate.Numerator = 0; // 0: don't care and don't correct it
    //descSwapC.BufferDesc.RefreshRate.Denominator = 1;
    //descSwapC.BufferDesc.Format = colourFormat; // display format
    //descSwapC.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // scan-line drawing
    //descSwapC.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // image size adjustment to back buffer resolution
    //// number of multi samplings per pixel and image quality (1 and 0: disable multi sampling (no anti-aliasing))
    //descSwapC.SampleDesc.Count = 1;
    //descSwapC.SampleDesc.Quality = 0;
    //descSwapC.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // back buffer as render output target
    //descSwapC.BufferCount = 3; // including the front buffer (one front buffer and two back buffers)
    //descSwapC.OutputWindow = core->getHandle (); // handle to main window
    //descSwapC.Windowed = true; // recommendation: windowed creation and switch to full screen
    //// flip (in windowed mode: blit) and discard the content of back buffer after presentation
    //descSwapC.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    //descSwapC.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow switching the display mode

    //hResult = D3D10CreateDeviceAndSwapChain1 ( nullptr, D3D10_DRIVER_TYPE_HARDWARE, NULL,
    //                                           deviceFlags, featureLevel, D3D10_1_SDK_VERSION,
    //                                           &descSwapC, &swapChain, &device );
    //created = true;
    //resize ();



    if (SUCCEEDED ( hResult ))
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"Direct3D device is successfully created." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "crD" );
      throw* PointerProvider::getException ();
    }

    hResult = device->QueryInterface ( __uuidof(ID3D10Debug), &debug );
    if (SUCCEEDED ( hResult ))
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"Direct3D device debug layer is successfully created." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "crDdl" );
      throw* PointerProvider::getException ();
    }

    createResources ();

    if (created && resized)
      initialized = true;

  }
  catch (const std::exception& ex)
  {

    if (ex.what () == "crD")
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"The creation of Direct3D device failed!" );
#endif // !_NOT_DEBUGGING

    } else
      if (ex.what () == "crDdl")
      {

#ifndef _NOT_DEBUGGING
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                  L"The creation of Direct3D device debug layer failed!" );
#endif // !_NOT_DEBUGGING

      } else
      {

#ifndef _NOT_DEBUGGING
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                  Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

      }
  }
};


const bool& Direct3D::isInitialized ()
{
  return initialized;
};


void Direct3D::createResources ( void )
{
  try
  {
    // swap chain creation:

    created = false;
    HRESULT hResult;

    // -- getting the DXGI factory (swap chain needs):
    // note that the factory used to created the Direct3D device must be retrieved
    // ---- retrieving the underlying DXGI device
    hResult = device.As ( &dxgiDevice );
    if (SUCCEEDED ( hResult ))
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"The underlying DXGI device is successfully retrieved." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "getF" );
      throw* PointerProvider::getException ();
    }

    // ---- physical GPU identification
    hResult = dxgiDevice->GetAdapter ( dxgiAdapter.GetAddressOf () );
    if (SUCCEEDED ( hResult ))
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"The Physical GPU is successfully identified." );
#endif // !_NOT_DEBUGGING
    } else
    {
      PointerProvider::getException ()->set ( "getF" );
      throw* PointerProvider::getException ();
    }

    // ---- retrieving the factory
    hResult = dxgiAdapter->GetParent ( __uuidof(IDXGIFactory), &dxgiFactory );
    if (SUCCEEDED ( hResult ))
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"The factory is successfully retrieved." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "getF" );
      throw* PointerProvider::getException ();
    }

    // -- filling a swap chain description structure (the type of swap chain)
    DXGI_SWAP_CHAIN_DESC descSwapC;
    descSwapC.BufferDesc.Width = 0; // back buffer size, 0: automatic adjustment
    descSwapC.BufferDesc.Height = 0; // the same
    descSwapC.BufferDesc.RefreshRate.Numerator = 0; // 0: don't care and don't correct it
    descSwapC.BufferDesc.RefreshRate.Denominator = 1;
    descSwapC.BufferDesc.Format = colourFormat; // display format
    descSwapC.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // scan-line drawing
    descSwapC.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // image size adjustment to back buffer resolution
    // number of multi samplings per pixel and image quality (1 and 0: disable multi sampling (no anti-aliasing))
    descSwapC.SampleDesc.Count = 1;
    descSwapC.SampleDesc.Quality = 0;
    descSwapC.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // back buffer as render output target
    descSwapC.BufferCount = 3; // including the front buffer (one front buffer and two back buffers)
    descSwapC.OutputWindow = core->getHandle (); // handle to main window
    descSwapC.Windowed = true; // recommendation: windowed creation and switch to full screen
    // flip (in windowed mode: blit) and discard the content of back buffer after presentation
    descSwapC.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    descSwapC.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow switching the display mode

    // -- and finally the creation of the swap chain
    hResult = dxgiFactory->CreateSwapChain ( device.Get (), &descSwapC, swapChain.GetAddressOf () );
    if (SUCCEEDED ( hResult ))
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"Swap chain is successfully created." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "crS" );
      throw* PointerProvider::getException ();
    }

    created = true;

    resize ();

  }
  catch (const std::exception& ex)
  {
    if (ex.what () == "getF")
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Getting the factory by the DGGI Adapter failed!" );
#endif // !_NOT_DEBUGGING

    } else
      if (ex.what () == "crS")
      {

#ifndef _NOT_DEBUGGING
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                  L"Creation of swap chain failed!" );
#endif // !_NOT_DEBUGGING

      } else
      {

#ifndef _NOT_DEBUGGING
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                  Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

      }

  }
};


void Direct3D::resize ( void )
{
  try
  {

    HRESULT hResult;

    // release and reset all resources
    if (core->initialized && resized)
    {
      resized = false;

      if (core->d2d)
      {
        core->d2d->textLayoutLogs.Reset ();
        core->d2d->textLayoutFPS.Reset ();
        core->d2d->textFormatLogs.Reset ();
        core->d2d->textFormatFPS.Reset ();
        core->d2d->brushBlack.Reset ();
        core->d2d->brushWhite.Reset ();
        core->d2d->brushYellow.Reset ();
        core->d2d->dcBitmap.Reset ();
        core->d2d->dcBuffer.Reset ();
        core->d2d->deviceCon.Reset ();
      }
      if (dsView && rtView)
      {
        device->OMSetRenderTargets ( 0, nullptr, nullptr );
        dsBuffer.Reset ();
        dsView.Reset ();
        rtBuffer.Reset ();
        auto ref = rtView.Reset ();
        device->ClearState ();

#ifndef _NOT_DEBUGGING
        if (ref == 0)
          PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                    L"The resizing procedure preparations is successfully complete." );
#endif // !_NOT_DEBUGGING

      }
    }

    // resizing the swap chain buffers (on resize of the client window)
    // BufferCount and SwapChainFlags: 0 do not change the current
    // 0 for the next two parameters to adjust to the current client window size
    // next parameter: set to DXGI_FORMAT_UNKNOWN to preserve the current
    device->ClearState ();
    hResult = swapChain->ResizeBuffers ( 0, 0, 0, DXGI_FORMAT_UNKNOWN, 0 );
    if (SUCCEEDED ( hResult ))
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"The buffers of swap chain are successfully resized." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "reS" );
      throw* PointerProvider::getException ();
    }

    // the render target view recreation
    // obtain a pointer to the current back buffer in the swap chain
    // the zero-th buffer is accessible, since already created using flip discarding effect.
    // second parameter: interface type (most cases 2D- texture)
    // the last parameter returns a pointer to the actual back buffer
    hResult = swapChain->GetBuffer ( 0, __uuidof(ID3D10Texture2D), reinterpret_cast<void**>(rtBuffer.GetAddressOf ()) );
    if (SUCCEEDED ( hResult ))
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"Swap chain back buffer is successfully obtained." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "backB" );
      throw* PointerProvider::getException ();
    }

    // first parameter: the resource for which the render target is created for
    // second parameter describes data type of the specified resource (mipmap but 0 for now)
    // the last parameter returns a pointer to the created render target view
    hResult = device->CreateRenderTargetView ( rtBuffer.Get (), NULL, &rtView );
    if (SUCCEEDED ( hResult ))
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"The swap chain is successfully recreated." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "crR" );
      throw* PointerProvider::getException ();
    }

    // depth-stencil buffer creation
    CD3D10_TEXTURE2D_DESC descDepth;
    rtBuffer->GetDesc ( &descDepth ); // retrieves the description of the back buffer and fill! :)
    //descDepth.Width = ;
    //descDepth.Height = ;
    //descDepth.MipLevels = 1;
    //descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 24 bits for depth and 8 bits for stencil
    //descDepth.SampleDesc.Count = 1; // multi-sampling (anti-aliasing) match to settings of render target
    //descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D10_USAGE_DEFAULT; // value: only GPU will be reading and writing to the resource
    descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL; // how to bind to the different pipeline stages
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    // texture creation:
    // the second parameter: pointer to initial data (zero for any data, since depth-stencil buffer)
    hResult = device->CreateTexture2D ( &descDepth, nullptr, dsBuffer.GetAddressOf () );
    if (SUCCEEDED ( hResult ))
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"The texture is successfully created." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "cr2Dt" );
      throw* PointerProvider::getException ();
    }

    // depth-stencil view description
    D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
    descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    // depth-stencil view creation
    // the second parameter: zero to access the mipmap level 0
    hResult = device->CreateDepthStencilView ( dsBuffer.Get (), &descDSV, dsView.GetAddressOf () );
    if (SUCCEEDED ( hResult ))
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"The depth-stencil view is successfully created." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "crD-Sb" );
      throw* PointerProvider::getException ();
    }

    // binding the depth-stencil view (for now one render target view)
    // second parameter: pointer to first element of a list of render target view pointers
    device->OMSetRenderTargets ( 1, rtView.GetAddressOf (), dsView.Get () );

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                              L"The depth-stencil buffer is successfully activated." );
#endif // !_NOT_DEBUGGING

    // viewport structure: set the viewport to entire back buffer (what area should be rendered to)
    D3D10_VIEWPORT viewPort;
    viewPort.TopLeftX = 0; // first four integers: viewport rectangle (relative to client window rectangle)
    viewPort.TopLeftY = 0;
    viewPort.Width = descDepth.Width;
    viewPort.Height = descDepth.Height;
    viewPort.MinDepth = 0.0f; // minimum and maximum depth buffer values
    viewPort.MaxDepth = 1.0f;
    // setting the viewport
    // the second parameter is a pointer to an array of viewports
    device->RSSetViewports ( 1, &viewPort );

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                              L"The view port is successfully set." );
#endif // !_NOT_DEBUGGING

    // recreation of the Direct2D target bitmap associated with the swap chain back buffer

    if (core->initialized && core->appWindow->isResized ())
    {
      core->d2d->createResources ();
      core->d2d->initializeTextFormats ();
      core->appWindow->isResized () = false;
    }

    clearBuffers ();

    resized = true;
  }
  catch (const std::exception& ex)
  {
    if (ex.what () == "reS")
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Resizing the swap chain failed!" );
#endif // !_NOT_DEBUGGING

    } else
      if (ex.what () == "backB")
      {

#ifndef _NOT_DEBUGGING
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                  L"Acquiring the back buffer failed!" );
#endif // !_NOT_DEBUGGING

      } else
        if (ex.what () == "crR")
        {

#ifndef _NOT_DEBUGGING
          PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                    L"Creation of render target view failed!" );
#endif // !_NOT_DEBUGGING

        } else
          if (ex.what () == "cr2Dt")
          {

#ifndef _NOT_DEBUGGING
            PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                      L"Creation of 2D-texture failed!" );
#endif // !_NOT_DEBUGGING

          } else
            if (ex.what () == "crD-Sb")
            {

#ifndef _NOT_DEBUGGING
              PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                        L"Getting the depth-stencil buffer failed!" );
#endif // !_NOT_DEBUGGING

            } else
            {

#ifndef _NOT_DEBUGGING
              PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                        Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

            }
  }
};


void Direct3D::clearBuffers ( void )
{
  try
  {
    // make ready for the new scenes after each frame (clear all leftover artefacts)
    float black [] { 0.0f, 0.0f, 0.0f, 0.0f };
    // filling the entire back buffer with a single colour
    device->ClearRenderTargetView ( rtView.Get (), black );
    // second parameter: the type of data to clear (obviously set to clear both depth-stencil)
    // the values are used to override the entire depth-stencil buffer with
    device->ClearDepthStencilView ( dsView.Get (), D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0 );

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                              L"All leftover artefacts are now successfully cleared from the buffers." );
#endif // !_NOT_DEBUGGING

  }
  catch (const std::exception& ex)
  {

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

  }
};


void Direct3D::present ( void )
{
  try
  {
    // SyncInterval parameter: the way a frame is synchronized with VBLANK:
    // in flip mode: the n = 0 sets the DirectX to cancel the remaining time of previously rendered scene
    // and discard this frame if a newer frame is on the queue. (screen tearing might occur)
    // the n = 1 to 4 values: synchronize the presentation after n-th vertical blank.
    // the second parameter: not waiting for v-sync.
    HRESULT hResult { swapChain->Present ( 0, DXGI_PRESENT_DO_NOT_WAIT ) };

    if ((FAILED ( hResult )) &&
      (hResult != DXGI_ERROR_WAS_STILL_DRAWING)) // occurs, if the calling thread is blocked
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::warning, std::this_thread::get_id (), L"mainThread",
                                                L"The presentation of the scene failed!" );
#endif // !_NOT_DEBUGGING

    }
  }
  catch (const std::exception& ex)
  {

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

  }
};


void Direct3D::shutdown ( void )
{
  try
  {
    device->OMSetRenderTargets ( 0, nullptr, nullptr );
    auto ref = dsBuffer.Reset ();
    ref = dsView.Reset ();
    ref = rtBuffer.Reset ();
    ref = rtView.Reset ();
    device->ClearState ();
    ref = dxgiFactory.Reset ();
    ref = dxgiAdapter.Reset ();
    ref = dxgiDevice.Reset ();
    ref = swapChain.Reset ();
    device->ClearState ();
    ref = debug.Reset ();
    //ref = device->Release ();
    //ref = device.Reset ();
    initialized = false;
    if (core)
    {
      core = nullptr;
      delete core;
    }

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                              L"Direct3D is successfully destructed." );
#endif // !_NOT_DEBUGGING

  }
  catch (const std::exception& ex)
  {

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

  }
};