#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main() {
    double weight, height, bmi;
    int pipe_fd[2];
    pid_t child_pid;

    // Create a pipe to communicate between parent and child processes
    if (pipe(pipe_fd) == -1) {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }

    // Create a child process
    if ((child_pid = fork()) == -1) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
if (child_pid == 0) {
        // Child process calculates BMI
        close(pipe_fd[0]); // Close the read end of the pipe

        printf("Enter your weight (in kg): ");
        scanf("%lf", &weight);

        printf("Enter your height (in meters): ");
        scanf("%lf", &height);

        // Calculate BMI
        bmi = weight / (height * height);

        // Write the BMI value to the pipe
        write(pipe_fd[1], &bmi, sizeof(double));

        close(pipe_fd[1]); // Close the write end of the pipe

        // Replace the child process with the 'clear' command to clear the terminal
        execlp("clear", "clear", NULL);

        // If exec fails, report an error
        perror("Exec failed");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        close(pipe_fd[1]); // Close the write end of the pipe

        // Read the BMI value from the pipe
        double received_bmi;
        read(pipe_fd[0], &received_bmi, sizeof(double));

        // Close the read end of the pipe
        close(pipe_fd[0]);

        printf("Your BMI is: %.2lf\n", received_bmi);

        // Classify the BMI
        if (received_bmi < 18.5)
            printf("Underweight\n");
        else if (received_bmi >= 18.5 && received_bmi < 25)
            printf("Normal weight\n");
        else if (received_bmi >= 25 && received_bmi < 30)
            printf("Overweight\n");
        else
            printf("Obese\n");

        char command[10];
        printf("Enter 'exit' to quit: ");
        scanf("%s", command);

        if (strcmp(command, "exit") == 0) {
            // Send a signal to the child process to terminate it gracefully
            kill(child_pid, SIGTERM);

            // Wait for the child process to exit
            wait(NULL);

            printf("Parent process exiting.\n");
            exit(EXIT_SUCCESS);
        }
    }

    return 0;
}
