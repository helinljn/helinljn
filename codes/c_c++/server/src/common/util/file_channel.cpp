#include "file_channel.h"
#include "datetime.h"
#include "numeric_cast.hpp"
#include "Poco/Path.h"
#include "Poco/File.h"
#include "Poco/String.h"
#include "Poco/Message.h"
#include "Poco/NumberFormatter.h"

namespace Poco {

class RotateByDateStrategy : public RotateStrategy
{
public:
    RotateByDateStrategy(const std::string& rtime)
        : RotateStrategy()
        , _threshold()
        , _hour(-1)
        , _minute(0)
    {
        if (Poco::icompare(rtime, "daily") != 0 && Poco::icompare(rtime, "hourly") != 0)
            throw InvalidArgumentException("Invalid rotation time specified.");

        if (Poco::icompare(rtime, "daily") == 0)
            _hour = 0;

        // 先将阈值时间修正为下一个整点小时
        const int oldday = _threshold.day();
        _threshold += Timespan(0, 1, 0, 0, 0);
        _threshold.assign(_threshold.year(), _threshold.month(), _threshold.day(), _threshold.hour());

        // 然后将阈值时间修正为下一天的00:00
        if (_hour == 0)
        {
            // 修正后的时间如果没有跨天，则直接加一天
            if (_threshold.day() == oldday)
                _threshold += Timespan(1, 0, 0, 0, 0);

            // 强制保证阈值时间为下一天的00:00
            _threshold.assign(_threshold.year(), _threshold.month(), _threshold.day());
        }
    }

    bool mustRotate(LogFile* /*pFile*/) override
    {
        if (common::datetime() >= _threshold)
        {
            do
            {
                // 每次累加一小时
                _threshold += Timespan(0, 1, 0, 0, 0);
            } while (!(_hour != 0 || _hour == _threshold.hour()));

            _threshold.assign(_threshold.year(), _threshold.month(), _threshold.day(), _threshold.hour());

            return true;
        }

        return false;
    }

private:
    common::datetime _threshold;
    int              _hour;
    int              _minute;
};

} // namespace Poco

namespace common {

file_channel::file_channel(void)
    : file_channel(std::string("unnamed"))
{
}

file_channel::file_channel(std::string path)
    : _path(std::move(path))
    , _rotation_size()
    , _rotation_date()
    , _flush()
    , _file(nullptr)
    , _rotate_by_size_strategy(nullptr)
    , _rotate_by_date_strategy(nullptr)
    , _mutex()
{
}

void file_channel::open(void)
{
    Poco::FastMutex::ScopedLock lock(_mutex);
    if (!_file)
    {
        const auto& path = get_archive_name(_path);
        Poco::Path  p    = path;
        if (p.depth() > 0)
        {
            p.setFileName("");
            p.makeDirectory();

            Poco::File f(p);
            f.createDirectories();
        }

        _file = std::make_unique<Poco::LogFile>(path);
    }
}

void file_channel::close(void)
{
    Poco::FastMutex::ScopedLock lock(_mutex);
    _file.reset();
}

void file_channel::log(const Poco::Message& msg)
{
    // 保证文件已创建
    open();

    // 保证已正确设置所有属性
    poco_assert(!_path.empty());
    poco_assert(!_rotation_size.empty());
    poco_assert(!_rotation_date.empty());
    poco_assert(!_flush.empty());
    poco_assert(_file);
    poco_assert(_rotate_by_size_strategy);
    poco_assert(_rotate_by_date_strategy);

    Poco::FastMutex::ScopedLock lock(_mutex);

    if (_rotate_by_date_strategy->mustRotate(_file.get()) || _rotate_by_size_strategy->mustRotate(_file.get()))
    {
        std::string path = get_archive_name(_path);
        if (Poco::File f(path); f.exists())
            path = archive_by_number(path);

        _file = std::make_unique<Poco::LogFile>(path);
    }

    _file->write(msg.getText(), _flush.front() == 't');
}

void file_channel::setProperty(const std::string& name, const std::string& value)
{
    Poco::FastMutex::ScopedLock lock(_mutex);

    if (name == "path")
    {
        _path = value;
    }
    else if (name == "rotation_size")
    {
        Poco::StringTokenizer str(value, " ", Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        if (str.count() != 2)
            throw Poco::InvalidArgumentException("value", value);

        const auto rsize = from_string<uint64>(str[0]);
        if (rsize == 0)
            throw Poco::InvalidArgumentException("value", value);

        const auto& unit = str[1];
        if (Poco::icompare(unit, "k") != 0 && Poco::icompare(unit, "m") != 0)
            throw Poco::InvalidArgumentException("value", value);

        _rotate_by_size_strategy = std::make_unique<Poco::RotateBySizeStrategy>(Poco::icompare(unit, "k") == 0 ? rsize * 1024 : rsize * 1024 * 1024);
        _rotation_size           = value;
    }
    else if (name == "rotation_date")
    {
        _rotate_by_date_strategy = std::make_unique<Poco::RotateByDateStrategy>(value);
        _rotation_date           = value;
    }
    else if (name == "flush")
    {
        _flush = Poco::icompare(value, "true") == 0 ? "true" : "false";
    }
    else
    {
        Channel::setProperty(name, value);
    }
}

std::string file_channel::getProperty(const std::string& name) const
{
    Poco::FastMutex::ScopedLock lock(_mutex);

    if (name == "path")
        return _path;
    else if (name == "rotation_size")
        return _rotation_size;
    else if (name == "rotation_date")
        return _rotation_date;
    else if (name == "flush")
        return _flush;
    else
        return Channel::getProperty(name);
}

std::string file_channel::get_archive_name(const std::string& file_path) const
{
    std::string name;
    Poco::Path  path(file_path);

    name  = path.getBaseName();
    name += '-';

    Poco::toLowerInPlace(name);
    if (Poco::icompare(_rotation_date, "daily") == 0)
        Poco::DateTimeFormatter::append(name, common::datetime().utc_local(), "%Y%m%d.log");
    else
        Poco::DateTimeFormatter::append(name, common::datetime().utc_local(), "%Y%m%d-%H.log");

    path.setFileName(name);
    name.assign(path.toString());

    return name;
}

std::string file_channel::archive_by_number(const std::string& path) const
{
    std::string ret;
    int64       idx = -1;

    do
    {
        ret.assign(path);
        ret.append(".");
        Poco::NumberFormatter::append(ret, ++idx);

        if (Poco::File f(ret); f.exists())
            continue;

        break;
    } while (true);

    return ret;
}

} // namespace common