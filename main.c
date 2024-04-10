#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <fcntl.h>

const char *get_mime_type(const char *file_ext)
{
    if (strcasecmp(file_ext, "html") == 0 || strcasecmp(file_ext, "htm") == 0)
    {
        return "text/html";
    }
    else if (strcasecmp(file_ext, "txt") == 0)
    {
        return "text/plain";
    }
    else if (strcasecmp(file_ext, "jpg") == 0 || strcasecmp(file_ext, "jpeg") == 0)
    {
        return "image/jpeg";
    }
    else if (strcasecmp(file_ext, "png") == 0)
    {
        return "image/png";
    }
    else
    {
        return "application/octet-stream";
    }
}

int get_file_descriptor(const char *directory, const char *file_name)
{
    char src_file[100];
    strcpy(src_file, directory);
    strcat(src_file, file_name);

    int file_descriptor = open(src_file, O_RDONLY);

    return file_descriptor;
}

int main() {
    char buffer[1024];
    const char* mimeType = get_mime_type("html");

    FILE *fptr;

// Open a file in read mode
    fptr = fopen("./src/index.html", "r");

// Store the content of the file
    char myString[2048*2];
    char myStringFormated[2048*2] = "";


    while(fgets(myString, 2048*2, fptr)) {
        strcat(myStringFormated, myString);
    }
    // Print the file content
    fclose(fptr);



    char *resp = (char *)malloc(2048 * sizeof(char)) ;
    sprintf(resp,"HTTP/1.0 200 OK\r\n"
                  "Server: webserver-c\r\n"
                  "Content-type: %s\n\r\n"
                  "%s\r\n",
                  mimeType,
                  myStringFormated);

    int socketWeb = socket(AF_INET, SOCK_STREAM, 0);
    if(socketWeb == -1)
    {
        perror("webserver (socket)");
        return 1;
    }
    printf("Socket created\n");
    struct sockaddr_in host_addr;
    int host_addrlen = sizeof (host_addr);

    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(8080);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(socketWeb, (struct sockaddr *)&host_addr, host_addrlen) != 0)
    {
        perror("Webserver (bind)");
        return 1;
    }
    printf("Socket bind\n");

    if(listen(socketWeb, 1) != -0)
    {
        printf("Socket bind\n");
        perror("Webserver (listen)");
        return 1;
    }
    printf("Socket listening\n");

    for(;;)
    {
        int newsocketfd = accept(socketWeb, (struct sockaddr *)&host_addr, (socklen_t *)&host_addrlen);
        if(newsocketfd < 0)
        {
            perror("Webserver (connection)");
            continue;
        }
        printf("connexion accepté\n");

        int valread = read(newsocketfd, buffer, 1024);
        if (valread < 0) {
            perror("webserver (read)");
            continue;
        }

        //int valwrite = write(newsocketfd, resp, strlen(resp));
        int valwrite = send(newsocketfd, resp, strlen(resp), 0);
        if (valwrite < 0) {
            perror("webserver (write)");
            continue;
        }

        close(newsocketfd);
    }

    //return 0;
}

