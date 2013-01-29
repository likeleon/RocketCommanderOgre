#include "stdafx.h"
#include "DxMesh.h"

namespace MeshXToOgre
{
	DxMesh::DxMesh(const std::wstring &mesh)
		: m_mesh(mesh)
		, m_loaded(false)
		, g_pD3D(NULL)
		, g_pd3dDevice(NULL)
		, g_pMesh(NULL)
		, g_pMeshMaterials(NULL)
		, g_pMeshTextures(NULL)
		, g_dwNumMaterials(0)
	{
	}

	DxMesh::~DxMesh()
	{
		Cleanup();
	}

	const LPD3DXMESH DxMesh::GetMesh()
	{
		if (IsLoaded())
		{
			return g_pMesh;
		}

		try
		{
			// Create the D3D object.
			if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
				throw std::exception("Direct3DCreate9 failed");

			// Set up the structure used to create the D3DDevice. Since we are now
			// using more complex geometry, we will create a device with a zbuffer.
			D3DPRESENT_PARAMETERS d3dpp;
			ZeroMemory( &d3dpp, sizeof( d3dpp ) );
			d3dpp.Windowed = TRUE;
			d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
			d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
			d3dpp.EnableAutoDepthStencil = TRUE;
			d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

			// Register the window class
			WNDCLASSEX wc =
			{
				sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L,
				GetModuleHandle( NULL ), NULL, NULL, NULL, NULL,
				L"MeshXToOgre", NULL
			};
			RegisterClassEx( &wc );

			// Create dummy window
			HWND hWnd = CreateWindow( L"MeshXToOgre", L"Mesh X To Ogre",
				WS_OVERLAPPEDWINDOW, 100, 100, 300, 300,
				NULL, NULL, wc.hInstance, NULL );
			ShowWindow(hWnd, SW_HIDE);

			// Create the D3DDevice
			if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&d3dpp, &g_pd3dDevice ) ) )
			{
				throw std::exception("CreateDevice failed");
			}

			LPD3DXBUFFER pD3DXMtrlBuffer;
			

			// Load the mesh from the specified file
			if( FAILED( D3DXLoadMeshFromX( m_mesh.c_str(), D3DXMESH_SYSTEMMEM,
				g_pd3dDevice, NULL,
				&pD3DXMtrlBuffer, NULL, &g_dwNumMaterials,
				&g_pMesh ) ) )
			{
				MessageBox( NULL, L"Could not find meshfile", L"MeshXToOgre", MB_OK );
				throw std::exception("D3DXLoadMeshFromX failed");
			}

			D3DXMATERIAL *material = (D3DXMATERIAL *)(pD3DXMtrlBuffer->GetBufferPointer());

			// Done with the material buffer
			pD3DXMtrlBuffer->Release();
		}
		catch (std::exception &e)
		{
			std::cerr << e.what() << std::endl;
			Cleanup();
			return false;
		}

		return g_pMesh;
	}

	void DxMesh::Cleanup()
	{
		if( g_pMeshMaterials != NULL )
			delete[] g_pMeshMaterials;

		if( g_pMeshTextures )
		{
			for( DWORD i = 0; i < g_dwNumMaterials; i++ )
			{
				if( g_pMeshTextures[i] )
					g_pMeshTextures[i]->Release();
			}
			delete[] g_pMeshTextures;
		}
		if( g_pMesh != NULL )
			g_pMesh->Release();

		if( g_pd3dDevice != NULL )
			g_pd3dDevice->Release();

		if( g_pD3D != NULL )
			g_pD3D->Release();
	}

	LRESULT WINAPI DxMesh::MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
	{
		return DefWindowProc( hWnd, msg, wParam, lParam );
	}
}