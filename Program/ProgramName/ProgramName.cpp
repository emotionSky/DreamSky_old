#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

//ps -eo pid,ppid,sid,tty,pgrp,comm,cmd | grep -E 'bash|PID|ProgramName'

#define MASTER_PROCESS_PROC 0 //master process
#define WORKER_PROCESS_PROC 1 //worker process

/* 这里直接使用变量的方式来实现，实际上应该可以通过配置文件来实现 */
int g_needDaemon = 1; //是否需要创建守护进程
int g_subNum = 2;     //创建的worker进程数目

/* 进程名称 */
static char master_name[] ="master process";

/* 全局变量，用于存储参数、环境变量的内容 */
size_t g_argvMemSize = 0; //用于存储参数的内存大小
size_t g_envMemSize  = 0;  //用于存储环境变量的内存大小
int    g_argc        = 0;        //用于存储参数的个数
char** g_argv        = nullptr;  //用于存储参数的数组指针
char*  g_env         = nullptr;    //用于存储环境变量的内存指针
int    g_daemonFlag  = 0;     //是否创建了守护进程的标志

/* 进程相关的全局变量 */
pid_t g_ownPid; //当前进程的pid
pid_t g_parentPid; //父进程的pid
int   g_processFlag; //进程类型，比如master,worker等
int   g_exitFlag; //程序退出标志位

/**
 * @brief 备份程序的环境变量
 * @return void
 */
void backup_proc_env()
{
    g_env = new char[g_envMemSize];
    memset(g_env, 0, g_envMemSize);
    char* pTmp = g_env;

    size_t size = 0;
    //将原来的内存内容备份到新的内存中
    for(int index = 0; environ[index]; ++index) //判断条件就看对应的是否不为空指针
    {
        size = strlen(environ[index]) + 1; //最后的'\0'也是需要占据一个字节
        strcpy(pTmp, environ[index]);     //将原先的内容进行拷贝
        environ[index] = pTmp; //让环境变量指针指向新的内存地址
        pTmp += size; //指针移动
    }
}

/**
 * @brief 初始化守护进程
 * @return -1表示初始化失败，1表示父进程，0表示子进程
 */
int init_daemon()
{
    //1.创建守护进程需要fork一个子进程
    switch (fork()) //fork出来的子进程就是我们需要的那个守护进程，即nginx中的master进程
    {
    case -1:
        //创建子进程失败
        return -1;
        break;
    
    case 0:
        //子进程创建成功
        break;

    default:
        //这是父进程
        return 1;
    }

    //2.只有fork出来的子进程才继续如下的代码
    g_parentPid = g_ownPid; //g_ownPid是原来父进程的pid，而这里是子进程，那么他就是现在子进程的父进程pid
    g_ownPid = getpid();    //重新获取当前进程的pid

    //3.设置脱离终端，即终端关闭后，子进程不会关闭
    if(setsid() == -1)
    {
        //说明脱离失败了
        return -1;
    }

    //如下的配置都是为了配合log模块进行处理的
    //4.设置为0，不让它来限制文件权限，避免引起混乱
    umask(0);

#if 0
    //6.打开黑洞设备
    int fd = open("/dev/null", O_RDWR);
    if (fd == -1) 
    {
        int err = errno;
        return -1;
    }
    if (dup2(fd, STDIN_FILENO) == -1) //先关闭STDIN_FILENO[这是规矩，已经打开的描述符，动他之前，先close]，类似于指针指向null，让/dev/null成为标准输入；
    {
        int err = errno;
        return -1;
    }
    if (dup2(fd, STDOUT_FILENO) == -1) //再关闭STDIN_FILENO，类似于指针指向null，让/dev/null成为标准输出；
    {
        int err = errno;
        return -1;
    }
    if (fd > STDERR_FILENO)  //fd应该是3，这个应该成立
     {
        if (close(fd) == -1)  //释放资源这样这个文件描述符就可以被复用；不然这个数字【文件描述符】会被一直占着；
        {
            int err = errno;
            return -1;
        }
    }
#endif
    return 0; //子进程返回0
}

/**
 * @brief 释放资源
 * @return void
 */
void free_resource()
{
    if(g_env)
    {
        delete[] g_env;
        g_env = nullptr;
    }

    //其他的资源释放
}

/**
 * @brief 设置可执行程序的标题
 * @param[in] title 程序的标题
 * @return void
 */
