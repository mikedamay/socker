
#define NULL (void *)0

int main()
{
    struct {short fam; short port; int ip4; int z0; int z1;}
      myaddr = {2, 31790, 16777343, 0, 0 };
    int sd = socket(2, 1, 0);
    bind(sd, (const struct sockaddr *)&myaddr, 16);
    listen(sd, 10);
    int pid = fork();
    if (pid != 0)
    {
        int sd2 = accept(sd, NULL, NULL);
        while (1)
        {
            char buffer[1024];
            int numBytes = read(sd2, buffer, sizeof buffer);
            if (numBytes == 0 )
                break;
        }
        sleep(20);
        return 253;
    }
    else
    {
        int sd3 = socket(2, 1, 0);
        int res = connect(sd3, (const struct sockaddr *)&myaddr, 16);
        int numBytes = write(sd3, "now is the time", 16);
        close(sd3);
        return numBytes;
    }
}