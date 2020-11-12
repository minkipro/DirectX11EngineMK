#include "Graphics.h"

bool Graphics::Initialize(HWND hwnd, int width, int height)
{
	_windowWidth = width;
	_windowHeight = height;
	_camera3DSpeed = 0.1f;
	_cameraRotSpeed = 0.01f;
	_fpsTimer.Start();
	if (!InitializeDirectX(hwnd))
		return false;
	if (!InitializeShaders())
		return false;
	if (!InitializeScene())
		return false;

	InitializeImgui(hwnd);
	return true;
}

void Graphics::RenderFrame()
{
	XMMATRIX viewMatrix = _camera3D.GetViewMatrix();
	XMMATRIX projectionMatrix = _camera3D.GetProjectionMatrix();
	XMMATRIX viewProj = viewMatrix * projectionMatrix;

	float bgcolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), bgcolor);
	_deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	_deviceContext->OMSetDepthStencilState(_depthStencilState.Get(), 0);

	
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	_deviceContext->RSSetState(_rasterizerState.Get());
	_deviceContext->PSSetSamplers(0, 1, _samplerState.GetAddressOf());

	_deviceContext->VSSetShader(_vertexShader_skeleton.GetShader(), NULL, 0);
	_deviceContext->PSSetShader(_pixelShader_nolight.GetShader(), NULL, 0);
	_deviceContext->IASetInputLayout(_vertexShader_skeleton.GetInputLayout());
	float currentTime = _fpsTimer.GetMilisecondsElapsed();
	for (size_t i = 0; i < _gameObjects.size(); i++)
	{
		if (_gameObjects[i]->GetIsAnim())
		{
			_gameObjects[i]->Draw(viewProj, &currentTime);
		}
		else
		{
			_gameObjects[i]->Draw(viewProj, nullptr);
		}
	}

	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
	_deviceContext->VSSetShader(_vertexShader_color.GetShader(), NULL, 0);
	_deviceContext->PSSetShader(_pixelShader_color.GetShader(), NULL, 0);
	_deviceContext->IASetInputLayout(_vertexShader_color.GetInputLayout());
	
	{
		_geometry.Draw(viewProj,_camera3D.GetPositionVector());
	}

	

	static int fpsCounter = 0;
	static std::string fpsString = "FPS : 0";
	fpsCounter += 1;
	if (_fpsTimer.GetMilisecondsElapsed() > 1000.0)
	{
		fpsString = "FPS: " + std::to_string(fpsCounter);
		fpsCounter = 0;
		_fpsTimer.Restart();
	}

	_spriteBatch->Begin();
	/*for (int i = 0; i < 100; i++)
	{
		XMVECTOR pos = { (float)i,(float)i,(float)i };
		pos *= 10.0f;
		pos = XMVector3Transform(pos, (*_gameObject.GetWorldMatrix()));

		XMFLOAT2 screenCoord;
		bool zSine = getScreenCoord(screenCoord, pos, viewProj);
		if (zSine > 0)
		{
			char buffer[1024] = {0,};
			_itoa_s(i, buffer,10);
			_spriteFont->DrawString(_spriteBatch.get(), StringHelper::StringToWide(buffer).c_str(), screenCoord, DirectX::Colors::Black, 0.0f, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f));

		}
	}*/

	_spriteFont->DrawString(_spriteBatch.get(), StringHelper::StringToWide(fpsString).c_str(), XMFLOAT2(0, 0), DirectX::Colors::Black, 0.0f, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f));
	
	
	_spriteBatch->End();

	static int counter = 0;
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	
	
	//ImGui::Begin("Camera Speed");
	//ImGui::DragFloat3("pos", &fpos.x);
	//ImGui::DragFloat3("viewS", viewS.m128_f32);
	//ImGui::DragFloat4("viewR", viewR.m128_f32);
	//ImGui::DragFloat3("viewT", viewT.m128_f32);
	///*ImGui::DragFloat("Camera Speed", &_camera3DSpeed, 0.01f, 0.0f, 1.0f);
	//ImGui::DragFloat("Camera Rot Speed", &_cameraRotSpeed, 0.001f, 0.0f, 1.0f);*/
	//ImGui::End();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	_swapchain->Present(0, NULL);
}

Graphics::~Graphics()
{
	for (size_t i = 0; i < _gameObjects.size(); i++)
	{
		if (_gameObjects[i]);
		{
			delete _gameObjects[i];
		}
	}
}