void set_proc_title(const char *title)
{
    //我们假设，所有的命令行参数我们都不需要用到了，可以被随意覆盖了；
    //注意：我们的标题长度，不会长到原始标题和原始环境变量都装不下，否则怕出问题，不处理
    
    //(1)计算新标题长度
    size_t ititlelen = strlen(title); 

    //(2)计算总的原始的argv那块内存的总长度【包括各种参数】    
    size_t esy = g_argvMemSize + g_envMemSize; //argv和environ内存总和
    if( esy <= ititlelen)
    {
        //你标题多长啊，我argv和environ总和都存不下？注意字符串末尾多了个 \0，所以这块判断是 <=【也就是=都算存不下】
        return;
    }

    //空间够保存标题的，够长，存得下，继续走下来    

    //(3)设置后续的命令行参数为空，表示只有argv[]中只有一个元素了，这是好习惯；防止后续argv被滥用，因为很多判断是用argv[] == NULL来做结束标记判断的;
    g_argv[1] = nullptr;  

    //(4)把标题弄进来，注意原来的命令行参数都会被覆盖掉，不要再使用这些命令行参数,而且g_os_argv[1]已经被设置为NULL了
    char *ptmp = g_argv[0]; //让ptmp指向g_os_argv所指向的内存
    strcpy(ptmp, title);
    ptmp += ititlelen; //跳过标题

    //(5)把剩余的原argv以及environ所占的内存全部清0，否则会出现在ps的cmd列可能还会残余一些没有被覆盖的内容；
    size_t cha = esy - ititlelen;  //内存总和减去标题字符串长度(不含字符串末尾的\0)，剩余的大小，就是要memset的；
    memset(ptmp,0,cha);
    return;
}

/**
 * @brief 初始化worker进程 
 * @param[in] sub_num worker进程的标签
 * @return void
 */
static void init_worker_process(int sub_num)
{
    //1.do something 
    //TODO
    sigset_t  set;      //信号集

    sigemptyset(&set);  //清空信号集
    if (sigprocmask(SIG_SETMASK, &set, NULL) == -1)  //原来是屏蔽那10个信号【防止fork()期间收到信号导致混乱】，现在不再屏蔽任何信号【接收任何信号】
    {
        int err = errno;
        //失败了
    }

    //2.一些其他的什么处理

    return;
}

/**
 * @brief 子进程的循环
 * @param[in] tag 子进程的编号
 * @param[in] sub_name 子进程的名称
 * @return void
 */
static void worker_process(int tag, const char* sub_name)
{
    //设置一下类型
    g_processFlag = WORKER_PROCESS_PROC;

    init_worker_process(tag);
    //配置子进程的名称
    set_proc_title(sub_name);
    for(;;)
    {
        //循环的函数处理
        //TODO

        printf("worker进程休息1秒\n");       
        fflush(stdout); //刷新标准输出缓冲区，把输出缓冲区里的东西打印到标准输出设备上，则printf里的东西会立即输出；
        sleep(1);       //休息1秒  
    }

    //一些资源的后续处理
    return;
}

/**
 * @brief 产生一个子进程
 * @param[in] tag 子进程的编号
 * @param[in] sub_name 子进程的名称
 * @return -1表示产生失败，其他为进程的pid
 */
static int spawn_process(int tag, const char* sub_name)
{
    pid_t pid;
    pid = fork(); //产生子进程
    switch (pid)
    {
    case -1:
        //产生子进程失败
        return -1;
        break;
    
    case 0:
        //产生的子进程分支
        //配置进程的pid
        g_parentPid = g_ownPid;
        g_ownPid = getpid(); 
        worker_process(tag, sub_name);
        break;

    default:
        //这是父进程自己的分支，直接break
        break;
    }

    //父进程分支会走到这里，子进程流程不往下边走-------------------------
    //若有需要，以后再扩展增加其他代码......
    return pid;
}

/**
 * @brief 创建worker进程
 * @param[in] sub_num worker进程的数目
 * @return void
 */
static void start_worker_processes(int sub_num)
{
    for(int index = 0; index < sub_num; ++index)
        spawn_process(index, "worker process");
    
    return;
}

/**
 * @brief 主循环函数
 * @return void
 */
