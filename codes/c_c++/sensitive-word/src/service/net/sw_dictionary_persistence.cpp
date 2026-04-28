#include "net/sw_dictionary_persistence.h"
#include "sw/word_dictionary_loader.h"
#include <filesystem>
#include <fstream>
#include <system_error>
#include <utility>
#include <vector>

namespace net {
namespace {

std::set<std::string> load_word_set_from_file(const std::string& path)
{
    std::set<std::string> result;
    for (auto& word : sensitive_word::load_words_from_file(path))
        result.insert(std::move(word));

    return result;
}

bool write_word_set_to_stream(std::ofstream& output, const std::set<std::string>& words)
{
    for (const auto& word : words)
        output << word << '\n';

    output.flush();
    return output.good();
}

bool write_word_set_to_temp_file(const std::filesystem::path& temp_path,
                                 const std::set<std::string>& words)
{
    std::ofstream output(temp_path, std::ios::binary | std::ios::trunc);
    if (!output.is_open())
        return false;

    return write_word_set_to_stream(output, words);
}

bool move_existing_file_to_backup(const std::filesystem::path& target_path,
                                  const std::filesystem::path& backup_path)
{
    std::error_code ec;
    std::filesystem::remove(backup_path, ec);
    ec.clear();

    if (!std::filesystem::exists(target_path, ec))
        return true;

    ec.clear();
    std::filesystem::rename(target_path, backup_path, ec);
    return !ec;
}

bool promote_temp_file(const std::filesystem::path& temp_path,
                       const std::filesystem::path& target_path)
{
    std::error_code ec;
    std::filesystem::rename(temp_path, target_path, ec);
    if (ec)
    {
        std::filesystem::remove(temp_path, ec);
        return false;
    }

    return true;
}

void cleanup_file(const std::filesystem::path& path)
{
    std::error_code ec;
    std::filesystem::remove(path, ec);
}

bool restore_backup_file(const std::filesystem::path& backup_path,
                         const std::filesystem::path& target_path)
{
    std::error_code ec;
    if (!std::filesystem::exists(backup_path, ec))
        return true;

    ec.clear();
    std::filesystem::remove(target_path, ec);
    ec.clear();
    std::filesystem::rename(backup_path, target_path, ec);
    return !ec;
}

} // namespace

word_repository load_word_repository(const sw_http_server_config& config)
{
    word_repository repository;
    repository.deny_words  = load_word_set_from_file(config.deny_file_path);
    repository.allow_words = load_word_set_from_file(config.allow_file_path);
    return repository;
}

sensitive_word::sensitive_word_engine build_engine_from_repository(
    const sensitive_word::sensitive_word_config& config,
    const word_repository&                       repository)
{
    sensitive_word::sensitive_word_builder builder;
    builder.ignore_case(config.ignore_case)
           .ignore_width(config.ignore_width)
           .ignore_num_style(config.ignore_num_style)
           .ignore_chinese_style(config.ignore_chinese_style)
           .ignore_english_style(config.ignore_english_style)
           .ignore_repeat(config.ignore_repeat)
           .enable_word_check(config.enable_word_check)
           .enable_num_check(config.enable_num_check)
           .num_check_len(config.num_check_len);

    std::vector<std::string> deny_list;
    deny_list.reserve(repository.deny_words.size());
    for (const auto& word : repository.deny_words)
        deny_list.push_back(word);

    std::vector<std::string> allow_list;
    allow_list.reserve(repository.allow_words.size());
    for (const auto& word : repository.allow_words)
        allow_list.push_back(word);

    builder.deny_words(std::move(deny_list));
    builder.allow_words(std::move(allow_list));
    return builder.build();
}

void apply_update_to_word_sets(word_repository& repository, const update_command& command)
{
    auto insert_words = [](std::set<std::string>& words_set, const std::vector<std::string>& words) {
        for (const auto& word : words)
            words_set.insert(word);
    };

    auto erase_words = [](std::set<std::string>& words_set, const std::vector<std::string>& words) {
        for (const auto& word : words)
            words_set.erase(word);
    };

    switch (command.op)
    {
        case update_op::add_deny:     insert_words(repository.deny_words, command.words); break;
        case update_op::remove_deny:  erase_words(repository.deny_words, command.words); break;
        case update_op::add_allow:    insert_words(repository.allow_words, command.words); break;
        case update_op::remove_allow: erase_words(repository.allow_words, command.words); break;
    }
}

update_command make_inverse_command(const update_command& command)
{
    update_command inverse = command;
    switch (command.op)
    {
        case update_op::add_deny:     inverse.op = update_op::remove_deny; break;
        case update_op::remove_deny:  inverse.op = update_op::add_deny; break;
        case update_op::add_allow:    inverse.op = update_op::remove_allow; break;
        case update_op::remove_allow: inverse.op = update_op::add_allow; break;
    }
    return inverse;
}

bool persist_word_repository(const sw_http_server_config& config,
                             const word_repository&       repository)
{
    namespace fs = std::filesystem;

    const fs::path deny_path(config.deny_file_path);
    const fs::path allow_path(config.allow_file_path);
    const fs::path deny_temp_path    = deny_path.string() + ".tmp";
    const fs::path allow_temp_path   = allow_path.string() + ".tmp";
    const fs::path deny_backup_path  = deny_path.string() + ".bak";
    const fs::path allow_backup_path = allow_path.string() + ".bak";

    std::error_code ec;
    if (deny_path.has_parent_path())
    {
        fs::create_directories(deny_path.parent_path(), ec);
        if (ec)
            return false;
    }

    ec.clear();
    if (allow_path.has_parent_path())
    {
        fs::create_directories(allow_path.parent_path(), ec);
        if (ec)
            return false;
    }

    if (!write_word_set_to_temp_file(deny_temp_path, repository.deny_words) ||
        !write_word_set_to_temp_file(allow_temp_path, repository.allow_words))
    {
        cleanup_file(deny_temp_path);
        cleanup_file(allow_temp_path);
        return false;
    }

    if (!move_existing_file_to_backup(deny_path, deny_backup_path) ||
        !move_existing_file_to_backup(allow_path, allow_backup_path))
    {
        restore_backup_file(deny_backup_path, deny_path);
        restore_backup_file(allow_backup_path, allow_path);
        cleanup_file(deny_temp_path);
        cleanup_file(allow_temp_path);
        return false;
    }

    if (!promote_temp_file(deny_temp_path, deny_path) ||
        !promote_temp_file(allow_temp_path, allow_path))
    {
        restore_backup_file(deny_backup_path, deny_path);
        restore_backup_file(allow_backup_path, allow_path);
        cleanup_file(deny_temp_path);
        cleanup_file(allow_temp_path);
        return false;
    }

    cleanup_file(deny_backup_path);
    cleanup_file(allow_backup_path);
    return true;
}

} // namespace net
