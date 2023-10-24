#ifndef PIPE_COMMAND_H_
#define PIPE_COMMAND_H_

#include <memory>
#include "command.h"

namespace msh {

/* 管道命令 */
class PipeCommand : public Command
{
public:
    std::shared_ptr<Command> left_cmd_;
    std::shared_ptr<Command> right_cmd_;

    PipeCommand() { this->type_ = Command_Type::CMD_TYPE_PIPE; }
    PipeCommand(std::shared_ptr<Command> left_cmd, std::shared_ptr<Command> right_cmd)
        : left_cmd_(left_cmd), right_cmd_(right_cmd) {
        this->type_ = Command_Type::CMD_TYPE_PIPE;
    }
    ~PipeCommand() = default;
};

}

#endif