bool Graphics::InitializeDirectX(HWND hwnd)
{
	try
	{
		std::vector<AdapterData> adapters = AdapterReader::GetAdapters();

		if (adapters.empty())
		{
			ErrorLogger::Log("No DXGI Adapters found.");
			return false;
		}
		
		DXGI_SWAP_CHAIN_DESC scd = { 0 };
		scd.BufferDesc.Width = _windowWidth;
		scd.BufferDesc.Height = _windowHeight;
		scd.BufferDesc.RefreshRate.Numerator = 60;
		scd.BufferDesc.RefreshRate.Denominator = 1;
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		scd.SampleDesc.Count = 1;
		scd.SampleDesc.Quality = 0;

		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.BufferCount = 1;
		scd.OutputWindow = hwnd;
		scd.Windowed = TRUE;
		scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		HRESULT hr;
		hr = D3D11CreateDeviceAndSwapChain(adapters[0]._pAdapter,
			D3D_DRIVER_TYPE_UNKNOWN,
			NULL, //FOR SOFTWARE DRIVER TYPE
			NULL, //FLAGS FOR RUNTIME LAYERS
			NULL, //FEATURE LEVELS ARRAY
			0, //# OF FEATURE LEVEL IN ARRAY
			D3D11_SDK_VERSION,
			&scd, // Swapchain Description
			_swapchain.GetAddressOf(), //Swapchain Address
			_device.GetAddressOf(),
			NULL, // Supported feature level
			_deviceContext.GetAddressOf());

		COM_ERROR_IF_FAILED(hr, "Failed to create device and swapchain.");

		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		hr = _swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		COM_ERROR_IF_FAILED(hr, "GetBuffer Failed.");
		hr = _device->CreateRenderTargetView(backBuffer.Get(), NULL, _renderTargetView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create render target view.");

		//Describe our Depth/Stencil Buffer
		CD3D11_TEXTURE2D_DESC depthStancilTextureDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, _windowWidth, _windowWidth);
		depthStancilTextureDesc.MipLevels = 1;
		depthStancilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		hr = _device->CreateTexture2D(&depthStancilTextureDesc, NULL, _depthStencilBuffer.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil buffer.");

		hr = _device->CreateDepthStencilView(_depthStencilBuffer.Get(), NULL, _depthStencilView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil view.");

		//Create depth stencil state
		CD3D11_DEPTH_STENCIL_DESC depthstencildesc(D3D11_DEFAULT);
		depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

		hr = _device->CreateDepthStencilState(&depthstencildesc, _depthStencilState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state.");
		
		_deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), _depthStencilView.Get());

		//Create & Set the Viewport
		CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(_windowWidth), static_cast<float>(_windowHeight));

		_deviceContext->RSSetViewports(1, &viewport);
		//Create Rasterizer State
		CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
		hr = _device->CreateRasterizerState(&rasterizerDesc, _rasterizerState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");

		//Create Blend State
		D3D11_BLEND_DESC blendDesc = { 0 };

		D3D11_RENDER_TARGET_BLEND_DESC rtbd = { 0 };

		rtbd.BlendEnable = true;
		rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
		rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
		rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

		blendDesc.RenderTarget[0] = rtbd;

		hr = _device->CreateBlendState(&blendDesc, _blendState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create blend state.");

		_spriteBatch = std::make_unique<SpriteBatch>(_deviceContext.Get());
		_spriteFont = std::make_unique<SpriteFont>(_device.Get(), L"Data\\Fonts\\comic_sans_ms_16.spritefont");

		//Create sampler description for sampler state
		CD3D11_SAMPLER_DESC sampDesc(D3D11_DEFAULT);
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		hr = _device->CreateSamplerState(&sampDesc, _samplerState.GetAddressOf()); //Create sampler state
		COM_ERROR_IF_FAILED(hr, "Failed to create sampler state.");
	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	

	return true;
}

bool Graphics::InitializeShaders()
{
	std::wstring shaderfolder;
#pragma region DetermineShaderPath
	if (IsDebuggerPresent() == TRUE)
	{
#ifdef _DEBUG
		
#ifdef _WIN64
		shaderfolder = L"../x64/Debug/";
#else
		shaderfolder = L"../Debug/";
#endif

#else
#ifdef _WIN64
		shaderfolder = L"../x64/Release/";
#else
		shaderfolder = L"../Release/";
#endif

#endif

	}

	D3D11_INPUT_ELEMENT_DESC layoutColor[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT , 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	if(!_vertexShader_color.Initialize(_device, shaderfolder + L"vertexshader_color.cso", layoutColor, ARRAYSIZE(layoutColor)))
		return false;

	if (!_pixelShader_color.Initialize(_device, shaderfolder + L"pixelshader_color.cso"))
		return false;

	//2d shader
	D3D11_INPUT_ELEMENT_DESC layout2D[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT , 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	if (!_vertexShader_2d.Initialize(_device, shaderfolder + L"vertexshader_2d.cso", layout2D, ARRAYSIZE(layout2D)))
		return false;

	if (!_pixelShader_2d.Initialize(_device, shaderfolder + L"pixelshader_2d.cso"))
		return false;

	//3d shader
	D3D11_INPUT_ELEMENT_DESC layout3D[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT , 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	
	if (!_vertexShader.Initialize(_device, shaderfolder + L"vertexshader.cso", layout3D, ARRAYSIZE(layout3D)))
		return false;

	D3D11_INPUT_ELEMENT_DESC layout3D_skeleton[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT , 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BONEIDA", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BONEIDB", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BONEWEIGHTA", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BONEWEIGHTB", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	if (!_vertexShader_skeleton.Initialize(_device, shaderfolder + L"vertexshader_skeleton.cso", layout3D_skeleton, ARRAYSIZE(layout3D_skeleton)))
		return false;

	if (!_pixelShader.Initialize(_device, shaderfolder + L"pixelshader.cso"))
		return false;

	if (!_pixelShader_nolight.Initialize(_device, shaderfolder + L"pixelshader_nolight.cso"))
		return false;

	return true;
}

bool Graphics::InitializeScene()
{
	try
	{
		//Load Texture
		HRESULT hr = CreateWICTextureFromFile(_device.Get(), L"Data\\Textures\\seamless_grass.jpg", nullptr, _grassTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");

		hr = CreateWICTextureFromFile(_device.Get(), L"Data\\Textures\\pinksquare.jpg", nullptr, _pinkTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");

		hr = CreateWICTextureFromFile(_device.Get(), L"Data\\Textures\\seamless_pavement.jpg", nullptr, _pavementTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");

		hr = CreateWICTextureFromFile(_device.Get(), L"Data\\Textures\\piano.PNG", nullptr, _pianoTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");
		
		//Initialize Constant Buffer
		hr = _cb_vs_vertexshader_2d.Initialize(_device.Get(), _deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");

		hr = _cb_vs_vertexshader.Initialize(_device.Get(), _deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");

		hr = _cb_vs_vertexshader_color.Initialize(_device.Get(), _deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");

		hr = _cb_vs_vertexshader_skeleton.Initialize(_device.Get(), _deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");

		//Initialize Model(s)

		for (int i = 0; i < 1; i++)
		{
			_gameObjects.push_back(new RenderableGameObject);
		}

		/*if (!_gameObjects[0]->Initialize("Data\\Objects\\Nanosuit\\rp_nathan_animated_003_walking.fbx", _device.Get(), _deviceContext.Get(), _cb_vs_vertexshader_skeleton))
		{
			return false;
		}*/

		/*if (!_gameObjects[1]->Initialize("Data\\Objects\\Nanosuit\\rp_sophia_animated_003_idling.fbx", _device.Get(), _deviceContext.Get(), _cb_vs_vertexshader_skeleton))
		{
			return false;
		}*/
		if (!_gameObjects[0]->Initialize("Data\\Objects\\characters\\garen_2013_attack_01.fbx", _device.Get(), _deviceContext.Get(), _cb_vs_vertexshader_skeleton))
		{
			return false;
		}
	
		//_gameObjects[0]->SetPosition(100, 0, 0);


		/*if (!_gameObjects[2]->Initialize("Data\\Objects\\scifi tropical city\\Sci-fi Tropical city.obj", _device.Get(), _deviceContext.Get(), _cb_vs_vertexshader_skeleton))
		{
			return false;
		}*/


		

		if (!_sprite.Initialize(_device.Get(), _deviceContext.Get(), 256, 256, "Data/Textures/circle.png", _cb_vs_vertexshader_2d))
			return false;

		_sprite.SetPosition(XMFLOAT3(_windowWidth / 2 - _sprite.GetWidth() / 2, _windowHeight / 2 - _sprite.GetHeight(), 0.0f));
		_camera2D.SetProjectionValues(_windowWidth, _windowHeight, 0.0f, 1.0f);

		_camera3D.SetPosition(0.0f, 0.0f, -2.0f);
		_camera3D.SetProjectionValues(90.0f, static_cast<float>(_windowWidth) / static_cast<float>(_windowHeight), 0.1f, 3000.0f);

		_geometry.Initialize(_device.Get(), _deviceContext.Get(), _cb_vs_vertexshader_color);
	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}

	return true;
}

void Graphics::InitializeImgui(HWND hwnd)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(_device.Get(), _deviceContext.Get());
	ImGui::StyleColorsDark();
}

bool Graphics::getScreenCoord(DirectX::XMFLOAT2& output, DirectX::XMVECTOR input, DirectX::XMMATRIX& viewProj)
{
	XMVECTOR pos = XMVector3TransformCoord(input, viewProj);
	XMVECTOR pos_ = XMVector3Transform(input, viewProj);

	XMFLOAT3 fpos;
	XMStoreFloat3(&fpos, pos);

	output.x = (fpos.x + 1.0f) * _windowWidth * 0.5f;
	output.y = (-fpos.y + 1.0f) * _windowHeight * 0.5f;

	return pos_.m128_f32[2] > 0.0f;
}
