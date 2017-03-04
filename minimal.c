
/**
 * create a server and fork a client which sends the server a single message both processes then exit.
 * where does gcc get sockaddr from - presumably comes with system calls
 * @return 253 is returned from the server socket, number of bytes written is returned by the client
 */
int main()
{
    // ip address 127.0.0.1 (16777343) port 11900 (31790)
    struct {short fam; short port; int ip4; int z0; int z1;}
      myaddr = {2, 31790, 16777343, 0, 0 };
    int sd = socket(2, 1, 0);
    bind(sd, (const struct sockaddr *)&myaddr, 16);
    listen(sd, 10);
    int pid = fork();
    if (pid != 0)   // server
    {
        int sd2 = accept(sd, (void *)0, (void *)0);
        while (1)
        {
            char buffer[1024];
            int numBytes = read(sd2, buffer, sizeof buffer);
            if (numBytes == 0 )
                break;
        }
        return 253;
    }
    else            // client
    {
        int sd3 = socket(2, 1, 0);
        int res = connect(sd3, (const struct sockaddr *)&myaddr, 16);
        int numBytes = write(sd3, "now is the time", 16);
        close(sd3);
        write(1, numBytes == 16 ? "16 bytes written" : "wrong byte count");
        return numBytes;
    }
}