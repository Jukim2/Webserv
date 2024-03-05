#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	int outward_fd[2];
	int inward_fd[2];

	pipe(outward_fd);
	pipe(inward_fd);

	fcntl(outward_fd[0], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	fcntl(outward_fd[1], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	fcntl(inward_fd[0], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	fcntl(inward_fd[1], F_SETFL, O_NONBLOCK, FD_CLOEXEC);

	char **envp = (char **)calloc(6, sizeof(char *));
	envp[0] = "SERVER_PROTOCOL=HTTP/1.1";
	envp[1] = "PATH_INFO=/";
	envp[2] = "REQUEST_METHOD=GET";
	envp[3] = "CONTENT_TYPE=text/plain";
	envp[4] = "CONTENT_LENGTH=10";
	envp[5] = 0;

	int pid = fork();
	if (pid == 0)
	{
		close(outward_fd[0]);
		close(inward_fd[1]);
		dup2(outward_fd[1], STDOUT_FILENO);
		dup2(inward_fd[0], STDIN_FILENO);

		execve(argv[1], argv, envp);
	}
	else
	{
		close(outward_fd[1]);
		close(inward_fd[0]);

		char buffer[1024 + 1];
		ssize_t bytes_read;
		std::ostringstream output;

		char *str = (char *)calloc(100000000, sizeof(char));
		for (int i = 0; i < 100000000; i++)
			str[0] = 'a';
		write(inward_fd[1], str, 100000000);
		int status;

		fd_set read_fds;
		FD_ZERO(&read_fds);
		FD_SET(outward_fd[0], &read_fds);

		struct timeval timeout;
		timeout.tv_sec = 5; // 5 seconds timeout
		timeout.tv_usec = 0;

		while (true)
		{
			int ready = select(outward_fd[0] + 1, &read_fds, NULL, NULL, &timeout);
			if (ready == -1)
			{
				perror("select");
				exit(EXIT_FAILURE);
			}
			else if (ready == 0)
			{
				std::cout << "Timeout occurred" << std::endl;
			}
			else
			{
				bytes_read = read(outward_fd[0], buffer, 1024);
				if (bytes_read == 0)
					break;
				if (bytes_read > 0)
					output.write(buffer, bytes_read);
				if (bytes_read < 0)
					perror("jinho hye");
				std::cout << "bytes_read:" << bytes_read << std::endl;
			}
		}

		close(inward_fd[1]);
		close(outward_fd[0]);
	}
	return 0;
}
