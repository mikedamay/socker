static void ssleep(int seconds)
{
    struct timeval tv;
    tv.tv_sec = seconds;
    tv.tv_usec = 0;
    select(0, NULL, NULL, NULL, &tv);

}

