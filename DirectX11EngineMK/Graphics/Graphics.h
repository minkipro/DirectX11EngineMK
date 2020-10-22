#pragma once
#include "AdapterReader.h"
#include "Shaders.h"
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <WICTextureLoader.h>
#include "Camera3D.h"

#include "../Timer.h"
#include "ImGUI\\imgui.h"
#include "ImGUI\\imgui_impl_win32.h"
#include "ImGUI\\imgui_impl_dx11.h"
#include "RenderableGameObject.h"

#include "Camera2D.h"
#include "Sprite.h"

#include "Geometry.h"

class Graphics
{
public:
	bool Initialize(HWND hwnd, int width, int height);
	void RenderFrame();

private:
	bool InitializeDirectX(HWND hwnd);
	bool InitializeShaders();
	bool InitializeScene();
	void InitializeImgui(HWND hwnd);

	bool getScreenCoord(DirectX::XMFLOAT2& output, DirectX::XMVECTOR input, DirectX::XMMATRIX& viewProj);

private:
	Microsoft::WRL::ComPtr<ID3D11Device>			_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		_deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain>			_swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	_renderTargetView;
	UINT _numViewport = 0;
	

	VertexShader									_vertexShader_2d;
	VertexShader									_vertexShader;
	VertexShader									_vertexShader_skeleton;

	PixelShader										_pixelShader_2d;
	PixelShader										_pixelShader;
	PixelShader										_pixelShader_nolight;

	VertexShader									_vertexShader_color;
	PixelShader										_pixelShader_color;
	ConstantBuffer<CB_VS_vertexshader_color>		_cb_vs_vertexshader_color;

	ConstantBuffer<CB_VS_vertexshader>				_cb_vs_vertexshader;
	ConstantBuffer<CB_VS_vertexshader_2D>			_cb_vs_vertexshader_2d;
	ConstantBuffer<CB_VS_vertexshader_skeleton>		_cb_vs_vertexshader_skeleton;


	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	_depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>			_depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> _depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	_rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11BlendState>		_blendState;

	std::unique_ptr<DirectX::SpriteBatch>			_spriteBatch;
	std::unique_ptr<DirectX::SpriteFont>			_spriteFont;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>		 _samplerState;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _pinkTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _grassTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _pavementTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _pianoTexture;

	int _windowWidth = 0;
	int _windowHeight = 0;

public:
	float _camera3DSpeed;
	float _cameraRotSpeed;
	Camera3D _camera3D;
	RenderableGameObject _gameObject;
	Timer _fpsTimer;

	Camera2D _camera2D;
	Sprite _sprite;
	Geometry _geometry;

	float _factor = 1.0f;
};