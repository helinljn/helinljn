#include "util/Base64.h"
#include "Poco/Base64Encoder.h"
#include "Poco/Base64Decoder.h"

#include <array>
#include <sstream>

namespace common {

std::string base64_encode(const std::string_view& data)
{
    std::string         ret;
    std::ostringstream  oss;
    Poco::Base64Encoder encoder(oss);

    encoder.write(data.data(), data.size());
    encoder.close();

    if (oss.good() && encoder.good())
        ret = oss.str();

    return ret;
}

std::string base64_decode(const std::string_view& data)
{
    std::string         ret;
    std::istringstream  iss(data.data());
    Poco::Base64Decoder decoder(iss);

    std::array<char, 1024> buf;
    ret.reserve(data.size());
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