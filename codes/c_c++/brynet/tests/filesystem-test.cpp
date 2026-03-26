#include "doctest/doctest.h"
#include "fmt/format.h"
#include <fstream>
#include <filesystem>

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
}