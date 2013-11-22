#pragma once
#include <set>
#include <string>
#include <vector>
#include <sstream>
#include <string.h> // memcpy
#include "core/messages.h"

#ifdef ASTRON_32BIT_DATAGRAMS
	typedef uint32_t dgsize_t;
#else
	typedef uint16_t dgsize_t;
#endif

#define DGSIZE_MAX ((dgsize_t)(-1))

class DatagramOverflow : public std::runtime_error
{
	public:
		DatagramOverflow(const std::string &what) : std::runtime_error(what) { }
};

class Datagram
{
	protected:
		uint8_t* buf;
		dgsize_t buf_cap;
		dgsize_t buf_end;

		void check_add_length(dgsize_t len)
		{
			if(buf_end + len > DGSIZE_MAX)
			{
				std::stringstream err_str;
				err_str << "dg tried to add data past max datagram size, buf_end+len("
				    << buf_end + len << ")" << " max_size(" << DGSIZE_MAX << ")" << std::endl;
				throw DatagramOverflow(err_str.str());
			}

			if(buf_end + len > buf_cap)
			{
				uint8_t *tmp_buf = new uint8_t[buf_cap + len + 64];
				memcpy(tmp_buf, buf, buf_cap);
				delete [] buf;
				buf = tmp_buf;
				buf_cap = buf_cap + len + 64;
			}
		}
	public:
		Datagram() : buf(new uint8_t[64]), buf_cap(64), buf_end(0)
		{
		}

		Datagram(const Datagram &dg) : buf(new uint8_t[dg.size()]), buf_cap(dg.size()),
			buf_end(dg.size())
		{
			memcpy(buf, dg.buf, dg.size());
		}

		Datagram(const std::vector<uint8_t> &data) : buf(new uint8_t[data.size()]),
			buf_cap(data.size()), buf_end(data.size())
		{
			memcpy(buf, &data[0], data.size());
		}

		Datagram(const std::string &data) : buf(new uint8_t[data.length()]), buf_cap(data.length()),
			buf_end(data.length())
		{
			memcpy(buf, data.c_str(), data.length());
		}

		Datagram(const uint8_t *data, dgsize_t length) : buf(new uint8_t[length]), buf_cap(length),
			buf_end(length)
		{
			memcpy(buf, data, length);
		}

		Datagram(uint64_t to_channel, uint64_t from_channel, uint16_t message_type) :
			buf(new uint8_t[64]), buf_cap(64), buf_end(0)
		{
			add_server_header(to_channel, from_channel, message_type);
		}

		Datagram(const std::set<uint64_t> &to_channels, uint64_t from_channel,
		         uint16_t message_type) : buf(new uint8_t[64]), buf_cap(64), buf_end(0)
		{
			add_server_header(to_channels, from_channel, message_type);
		}

		Datagram(uint16_t message_type) : buf(new uint8_t[64]), buf_cap(64), buf_end(0)
		{
			add_control_header(message_type);
		}

		~Datagram()
		{
			delete [] buf;
		}

		void add_bool(const bool &v)
		{
			if(v) add_uint8(1);
			else add_uint8(0);
		}

		void add_uint8(const uint8_t &v)
		{
			check_add_length(1);
			memcpy(buf + buf_end, &v, 1);
			buf_end += 1;
		}

		void add_uint16(const uint16_t &v)
		{
			check_add_length(2);
			memcpy(buf + buf_end, &v, 2);
			buf_end += 2;
		}

		void add_uint32(const uint32_t &v)
		{
			check_add_length(4);
			memcpy(buf + buf_end, &v, 4);
			buf_end += 4;
		}

		void add_uint64(const uint64_t &v)
		{
			check_add_length(8);
			memcpy(buf + buf_end, &v, 8);
			buf_end += 8;
		}

		void add_float64(const double &v)
		{
			check_add_length(8);
			memcpy(buf + buf_end, &v, 8);
			buf_end += 8;
		}

		void add_float32(const float &v)
		{
			check_add_length(4);
			memcpy(buf + buf_end, &v, 4);
			buf_end += 4;
		}

		void add_size(const dgsize_t &v)
		{
			check_add_length(sizeof(dgsize_t));
			memcpy(buf + buf_end, &v, sizeof(dgsize_t));
			buf_end += sizeof(dgsize_t);
		}

		void add_channel(const channel_t &v)
		{
			check_add_length(sizeof(channel_t));
			memcpy(buf + buf_end, &v, sizeof(channel_t));
			buf_end += sizeof(channel_t);
		}

		void add_doid(const doid_t &v)
		{
			check_add_length(sizeof(doid_t));
			memcpy(buf + buf_end, &v, sizeof(doid_t));
			buf_end += sizeof(doid_t);
		}

		void add_zone(const zone_t &v)
		{
			check_add_length(sizeof(zone_t));
			memcpy(buf + buf_end, &v, sizeof(zone_t));
			buf_end += sizeof(zone_t);
		}

		void add_location(const doid_t &parent, const zone_t &zone)
		{
			check_add_length(sizeof(doid_t) + sizeof(zone_t));
			memcpy(buf + buf_end, &parent, sizeof(doid_t));
			buf_end += sizeof(doid_t);
			memcpy(buf + buf_end, &zone, sizeof(zone_t));
			buf_end += sizeof(zone_t);
		}

		void add_data(const std::vector<uint8_t> &data)
		{
			if(data.size())
			{
				check_add_length(data.size());
				memcpy(buf + buf_end, &data[0], data.size());
				buf_end += data.size();
			}
		}

		void add_data(const std::string &str)
		{
			check_add_length(str.length());
			memcpy(buf + buf_end, str.c_str(), str.length());
			buf_end += str.length();
		}

		void add_data(uint8_t* data, dgsize_t length) {
			check_add_length(length);
			memcpy(buf + buf_end, data, length);
			buf_end += length;
		}

		void add_datagram(const Datagram &dg)
		{
			check_add_length(dg.buf_end);
			memcpy(buf + buf_end, dg.buf, dg.buf_end);
			buf_end += dg.buf_end;
		}

		void add_string(const std::string &str)
		{
			add_size(str.length());
			check_add_length(str.length());
			memcpy(buf + buf_end, str.c_str(), str.length());
			buf_end += str.length();
		}

		void add_blob(const std::vector<uint8_t> &blob)
		{
			add_size(blob.size());
			check_add_length(blob.size());
			memcpy(buf + buf_end, &blob[0], blob.size());
			buf_end += blob.size();
		}

		void add_blob(uint8_t* data, uint16_t length) {
			add_size(length);
			check_add_length(length);
			memcpy(buf + buf_end, data, length);
			buf_end += length;
		}

		void add_server_header(channel_t to, channel_t from, uint16_t message_type)
		{
			add_uint8(1);
			add_channel(to);
			add_channel(from);
			add_uint16(message_type);
		}

		void add_server_header(const std::set<channel_t> &to, channel_t from, uint16_t message_type)
		{
			add_uint8(to.size());
			for(auto it = to.begin(); it != to.end(); ++it)
			{
				add_channel(*it);
			}
			add_channel(from);
			add_uint16(message_type);
		}

		void add_control_header(uint16_t message_type)
		{
			add_uint8(1);
			add_channel(CONTROL_MESSAGE);
			add_uint16(message_type);
		}

		const dgsize_t size() const
		{
			return buf_end;
		}

		const uint8_t* get_data() const
		{
			return buf;
		}
};
