// Filename: format.cpp
#include "value/format.h"
#include "DistributedType.h"
#include "ArrayType.h"
#include "Struct.h"
#include "Method.h"
#include "Field.h"
#include "Parameter.h"
#include <cctype>   // std::isprint()
#include <sstream>  // std::ostringstream
using namespace std;
namespace dclass   // open namespace dclass
{

// A Formatter steps through packed data and unpacks it as a .dc file parameter format.
//     This is created and called by format() to handle formatting.
struct Formatter
{
	const uint8_t* in;
	ostream &out;
	size_t offset;
	size_t end;

	Formatter(const vector<uint8_t> &in, ostream &out) :
		in(&in[0]), out(out), offset(0), end(in.size())
	{
	}

	Formatter(const string &in, ostream &out) :
		in((const uint8_t*)&in[0]), out(out), offset(0), end(in.size())
	{
	}

	Formatter(const uint8_t* buffer, size_t length, ostream &out) :
		in(buffer), out(out), offset(0), end(length)
	{
	}

	inline bool remaining(sizetag_t length)
	{
		return (offset + length) < end;
	}

	inline sizetag_t read_length()
	{
		sizetag_t v = *(sizetag_t*)(in+offset);
		offset += sizeof(sizetag_t);
		return v;
	}

	bool format(const DistributedType* dtype)
	{
		Type type = dtype->get_type();
		switch(type)
		{
			case INVALID:
			{
				out << "<invalid>";
				break;
			}
			case INT8:
			{
				if(!remaining(sizeof(int8_t)))
					return false;
				int v = *(int8_t*)(in+offset);
				offset += sizeof(int8_t);
				out << v;
				break;
			}
			case INT16:
			{
				if(!remaining(sizeof(int16_t)))
					return false;
				int v = *(int16_t*)(in+offset);
				offset += sizeof(int16_t);
				out << v;
				break;
			}
			case INT32:
			{
				if(!remaining(sizeof(int32_t)))
					return false;
				int v = *(int32_t*)(in+offset);
				offset += sizeof(int32_t);
				out << v;
				break;
			}
			case INT64:
			{
				if(!remaining(sizeof(int64_t)))
					return false;
				int v = *(int64_t*)(in+offset);
				offset += sizeof(int64_t);
				out << v;
				break;
			}
			case UINT8:
			{
				if(!remaining(sizeof(uint8_t)))
					return false;
				unsigned int v = *(uint8_t*)(in+offset);
				offset += sizeof(uint8_t);
				out << v;
				break;
			}
			case UINT16:
			{
				if(!remaining(sizeof(uint16_t)))
					return false;
				unsigned int v = *(uint16_t*)(in+offset);
				offset += sizeof(uint16_t);
				out << v;
				break;
			}
			case UINT32:
			{
				if(!remaining(sizeof(uint32_t)))
					return false;
				unsigned int v = *(uint32_t*)(in+offset);
				offset += sizeof(uint32_t);
				out << v;
				break;
			}
			case UINT64:
			{
				if(!remaining(sizeof(uint64_t)))
					return false;
				unsigned int v = *(uint64_t*)(in+offset);
				offset += sizeof(uint64_t);
				out << v;
				break;
			}
			case FLOAT32:
			{
				if(!remaining(sizeof(float)))
					return false;
				float v = *(float*)(in+offset);
				offset += sizeof(float);
				out << v;
				break;
			}
			case FLOAT64:
			{
				if(!remaining(sizeof(double)))
					return false;
				double v = *(double*)(in+offset);
				offset += sizeof(double);
				out << v;
				break;
			}
			case CHAR:
			{
				if(!remaining(sizeof(char)))
					return false;
				char v = *(char*)(in+offset);
				format_quoted('\'', string(1, v), out);
				break;
			}
			case STRING:
			{
				// Read string length
				sizetag_t length = dtype->get_size();

				// Read string
				if(!remaining(length))
					return false;
				string str((const char*)in + offset, length);
				offset += length;

				// Enquoute and escape string then output
				format_quoted('"', str, out);
				break;
			}
			case VARSTRING:
			{
				// Read string length
				if(!remaining(sizeof(sizetag_t)))
					return false;
				sizetag_t length = read_length();

				// Read string
				if(!remaining(length))
					return false;
				string str((const char*)in + offset, length);
				offset += length;

				// Enquoute and escape string then output
				format_quoted('"', str, out);
				break;
			}
			case BLOB:
			{
				// Read blob length
				sizetag_t length = dtype->get_size();

				// Read blob
				if(!remaining(length))
					return false;
				string blob((const char*)in + offset, length);
				offset += length;

				// Format blob as a hex constant then output
				format_hex(blob, out);
				break;
			}
			case VARBLOB:
			{
				// Read blob length
				if(!remaining(sizeof(sizetag_t)))
					return false;
				sizetag_t length = read_length();

				// Read blob
				if(!remaining(length))
					return false;
				string blob((const char*)in + offset, length);
				offset += length;

				// Format blob as a hex constant then output
				format_hex(blob, out);
				break;
			}
			case ARRAY:
			{
				out << '[';
				const ArrayType* arr = dtype->as_array();
				bool ok = format(arr->get_element_type());
				if(!ok) return false;
				for(int i = 1; i < arr->get_array_size(); ++i)
				{
					ok = format(arr->get_element_type());
					if(!ok) return false;
					out << ", ";
				}

				out << ']';
				break;
			}
			case VARARRAY:
			{
				out << '[';
				// Read array byte length
				if(!remaining(sizeof(sizetag_t)))
					return false;
				sizetag_t length = read_length();

				// Read array
				if(!remaining(length))
					return false;
				size_t array_end = offset + length;

				const ArrayType* arr = dtype->as_array();
				bool ok = format(arr->get_element_type());
				if(!ok) return false;
				while(offset < array_end)
				{
					ok = format(arr->get_element_type());
					if(!ok) return false;
					out << ", ";
				}

				// Check to make sure we didn't overshoot the array while reading
				if(offset > array_end)
				{
					return false;
				}

				out << ']';
				break;
			}
			case STRUCT:
			{
				out << '{';
				const Struct* strct = dtype->as_struct();
				size_t num_fields = strct->get_num_fields();
				if(num_fields > 0)
				{
					bool ok = format(strct->get_field(0)->get_type());
					if(!ok) return false;
					for(unsigned int i = 1; i < num_fields; ++i)
					{
						ok = format(strct->get_field(i)->get_type());
						if(!ok) return false;
						out << ", ";
					}
				}
				out << '}';
				break;
			}
			case METHOD:
			{
				out << '(';
				const Method* method = dtype->as_method();
				size_t num_params = method->get_num_parameters();
				if(num_params > 0)
				{
					bool ok = format(method->get_parameter(0)->get_type());
					if(!ok) return false;
					for(unsigned int i = 1; i < num_params; ++i)
					{
						ok = format(method->get_parameter(i)->get_type());
						if(!ok) return false;
						out << ", ";
					}
				}
					out << ')';
					break;
			}
			default:
			{
				out << "<error>";
				return false;
			}
		}
		return true;
	}
};

// format unpacks the packed data into a string formatted for a .dc file.
//     This is used to produce default values when outputting a distributed class to a file.
string format_value(const DistributedType *dtype, const vector<uint8_t> &packed)
{
	ostringstream ss;
	format_value(dtype, packed, ss);
	return ss.str();
}
string format_value(const DistributedType *dtype, const string &packed)
{
	ostringstream ss;
	format_value(dtype, packed, ss);
	return ss.str();
}
void format_value(const DistributedType *dtype, const vector<uint8_t> &packed, ostream &out)
{
	Formatter formatter(packed, out);
	formatter.format(dtype);
}
void format_value(const DistributedType *dtype, const string &packed, ostream &out)
{
	Formatter formatter(packed, out);
	formatter.format(dtype);
}

// format_hex outputs <str> to <out> as a hexidecimal constant enclosed in angle-brackets (<>).
void format_hex(const string &str, ostream &out)
{
	out << '<';
	for(auto it = str.begin(); it != str.end(); ++it)
	{
		char infer[10];
		sprintf(infer, "%02x", (unsigned char)(*it));
		out << infer;
	}
	out << '>';
}
string format_hex(const string &str)
{
	ostringstream ss;
	format_hex(str, ss);
	return ss.str();
}

// format_quoted outputs <str> to <out> quoted with the character <quote_mark>.
//     Any instances of backslash (\) or the quoute character in the string are escaped.
//     Non-printable characters are replaced with an escaped hexidecimal constant.
void format_quoted(char quote_mark, const string &str, ostream &out)
{
	out << quote_mark;
	for(auto it = str.begin(); it != str.end(); ++it)
	{
		char c = *it;
		if(c == quote_mark || c == '\\')
		{
			// escape the character
			out << '\\' << c;

		}
		else if(!isprint(c)) // character is not a printable ascii character
		{
			// print the character as an escaped hexidecimal character constant
			char infer[10];
			sprintf(infer, "%02x", (unsigned char)c);
			out << "\\x" << infer;
		}
		else
		{
			out << c;
		}
	}
	out << quote_mark;
}
string format_quoted(char quote_mark, const string &str)
{
	ostringstream ss;
	format_quoted(quote_mark, str, ss);
	return ss.str();
}


} // close namespace dclass