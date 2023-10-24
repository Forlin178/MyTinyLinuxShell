#include "includes/my_shell.h"

#define CHAR_BUF_SIZE 512

namespace msh {

void MyShell::Loop() {
    int state{1};
    while (state)
    {
        ShowCmdPrompt();
        std::string line;
        std::getline(std::cin, line);
        // test
        // std::cout << line << std::endl;
        // 丢给cmd_manager分析并执行 得到state
        state = this->cmd_mng_.Parser(cmd_mng_.Trim(line));
    }
}

void MyShell::ShowCmdPrompt() {
    char char_buf[CHAR_BUF_SIZE];

    // 获取user_name
    passwd *pwd = getpwuid(getuid());
    this->cmd_mng_.user_name_ = pwd->pw_name;

    // 获取当前工作目录
    getcwd(char_buf, CHAR_BUF_SIZE);
    this->cmd_mng_.cur_word_dir_ = char_buf;

    // 设置home目录
    if (this->cmd_mng_.user_name_ == "root") {
        this->cmd_mng_.home_dir_ = "/root";
    } else {
        this->cmd_mng_.home_dir_ = "/home/" + this->cmd_mng_.user_name_;
    }

    // 省略工作目录路径
    // 只保留最后一级目录
    if (cmd_mng_.cur_word_dir_ == cmd_mng_.home_dir_) {
        this->cmd_mng_.cur_word_dir_ = "~";
    } else if(cmd_mng_.cur_word_dir_ != "/") {
        auto temp_vector = CommandManager::StringSplit(cmd_mng_.cur_word_dir_, "/");
        cmd_mng_.cur_word_dir_ = temp_vector[temp_vector.size() - 1];
    }

    gethostname(char_buf, CHAR_BUF_SIZE);
    this->cmd_mng_.host_name_ = char_buf;
    auto temp_vector = CommandManager::StringSplit(cmd_mng_.host_name_, ".");
    cmd_mng_.host_name_ = temp_vector[0];

    std::cout << cmd_mng_.user_name_ << "@" << cmd_mng_.host_name_ << ":" << cmd_mng_.cur_word_dir_ << "% ";
}



}