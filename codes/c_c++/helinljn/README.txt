Dependencies:
    1. openssl[static libs]
        vcpkg install openssl:x64-windows-static
        ./vcpkg install openssl:x64-linux

    2. asio with openssl

    3. fmt

    4. spdlog

    5. date

Change list:
    1. date/iso_week.h
        diff --git a/include/date/iso_week.h b/include/date/iso_week.h
        index 4a0a4a9..859bf1e 100644
        --- a/include/date/iso_week.h
        +++ b/include/date/iso_week.h
        @@ -451,7 +451,7 @@ weekday::weekday(unsigned wd) NOEXCEPT
         CONSTCD11
         inline
         weekday::weekday(date::weekday wd) NOEXCEPT
        -    : wd_(wd.iso_encoding())
        +    : wd_(static_cast<decltype(wd_)>(wd.iso_encoding()))
             {}