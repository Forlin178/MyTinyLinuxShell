#ifndef COMMANDMANAGER_H_
#define COMMANDMANAGER_H_

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include "command.h"
#include "exec_command.h"
#include "pipe_command.h"

namespace msh {

/* 包含tab、换行 没有空格 */
const std::string WHITE_SPACE = "\t\r\n";

class CommandManager{
public:
    /* env message */
    std::vector<std::string> history_;
    std::string user_name_;
    std::string cur_word_dir_;
    std::string home_dir_;
    std::string host_name_;


    CommandManager() = default;
    ~CommandManager() = default;

    /* 按delims分割字符串 */
    static auto StringSplit(const std::string &s, const std::string delims) -> std::vector<std::string>;
    /* 判断字符是否属于 缩进、空格、回车 */
    static auto IsWhiteSpace(char ch) -> bool { return WHITE_SPACE.find(ch) != -1; }
    /* 裁剪掉两边多余的缩进、换行 */
    static auto Trim(const std::string &s) -> std::string;

    /* shell把命令直接丢给Parser就完事了 */
    auto Parser(const std::string &line) -> int;
    /* 处理内建命令 */
    auto ProcessBuiltinCommand(const std::string &line) ->int;
    /* 对fork操作的包装 */
    auto ForkWrap() -> int;
    /* 对exec和pipe进行处理 */
    auto ExecPipeParser(const std::string &line) -> std::shared_ptr<Command>;
    /* 运行exec或pipe命令 */
    void Run(std::shared_ptr<Command> cmd);
};
    
}

#endif /* COMMANDMANAGER_H_ */