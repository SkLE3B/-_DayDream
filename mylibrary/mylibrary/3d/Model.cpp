#include "Model.h"

void Model::CreateBuffers(ID3D12Device* device)
{
	HRESULT result;

	UINT sizeVB =
		static_cast<UINT>(sizeof(VertexPosNormalUvSkin) *
			vertices.size());
		
	CD3DX12_HEAP_PROPERTIES heapPro = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC resource = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);

	result = device->CreateCommittedResource(
		&heapPro,
		D3D12_HEAP_FLAG_NONE,
		&resource,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	VertexPosNormalUvSkin* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result))
	{
		std::copy(vertices.begin(), vertices.end(), vertMap);
		vertBuff->Unmap(0, nullptr);
	}

	vbView.BufferLocation =
		vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(vertices[0]);

	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());
	CD3DX12_RESOURCE_DESC resourceIb = CD3DX12_RESOURCE_DESC::Buffer(sizeIB);

	result = device->CreateCommittedResource(
		&heapPro,
		D3D12_HEAP_FLAG_NONE,
		&resourceIb,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	if (SUCCEEDED(result))
	{
		std::copy(indices.begin(), indices.end(), indexMap);
		indexBuff->Unmap(0, nullptr);
	}

	ibView.BufferLocation = 
		indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

	const DirectX::Image* img = scratchImg.GetImage(0, 0, 0);

	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);

	CD3DX12_HEAP_PROPERTIES heapProCD3X12 = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,D3D12_MEMORY_POOL_L0);

	result = device->CreateCommittedResource(
		&heapProCD3X12,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff));

	result = texBuff->WriteToSubresource(
		0,
		nullptr,
		img->pixels,
		(UINT)img->rowPitch,
		(UINT)img->slicePitch
	);

	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = 1;
	result = device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeapSRV));

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	D3D12_RESOURCE_DESC resDesc = texBuff->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(texBuff.Get(),
		&srvDesc,
		descHeapSRV->GetCPUDescriptorHandleForHeapStart()
	);
}

void Model::Draw(ID3D12GraphicsCommandList* cmdList)
{
	// ???_?o?b?t?@???Z?b?g(VBV)
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	// ?C???f?b?N?X?o?b?t?@???Z?b?g(IBV)
	cmdList->IASetIndexBuffer(&ibView);
	// ?f?X?N???v?^?q?[?v???Z?b?g
	ID3D12DescriptorHeap* ppHeaps[] = { descHeapSRV.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	// ?V?F?[?_???\?[?X?r???[???Z?b?g
	cmdList->SetGraphicsRootDescriptorTable(1, descHeapSRV->GetGPUDescriptorHandleForHeapStart());
	// ?`???R?}???h
	cmdList->DrawIndexedInstanced((UINT)indices.size(), 1, 0, 0, 0);
}

Model::~Model()
{
	//FBX?V?[????????
	fbxScene->Destroy();
}
