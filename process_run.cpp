/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright 2015 Sputnik7
 */

#include "process_run.h"
#include "prints.h"

#include <stdio.h>
#include <stdexcept>
/*
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
*/

/**
 * @brief process_run::process_run
 *  execute command and capture output
 *  from http://stackoverflow.com/a/478960/874024
 */
process_run::process_run(std::string cmd, std::string pwd, bool add_stderr)
{
    std::string arg;

    if (!pwd.empty())
        arg += "echo " + pwd + " | sudo -S " + cmd;
    else
        arg += cmd;

    if (add_stderr)
        arg += " 2>&1";

    FILE* pipe = popen(arg.c_str(), "r");
    if (!pipe)
        throw std::invalid_argument("popen '" + cmd + "'' failed");

    char buffer[1 << 10];
    while (!feof(pipe))
        if (fgets(buffer, sizeof(buffer), pipe))
            result += buffer;

    pclose(pipe);

    if (!pwd.empty()) {
        // remove eventual warning
        if (result.substr(0, 6) == "[sudo]") {
            auto nl = result.find('\n');
            if (nl != std::string::npos)
                result = result.substr(nl + 1);
        }
    }
}

#if 0
int
__main(int argc, char *argv[])
{
   int pipefd[2];
   pid_t cpid;
   char buf;

   if (argc != 2) {
    fprintf(stderr, "Usage: %s <string>\n", argv[0]);
    exit(EXIT_FAILURE);
   }

   if (pipe(pipefd) == -1) {
       perror("pipe");
       exit(EXIT_FAILURE);
   }

   cpid = fork();
   if (cpid == -1) {
       perror("fork");
       exit(EXIT_FAILURE);
   }

   if (cpid == 0) {    /* Child reads from pipe */
       close(pipefd[1]);          /* Close unused write end */

       while (read(pipefd[0], &buf, 1) > 0) {
           write(STDOUT_FILENO, &buf, 1);
           write(STDOUT_FILENO, &buf, 1);
        }

       write(STDOUT_FILENO, "\n", 1);
       close(pipefd[0]);
       _exit(EXIT_SUCCESS);

   } else {            /* Parent writes argv[1] to pipe */
       close(pipefd[0]);          /* Close unused read end */
       write(pipefd[1], argv[1], strlen(argv[1]));
       close(pipefd[1]);          /* Reader will see EOF */
       wait(NULL);                /* Wait for child */
       exit(EXIT_SUCCESS);
   }
}
#endif
