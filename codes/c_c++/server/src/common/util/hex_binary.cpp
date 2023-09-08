#include "util/hex_binary.h"
#include "Poco/HexBinaryEncoder.h"
#include "Poco/HexBinaryDecoder.h"

#include <array>
#include <sstream>
#include <cstring>

namespace common {

bool to_hex_string(const void* mem, const size_t memlen, std::string& outstr, const bool uppercase)
{
    std::ostringstream     oss;
    Poco::HexBinaryEncoder encoder(oss);
    const std::string_view text(reinterpret_cast<const char*>(mem), memlen);

    encoder.rdbuf()->setLineLength(0);
    encoder.rdbuf()->setUppercase(uppercase);
    encoder.write(text.data(), text.size());
    encoder.close();

    if (oss.good() && encoder.good() && oss.tellp() == static_cast<std::streamoff>(memlen * 2))
    {
        outstr = oss.str();
        return true;
    }

    return false;
}

bool from_hex_string(const std::string_view hexstr, void* outbuf, size_t outbuflen)
{
    if (hexstr.size() / 2 != outbuflen)
        return false;

    std::istringstream     iss(hexstr.data());
    Poco::HexBinaryDecoder decoder(iss);

    std::streamsize      count = 0;
    std::array<char, 64> buf;
    while (decoder.good())
    {
        decoder.read(buf.data(), buf.size());
        if (decoder.gcount() > 0)
        {
            memcpy(reinterpret_cast<char*>(outbuf) + count, buf.data(), decoder.gcount());
            count += decoder.gcount();
        }
    }

    if (iss.good() && decoder.eof() && hexstr.size() / 2 == static_cast<size_t>(count))
        return true;

    return false;
}

} // namespace common