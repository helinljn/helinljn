#include "doctest/doctest.h"
#include "fmt/format.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <locale>

namespace fs = std::filesystem;

DOCTEST_TEST_SUITE("FileSystem")
{
    DOCTEST_TEST_CASE("FilesystemPath")
    {
        // 测试路径构造
        fs::path p1;
        DOCTEST_CHECK(p1.empty());

        fs::path p2("test");
        DOCTEST_CHECK(!p2.empty());

        fs::path p3("test", fs::path::format::native_format);
        DOCTEST_CHECK(!p3.empty());

        // 测试路径操作
        fs::path p4 = p2 / "subdir" / "file.txt";
        std::string p4_str = p4.string();
        bool p4_valid = (p4_str == "test/subdir/file.txt" || p4_str == "test\\subdir\\file.txt");
        DOCTEST_CHECK(p4_valid);

        // 测试路径组件
        std::string parent_str = p4.parent_path().string();
        bool parent_valid = (parent_str == "test/subdir" || parent_str == "test\\subdir");
        DOCTEST_CHECK(parent_valid);
        DOCTEST_CHECK(p4.filename().string() == "file.txt");
        DOCTEST_CHECK(p4.stem().string() == "file");
        DOCTEST_CHECK(p4.extension().string() == ".txt");

        // 测试路径查询
        DOCTEST_CHECK(p4.is_absolute() == false);
        DOCTEST_CHECK(p4.is_relative() == true);
    }

    DOCTEST_TEST_CASE("FilesystemStatus")
    {
        // 测试文件状态
        fs::path current_dir(".");
        DOCTEST_CHECK(fs::exists(current_dir));
        DOCTEST_CHECK(fs::is_directory(current_dir));

        // 测试不存在的文件
        fs::path non_existent("non_existent_file.txt");
        DOCTEST_CHECK(!fs::exists(non_existent));
    }

    DOCTEST_TEST_CASE("FilesystemOperations")
    {
        // 测试创建和删除目录
        fs::path test_dir("test_dir");
        if (fs::exists(test_dir))
        {
            fs::remove_all(test_dir);
        }

        DOCTEST_CHECK(fs::create_directory(test_dir));
        DOCTEST_CHECK(fs::exists(test_dir));
        DOCTEST_CHECK(fs::is_directory(test_dir));

        // 测试创建和删除文件
        fs::path test_file = test_dir / "test_file.txt";
        {
            std::ofstream f(test_file.string());
            f << "test content";
        }
        DOCTEST_CHECK(fs::exists(test_file));
        DOCTEST_CHECK(fs::is_regular_file(test_file));

        // 测试文件大小
        DOCTEST_CHECK(fs::file_size(test_file) > 0);

        // 测试重命名文件
        fs::path renamed_file = test_dir / "renamed_file.txt";
        fs::rename(test_file, renamed_file);
        DOCTEST_CHECK(!fs::exists(test_file));
        DOCTEST_CHECK(fs::exists(renamed_file));

        // 测试删除文件
        fs::remove(renamed_file);
        DOCTEST_CHECK(!fs::exists(renamed_file));

        // 测试递归删除目录
        fs::remove_all(test_dir);
        DOCTEST_CHECK(!fs::exists(test_dir));
    }

    DOCTEST_TEST_CASE("FilesystemDirectoryIterator")
    {
        // 测试目录迭代器
        fs::path current_dir(".");
        int count = 0;
        for (const auto& entry : fs::directory_iterator(current_dir))
        {
            std::ignore = entry;
            ++count;
        }
        DOCTEST_CHECK(count > 0); // 当前目录至少应该有一些文件

        // 测试递归目录迭代器
        int recursive_count = 0;
        for (const auto& entry : fs::recursive_directory_iterator(current_dir))
        {
            std::ignore = entry;
            ++recursive_count;
        }
        DOCTEST_CHECK(recursive_count >= count);
    }

    DOCTEST_TEST_CASE("FilesystemPermissions")
    {
        // 测试权限操作
        fs::path test_file("test_permissions.txt");
        {
            std::ofstream f(test_file.string());
            f << "test content";
        }

        // 获取当前权限
        auto perms = fs::status(test_file).permissions();
        DOCTEST_CHECK(perms != fs::perms::none);

        // 清理
        fs::remove(test_file);
        DOCTEST_CHECK(!fs::exists(test_file));
    }

    DOCTEST_TEST_CASE("FilesystemError")
    {
        // 测试文件系统错误
        fs::path non_existent("non_existent_dir/file.txt");
        try
        {
            fs::create_directories(non_existent.parent_path());
            DOCTEST_CHECK(fs::exists(non_existent.parent_path()));
            fs::remove_all(non_existent.parent_path());
        }
        catch (const fs::filesystem_error& e)
        {
            std::ignore = e;
            // 不应该抛出异常，因为 create_directories 会创建所有必要的目录
            DOCTEST_CHECK(false);
        }
    }

    DOCTEST_TEST_CASE("FilesystemPathOperations")
    {
        // 测试路径规范化
        fs::path p1("test/../dir/file.txt");
        fs::path p1_normalized = p1.lexically_normal();
        std::string p1_str = p1_normalized.string();
        bool p1_valid = (p1_str == "dir/file.txt" || p1_str == "dir\\file.txt");
        DOCTEST_CHECK(p1_valid);

        // 测试相对路径
        fs::path p2("a/b/c");
        fs::path p3("a/d/e");
        fs::path relative = fs::relative(p2, p3.parent_path());
        std::string relative_str = relative.string();
        bool relative_valid = (relative_str == "../b/c" || relative_str == "..\\b\\c");
        DOCTEST_CHECK(relative_valid);

        // 测试绝对路径
        fs::path p4("test");
        fs::path p4_absolute = fs::absolute(p4);
        DOCTEST_CHECK(p4_absolute.is_absolute());

        // 测试当前路径
        fs::path current = fs::current_path();
        DOCTEST_CHECK(current.is_absolute());
    }

    DOCTEST_TEST_CASE("FilesystemFileTypes")
    {
        // 测试文件类型判断
        fs::path current_dir(".");
        DOCTEST_CHECK(fs::is_directory(current_dir));

        // 创建测试文件
        fs::path test_file("test_file_type.txt");
        {
            std::ofstream f(test_file.string());
            f << "test";
        }
        DOCTEST_CHECK(fs::is_regular_file(test_file));

        // 清理
        fs::remove(test_file);
        DOCTEST_CHECK(!fs::exists(test_file));
    }

    DOCTEST_TEST_CASE("FilesystemCopyOperations")
    {
        // 测试文件复制
        fs::path src_file("source_file.txt");
        fs::path dest_file("destination_file.txt");

        // 创建源文件
        {
            std::ofstream f(src_file.string());
            f << "test content";
        }
        DOCTEST_CHECK(fs::exists(src_file));

        // 复制文件
        fs::copy_file(src_file, dest_file);
        DOCTEST_CHECK(fs::exists(dest_file));
        DOCTEST_CHECK(fs::file_size(src_file) == fs::file_size(dest_file));

        // 测试目录复制
        fs::path src_dir("source_dir");
        fs::path dest_dir("destination_dir");

        // 创建源目录和文件
        fs::create_directory(src_dir);
        fs::path src_subfile = src_dir / "subfile.txt";
        {
            std::ofstream f(src_subfile.string());
            f << "subfile content";
        }

        // 复制目录
        fs::copy(src_dir, dest_dir, fs::copy_options::recursive);
        DOCTEST_CHECK(fs::exists(dest_dir));
        DOCTEST_CHECK(fs::exists(dest_dir / "subfile.txt"));

        // 清理
        fs::remove(src_file);
        fs::remove(dest_file);
        fs::remove_all(src_dir);
        fs::remove_all(dest_dir);
    }

    DOCTEST_TEST_CASE("FilesystemLinks")
    {
        // 测试硬链接和符号链接（注意：Windows 可能需要管理员权限）
        fs::path target_file("link_target.txt");
        fs::path hard_link("hard_link.txt");
        fs::path sym_link("sym_link.txt");

        // 创建目标文件
        {
            std::ofstream f(target_file.string());
            f << "link target";
        }
        DOCTEST_CHECK(fs::exists(target_file));

        try
        {
            // 创建硬链接
            fs::create_hard_link(target_file, hard_link);
            DOCTEST_CHECK(fs::exists(hard_link));

            // 创建符号链接
            fs::create_symlink(target_file, sym_link);
            DOCTEST_CHECK(fs::exists(sym_link));
            DOCTEST_CHECK(fs::is_symlink(sym_link));
        }
        catch (const fs::filesystem_error& e)
        {
            // 忽略权限错误，因为某些平台可能需要特殊权限
            std::ignore = e;
        }

        // 清理
        fs::remove(hard_link);
        fs::remove(sym_link);
        fs::remove(target_file);
    }

    DOCTEST_TEST_CASE("FilesystemSpace")
    {
        // 测试空间信息
        fs::path current_dir(".");
        auto space_info = fs::space(current_dir);
        DOCTEST_CHECK(space_info.capacity > 0);
        DOCTEST_CHECK(space_info.free >= 0);
        DOCTEST_CHECK(space_info.available >= 0);
    }

    DOCTEST_TEST_CASE("FilesystemPermissions")
    {
        // 测试权限设置
        fs::path test_file("test_perm.txt");
        {
            std::ofstream f(test_file.string());
            f << "test";
        }

        // 获取当前权限
        auto perms = fs::status(test_file).permissions();
        DOCTEST_CHECK(perms != fs::perms::none);

        // 清理
        fs::remove(test_file);
    }

    DOCTEST_TEST_CASE("FilesystemResize")
    {
        // 测试文件大小调整
        fs::path test_file("test_resize.txt");
        {
            std::ofstream f(test_file.string());
            f << "test content";
        }

        // 调整文件大小
        fs::resize_file(test_file, 100);
        DOCTEST_CHECK(fs::file_size(test_file) == 100);

        // 清理
        fs::remove(test_file);
    }

    DOCTEST_TEST_CASE("FStreamOperations")
    {
        // 测试文件路径
        fs::path test_file("test_fstream.txt");

        // 1. 测试 ofstream 写入
        {
            std::ofstream out_file(test_file.string());
            DOCTEST_CHECK(out_file.is_open());

            // 测试写入操作
            out_file << "Hello, fstream!" << std::endl;
            out_file << 123 << " " << 3.14 << std::endl;

            // 测试文件状态
            DOCTEST_CHECK(out_file.good());

            // 测试关闭文件
            out_file.close();
            DOCTEST_CHECK(!out_file.is_open());
        }

        // 2. 测试 ifstream 读取
        {
            std::ifstream in_file(test_file.string());
            DOCTEST_CHECK(in_file.is_open());

            // 测试读取操作
            std::string line;
            DOCTEST_CHECK(std::getline(in_file, line));
            DOCTEST_CHECK(line == "Hello, fstream!");

            // 测试格式化读取
            int int_val;
            double double_val;
            in_file >> int_val >> double_val;
            DOCTEST_CHECK(int_val == 123);
            DOCTEST_CHECK(double_val == 3.14);

            // 尝试读取更多内容，直到文件结束
            std::string dummy;
            in_file >> dummy;

            // 测试文件状态
            DOCTEST_CHECK(in_file.eof());
            // 当读取超出文件末尾时，fail() 会被设置，这是正常的
            // DOCTEST_CHECK(!in_file.fail());

            // 测试关闭文件
            in_file.close();
            DOCTEST_CHECK(!in_file.is_open());
        }

        // 3. 测试 fstream 读写
        {
            std::fstream file(test_file.string(), std::ios::in | std::ios::out);
            DOCTEST_CHECK(file.is_open());

            // 测试定位到文件末尾
            file.seekp(0, std::ios::end);

            // 测试追加写入
            file << "Appended content" << std::endl;

            // 测试定位到文件开头
            file.seekg(0, std::ios::beg);

            // 测试读取整个文件
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            DOCTEST_CHECK(content.find("Hello, fstream!") != std::string::npos);
            DOCTEST_CHECK(content.find("Appended content") != std::string::npos);

            // 测试文件状态
            DOCTEST_CHECK(file.good());

            // 测试关闭文件
            file.close();
            DOCTEST_CHECK(!file.is_open());
        }

        // 4. 测试二进制模式
        {
            // 写入二进制数据
            std::ofstream bin_out(test_file.string(), std::ios::binary);
            DOCTEST_CHECK(bin_out.is_open());

            int bin_data[] = {1, 2, 3, 4, 5};
            bin_out.write(reinterpret_cast<const char*>(bin_data), sizeof(bin_data));
            bin_out.close();

            // 读取二进制数据
            std::ifstream bin_in(test_file.string(), std::ios::binary);
            DOCTEST_CHECK(bin_in.is_open());

            int read_data[5];
            bin_in.read(reinterpret_cast<char*>(read_data), sizeof(read_data));
            DOCTEST_CHECK(bin_in.gcount() == sizeof(read_data));

            for (int i = 0; i < 5; ++i)
            {
                DOCTEST_CHECK(read_data[i] == bin_data[i]);
            }

            bin_in.close();
        }

        // 5. 测试文件状态标志
        {
            std::ifstream in_file(test_file.string());
            DOCTEST_CHECK(in_file.is_open());

            // 测试 good()
            DOCTEST_CHECK(in_file.good());

            // 测试 eof()
            in_file.seekg(0, std::ios::end);
            DOCTEST_CHECK(!in_file.eof());
            char c;
            in_file >> c;
            DOCTEST_CHECK(in_file.eof());

            // 测试 fail()
            in_file.clear();
            DOCTEST_CHECK(!in_file.fail());

            in_file.close();
        }

        // 6. 测试异常处理
        {
            std::ifstream in_file("non_existent_file.txt");
            DOCTEST_CHECK(!in_file.is_open());
            DOCTEST_CHECK(in_file.fail());
        }

        // 7. 测试文件缓冲区操作
        {
            std::ofstream out_file(test_file.string());
            DOCTEST_CHECK(out_file.is_open());

            // 测试 flush()
            out_file << "Test flush";
            out_file.flush();
            DOCTEST_CHECK(out_file.good());

            out_file.close();
        }

        // 8. 测试文件打开模式
        {
            // 测试 trunc 模式
            std::ofstream trunc_file(test_file.string(), std::ios::trunc);
            DOCTEST_CHECK(trunc_file.is_open());
            trunc_file << "Truncated content";
            trunc_file.close();

            // 测试 app 模式
            std::ofstream app_file(test_file.string(), std::ios::app);
            DOCTEST_CHECK(app_file.is_open());
            app_file << " Appended";
            app_file.close();

            // 验证内容
            std::ifstream check_file(test_file.string());
            std::string content((std::istreambuf_iterator<char>(check_file)), std::istreambuf_iterator<char>());
            DOCTEST_CHECK(content == "Truncated content Appended");
            check_file.close();
        }

        // 9. 测试文件大小和位置
        {
            std::fstream file(test_file.string(), std::ios::in | std::ios::out | std::ios::binary);
            DOCTEST_CHECK(file.is_open());

            // 获取文件大小
            file.seekg(0, std::ios::end);
            std::streampos file_size = file.tellg();
            DOCTEST_CHECK(file_size > 0);

            // 测试定位
            file.seekg(0, std::ios::beg);
            DOCTEST_CHECK(file.tellg() == 0);

            file.seekg(file_size / 2);
            DOCTEST_CHECK(file.tellg() == file_size / 2);

            file.close();
        }

        // 10. 测试移动语义
        {
            // 测试移动构造函数
            std::ofstream out1(test_file.string());
            DOCTEST_CHECK(out1.is_open());

            std::ofstream out2(std::move(out1));
            DOCTEST_CHECK(!out1.is_open());
            DOCTEST_CHECK(out2.is_open());

            out2 << "Test move semantics";
            out2.close();

            // 测试移动赋值运算符
            std::ifstream in1(test_file.string());
            DOCTEST_CHECK(in1.is_open());

            std::ifstream in2;
            in2 = std::move(in1);
            DOCTEST_CHECK(!in1.is_open());
            DOCTEST_CHECK(in2.is_open());

            in2.close();
        }

        // 11. 测试 rdbuf 操作
        {
            // 测试获取 rdbuf
            std::ifstream in_file(test_file.string());
            DOCTEST_CHECK(in_file.is_open());

            std::streambuf* buf = in_file.rdbuf();
            DOCTEST_CHECK(buf != nullptr);

            // 测试 rdbuf 的 in_avail() 方法
            std::streamsize avail = buf->in_avail();
            DOCTEST_CHECK(avail >= 0);

            // 测试 rdbuf 的 sgetc() 方法
            int c = buf->sgetc();
            DOCTEST_CHECK(c != EOF);

            // 测试 rdbuf 的 sbumpc() 方法
            int c2 = buf->sbumpc();
            DOCTEST_CHECK(c2 == c);

            // 测试 rdbuf 的 sgetn() 方法
            char buffer[100];
            std::streamsize n = buf->sgetn(buffer, 10);
            DOCTEST_CHECK(n > 0);

            // 测试 rdbuf 的 sputbackc() 方法
            int putback_result = buf->sputbackc('A');
            DOCTEST_CHECK(putback_result == 'A');

            // 测试 rdbuf 的 sungetc() 方法
            int unget_result = buf->sungetc();
            DOCTEST_CHECK(unget_result != EOF);

            // 测试 rdbuf 的 pubsetbuf() 方法
            char custom_buf[256];
            buf->pubsetbuf(custom_buf, sizeof(custom_buf));

            // 测试 rdbuf 的 pubseekpos() 方法
            std::streampos pos = buf->pubseekpos(0);
            DOCTEST_CHECK(pos != static_cast<std::streampos>(-1));

            // 测试 rdbuf 的 pubseekoff() 方法
            std::streampos off_pos = buf->pubseekoff(5, std::ios::cur);
            DOCTEST_CHECK(off_pos != static_cast<std::streampos>(-1));

            // 测试 rdbuf 的 pubsync() 方法
            int sync_result = buf->pubsync();
            bool sync_valid = (sync_result == 0 || sync_result == -1); // -1 表示不支持
            DOCTEST_CHECK(sync_valid);

            in_file.close();
        }

        // 12. 测试其他 fstream 接口
        {
            // 测试 swap() 方法
            std::ifstream in1(test_file.string());
            std::ifstream in2;
            DOCTEST_CHECK(in1.is_open());
            DOCTEST_CHECK(!in2.is_open());

            in1.swap(in2);
            DOCTEST_CHECK(!in1.is_open());
            DOCTEST_CHECK(in2.is_open());

            in2.close();

            // 测试 copyfmt() 方法
            std::ofstream out1(test_file.string());
            std::ofstream out2;

            // 设置一些格式标志
            out1 << std::hex << std::setw(8) << std::setfill('0');

            // 复制格式
            out2.copyfmt(out1);

            // 测试 imbue() 方法
            std::locale loc = std::locale();
            out1.imbue(loc);

            out1.close();
            out2.close();

            // 测试 tie() 方法
            std::ofstream out3(test_file.string());
            std::ostream* tied = out3.tie();
            DOCTEST_CHECK(tied == nullptr); // 默认未绑定

            // 绑定到 cout
            out3.tie(&std::cout);
            tied = out3.tie();
            DOCTEST_CHECK(tied == &std::cout);

            out3.close();
        }

        // 13. 测试 wide stream 操作
        {
            // 测试 wofstream 写入
            std::wofstream wout(test_file.string());
            DOCTEST_CHECK(wout.is_open());

            wout << L"Hello, wide fstream!" << std::endl;
            wout.close();

            // 测试 wifstream 读取
            std::wifstream win(test_file.string());
            DOCTEST_CHECK(win.is_open());

            std::wstring wline;
            DOCTEST_CHECK(std::getline(win, wline));
            DOCTEST_CHECK(wline == L"Hello, wide fstream!");

            win.close();
        }

        // 14. 测试格式化操作
        {
            std::ofstream out_file(test_file.string());
            DOCTEST_CHECK(out_file.is_open());

            // 测试 fill() 方法
            char old_fill = out_file.fill('*');
            DOCTEST_CHECK(old_fill == ' ');

            // 测试 width() 方法
            out_file.width(10);
            DOCTEST_CHECK(out_file.width() == 10);

            // 测试 precision() 方法
            out_file.precision(5);
            DOCTEST_CHECK(out_file.precision() == 5);

            // 测试 setf() 方法
            std::ios_base::fmtflags old_flags = out_file.setf(std::ios::hex | std::ios::showbase);
            std::ignore = old_flags;

            // 测试 unsetf() 方法
            out_file.unsetf(std::ios::hex);

            // 测试 flags() 方法
            std::ios_base::fmtflags current_flags = out_file.flags();
            DOCTEST_CHECK((current_flags & std::ios::showbase) != 0);

            out_file.close();
        }

        // 15. 测试流状态操作
        {
            // 先创建一个有内容的文件
            {
                std::ofstream out_file(test_file.string());
                DOCTEST_CHECK(out_file.is_open());
                out_file << "Test content";
                out_file.close();
            }

            std::ifstream in_file(test_file.string());
            DOCTEST_CHECK(in_file.is_open());

            // 测试 rdstate() 方法
            std::ios_base::iostate state = in_file.rdstate();
            // goodbit 是默认状态，值为 0，所以应该检查 state == 0
            DOCTEST_CHECK(state == std::ios::goodbit);

            // 测试 setstate() 方法
            in_file.setstate(std::ios::eofbit);
            state = in_file.rdstate();
            DOCTEST_CHECK((state & std::ios::eofbit) != 0);

            // 测试 clear() 方法
            in_file.clear();
            state = in_file.rdstate();
            DOCTEST_CHECK(state == std::ios::goodbit);

            in_file.close();
        }

        // 清理
        fs::remove(test_file);
    }
}