#include "pch.h"

bool CriUtfFieldDesc::read(std::ifstream& stream)
{
	stream_read(stream, &header);
	header.bswap();

	if (header.type.isConstant)
	{
		switch (header.type.type)
		{
		case CriFieldType::kInt8:
		case CriFieldType::kUint8:
			stream_read(stream, &constant_u8);
			break;
		case CriFieldType::kInt16:
		case CriFieldType::kUint16:
			stream_read(stream, &constant_u16);
			break;
		case CriFieldType::kInt32:
		case CriFieldType::kUint32:
		case CriFieldType::kFloat:
			stream_read(stream, &constant_u32);
			break;
		case CriFieldType::kInt64:
		case CriFieldType::kUint64:
		case CriFieldType::kDouble:
			stream_read(stream, &constant_u64);
			break;
		case CriFieldType::kString:
			stream_read(stream, &constant_u32);
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
			value_u8 = field_info->constant_u8;
			break;
		case CriFieldType::kInt16:
		case CriFieldType::kUint16:
			value_u16 = field_info->constant_u16;
			break;
		case CriFieldType::kInt32:
		case CriFieldType::kUint32:
		case CriFieldType::kFloat:
			value_u32 = field_info->constant_u32;
			break;
		case CriFieldType::kInt64:
		case CriFieldType::kUint64:
		case CriFieldType::kDouble:
			value_u64 = field_info->constant_u64;
			break;
		case CriFieldType::kString:
			value_u32 = field_info->constant_u32;
			break;
		}
	}
	else
	{
		switch (field_info->header.type.type)
		{
		case CriFieldType::kInt8:
		case CriFieldType::kUint8:
			stream_read(stream, &value_u8);
			break;
		case CriFieldType::kInt16:
		case CriFieldType::kUint16:
			stream_read(stream, &value_u16);
			break;
		case CriFieldType::kInt32:
		case CriFieldType::kUint32:
		case CriFieldType::kFloat:
			stream_read(stream, &value_u32);
			break;
		case CriFieldType::kInt64:
		case CriFieldType::kUint64:
		case CriFieldType::kDouble:
			stream_read(stream, &value_u64);
			break;
		case CriFieldType::kString:
			stream_read(stream, &value_u32);
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
		CriUtfField field(&field_info);
		if (!field.read(stream))
			return false;

		fields.push_back(field);
	}

	return true;
}

bool CriUtfRow::read_strings(char* string_table, uint32_t string_table_size)
{
	for (auto& field : fields)
	{
		if (field.type() != CriFieldType::kString)
			continue;

		if (field.value_u32 >= string_table_size)
			return false;

		field.value_string = &string_table[field.value_u32];
	}

	return true;
}

bool CriUtfRow::get_u8(std::string_view field_name, uint8_t& result)
{
	for (auto& field : fields)
	{
		if (field.name() == field_name)
		{
			result = field.value_u8;
			return true;
		}
	}
	return false;
}

bool CriUtfRow::get_u16(std::string_view field_name, uint16_t& result)
{
	for (auto& field : fields)
	{
		if (field.name() == field_name)
		{
			result = field.value_u16;
			return true;
		}
	}
	return false;
}

bool CriUtfRow::get_u32(std::string_view field_name, uint32_t& result)
{
	for (auto& field : fields)
	{
		if (field.name() == field_name)
		{
			result = field.value_u32;
			return true;
		}
	}
	return false;
}

bool CriUtfRow::get_u64(std::string_view field_name, uint64_t& result)
{
	for (auto& field : fields)
	{
		if (field.name() == field_name)
		{
			result = field.value_u64;
			return true;
		}
	}
	return false;
}

bool CriUtfRow::get_string(std::string_view field_name, std::string& result)
{
	for (auto& field : fields)
	{
		if (field.name() == field_name)
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
	stream_read(stream, &header);
	if (stream.fail())
		return false;

	header.bswap();

	if (header.magic != MAGIC_UTF)
		return false;

	auto end_pos = pos;
	end_pos += 8; // block id/size
	end_pos += header.table_size;

	auto strings_pos = pos;
	strings_pos += 8;
	strings_pos += header.strings_table_offset;

	auto rows_pos = pos;
	rows_pos += 8;
	rows_pos += header.rows_offset;

	if (strings_pos >= end_pos || stream.tellg() >= block_end)
		return false;

	for (int i = 0; i < header.num_columns; i++)
	{
		if (stream.tellg() >= end_pos || stream.tellg() >= block_end)
			return false; // too many columns

		CriUtfFieldDesc info;
		if (!info.read(stream))
			return false;

		field_info.push_back(info);
	}

	stream.seekg(rows_pos);
	for (uint32_t i = 0; i < header.num_rows; i++)
	{
		if (stream.tellg() >= end_pos || stream.tellg() >= block_end)
			return false; // too many rows

		CriUtfRow row(&field_info);
		if (!row.read(stream))
			return false;

		rows.push_back(row);
	}

	// Populate strings...
	uint32_t strings_size = header.data_offset - header.strings_table_offset;
	string_table = (char*)malloc(strings_size);

	stream.seekg(strings_pos);
	stream_read(stream, string_table, strings_size);

	for (auto& field : field_info)
	{
		if (field.header.name_offset >= strings_size)
			return false;

		field.name = &string_table[field.header.name_offset];
	}

	for (auto& row : rows)
		if (!row.read_strings(string_table, strings_size))
			return false;

	return !stream.fail();
}

bool CriUtf::get_row(int row_idx, CriUtfRow** result)
{
	if (row_idx >= rows.size() || row_idx < 0)
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
		stream_read(stream, &chunk_header);
		if (stream.fail())
			return false;

		chunk_header.bswap();

		auto end_pos = pos;
		end_pos += 8;
		end_pos += chunk_header.block_size;

		if (chunk_header.stmid == MAGIC_CRID)
		{
			crid_header = chunk_header;
			if (!crid_utf.read(stream, end_pos))
				return false;
			found_crid = true;
		}
		else
		{
			if (!found_crid)
			{
				// not a CRID header, and we haven't found one already... invalid file!
				return false;
			}

			if (chunk_header.stmid == MAGIC_SFV && chunk_header.block_type == 1) // @SFV
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

bool CriUsm::get_u32(std::string_view name, uint32_t& result, int row_idx)
{
	CriUtfRow* row;
	if (!sfv_utf.get_row(row_idx, &row))
		return false;
	return row->get_u32(name, result);
}
