// Copyright 2014 Dolphin Emulator Project
// Licensed under GPLv2
// Refer to the license.txt file included.

#include <vector>

#include "Common/StringUtil.h"

#include "VideoCommon/VertexLoader.h"
#include "VideoCommon/VertexLoaderBase.h"

VertexLoaderBase::VertexLoaderBase(const TVtxDesc &vtx_desc, const VAT &vtx_attr)
{
	m_numLoadedVertices = 0;
	m_VertexSize = 0;
	m_native_vertex_format = nullptr;

	SetVAT(vtx_attr);
	m_VtxDesc = vtx_desc;
	m_vat = vtx_attr;
}

void VertexLoaderBase::SetVAT(const VAT& vat)
{
	m_VtxAttr.PosElements          = vat.g0.PosElements;
	m_VtxAttr.PosFormat            = vat.g0.PosFormat;
	m_VtxAttr.PosFrac              = vat.g0.PosFrac;
	m_VtxAttr.NormalElements       = vat.g0.NormalElements;
	m_VtxAttr.NormalFormat         = vat.g0.NormalFormat;
	m_VtxAttr.color[0].Elements    = vat.g0.Color0Elements;
	m_VtxAttr.color[0].Comp        = vat.g0.Color0Comp;
	m_VtxAttr.color[1].Elements    = vat.g0.Color1Elements;
	m_VtxAttr.color[1].Comp        = vat.g0.Color1Comp;
	m_VtxAttr.texCoord[0].Elements = vat.g0.Tex0CoordElements;
	m_VtxAttr.texCoord[0].Format   = vat.g0.Tex0CoordFormat;
	m_VtxAttr.texCoord[0].Frac     = vat.g0.Tex0Frac;
	m_VtxAttr.ByteDequant          = vat.g0.ByteDequant;
	m_VtxAttr.NormalIndex3         = vat.g0.NormalIndex3;

	m_VtxAttr.texCoord[1].Elements = vat.g1.Tex1CoordElements;
	m_VtxAttr.texCoord[1].Format   = vat.g1.Tex1CoordFormat;
	m_VtxAttr.texCoord[1].Frac     = vat.g1.Tex1Frac;
	m_VtxAttr.texCoord[2].Elements = vat.g1.Tex2CoordElements;
	m_VtxAttr.texCoord[2].Format   = vat.g1.Tex2CoordFormat;
	m_VtxAttr.texCoord[2].Frac     = vat.g1.Tex2Frac;
	m_VtxAttr.texCoord[3].Elements = vat.g1.Tex3CoordElements;
	m_VtxAttr.texCoord[3].Format   = vat.g1.Tex3CoordFormat;
	m_VtxAttr.texCoord[3].Frac     = vat.g1.Tex3Frac;
	m_VtxAttr.texCoord[4].Elements = vat.g1.Tex4CoordElements;
	m_VtxAttr.texCoord[4].Format   = vat.g1.Tex4CoordFormat;

	m_VtxAttr.texCoord[4].Frac     = vat.g2.Tex4Frac;
	m_VtxAttr.texCoord[5].Elements = vat.g2.Tex5CoordElements;
	m_VtxAttr.texCoord[5].Format   = vat.g2.Tex5CoordFormat;
	m_VtxAttr.texCoord[5].Frac     = vat.g2.Tex5Frac;
	m_VtxAttr.texCoord[6].Elements = vat.g2.Tex6CoordElements;
	m_VtxAttr.texCoord[6].Format   = vat.g2.Tex6CoordFormat;
	m_VtxAttr.texCoord[6].Frac     = vat.g2.Tex6Frac;
	m_VtxAttr.texCoord[7].Elements = vat.g2.Tex7CoordElements;
	m_VtxAttr.texCoord[7].Format   = vat.g2.Tex7CoordFormat;
	m_VtxAttr.texCoord[7].Frac     = vat.g2.Tex7Frac;

	if (!m_VtxAttr.ByteDequant)
	{
		ERROR_LOG(VIDEO, "ByteDequant is set to zero");
	}
};