void process_cycle()
{
    //1.do something 
    //TODO
    sigset_t set;        //信号集
    sigemptyset(&set);   //清空信号集
    //下列这些信号在执行本函数期间不希望收到【考虑到官方nginx中有这些信号，老师就都搬过来了】（保护不希望由信号中断的代码临界区）
    //建议fork()子进程时学习这种写法，防止信号的干扰；
    sigaddset(&set, SIGCHLD);     //子进程状态改变
    sigaddset(&set, SIGALRM);     //定时器超时
    sigaddset(&set, SIGIO);       //异步I/O
    sigaddset(&set, SIGINT);      //终端中断符
    sigaddset(&set, SIGHUP);      //连接断开
    sigaddset(&set, SIGUSR1);     //用户定义信号
    sigaddset(&set, SIGUSR2);     //用户定义信号
    sigaddset(&set, SIGWINCH);    //终端窗口大小改变
    sigaddset(&set, SIGTERM);     //终止
    sigaddset(&set, SIGQUIT);     //终端退出符
    //.........可以根据开发的实际需要往其中添加其他要屏蔽的信号......
    
    //设置，此时无法接受的信号；阻塞期间，你发过来的上述信号，多个会被合并为一个，暂存着，等你放开信号屏蔽后才能收到这些信号。。。
    //sigprocmask()在第三章第五节详细讲解过
    if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) //第一个参数用了SIG_BLOCK表明设置 进程 新的信号屏蔽字 为 “当前信号屏蔽字 和 第二个参数指向的信号集的并集
    {        
        int err = errno;
        //失败了
    }
    //即便sigprocmask失败，程序流程 也继续往下走

    //2.设置主进程的标题
    size_t size = sizeof(master_name); //使用sizeof就可以直接把字符串末尾的'\0'算进来
    int i;
    size += g_argvMemSize; //加上原有的参数长度

    if(size < 1000) //只有当长度小于这个的时候，才设置标题
    {
        char title[1000] = {0};
        strcpy(title, master_name); //"master process"
        strcat(title," ");  //跟一个空格分开一些，清晰    //"master process "
        for (i = 0; i < g_argc; i++)         //"master process ./nginx"
        {
            strcat(title, g_argv[i]);
        }
        set_proc_title(title); //设置标题
    }

    start_worker_processes(g_subNum); //这里要创建worker子进程
    //创建子进程后，父进程的执行流程会返回到这里，子进程不会走进来    
    sigemptyset(&set); //信号屏蔽字为空，表示不屏蔽任何信号
    for(;;)
    {
        // sigsuspend(const sigset_t *mask))用于在接收到某个信号之前, 临时用mask替换进程的信号掩码, 并暂停进程执行，直到收到信号为止。
        // sigsuspend 返回后将恢复调用之前的信号掩码。信号处理函数完成后，进程将继续执行。该系统调用始终返回-1，并将errno设置为EINTR。

        //sigsuspend是一个原子操作，包含4个步骤：
        //a)根据给定的参数设置新的mask 并 阻塞当前进程【因为是个空集，所以不阻塞任何信号】
        //b)此时，一旦收到信号，便恢复原先的信号屏蔽【我们原来调用sigprocmask()的mask在上边设置的，阻塞了多达10个信号，从而保证我下边的执行流程不会再次被其他信号截断】
        //c)调用该信号对应的信号处理函数
        //d)信号处理函数返回后，sigsuspend返回，使程序流程继续往下走
        //printf("for进来了！\n"); //发现，如果print不加\n，无法及时显示到屏幕上，是行缓存问题，以往没注意；可参考https://blog.csdn.net/qq_26093511/article/details/53255970

        //sigsuspend(&set); //阻塞在这里，等待一个信号，此时进程是挂起的，不占用cpu时间，只有收到信号才会被唤醒（返回）；
                         //此时master进程完全靠信号驱动干活    

        //printf("执行到sigsuspend()下边来了\n");
        
        printf("master进程休息1秒\n");
        fflush(stdout); //刷新标准输出缓冲区，把输出缓冲区里的东西打印到标准输出设备上，则printf里的东西会立即输出；
        sleep(1); //休息1秒        
        //以后扩充.......
    }
}

int main(int argc, char* argv[])
{
    int exit_code = 0; //退出的返回值

    //1.初始化变量
    g_exitFlag = 0; //初始化退出标志位，0表示不退出

    //2.获取pid
    g_ownPid = getpid(); //进程本身的pid
    g_parentPid = getppid(); //父进程的pid

    //3.argv的内存
    for(int index = 0; index < argc; ++index)
        g_argvMemSize += strlen(argv[index]) + 1; //不能忘记最后的'\0'也需要一个字节的空间

    //4.env的内存
    for(int index = 0; environ[index]; ++index)
        g_envMemSize += strlen(environ[index]) + 1; //不能忘记最后'\0'也需要一个字节的空间

    //5.参数的变量赋值
    g_argc = argc;
    g_argv = argv;

    //6.做一些需要的处理
    //TODO
    //比如为程序进行一些必要的初始化等等操作

    //7.备份环境变量
    backup_proc_env();

    //8.创建守护进程
    if(g_needDaemon)
    {
        int flag = init_daemon();
        if(flag == -1)
        {
            //说明初始化失败了，那么就需要标记失败，然后处理失败
            exit_code = 1;
            goto label_exit;
        }
        if(flag == 1)
        {
            //说明这是原始的父进程。
            //只有进程退出了才goto到 label_exit，用于提醒用户进程退出了。
            //而我现在这个情况属于正常fork()守护进程后的正常退出，
            //不应该跑到lblexit()去执行，因为那里有一条打印语句标记整个进程的退出，
            //这里不该限制该条打印语句。
            free_resource(); 
            exit_code = 0;
            return exit_code; //父进程的退出，然后就可以将子进程直接交给系统管理，从而变成守护进程
        }

        //走到这里说明已经成功创建了守护进程，并且是fork出来的子进程，
        //那么现在这个进程就相当于是nginx中的master进程了。
        g_daemonFlag = 1; //守护进程成功创建的标志位
    }

    //9.正式的工作流程
    //不管是守护进程还是被守护进程看护的进程，工作期间都应该在这个函数中循环
    process_cycle();

label_exit:
    //释放资源，程序退出
    free_resource();
    return exit_code;
}