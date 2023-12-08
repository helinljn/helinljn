#ifndef __FILE_CHANNEL_H__
#define __FILE_CHANNEL_H__

#include "util/types.h"
#include "Poco/Mutex.h"
#include "Poco/Channel.h"
#include "Poco/LogFile.h"
#include "Poco/RotateStrategy.h"

namespace common {

////////////////////////////////////////////////////////////////
// 文件写入Channel，支持按文件大小和时间(本地时间)进行归档
////////////////////////////////////////////////////////////////
class COMMON_API file_channel final : public Poco::Channel
{
public:
    file_channel(void);
    file_channel(std::string path);

    file_channel(const file_channel&) = delete;
    file_channel& operator=(const file_channel&) = delete;

    file_channel(file_channel&&) = delete;
    file_channel& operator=(file_channel&&) = delete;

    ////////////////////////////////////////////////////////////////
    // @brief 打开文件写入Channel
    //
    // @param
    // @return
    ////////////////////////////////////////////////////////////////
    void open(void) override;

    ////////////////////////////////////////////////////////////////
    // @brief 关闭文件写入Channel
    //
    // @param
    // @return
    ////////////////////////////////////////////////////////////////
    void close(void) override;

    ////////////////////////////////////////////////////////////////
    // @brief 将日志信息写入至文件
    //
    // @param msg 日志信息
    // @return
    ////////////////////////////////////////////////////////////////
    void log(const Poco::Message& msg) override;

    ////////////////////////////////////////////////////////////////
    // @brief 设置属性，支持以下属性：
    //        * path:          日志文件名(可包含相对或绝对路径)，最后会被
    //                         拼接为：xxxx-20200202[-20].log[.0 ~ n]
    //        * rotation_size: 按大小滚动
    //            * <n> K:  当文件的大小超过<n> KB时，将滚动文件
    //            * <n> M:  当文件的大小超过<n> MB时，将滚动文件
    //        * rotation_date: 按时间滚动
    //            * daily:  每天滚动一次
    //            * hourly: 每小时滚动一次
    //        * flush: 是否将日志信息实时写入至文件
    //            * true:   立刻将日志信息写入至文件(保证不丢失，但影响性能)
    //            * false:  将日志信息写入文件缓存
    //
    // @param name  属性名
    // @param value 属性值
    // @return
    ////////////////////////////////////////////////////////////////
    void setProperty(const std::string& name, const std::string& value) override;

    ////////////////////////////////////////////////////////////////
    // @brief 获取属性值
    //
    // @param name 属性名
    // @return
    ////////////////////////////////////////////////////////////////
    std::string getProperty(const std::string& name) const override;

private:
    ////////////////////////////////////////////////////////////////
    // @brief 获取归档的文件名，最终会被拼接为：xxxx-20200202[-20].log
    //        输入1：get_archive_name("/home/user/test.log")
    //        输出1：/home/user/test-20200202[-20].log
    //        输入2：get_archive_name("./logs/server.log")
    //        输出2：./logs/server-20200202[-20].log
    //
    // @param file_path 可包含相对路径或绝对路径的文件名
    // @return
    ////////////////////////////////////////////////////////////////
    std::string get_archive_name(const std::string& file_path) const;

    ////////////////////////////////////////////////////////////////
    // @brief 为归档的文件编号，会被拼接为：xxxx-20200202[-20].log[.0 ~ n]
    //
    // @param path 文件名
    // @return
    ////////////////////////////////////////////////////////////////
    std::string archive_by_number(const std::string& path) const;

private:
    std::string                            _path;
    std::string                            _rotation_size;
    std::string                            _rotation_date;
    std::string                            _flush;
    std::unique_ptr<Poco::LogFile>         _file;
    std::unique_ptr<Poco::RotateStrategy>  _rotate_by_size_strategy;
    std::unique_ptr<Poco::RotateStrategy>  _rotate_by_date_strategy;
    mutable Poco::FastMutex                _mutex;
};

} // namespace common

#endif // __FILE_CHANNEL_H__