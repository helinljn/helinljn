#include "doctest.h"
#include "fmt/format.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>

namespace fs = std::filesystem;

// 辅助函数：创建隔离的临时目录，避免污染工作目录
static fs::path make_temp_dir(const std::string& name)
{
    fs::path tmp = fs::temp_directory_path() / ("brynet_test_" + name);
    if (fs::exists(tmp))
    {
        fs::remove_all(tmp);
    }
    fs::create_directories(tmp);
    return tmp;
}

DOCTEST_TEST_SUITE("FileSystem")
{
    /**
     * 测试用例 1：路径构造与路径组件操作
     *
     * 功能说明：
     * - 演示 fs::path 的默认构造、字符串构造和指定格式构造
     * - 演示 operator/ 拼接路径，并使用 generic_string() 统一跨平台比较
     * - 演示 parent_path / filename / stem / extension 等路径组件的拆分
     * - 演示 is_absolute / is_relative 路径类型查询
     * - 演示 fs::path 的 operator== 比较语义
     */
    DOCTEST_TEST_CASE("FilesystemPath")
    {
        // 测试路径构造
        fs::path p1;
        DOCTEST_CHECK(p1.empty());

        fs::path p2("test");
        DOCTEST_CHECK(!p2.empty());

        fs::path p3("test", fs::path::format::native_format);
        DOCTEST_CHECK(!p3.empty());

        // 测试路径操作 — 使用 generic_string() 统一比较
        fs::path p4 = p2 / "subdir" / "file.txt";
        DOCTEST_CHECK(p4.generic_string() == "test/subdir/file.txt");

        // 测试路径组件
        DOCTEST_CHECK(p4.parent_path().generic_string() == "test/subdir");
        DOCTEST_CHECK(p4.filename().string() == "file.txt");
        DOCTEST_CHECK(p4.stem().string() == "file");
        DOCTEST_CHECK(p4.extension().string() == ".txt");

        // 测试路径查询
        DOCTEST_CHECK(p4.is_absolute() == false);
        DOCTEST_CHECK(p4.is_relative() == true);

        // 测试路径比较
        DOCTEST_CHECK(fs::path("a/b") == fs::path("a") / "b");
    }

    /**
     * 测试用例 2：文件状态查询
     *
     * 功能说明：
     * - 演示 fs::exists 判断路径是否存在
     * - 演示 fs::is_directory 判断是否为目录
     * - 演示 fs::status 获取文件状态，验证 status_known 和 file_type
     * - 验证对不存在路径，status() 返回 file_type::not_found
     */
    DOCTEST_TEST_CASE("FilesystemStatus")
    {
        // 测试文件状态
        fs::path current_dir(".");
        DOCTEST_CHECK(fs::exists(current_dir));
        DOCTEST_CHECK(fs::is_directory(current_dir));

        // 测试不存在的文件
        fs::path non_existent("non_existent_file.txt");
        DOCTEST_CHECK(!fs::exists(non_existent));

        // 测试 status_known
        auto s = fs::status(non_existent);
        DOCTEST_CHECK(fs::status_known(s));
        DOCTEST_CHECK(s.type() == fs::file_type::not_found);
    }

    /**
     * 测试用例 3：目录与文件的创建、删除、重命名操作
     *
     * 功能说明：
     * - 演示 fs::create_directory 创建单级目录
     * - 演示 fs::create_directories 创建多级目录，并与 create_directory 对比返回值
     * - 演示 fs::rename 重命名文件
     * - 演示 fs::remove 删除文件、fs::remove_all 递归删除目录
     * - 演示 fs::file_size 获取文件大小
     * - 所有临时文件在隔离目录中创建，测试结束后统一清理
     */
    DOCTEST_TEST_CASE("FilesystemOperations")
    {
        auto tmp = make_temp_dir("operations");
        fs::path test_dir = tmp / "test_dir";

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

        // 测试 create_directories 创建多级目录
        fs::path deep_dir = tmp / "a" / "b" / "c";
        DOCTEST_CHECK(fs::create_directories(deep_dir));
        DOCTEST_CHECK(fs::exists(deep_dir));
        DOCTEST_CHECK(fs::is_directory(deep_dir));

        // create_directory 对已存在的目录返回 false
        DOCTEST_CHECK(fs::create_directory(deep_dir) == false);

        // create_directories 对已存在的目录返回 false
        DOCTEST_CHECK(fs::create_directories(deep_dir) == false);

        fs::remove_all(tmp);
    }

    /**
     * 测试用例 4：目录迭代器与 directory_entry 接口
     *
     * 功能说明：
     * - 演示 fs::directory_iterator 遍历目录内条目
     * - 演示 fs::recursive_directory_iterator 递归遍历子目录
     * - 演示 fs::directory_entry 的缓存接口：exists / is_directory / is_regular_file / file_size
     * - 验证递归迭代器返回的条目数不少于非递归迭代器
     */
    DOCTEST_TEST_CASE("FilesystemDirectoryIterator")
    {
        auto tmp = make_temp_dir("iterator");

        // 创建测试目录结构
        fs::create_directory(tmp / "sub1");
        fs::create_directory(tmp / "sub2");
        {
            std::ofstream f1((tmp / "file1.txt").string());
            f1 << "1";
            std::ofstream f2((tmp / "file2.txt").string());
            f2 << "2";
        }

        // 测试目录迭代器
        int count = 0;
        for (const auto& entry : fs::directory_iterator(tmp))
        {
            std::ignore = entry;
            ++count;
        }
        DOCTEST_CHECK(count == 4); // sub1, sub2, file1.txt, file2.txt

        // 测试递归目录迭代器
        int recursive_count = 0;
        for (const auto& entry : fs::recursive_directory_iterator(tmp))
        {
            std::ignore = entry;
            ++recursive_count;
        }
        DOCTEST_CHECK(recursive_count >= count);

        // 测试 directory_entry 接口
        for (const auto& entry : fs::directory_iterator(tmp))
        {
            DOCTEST_CHECK(entry.exists());
            bool is_dir_or_file = entry.is_directory() || entry.is_regular_file();
            DOCTEST_CHECK(is_dir_or_file);
            if (entry.is_regular_file())
            {
                DOCTEST_CHECK(entry.file_size() > 0);
            }
        }

        fs::remove_all(tmp);
    }

    /**
     * 测试用例 5：文件权限读取与修改
     *
     * 功能说明：
     * - 演示 fs::status().permissions() 获取当前权限
     * - 演示 fs::permissions() 的 perm_options::remove / add 操作
     * - 验证移除写权限后，权限位确实变化
     * - 验证恢复写权限后可正常删除文件
     */
    DOCTEST_TEST_CASE("FilesystemPermissions")
    {
        auto tmp = make_temp_dir("permissions");
        fs::path test_file = tmp / "test_perm.txt";
        {
            std::ofstream f(test_file.string());
            f << "test";
        }

        // 获取当前权限
        auto perms = fs::status(test_file).permissions();
        DOCTEST_CHECK(perms != fs::perms::none);

        // 测试修改权限：移除所有写权限
        fs::permissions(test_file,
                        fs::perms::owner_write | fs::perms::group_write | fs::perms::others_write,
                        fs::perm_options::remove);

        auto new_perms = fs::status(test_file).permissions();
        DOCTEST_CHECK((new_perms & fs::perms::owner_write) != fs::perms::owner_write);

        // 恢复写权限以便删除
        fs::permissions(test_file,
                        fs::perms::owner_write,
                        fs::perm_options::add);

        fs::remove_all(tmp);
    }

    /**
     * 测试用例 6：文件系统错误与异常处理
     *
     * 功能说明：
     * - 验证对不存在的文件调用 fs::file_size 会抛出 filesystem_error 异常
     * - 验证异常对象中携带的错误码非零
     * - 验证 fs::create_directories 对合法路径不抛异常
     */
    DOCTEST_TEST_CASE("FilesystemError")
    {
        // 测试操作不存在的文件确实会抛出 filesystem_error
        fs::path non_existent("non_existent_dir_xyz/file.txt");
        bool caught = false;
        try
        {
            std::ignore = fs::file_size(non_existent);
        }
        catch (const fs::filesystem_error& e)
        {
            caught = true;
            DOCTEST_CHECK(e.code().value() != 0);
        }
        DOCTEST_CHECK(caught);

        // 测试 create_directories 成功场景（不应抛异常）
        auto tmp = make_temp_dir("error");
        fs::path nested = tmp / "sub1" / "sub2";
        try
        {
            fs::create_directories(nested);
            DOCTEST_CHECK(fs::exists(nested));
        }
        catch (const fs::filesystem_error&)
        {
            DOCTEST_CHECK(false); // 不应该抛出异常
        }

        fs::remove_all(tmp);
    }

    /**
     * 测试用例 7：路径规范化与相对/绝对路径操作
     *
     * 功能说明：
     * - 演示 fs::path::lexically_normal 路径词法规范化
     * - 演示 fs::relative 计算两个路径之间的相对路径
     * - 演示 fs::proximate 计算近似相对路径
     * - 演示 fs::absolute 将相对路径转为绝对路径
     * - 演示 fs::current_path 获取当前工作目录
     * - 演示 fs::canonical 解析符号链接并获取规范路径（需路径真实存在）
     */
    DOCTEST_TEST_CASE("FilesystemPathOperations")
    {
        // 测试路径规范化 — 使用 generic_string()
        fs::path p1("test/../dir/file.txt");
        DOCTEST_CHECK(p1.lexically_normal().generic_string() == "dir/file.txt");

        // 测试相对路径
        fs::path p2("a/b/c");
        fs::path p3("a/d/e");
        fs::path relative = fs::relative(p2, p3.parent_path());
        DOCTEST_CHECK(relative.generic_string() == "../b/c");

        // 测试绝对路径
        fs::path p4("test");
        fs::path p4_absolute = fs::absolute(p4);
        DOCTEST_CHECK(p4_absolute.is_absolute());

        // 测试当前路径
        fs::path current = fs::current_path();
        DOCTEST_CHECK(current.is_absolute());

        // 测试 canonical（需要路径真实存在）
        auto tmp = make_temp_dir("pathops");
        fs::path canon = tmp / "sub" / "..";
        fs::create_directory(tmp / "sub");
        fs::path result = fs::canonical(canon);
        DOCTEST_CHECK(result == fs::canonical(tmp));

        // 测试 proximate
        fs::path prox = fs::proximate(tmp / "a" / "b", tmp);
        DOCTEST_CHECK(prox.generic_string() == "a/b");

        fs::remove_all(tmp);
    }

    /**
     * 测试用例 8：文件类型判断
     *
     * 功能说明：
     * - 演示 fs::is_directory / is_regular_file / is_symlink 类型判断
     * - 演示 fs::status().type() 获取 file_type 枚举值
     * - 验证目录的 file_type 为 directory，普通文件的 file_type 为 regular
     */
    DOCTEST_TEST_CASE("FilesystemFileTypes")
    {
        auto tmp = make_temp_dir("filetypes");

        // 测试目录类型
        DOCTEST_CHECK(fs::is_directory(tmp));
        DOCTEST_CHECK(!fs::is_regular_file(tmp));
        DOCTEST_CHECK(!fs::is_symlink(tmp));

        // 测试普通文件类型
        fs::path test_file = tmp / "test_file_type.txt";
        {
            std::ofstream f(test_file.string());
            f << "test";
        }
        DOCTEST_CHECK(fs::is_regular_file(test_file));
        DOCTEST_CHECK(!fs::is_directory(test_file));

        // 测试 file_type
        auto status = fs::status(test_file);
        DOCTEST_CHECK(status.type() == fs::file_type::regular);

        auto dir_status = fs::status(tmp);
        DOCTEST_CHECK(dir_status.type() == fs::file_type::directory);

        fs::remove_all(tmp);
    }

    /**
     * 测试用例 9：文件与目录复制操作
     *
     * 功能说明：
     * - 演示 fs::copy_file 复制单个文件
     * - 演示 copy_options::skip_existing 跳过已存在目标（不覆盖）
     * - 演示 copy_options::overwrite_existing 强制覆盖已存在目标
     * - 演示 fs::copy 配合 copy_options::recursive 递归复制目录
     */
    DOCTEST_TEST_CASE("FilesystemCopyOperations")
    {
        auto tmp = make_temp_dir("copy");
        fs::path src_file = tmp / "source_file.txt";
        fs::path dest_file = tmp / "destination_file.txt";

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

        // 测试 copy_options::skip_existing
        fs::path dest_file2 = tmp / "destination_file2.txt";
        {
            std::ofstream f(dest_file2.string());
            f << "different content";
        }
        auto orig_size = fs::file_size(dest_file2);
        fs::copy_file(src_file, dest_file2, fs::copy_options::skip_existing);
        DOCTEST_CHECK(fs::file_size(dest_file2) == orig_size); // 不应覆盖

        // 测试 copy_options::overwrite_existing
        fs::copy_file(src_file, dest_file2, fs::copy_options::overwrite_existing);
        DOCTEST_CHECK(fs::file_size(dest_file2) == fs::file_size(src_file)); // 应覆盖

        // 测试目录复制
        fs::path src_dir = tmp / "source_dir";
        fs::path dest_dir = tmp / "destination_dir";

        fs::create_directory(src_dir);
        fs::path src_subfile = src_dir / "subfile.txt";
        {
            std::ofstream f(src_subfile.string());
            f << "subfile content";
        }

        // 复制目录（递归）
        fs::copy(src_dir, dest_dir, fs::copy_options::recursive);
        DOCTEST_CHECK(fs::exists(dest_dir));
        DOCTEST_CHECK(fs::exists(dest_dir / "subfile.txt"));

        fs::remove_all(tmp);
    }

    /**
     * 测试用例 10：硬链接与符号链接
     *
     * 功能说明：
     * - 演示 fs::create_hard_link 创建硬链接
     * - 演示 fs::create_symlink 创建符号链接
     * - 演示 fs::hard_link_count 查询硬链接计数
     * - 演示 fs::equivalent 判断两个路径是否指向同一文件
     * - 演示 fs::is_symlink 判断是否为符号链接
     * - 演示 fs::read_symlink 读取符号链接目标
     * - 注意：Windows 平台可能需要管理员权限，权限不足时跳过
     */
    DOCTEST_TEST_CASE("FilesystemLinks")
    {
        auto tmp = make_temp_dir("links");
        fs::path target_file = tmp / "link_target.txt";
        fs::path hard_link_path = tmp / "hard_link.txt";
        fs::path sym_link_path = tmp / "sym_link.txt";

        // 创建目标文件
        {
            std::ofstream f(target_file.string());
            f << "link target";
        }
        DOCTEST_CHECK(fs::exists(target_file));

        bool hard_link_ok = false;
        bool sym_link_ok = false;

        try
        {
            // 创建硬链接
            fs::create_hard_link(target_file, hard_link_path);
            hard_link_ok = true;
            DOCTEST_CHECK(fs::exists(hard_link_path));
            DOCTEST_CHECK(fs::hard_link_count(target_file) >= 2);
            DOCTEST_CHECK(fs::equivalent(target_file, hard_link_path));

            // 创建符号链接
            fs::create_symlink(target_file, sym_link_path);
            sym_link_ok = true;
            DOCTEST_CHECK(fs::exists(sym_link_path));
            DOCTEST_CHECK(fs::is_symlink(sym_link_path));

            // 测试 read_symlink
            fs::path link_target = fs::read_symlink(sym_link_path);
            DOCTEST_CHECK(link_target == target_file);
        }
        catch (const fs::filesystem_error&)
        {
            // 某些平台可能需要特殊权限，跳过
        }

        // 清理 — 只删除确实创建成功的文件
        if (hard_link_ok)
        {
            fs::remove(hard_link_path);
        }
        if (sym_link_ok)
        {
            fs::remove(sym_link_path);
        }
        fs::remove(target_file);
    }

    /**
     * 测试用例 11：磁盘空间信息查询
     *
     * 功能说明：
     * - 演示 fs::space 获取磁盘容量、空闲空间和可用空间
     * - 验证 capacity > 0、free > 0、available > 0（均为无符号整数，使用 > 0 有意义）
     * - 验证 capacity >= free >= available 的逻辑关系
     * - 验证对不存在的路径调用 fs::space 会抛出 filesystem_error
     */
    DOCTEST_TEST_CASE("FilesystemSpace")
    {
        // 测试空间信息
        fs::path current_dir(".");
        auto space_info = fs::space(current_dir);
        DOCTEST_CHECK(space_info.capacity > 0);
        DOCTEST_CHECK(space_info.free > 0);
        DOCTEST_CHECK(space_info.available > 0);
        DOCTEST_CHECK(space_info.capacity >= space_info.free);
        DOCTEST_CHECK(space_info.free >= space_info.available);

        // 测试不存在的路径应该抛异常
        bool caught = false;
        try
        {
            std::ignore = fs::space("non_existent_dir_xyz");
        }
        catch (const fs::filesystem_error&)
        {
            caught = true;
        }
        DOCTEST_CHECK(caught);
    }

    /**
     * 测试用例 12：文件大小调整（resize_file）
     *
     * 功能说明：
     * - 演示 fs::resize_file 扩大文件大小
     * - 演示 fs::resize_file 缩小文件大小
     * - 验证调整后 file_size 与目标大小一致
     */
    DOCTEST_TEST_CASE("FilesystemResize")
    {
        auto tmp = make_temp_dir("resize");
        fs::path test_file = tmp / "test_resize.txt";
        {
            std::ofstream f(test_file.string());
            f << "test content";
        }

        // 调整文件大小（扩大）
        fs::resize_file(test_file, 100);
        DOCTEST_CHECK(fs::file_size(test_file) == 100);

        // 调整文件大小（缩小）
        fs::resize_file(test_file, 10);
        DOCTEST_CHECK(fs::file_size(test_file) == 10);

        fs::remove_all(tmp);
    }

    /**
     * 测试用例 13：文件最后修改时间（last_write_time）
     *
     * 功能说明：
     * - 演示 fs::last_write_time 获取文件最后修改时间
     * - 验证修改文件后，last_write_time 不会早于修改前
     * - 演示 fs::last_write_time 设置文件最后修改时间
     * - 验证设置后再读取，时间戳与设置值一致
     */
    DOCTEST_TEST_CASE("FilesystemLastWriteTime")
    {
        auto tmp = make_temp_dir("lwt");
        fs::path test_file = tmp / "test_lwt.txt";
        {
            std::ofstream f(test_file.string());
            f << "test";
        }

        // 获取最后修改时间
        auto ftime = fs::last_write_time(test_file);
        // 等待一小段时间后修改文件
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        {
            std::ofstream f(test_file.string(), std::ios::app);
            f << " more";
        }
        auto ftime2 = fs::last_write_time(test_file);
        DOCTEST_CHECK(ftime2 >= ftime);

        // 设置最后修改时间
        fs::last_write_time(test_file, ftime);
        auto ftime3 = fs::last_write_time(test_file);
        DOCTEST_CHECK(ftime3 == ftime);

        fs::remove_all(tmp);
    }

    /**
     * 测试用例 14：临时目录路径
     *
     * 功能说明：
     * - 演示 fs::temp_directory_path 获取系统临时目录
     * - 验证临时目录存在、是目录、且为绝对路径
     */
    DOCTEST_TEST_CASE("FilesystemTempDirectory")
    {
        fs::path tmp = fs::temp_directory_path();
        DOCTEST_CHECK(fs::exists(tmp));
        DOCTEST_CHECK(fs::is_directory(tmp));
        DOCTEST_CHECK(tmp.is_absolute());
    }

    /**
     * 测试用例 15：fstream 直接接受 fs::path 参数（C++17 特性）
     *
     * 功能说明：
     * - 演示 std::ofstream / std::ifstream 构造时直接接受 fs::path 参数
     * - 验证写入后可正确读取文件内容
     * - 这是 C++17 新增的 filesystem 与 fstream 的互操作特性
     */
    DOCTEST_TEST_CASE("FilesystemFStreamWithFsPath")
    {
        // 测试 fstream 直接接受 fs::path（C++17 特性）
        auto tmp = make_temp_dir("fstream_path");
        fs::path test_file = tmp / "test_path.txt";

        {
            std::ofstream f(test_file); // 直接传 fs::path
            DOCTEST_CHECK(f.is_open());
            f << "path test";
        }

        {
            std::ifstream f(test_file);
            DOCTEST_CHECK(f.is_open());
            std::string content;
            std::getline(f, content);
            DOCTEST_CHECK(content == "path test");
        }

        fs::remove_all(tmp);
    }
}