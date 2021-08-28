#include "pch.h"

void CriUtfFieldDesc::read(std::ifstream& stream)
{
	stream.read((char*)&header, sizeof(header));
	header.bswap();

	name = string_table + header.name_offset;

	if (header.type.isConstant)
	{
		switch (header.type.type)
		{
		case CriFieldType::kInt8:
		case CriFieldType::kUint8:
			stream.read((char*)&value_u8, sizeof(value_u8));
			break;
		case CriFieldType::kInt16:
		case CriFieldType::kUint16:
			stream.read((char*)&value_u16, sizeof(value_u16));
			break;
		case CriFieldType::kInt32:
		case CriFieldType::kUint32:
		case CriFieldType::kFloat:
			stream.read((char*)&value_u32, sizeof(value_u32));
			break;
		case CriFieldType::kInt64:
		case CriFieldType::kUint64:
		case CriFieldType::kDouble:
			stream.read((char*)&value_u64, sizeof(value_u64));
			break;

		case CriFieldType::kString:
			uint32_t string_offset;
			stream.read((char*)&string_offset, sizeof(string_offset));
			string_offset = _byteswap_ulong(string_offset);
			value_string = string_table + string_offset;
			break;
		}
	}
}

void CriUtfField::read(std::ifstream& stream)
{
	if (field_info->header.type.isConstant)
	{
		switch (field_info->header.type.type)
		{
		case CriFieldType::kInt8:
		case CriFieldType::kUint8:
			value_u8 = field_info->value_u8;
			break;
		case CriFieldType::kInt16:
		case CriFieldType::kUint16:
			value_u16 = field_info->value_u16;
			break;
		case CriFieldType::kInt32:
		case CriFieldType::kUint32:
		case CriFieldType::kFloat:
			value_u32 = field_info->value_u32;
			break;
		case CriFieldType::kInt64:
		case CriFieldType::kUint64:
		case CriFieldType::kDouble:
			value_u64 = field_info->value_u64;
			break;
		case CriFieldType::kString:
			value_string = field_info->value_string;
			break;
		}
	}
	else
	{
		switch (field_info->header.type.type)
		{
		case CriFieldType::kInt8:
		case CriFieldType::kUint8:
			stream.read((char*)&value_u8, sizeof(value_u8));
			break;
		case CriFieldType::kInt16:
		case CriFieldType::kUint16:
			stream.read((char*)&value_u16, sizeof(value_u16));
			break;
		case CriFieldType::kInt32:
		case CriFieldType::kUint32:
		case CriFieldType::kFloat:
			stream.read((char*)&value_u32, sizeof(value_u32));
			break;
		case CriFieldType::kInt64:
		case CriFieldType::kUint64:
		case CriFieldType::kDouble:
			stream.read((char*)&value_u64, sizeof(value_u64));
			break;
		case CriFieldType::kString:
			uint32_t string_offset;
			stream.read((char*)&string_offset, sizeof(string_offset));
			string_offset = _byteswap_ulong(string_offset);
			value_string = string_table + string_offset;
			break;
		}
	}

	value_u16 = _byteswap_ushort(value_u16);
	value_u32 = _byteswap_ulong(value_u32);
	value_u64 = _byteswap_uint64(value_u64);
}

void CriUtfRow::read(std::ifstream& stream)
{
	for (auto& field_info : *field_infos)
	{
		CriUtfField field(&field_info, string_table);
		field.read(stream);
		fields.push_back(field);
	}
}

bool CriUtfRow::get_u8(std::string_view field_name, uint8_t& result)
{
	for (auto field : fields)
	{
		if (!strcmp(field.name(), field_name.data()))
		{
			result = field.value_u8;
			return true;
		}
	}
	return false;
}

bool CriUtfRow::get_u16(std::string_view field_name, uint16_t& result)
{
	for (auto field : fields)
	{
		if (!strcmp(field.name(), field_name.data()))
		{
			result = field.value_u16;
			return true;
		}
	}
	return false;
}

bool CriUtfRow::get_u32(std::string_view field_name, uint32_t& result)
{
	for (auto field : fields)
	{
		if (!strcmp(field.name(), field_name.data()))
		{
			result = field.value_u32;
			return true;
		}
	}
	return false;
}

bool CriUtfRow::get_u64(std::string_view field_name, uint64_t& result)
{
	for (auto field : fields)
	{
		if (!strcmp(field.name(), field_name.data()))
		{
			result = field.value_u64;
			return true;
		}
	}
	return false;
}

bool CriUtfRow::get_string(std::string_view field_name, std::string& result)
{
	for (auto field : fields)
	{
		if (!strcmp(field.name(), field_name.data()))
		{
			result = field.value_string;
			return true;
		}
	}
	return false;
}

CriUtf::~CriUtf()
{
	if (string_table)
	{
		free(string_table);
		string_table = nullptr;
	}
}

void CriUtf::read(std::ifstream& stream)
{
	auto pos = stream.tellg();
	stream.read((char*)&header, sizeof(header));
	header.bswap();

	uint32_t strings_size = header.data_offset - header.strings_table_offset - 8;
	string_table = (char*)malloc(strings_size);

	for (int i = 0; i < header.num_columns; i++)
	{
		CriUtfFieldDesc info(string_table);
		info.read(stream);
		field_info.push_back(info);
	}

	for (int i = 0; i < header.num_rows; i++)
	{
		CriUtfRow row(&field_info, string_table);
		row.read(stream);
		rows.push_back(row);
	}

	auto strings_pos = pos;
	strings_pos += 8; // block id/size
	strings_pos += header.strings_table_offset;

	stream.seekg(strings_pos);
	stream.read(string_table, strings_size);
}

bool CriUtf::get_row(int row_idx, CriUtfRow** result)
{
	if (row_idx >= rows.size())
		return false;
	if (row_idx < 0)
		return false;

	*result = &rows[row_idx];
	return true;
}

bool CriUsm::read(std::ifstream& stream)
{
	bool found_crid = false;
	while (true)
	{
		auto pos = stream.tellg();
		CriUsmChunkHeader chunk_header;
		stream.read((char*)&chunk_header, sizeof(chunk_header));
		chunk_header.bswap();

		if (chunk_header.stmid == 0x43524944) // CRID
		{
			crid_header = chunk_header;
			// crid_utf.read(stream); - skip crid UTF reading, don't really care about that right now
			found_crid = true;
		}
		else
		{
			if (!found_crid)
			{
				// not a CRID header, and we haven't found one already... invalid file!
				return false;
			}

			if (chunk_header.stmid == 0x40534656) // @SFV
			{
				sfv_header = chunk_header;
				sfv_utf.read(stream);

				break; // only need to read one of these for our needs
			}
		}

		pos += 8; // the chunk headers header
		pos += chunk_header.block_size;

		stream.seekg(pos);
	}
}

bool CriUsm::get_width(uint32_t& result, bool disp_width, int row_idx)
{
	CriUtfRow* row;
	bool res = sfv_utf.get_row(row_idx, &row);
	if (!res)
		return res;
	return row->get_u32(disp_width ? "disp_width" : "width", result);
}

bool CriUsm::get_height(uint32_t& result, bool disp_height, int row_idx)
{
	CriUtfRow* row;
	bool res = sfv_utf.get_row(row_idx, &row);
	if (!res)
		return res;
	return row->get_u32(disp_height ? "disp_height" : "height", result);
}
