#include "includes/command_manager.h"

namespace msh {

auto CommandManager::StringSplit(const std::string &s, const std::string delims) -> std::vector<std::string> {
    std::vector<std::string> res;
    int p = 0, q;
    while (p < s.size() && (q = s.find_first_of(delims, p)) != std::string::npos) {
        if (q > p)
            res.emplace_back(s.substr(p, q - p));
        p = q + 1;
    }
    if (p < s.size()) {
        res.emplace_back(s.substr(p));
    }
    return res;
}

auto CommandManager::Trim(const std::string &s) -> std::string {
    if (s.size() == 0) {
        return s;
    }
    int p = 0, q = s.size() - 1;
    while (IsWhiteSpace(s[p]))  p++;
    while (IsWhiteSpace(s[q]))  q--;
    return s.substr(p, q - p + 1);
}

auto CommandManager::ProcessBuiltinCommand(const std::string &line) -> int {
    // cd
    if (line == "cd") {
        // 默认回到home
        auto ret = chdir(this->home_dir_.c_str());
        if (ret < 0) {
            perror("change dir error");
        }
        return 1;
    } else if (line.substr(0, 2) == "cd") {
        // 获取cd之后的内容 ( cd xxxx -> temp=xxxx )
        auto temp_str = StringSplit(line, " ")[1];
        if (temp_str.find("~") == 0) {
            temp_str = home_dir_ + temp_str.substr(1);
        }

        auto ret = chdir(Trim(temp_str).c_str());
        if (ret < 0) {
            perror("change dir error");
        }
        return 1;
    }
    
    // pwd
    if (line == "pwd") {
        char char_buf[512];
        getcwd(char_buf, 512);
        std::cout << char_buf << std::endl;
        return 1;
    }

    // exit
    if (line == "exit") {
        std::cout << "Exit the terminal" << std::endl;
        exit(0);
    }

    // help
    if (line == "help") {
        std::cout << "有以下内建命令: " << std::endl;
        std::cout << "1.cd" << std::endl;
        std::cout << "2.pwd" << std::endl;
        std::cout << "3.exit" << std::endl;
        std::cout << "4.history" << std::endl;
        std::cout << "5.help" << std::endl;
        return 1;
    }

    // history
    if (line == "history") {
        for (auto iter = history_.begin(); iter != history_.end(); ++iter) {
            std::cout << *iter << std::endl;
        }
        return 1;
    }

    // 不是内建命令
    return 0;
}

auto CommandManager::ForkWrap() -> int {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork error");
    }
    return pid;
}

auto CommandManager::ExecPipeParser(const std::string &line) -> std::shared_ptr<Command> {
    std::string cur_read = "";
    std::shared_ptr<Command> cur_cmd = std::make_shared<Command>();

    for (size_t i = 0; i < line.size(); i++) {
        if (i < line.size() - 1 && line[i] == '|' && line[i+1] == '|') {
            auto cmd_right = ExecPipeParser(line.substr(i + 2));
            if (cur_cmd->type_ == Command_Type::CMD_TYPE_NULL) {
                cur_cmd = std::make_shared<ExecCommand>(StringSplit(cur_read, " "));
            }
            auto pipe_cmd = std::make_shared<PipeCommand>(cur_cmd, cmd_right);
            return pipe_cmd;
        } else {
            cur_read =cur_read + line[i];
        }
    }
    
    if (cur_cmd->type_ == Command_Type::CMD_TYPE_NULL) {
        cur_cmd = std::make_shared<ExecCommand>(StringSplit(cur_read, " "));
    }
    return cur_cmd;
}

void CommandManager::Run(std::shared_ptr<Command> cmd) {
    switch (cmd->type_)
    {
    case Command_Type::CMD_TYPE_EXEC : {
        // exec 外部命令
        auto exec_cmd = std::static_pointer_cast<ExecCommand>(cmd);

        /* vector<string> -> char ** */
        /* vector<string> -> vector<char *> */
        /* vector<char *> -> char ** */
        std::vector<char *> vec_char_ptr;
        for (const auto &str : exec_cmd->argv_) {
            vec_char_ptr.emplace_back(const_cast<char*>(str.c_str()));
        }
        vec_char_ptr.emplace_back(nullptr);

        char *char_arr[vec_char_ptr.size()];
        for (size_t i = 0; i < vec_char_ptr.size(); ++i) {
            char_arr[i] = vec_char_ptr[i];
        }
        /* conversion finish */

        // 执行exec 后面的程序全被替换，不会回来了
        int ret = execvp(char_arr[0], char_arr);
        if (ret < 0) {
            perror("execvp error");
        }
        break;
    }
    case Command_Type::CMD_TYPE_PIPE : {
        // pipe 命令
        // https://cloud.tencent.com/developer/article/1361656
        auto pipe_cmd = std::static_pointer_cast<PipeCommand>(cmd);

        int pipe_fd[2];
        int ret = pipe(pipe_fd);
        if (ret < 0) {
            perror("pipr error");
        }

        if (ForkWrap() == 0) {
            // 关闭读端(不使用)
            close(pipe_fd[0]);
            // 关掉stdout(fileno=2) 将fileno=2与管道写端关联
            // exec命令默认输出到fileno=2(stdout)
            /* left_cmd stdout -> pipe_write */
            dup2(pipe_fd[1], STDOUT_FILENO);
            // 输出到stdout(管道写端)
            Run(pipe_cmd->left_cmd_);
            // 写完记得关掉
            // 不一定能执行 如果left_cmd是exec会因为执行exec而不再回来
            close(pipe_fd[1]);
        }
        if (ForkWrap() == 0) {
            close(pipe_fd[1]);
            /* pipe_read -> right_cmd stdin */
            dup2(pipe_fd[0], STDIN_FILENO);
            Run(pipe_cmd->right_cmd_);
            close(pipe_fd[0]);
        }
        
        // 确保能关闭
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        wait(nullptr);
        wait(nullptr);
        break;
    }
    default:{
        std::cout << "command type is null" << std::endl;
        break;
    }
    }
}

auto CommandManager::Parser(const std::string &line) -> int {
    // 记录历史命令
    this->history_.emplace_back(line);
    
    // 处理内部命令
    if (ProcessBuiltinCommand(line) > 0) {
        return 1;
    }

    // 处理外部、管道命令
    // 子进程
    if (ForkWrap() == 0) {
        auto cmd = ExecPipeParser(line);
        Run(cmd);
        // 子进程exit
        exit(0);
    }
    wait(nullptr);

    return 1;
}

}