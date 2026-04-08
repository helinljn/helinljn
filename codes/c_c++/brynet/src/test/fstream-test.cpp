#include "doctest.h"
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <locale>
#include <string>

namespace fs = std::filesystem;

// 辅助函数：创建隔离的临时文件路径
static fs::path make_temp_file(const std::string& name)
{
    return fs::temp_directory_path() / ("brynet_test_fstream_" + name);
}

// 辅助函数：清理临时文件
static void cleanup(const fs::path& p)
{
    if (fs::exists(p))
    {
        fs::remove(p);
    }
}

DOCTEST_TEST_SUITE("FStream")
{
    /**
     * 测试用例 1：ofstream 写入与 ifstream 读取
     *
     * 功能说明：
     * - 演示 std::ofstream 写入文本和数值
     * - 演示 std::ifstream 逐行读取和格式化读取
     * - 演示 is_open / good / close 等流状态查询
     * - 验证浮点数读取使用 doctest::Approx 近似比较
     */
    DOCTEST_TEST_CASE("FStreamWriteAndRead")
    {
        auto test_file = make_temp_file("write_read.txt");

        // 测试 ofstream 写入
        {
            std::ofstream out_file(test_file.string());
            DOCTEST_CHECK(out_file.is_open());

            out_file << "Hello, fstream!" << std::endl;
            out_file << 123 << " " << 3.14 << std::endl;

            DOCTEST_CHECK(out_file.good());

            out_file.close();
            DOCTEST_CHECK(!out_file.is_open());
        }

        // 测试 ifstream 读取
        {
            std::ifstream in_file(test_file.string());
            DOCTEST_CHECK(in_file.is_open());

            std::string line;
            DOCTEST_CHECK(std::getline(in_file, line));
            DOCTEST_CHECK(line == "Hello, fstream!");

            int int_val;
            double double_val;
            in_file >> int_val >> double_val;
            DOCTEST_CHECK(int_val == 123);
            DOCTEST_CHECK(double_val == doctest::Approx(3.14));

            in_file.close();
            DOCTEST_CHECK(!in_file.is_open());
        }

        cleanup(test_file);
    }

    /**
     * 测试用例 2：fstream 读写模式
     *
     * 功能说明：
     * - 演示 std::fstream 以 in | out 模式打开文件
     * - 演示 seekp 定位写入位置并追加内容
     * - 演示 seekg 定位读取位置并用 istreambuf_iterator 读取全部内容
     * - 验证写入的追加内容可在读取时找到
     */
    DOCTEST_TEST_CASE("FStreamReadWrite")
    {
        auto test_file = make_temp_file("rw.txt");

        // 先写入一些内容
        {
            std::ofstream f(test_file.string());
            f << "Hello, fstream!";
        }

        // 测试 fstream 读写
        {
            std::fstream file(test_file.string(), std::ios::in | std::ios::out);
            DOCTEST_CHECK(file.is_open());

            // 追加写入
            file.seekp(0, std::ios::end);
            file << " Appended content" << std::endl;

            // 读取整个文件
            file.seekg(0, std::ios::beg);
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            DOCTEST_CHECK(content.find("Hello, fstream!") != std::string::npos);
            DOCTEST_CHECK(content.find("Appended content") != std::string::npos);

            file.close();
            DOCTEST_CHECK(!file.is_open());
        }

        cleanup(test_file);
    }

    /**
     * 测试用例 3：二进制模式读写
     *
     * 功能说明：
     * - 演示 std::ios::binary 模式打开文件
     * - 演示 ofstream::write 写入二进制数据块
     * - 演示 ifstream::read 读取二进制数据块
     * - 演示 gcount() 获取实际读取字节数
     * - 验证写入与读取的整型数组内容完全一致
     */
    DOCTEST_TEST_CASE("FStreamBinary")
    {
        auto test_file = make_temp_file("binary.bin");

        // 写入二进制数据
        {
            std::ofstream bin_out(test_file.string(), std::ios::binary);
            DOCTEST_CHECK(bin_out.is_open());

            int bin_data[] = {1, 2, 3, 4, 5};
            bin_out.write(reinterpret_cast<const char*>(bin_data), sizeof(bin_data));
            bin_out.close();
        }

        // 读取二进制数据
        {
            std::ifstream bin_in(test_file.string(), std::ios::binary);
            DOCTEST_CHECK(bin_in.is_open());

            int read_data[5];
            bin_in.read(reinterpret_cast<char*>(read_data), sizeof(read_data));
            DOCTEST_CHECK(bin_in.gcount() == sizeof(read_data));

            int expected[] = {1, 2, 3, 4, 5};
            for (int i = 0; i < 5; ++i)
            {
                DOCTEST_CHECK(read_data[i] == expected[i]);
            }

            bin_in.close();
        }

        cleanup(test_file);
    }

    /**
     * 测试用例 4：流状态标志（good / eof / fail / clear）
     *
     * 功能说明：
     * - 演示 good() 在正常状态下为 true
     * - 演示在文件末尾读取后 eof() 为 true
     * - 演示 clear() 可重置流状态
     * - 验证打开不存在的文件时 fail() 为 true、is_open() 为 false
     */
    DOCTEST_TEST_CASE("FStreamStateFlags")
    {
        auto test_file = make_temp_file("state.txt");

        // 先创建一个有内容的文件
        {
            std::ofstream f(test_file.string());
            f << "Test content";
        }

        {
            std::ifstream in_file(test_file.string());
            DOCTEST_CHECK(in_file.is_open());
            DOCTEST_CHECK(in_file.good());

            // 测试 eof()
            in_file.seekg(0, std::ios::end);
            DOCTEST_CHECK(!in_file.eof());
            char c;
            in_file >> c; // 尝试在文件末尾读取
            DOCTEST_CHECK(in_file.eof());

            // 测试 clear()
            in_file.clear();
            DOCTEST_CHECK(!in_file.fail());

            in_file.close();
        }

        // 测试打开不存在的文件
        {
            std::ifstream in_file("non_existent_file_xyz.txt");
            DOCTEST_CHECK(!in_file.is_open());
            DOCTEST_CHECK(in_file.fail());
        }

        cleanup(test_file);
    }

    /**
     * 测试用例 5：flush 刷新缓冲区
     *
     * 功能说明：
     * - 演示 std::flush 显式刷新输出流缓冲区
     * - 验证 flush 后流状态仍为 good
     * - 验证刷新后文件内容可被正确读取
     */
    DOCTEST_TEST_CASE("FStreamFlush")
    {
        auto test_file = make_temp_file("flush.txt");

        {
            std::ofstream out_file(test_file.string());
            DOCTEST_CHECK(out_file.is_open());

            out_file << "Test flush";
            out_file.flush();
            DOCTEST_CHECK(out_file.good());

            out_file.close();
        }

        // 验证内容确实写入了
        {
            std::ifstream in_file(test_file.string());
            std::string content;
            std::getline(in_file, content);
            DOCTEST_CHECK(content == "Test flush");
        }

        cleanup(test_file);
    }

    /**
     * 测试用例 6：文件打开模式（trunc / app）
     *
     * 功能说明：
     * - 演示 std::ios::trunc 截断模式打开文件（清空原有内容）
     * - 演示 std::ios::app 追加模式打开文件（在末尾追加）
     * - 验证 trunc 写入后再 app 追加，最终内容为两者拼接
     */
    DOCTEST_TEST_CASE("FStreamOpenModes")
    {
        auto test_file = make_temp_file("modes.txt");

        // 测试 trunc 模式
        {
            std::ofstream trunc_file(test_file.string(), std::ios::trunc);
            DOCTEST_CHECK(trunc_file.is_open());
            trunc_file << "Truncated content";
            trunc_file.close();
        }

        // 测试 app 模式
        {
            std::ofstream app_file(test_file.string(), std::ios::app);
            DOCTEST_CHECK(app_file.is_open());
            app_file << " Appended";
            app_file.close();
        }

        // 验证内容
        {
            std::ifstream check_file(test_file.string());
            std::string content((std::istreambuf_iterator<char>(check_file)), std::istreambuf_iterator<char>());
            DOCTEST_CHECK(content == "Truncated content Appended");
        }

        cleanup(test_file);
    }

    /**
     * 测试用例 7：文件定位与 tell（seekg / seekp / tellg）
     *
     * 功能说明：
     * - 演示 seekg / tellg 获取文件大小
     * - 演示 seekg 定位到文件开头、中间位置
     * - 演示 tellg 验证当前位置
     * - 验证定位后可正确读取对应位置的字符
     */
    DOCTEST_TEST_CASE("FStreamSeekAndTell")
    {
        auto test_file = make_temp_file("seek.txt");

        // 创建文件
        {
            std::ofstream f(test_file.string(), std::ios::binary);
            f << "0123456789"; // 10 bytes
        }

        {
            std::fstream file(test_file.string(), std::ios::in | std::ios::out | std::ios::binary);
            DOCTEST_CHECK(file.is_open());

            // 获取文件大小
            file.seekg(0, std::ios::end);
            std::streampos file_size = file.tellg();
            DOCTEST_CHECK(file_size == 10);

            // 定位到开头
            file.seekg(0, std::ios::beg);
            DOCTEST_CHECK(file.tellg() == 0);

            // 定位到中间
            file.seekg(5);
            DOCTEST_CHECK(file.tellg() == 5);

            // 读取并验证
            char c;
            file.get(c);
            DOCTEST_CHECK(c == '5');

            file.close();
        }

        cleanup(test_file);
    }

    /**
     * 测试用例 8：移动语义（移动构造与移动赋值）
     *
     * 功能说明：
     * - 演示 std::ofstream 的移动构造函数，原对象不再 is_open
     * - 演示 std::ifstream 的移动赋值运算符，原对象不再 is_open
     * - 验证移动后新对象持有文件句柄
     */
    DOCTEST_TEST_CASE("FStreamMoveSemantics")
    {
        auto test_file = make_temp_file("move.txt");

        // 先创建文件
        {
            std::ofstream f(test_file.string());
            f << "Test move semantics";
        }

        // 测试移动构造函数
        {
            std::ofstream out1(test_file.string());
            DOCTEST_CHECK(out1.is_open());

            std::ofstream out2(std::move(out1));
            DOCTEST_CHECK(!out1.is_open());
            DOCTEST_CHECK(out2.is_open());

            out2.close();
        }

        // 测试移动赋值运算符
        {
            std::ifstream in1(test_file.string());
            DOCTEST_CHECK(in1.is_open());

            std::ifstream in2;
            in2 = std::move(in1);
            DOCTEST_CHECK(!in1.is_open());
            DOCTEST_CHECK(in2.is_open());

            in2.close();
        }

        cleanup(test_file);
    }

    /**
     * 测试用例 9：streambuf 底层操作（rdbuf）
     *
     * 功能说明：
     * - 演示 rdbuf() 获取底层 streambuf 指针
     * - 演示 sgetc / sbumpc / sgetn 字符与块读取
     * - 演示 sputbackc / sungetc 字符回退（放回刚读取的字符）
     * - 演示 pubseekpos / pubseekoff 定位操作
     * - 演示 pubsync 同步操作、pubsetbuf 设置自定义缓冲区
     */
    DOCTEST_TEST_CASE("FStreamRdbuf")
    {
        auto test_file = make_temp_file("rdbuf.txt");

        // 创建有内容的文件
        {
            std::ofstream f(test_file.string());
            f << "Hello rdbuf!";
        }

        {
            std::ifstream in_file(test_file.string());
            DOCTEST_CHECK(in_file.is_open());

            std::streambuf* buf = in_file.rdbuf();
            DOCTEST_CHECK(buf != nullptr);

            // 测试 in_avail()
            std::streamsize avail = buf->in_avail();
            DOCTEST_CHECK(avail >= 0);

            // 测试 sgetc()
            int c = buf->sgetc();
            DOCTEST_CHECK(c == 'H');

            // 测试 sbumpc()
            int c2 = buf->sbumpc();
            DOCTEST_CHECK(c2 == 'H');

            // 测试 sgetn()
            char buffer[100];
            std::streamsize n = buf->sgetn(buffer, 4);
            DOCTEST_CHECK(n == 4);
            DOCTEST_CHECK(std::string(buffer, 4) == "ello");

            // 测试 sputbackc() — 放回刚读取的字符
            int putback_result = buf->sputbackc('o');
            DOCTEST_CHECK(putback_result == 'o');

            // 测试 sungetc()
            int unget_result = buf->sungetc();
            DOCTEST_CHECK(unget_result != EOF);

            // 测试 pubseekpos()
            std::streampos pos = buf->pubseekpos(0);
            DOCTEST_CHECK(pos != static_cast<std::streampos>(-1));

            // 测试 pubseekoff()
            std::streampos off_pos = buf->pubseekoff(5, std::ios::cur);
            DOCTEST_CHECK(off_pos != static_cast<std::streampos>(-1));

            // 测试 pubsync()
            int sync_result = buf->pubsync();
            DOCTEST_CHECK(sync_result == 0);

            // 测试 pubsetbuf()
            char custom_buf[256];
            buf->pubsetbuf(custom_buf, sizeof(custom_buf));

            in_file.close();
        }

        cleanup(test_file);
    }

    /**
     * 测试用例 10：swap / copyfmt / tie 操作
     *
     * 功能说明：
     * - 演示 swap() 交换两个流对象的状态
     * - 演示 copyfmt() 复制格式设置（fill / flags 等）
     * - 演示 tie() 绑定/解绑输出流，验证默认未绑定
     */
    DOCTEST_TEST_CASE("FStreamSwapAndCopyfmt")
    {
        auto test_file = make_temp_file("swap.txt");

        // 创建文件
        {
            std::ofstream f(test_file.string());
            f << "Test swap";
        }

        // 测试 swap()
        {
            std::ifstream in1(test_file.string());
            std::ifstream in2;
            DOCTEST_CHECK(in1.is_open());
            DOCTEST_CHECK(!in2.is_open());

            in1.swap(in2);
            DOCTEST_CHECK(!in1.is_open());
            DOCTEST_CHECK(in2.is_open());

            in2.close();
        }

        // 测试 copyfmt()
        {
            std::ofstream out1(test_file.string());
            std::ofstream out2;

            out1 << std::hex << std::setw(8) << std::setfill('0');
            out2.copyfmt(out1);

            // 验证格式被复制
            DOCTEST_CHECK(out2.fill() == '0');
            bool has_hex = (out2.flags() & std::ios::hex) != 0;
            DOCTEST_CHECK(has_hex);

            out1.close();
            out2.close();
        }

        // 测试 tie()
        {
            std::ofstream out3(test_file.string());
            std::ostream* tied = out3.tie();
            DOCTEST_CHECK(tied == nullptr);

            out3.tie(&std::cout);
            tied = out3.tie();
            DOCTEST_CHECK(tied == &std::cout);

            out3.close();
        }

        cleanup(test_file);
    }

    /**
     * 测试用例 11：宽字符流（wofstream / wifstream）
     *
     * 功能说明：
     * - 演示 std::wofstream 写入宽字符串
     * - 演示 std::wifstream 读取宽字符串
     * - 验证写入与读取的宽字符串内容一致
     */
    DOCTEST_TEST_CASE("FStreamWideStream")
    {
        auto test_file = make_temp_file("wide.txt");

        // 写入宽字符
        {
            std::wofstream wout(test_file.string());
            DOCTEST_CHECK(wout.is_open());

            wout << L"Hello, wide fstream!" << std::endl;
            wout.close();
        }

        // 读取宽字符
        {
            std::wifstream win(test_file.string());
            DOCTEST_CHECK(win.is_open());

            std::wstring wline;
            DOCTEST_CHECK(std::getline(win, wline));
            DOCTEST_CHECK(wline == L"Hello, wide fstream!");

            win.close();
        }

        cleanup(test_file);
    }

    /**
     * 测试用例 12：格式化控制接口（fill / width / precision / flags）
     *
     * 功能说明：
     * - 演示 fill() 设置和获取填充字符，默认为空格
     * - 演示 width() 设置字段宽度
     * - 演示 precision() 设置浮点精度
     * - 演示 setf() / unsetf() / flags() 设置和查询格式标志
     */
    DOCTEST_TEST_CASE("FStreamFormatting")
    {
        auto test_file = make_temp_file("format.txt");

        {
            std::ofstream out_file(test_file.string());
            DOCTEST_CHECK(out_file.is_open());

            // 测试 fill()
            char old_fill = out_file.fill('*');
            DOCTEST_CHECK(old_fill == ' ');

            // 测试 width()
            out_file.width(10);
            DOCTEST_CHECK(out_file.width() == 10);

            // 测试 precision()
            out_file.precision(5);
            DOCTEST_CHECK(out_file.precision() == 5);

            // 测试 setf()
            std::ios_base::fmtflags old_flags = out_file.setf(std::ios::hex | std::ios::showbase);
            std::ignore = old_flags;

            // 测试 unsetf()
            out_file.unsetf(std::ios::hex);

            // 测试 flags()
            std::ios_base::fmtflags current_flags = out_file.flags();
            DOCTEST_CHECK((current_flags & std::ios::showbase) != 0);

            out_file.close();
        }

        cleanup(test_file);
    }

    /**
     * 测试用例 13：流状态操作（rdstate / setstate / clear）
     *
     * 功能说明：
     * - 演示 rdstate() 获取当前流状态，默认为 goodbit（值为 0）
     * - 演示 setstate() 手动设置流状态位（如 eofbit）
     * - 演示 clear() 重置所有流状态位回 goodbit
     * - 验证各操作后 rdstate() 的返回值符合预期
     */
    DOCTEST_TEST_CASE("FStreamRdstateSetstateClear")
    {
        auto test_file = make_temp_file("rdstate.txt");

        {
            std::ofstream f(test_file.string());
            f << "Test content";
        }

        {
            std::ifstream in_file(test_file.string());
            DOCTEST_CHECK(in_file.is_open());

            // 测试 rdstate()
            std::ios_base::iostate state = in_file.rdstate();
            DOCTEST_CHECK(state == std::ios::goodbit);

            // 测试 setstate()
            in_file.setstate(std::ios::eofbit);
            state = in_file.rdstate();
            DOCTEST_CHECK((state & std::ios::eofbit) != 0);

            // 测试 clear()
            in_file.clear();
            state = in_file.rdstate();
            DOCTEST_CHECK(state == std::ios::goodbit);

            in_file.close();
        }

        cleanup(test_file);
    }
}