void VertexLoaderBase::AppendToString(std::string *dest) const
{
	dest->reserve(250);

	dest->append(GetName());
	dest->append(": ");

	static const char *posMode[4] = {
		"Inv",
		"Dir",
		"I8",
		"I16",
	};
	static const char *posFormats[5] = {
		"u8", "s8", "u16", "s16", "flt",
	};
	static const char *colorFormat[8] = {
		"565",
		"888",
		"888x",
		"4444",
		"6666",
		"8888",
		"Inv",
		"Inv",
	};

	dest->append(StringFromFormat("%ib skin: %i P: %i %s-%s ",
		m_VertexSize, (u32)m_VtxDesc.PosMatIdx,
		m_VtxAttr.PosElements ? 3 : 2, posMode[m_VtxDesc.Position], posFormats[m_VtxAttr.PosFormat]));

	if (m_VtxDesc.Normal)
	{
		dest->append(StringFromFormat("Nrm: %i %s-%s ",
			m_VtxAttr.NormalElements, posMode[m_VtxDesc.Normal], posFormats[m_VtxAttr.NormalFormat]));
	}

	u64 color_mode[2] = {m_VtxDesc.Color0, m_VtxDesc.Color1};
	for (int i = 0; i < 2; i++)
	{
		if (color_mode[i])
		{
			dest->append(StringFromFormat("C%i: %i %s-%s ", i, m_VtxAttr.color[i].Elements, posMode[color_mode[i]], colorFormat[m_VtxAttr.color[i].Comp]));
		}
	}
	u64 tex_mode[8] = {
		m_VtxDesc.Tex0Coord, m_VtxDesc.Tex1Coord, m_VtxDesc.Tex2Coord, m_VtxDesc.Tex3Coord,
		m_VtxDesc.Tex4Coord, m_VtxDesc.Tex5Coord, m_VtxDesc.Tex6Coord, m_VtxDesc.Tex7Coord
	};
	for (int i = 0; i < 8; i++)
	{
		if (tex_mode[i])
		{
			dest->append(StringFromFormat("T%i: %i %s-%s ",
				i, m_VtxAttr.texCoord[i].Elements, posMode[tex_mode[i]], posFormats[m_VtxAttr.texCoord[i].Format]));
		}
	}
	dest->append(StringFromFormat(" - %i v\n", m_numLoadedVertices));
}

// a hacky implementation to compare two vertex loaders
class VertexLoaderTester : public VertexLoaderBase
{
public:
	VertexLoaderTester(VertexLoaderBase* _a, VertexLoaderBase* _b, const TVtxDesc& vtx_desc, const VAT& vtx_attr)
	: VertexLoaderBase(vtx_desc, vtx_attr)
	{
		a = _a;
		b = _b;
		m_initialized = a && b && a->IsInitialized() && b->IsInitialized();
		m_initialized = m_initialized && (a->m_VertexSize == b->m_VertexSize);
		m_initialized = m_initialized && (a->m_native_vtx_decl.stride == b->m_native_vtx_decl.stride);
	}
	~VertexLoaderTester()
	{
		delete a;
		delete b;
	}

	int RunVertices(int primitive, int count, DataReader src, DataReader dst) override
	{
		buffer_a.resize(count * a->m_native_vtx_decl.stride);
		buffer_b.resize(count * b->m_native_vtx_decl.stride);

		int count_a = a->RunVertices(primitive, count, src, DataReader(buffer_a.data(), buffer_a.data()+buffer_a.size()));
		int count_b = b->RunVertices(primitive, count, src, DataReader(buffer_b.data(), buffer_b.data()+buffer_b.size()));

		if (count_a != count_b)
			ERROR_LOG(VIDEO, "Both vertexloaders have loaded a different amount of vertices.");

		if (memcmp(buffer_a.data(), buffer_b.data(), std::min(count_a, count_b)))
			ERROR_LOG(VIDEO, "Both vertexloaders have loaded different data.");

		u8* dstptr;
		dst.WritePointer(&dstptr);
		memcpy(dstptr, buffer_a.data(), count_a);
		return count_a;
	}
	std::string GetName() const override { return "CompareLoader"; }
	bool IsInitialized() override { return m_initialized; }

private:
	VertexLoaderBase *a, *b;
	bool m_initialized;
	std::vector<u8> buffer_a, buffer_b;
};

VertexLoaderBase* VertexLoaderBase::CreateVertexLoader(const TVtxDesc& vtx_desc, const VAT& vtx_attr)
{
	VertexLoaderBase* loader;

#if 0
	// first try: Any new VertexLoader vs the old one
	loader = new VertexLoaderTester(
			new VertexLoader(vtx_desc, vtx_attr), // the software one
			new VertexLoader(vtx_desc, vtx_attr), // the new one to compare
			vtx_desc, vtx_attr);
	if (loader->IsInitialized())
		return loader;
	delete loader;
#endif

	// last try: The old VertexLoader
	loader = new VertexLoader(vtx_desc, vtx_attr);
	if (loader->IsInitialized())
		return loader;
	delete loader;

	PanicAlert("No Vertex Loader found.");
	return nullptr;
}