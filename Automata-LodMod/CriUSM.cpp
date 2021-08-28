#include "pch.h"

bool CriUtfFieldDesc::read(std::ifstream& stream)
{
	stream.read((char*)&header, sizeof(header));
	header.bswap();

	if (header.name_offset >= string_table_size)
		return false; // invalid name offset

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
			if (string_offset >= string_table_size)
				return false;

			value_string = string_table + string_offset;
			break;
		}
	}

	return !stream.fail();
}

bool CriUtfField::read(std::ifstream& stream)
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
			if (string_offset >= string_table_size)
				return false;

			value_string = string_table + string_offset;
			break;
		}
	}

	value_u16 = _byteswap_ushort(value_u16);
	value_u32 = _byteswap_ulong(value_u32);
	value_u64 = _byteswap_uint64(value_u64);

	return !stream.fail();
}

bool CriUtfRow::read(std::ifstream& stream)
{
	for (auto& field_info : *field_infos)
	{
		CriUtfField field(&field_info, string_table, string_table_size);
		if (!field.read(stream))
			return false;

		fields.push_back(field);
	}

	return true;
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

bool CriUtf::read(std::ifstream& stream, const std::streampos& block_end)
{
	auto pos = stream.tellg();
	stream.read((char*)&header, sizeof(header));
	if (stream.fail())
		return false;

	header.bswap();

	if (header.magic != 0x40555446)
		return false;

	auto end_pos = pos;
	end_pos += 8;
	end_pos += header.table_size;

	uint32_t strings_size = header.data_offset - header.strings_table_offset - 8;
	string_table = (char*)malloc(strings_size);

	auto strings_pos = pos;
	strings_pos += 8; // block id/size
	strings_pos += header.strings_table_offset;

	if (strings_pos >= end_pos || stream.tellg() >= block_end)
		return false;

	for (int i = 0; i < header.num_columns; i++)
	{
		if (stream.tellg() >= end_pos || stream.tellg() >= block_end)
			return false; // too many columns

		CriUtfFieldDesc info(string_table, strings_size);
		if (!info.read(stream))
			return false;

		field_info.push_back(info);
	}

	for (uint32_t i = 0; i < header.num_rows; i++)
	{
		if (stream.tellg() >= end_pos || stream.tellg() >= block_end)
			return false; // too many rows

		CriUtfRow row(&field_info, string_table, strings_size);
		if (!row.read(stream))
			return false;

		rows.push_back(row);
	}

	stream.seekg(strings_pos);
	stream.read(string_table, strings_size);

	return !stream.fail();
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
	while (!stream.eof())
	{
		auto pos = stream.tellg();

		CriUsmChunkHeader chunk_header;
		stream.read((char*)&chunk_header, sizeof(chunk_header));
		if (stream.fail())
			return false;

		chunk_header.bswap();

		auto end_pos = pos;
		end_pos += 8;
		end_pos += chunk_header.block_size;

		if (chunk_header.stmid == 0x43524944) // CRID
		{
			crid_header = chunk_header;
			// if (!crid_utf.read(stream, end_pos)) - skip crid UTF reading, don't really care about that right now
			//	return false;
			found_crid = true;
		}
		else
		{
			if (!found_crid)
			{
				// not a CRID header, and we haven't found one already... invalid file!
				return false;
			}

			if (chunk_header.stmid == 0x40534656 && chunk_header.block_type == 1) // @SFV
			{
				sfv_header = chunk_header;
				if (!sfv_utf.read(stream, end_pos))
					return false;

				break; // only need to read one of these for our needs
			}
		}

		pos += 8; // the chunk headers header
		pos += chunk_header.block_size;

		stream.seekg(pos);

		if (stream.fail())
			return false;
	}

	return true;
}

bool CriUsm::get_width(uint32_t& result, bool disp_width, int row_idx)
{
	CriUtfRow* row;
	if (!sfv_utf.get_row(row_idx, &row))
		return false;
	return row->get_u32(disp_width ? "disp_width" : "width", result);
}

bool CriUsm::get_height(uint32_t& result, bool disp_height, int row_idx)
{
	CriUtfRow* row;
	if (!sfv_utf.get_row(row_idx, &row))
		return false;
	return row->get_u32(disp_height ? "disp_height" : "height", result);
}
