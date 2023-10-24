#ifndef MYSHELL_H_
#define MHSHELL_H_

#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include "command_manager.h"

namespace msh {

class MyShell{
private:
    CommandManager cmd_mng_;
public:
    MyShell() = default;
    ~MyShell() = default;

    void ShowCmdPrompt();
    void Loop();
};
    
}

#endif /* MHSHELL_H_ */