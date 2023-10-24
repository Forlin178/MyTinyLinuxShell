#ifndef EXEC_COMMAND_H_
#define EXEC_COMMAND_H_


#include <vector>
#include <string>
#include "command.h"

namespace msh {

/* 外部命令 */
class ExecCommand : public Command
{
public:
    std::vector<std::string> argv_;

    ExecCommand(std::vector<std::string> argv) {
        this->type_ = Command_Type::CMD_TYPE_EXEC;
        this->argv_ = argv;
    }
    ~ExecCommand() = default;
};

}

#endif /* EXEC_COMMAND_H_ */