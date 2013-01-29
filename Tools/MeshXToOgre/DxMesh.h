#pragma once

namespace MeshXToOgre
{
	class DxMesh
	{
	public:
		DxMesh(const std::wstring &mesh);
		~DxMesh();

		const LPD3DXMESH GetMesh();

	private:
		void Cleanup();
		bool IsLoaded() { return g_pMesh != NULL; }

		static LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

		std::wstring m_mesh;
		bool m_loaded;

		LPDIRECT3D9         g_pD3D; // Used to create the D3DDevice
		LPDIRECT3DDEVICE9   g_pd3dDevice; // Our rendering device
		LPD3DXMESH          g_pMesh; // Our mesh object in sysmem
		D3DMATERIAL9*       g_pMeshMaterials; // Materials for our mesh
		LPDIRECT3DTEXTURE9* g_pMeshTextures; // Textures for our mesh
		DWORD               g_dwNumMaterials;   // Number of mesh materials
	};
}