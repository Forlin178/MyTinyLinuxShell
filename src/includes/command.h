#ifndef COMMAND_H_
#define COMMAND_H_

namespace msh {

enum Command_Type{
    CMD_TYPE_NULL,
    CMD_TYPE_EXEC,
    CMD_TYPE_PIPE,
};

/* 除了内部命令以外的命令基类 */
class Command
{
public:
    Command_Type type_;

    Command() { this->type_ = Command_Type::CMD_TYPE_NULL; }
    ~Command() = default;
};

}

#endif /* COMMAND_H_ */