#pragma once

// Simple USM parser so we can get at the movie width/height
// UTF parsing is incomplete, needs GUID & variable-length data support
// Will likely only work well with files where 2nd chunk is a block_type 1 @SFV part
#pragma pack(push, 1)

#define MAGIC_CRID 0x43524944
#define MAGIC_SFV  0x40534656 // @SFV
#define MAGIC_UTF  0x40555446 // @UTF

enum class CriManaCodec : uint32_t
{
	Unknown = 0,
	MPEG2 = 1,
	H264 = 5,
	VP9 = 9
};

struct CriUtfHeader
{
	/* 0x00 */ uint32_t magic;
	/* 0x04 */ uint32_t table_size;

	/* 0x08 */ uint8_t unk8;
	/* 0x09 */ uint8_t unk9;

	/* 0x0A */ uint16_t rows_offset;
	/* 0x0C */ uint32_t strings_table_offset;
	/* 0x10 */ uint32_t data_offset;
	/* 0x14 */ uint32_t table_name_offset;

	/* 0x18 */ uint16_t num_columns;
	/* 0x1A */ uint16_t row_size;
	/* 0x1C */ uint32_t num_rows;

	void bswap()
	{
		magic = _byteswap_ulong(magic);
		table_size = _byteswap_ulong(table_size);

		rows_offset = _byteswap_ushort(rows_offset);
		strings_table_offset = _byteswap_ulong(strings_table_offset);
		data_offset = _byteswap_ulong(data_offset);
		table_name_offset = _byteswap_ulong(table_name_offset);

		num_columns = _byteswap_ushort(num_columns);
		row_size = _byteswap_ushort(row_size);
		num_rows = _byteswap_ulong(num_rows);
	}
};

enum class CriFieldType : uint8_t
{
	kUint8 = 0,
	kInt8 = 1,
	kUint16 = 2,
	kInt16 = 3,
	kUint32 = 4,
	kInt32 = 5,
	kUint64 = 6,
	kInt64 = 7,
	kFloat = 8,
	kDouble = 9,
	kString = 0xa,

	// todo:
	kData = 0xb,
	kGuid = 0xc, // how large?
};

struct CriType
{
	CriFieldType type : 4;

	uint8_t isZero : 1;
	uint8_t isConstant : 1;
	uint8_t IsPerRow : 1;
	uint8_t uint80 : 1;
};

struct CriUtfFieldDescHdr
{
	CriType type;
	uint32_t name_offset;

	void bswap()
	{
		name_offset = _byteswap_ulong(name_offset);
	}
};

class CriUtfFieldDesc
{
public:
	CriUtfFieldDescHdr header;
	std::string name;

	uint8_t constant_u8;
	uint16_t constant_u16;
	uint32_t constant_u32;
	uint64_t constant_u64;

	bool read(std::ifstream& stream);
};

class CriUtfField
{
	CriUtfFieldDesc* field_info;

public:
	uint8_t value_u8 = 0;
	uint16_t value_u16 = 0;
	uint32_t value_u32 = 0;
	uint64_t value_u64 = 0;

	std::string value_string;

	CriFieldType type()
	{
		if (field_info)
			return field_info->header.type.type;
		return CriFieldType::kUint8;
	}

	std::string name()
	{
		if (field_info)
			return field_info->name;
		return "";
	}

	CriUtfField(CriUtfFieldDesc* info) : field_info(info) {}

	bool read(std::ifstream& stream);
};

class CriUtfRow
{
	std::vector<CriUtfFieldDesc>* field_infos;

	std::vector<CriUtfField> fields;

public:
	CriUtfRow(std::vector<CriUtfFieldDesc>* info) : field_infos(info) {}

	bool read(std::ifstream& stream);
	bool read_strings(char* string_table, uint32_t string_table_size);

	bool get_u8(std::string_view field_name, uint8_t& result);
	bool get_u16(std::string_view field_name, uint16_t& result);
	bool get_u32(std::string_view field_name, uint32_t& result);
	bool get_u64(std::string_view field_name, uint64_t& result);
	bool get_string(std::string_view field_name, std::string& result);
};

class CriUtf
{
	CriUtfHeader header = { 0 };
	char* string_table = nullptr;

	std::vector<CriUtfFieldDesc> field_info;
	std::vector<CriUtfRow> rows;
public:
	~CriUtf();
	bool read(std::ifstream& stream, const std::streampos& block_end);

	bool get_row(int row_idx, CriUtfRow** result);
};

struct CriUsmChunkHeader
{
	/* 0x00 */ uint32_t stmid;
	/* 0x04 */ uint32_t block_size;
	/* 0x08 */ uint16_t header_size;
	/* 0x0A */ uint16_t footer_size;
	/* 0x0C */ uint8_t unkC;
	/* 0x0D */ uint8_t unkD;
	/* 0x0E */ uint8_t unkE;
	/* 0x0F */ uint8_t block_type;
	/* 0x10 */ uint32_t time;
	/* 0x14 */ uint32_t tunit;
	/* 0x18 */ uint32_t rsv;
	/* 0x1C */ uint32_t udata;

	void bswap()
	{
		stmid = _byteswap_ulong(stmid);
		block_size = _byteswap_ulong(block_size);
		header_size = _byteswap_ushort(header_size);
		footer_size = _byteswap_ushort(footer_size);
		time = _byteswap_ulong(time);
		tunit = _byteswap_ulong(tunit);
		rsv = _byteswap_ulong(rsv);
		udata = _byteswap_ulong(udata);
	}
};

class CriUsm
{
public:
	CriUsmChunkHeader crid_header = { 0 };
	CriUtf crid_utf;

	CriUsmChunkHeader sfv_header = { 0 };
	CriUtf sfv_utf;

	bool read(std::ifstream& stream);

	bool get_u32(std::string_view name, uint32_t& result, int row_idx = 0);
};
#pragma pack(pop)
