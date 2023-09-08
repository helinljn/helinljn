#include "util/Base32.h"
#include "Poco/Base32Encoder.h"
#include "Poco/Base32Decoder.h"

#include <array>
#include <sstream>

namespace common {

std::string base32_encode(const std::string_view data)
{
    std::string         ret;
    std::ostringstream  oss;
    Poco::Base32Encoder encoder(oss);

    encoder.write(data.data(), data.size());
    encoder.close();

    if (oss.good() && encoder.good())
        ret = oss.str();

    return ret;
}

std::string base32_decode(const std::string_view data)
{
    std::string         ret;
    std::istringstream  iss(data.data());
    Poco::Base32Decoder decoder(iss);

    std::array<char, 256> buf;
    while (decoder.good())
    {
        decoder.read(buf.data(), buf.size());
        if (decoder.gcount() > 0)
            ret.append(buf.data(), decoder.gcount());
    }

    if (!iss.good() || !decoder.eof())
        ret.clear();

    return ret;
}

} // namespace common