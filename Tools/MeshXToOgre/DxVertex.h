#pragma once

namespace MeshXToOgre
{
	class DxVertex
	{
	public:
		D3DXVECTOR3 position;
		D3DXVECTOR3 normal;
		D3DXVECTOR2 tex;
		static const unsigned long FVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_NORMAL;
	};
}