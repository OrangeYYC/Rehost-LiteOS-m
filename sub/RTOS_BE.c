#include <pthread.h>
#include <signal.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>
#include "RTOS_BE.h"

// #define DEBUG

#define SIG_SUSPEND             SIGUSR1
#define SIG_RESUME              SIGUSR2
#define SIG_TICK                SIGALRM
#define TIMER_TYPE              ITIMER_REAL 
// You may need to adjust this part for different OS
#define THREAD_NUMBER           16
#define MAX_NUMBER_OF_PARAMS    4
#define THREAD_FUNC             (*real_func)(unsigned int, unsigned int)
// End
      
int interrupt_flag = 1;         // 中断是否开启
int nesting_counter = 0;        // 嵌套层数
pthread_t init_thread;          // 初始化线程(开发板入口主函数线程)
pthread_t ender_thread;         // 托孤

typedef struct ThreadState
{
    pthread_t       Thread;
    int             valid;    /* Treated as a boolean */
    unsigned int    taskid;
    int             critical_nesting;
    void            THREAD_FUNC;
    unsigned int    real_params[MAX_NUMBER_OF_PARAMS];
    int             param_number;
    int             index;
} PMCU_Thread;

int first_time = 1;
PMCU_Thread pmcu_threads[THREAD_NUMBER];
// int next_valid_pmcu_thread = 0;
static pthread_mutex_t scheduler_mutex   = PTHREAD_MUTEX_INITIALIZER;       // 模拟调度互斥锁
static pthread_mutex_t systick_mutex     = PTHREAD_MUTEX_INITIALIZER;       // 模拟时钟互斥锁
static pthread_mutex_t single_core_mutex = PTHREAD_MUTEX_INITIALIZER;       // 模拟单线程互斥锁
int creat_not_done = 0;
void (*pmcu_systick_handler)(void);

// CPU 错误
int Fault_BE() {
    abort();
    return 0;
}

// 不再响应时钟信号
void PMCU_BE_Thread_Disable_Tick() {
    sigset_t signal;
    sigemptyset(&signal);
    sigaddset(&signal, SIG_TICK);
    pthread_sigmask(SIG_BLOCK, &signal, NULL);
}

// 响应时钟信号
void PMCU_BE_Thread_Enable_Tick() {
    sigset_t signal;
    sigemptyset(&signal);
    sigaddset(&signal, SIG_TICK);
    pthread_sigmask(SIG_UNBLOCK, &signal, NULL);
}

// 设置中断开启
int PMCU_BE_Disable_Irq(){
    interrupt_flag = 0;
    return interrupt_flag;
}

// 设置中断关闭
int PMCU_BE_Enable_Irq(){
    interrupt_flag = 1;
    return interrupt_flag;
}

// 进入临界区域
void PMCU_BE_Enter_Critical(){
    PMCU_BE_Disable_Irq();
    nesting_counter++;
}

// 离开临界区域
unsigned int PMCU_BE_Leave_Critical(){
    if (nesting_counter > 0)
        nesting_counter--;
    if (nesting_counter == 0)
        PMCU_BE_Enable_Irq();
    return nesting_counter;
}

// 设置嵌套层数
static void PMCU_SetNesting(int idx, int count){
    pmcu_threads[idx].critical_nesting = count;
}

// 设置嵌套层数
static int PMCU_GetNesting(int idx){
    return pmcu_threads[idx].critical_nesting;
}

// Setup_Ticker
// 设置时钟信号处理：时钟信号将在 time_interval 后发出
void Setup_Ticker(unsigned int time_interval){
    #ifdef DEBUG
        printf("[PMCU] Thread %lu: Setup Ticker with interval %u\n", pthread_self(), time_interval);
    #endif
    struct itimerval src_timer, tar_timer;
    suseconds_t microSeconds = (suseconds_t)(time_interval % 1000000);
    time_t seconds = time_interval / 1000000;
    getitimer(TIMER_TYPE, &src_timer);
    src_timer.it_interval.tv_sec = seconds;
    src_timer.it_interval.tv_usec = microSeconds;
    src_timer.it_value.tv_sec = seconds;
    src_timer.it_value.tv_usec = microSeconds;
    setitimer(TIMER_TYPE, &src_timer, &tar_timer);
}

unsigned int g_SysTickVal;

// 时钟信号处理程序
void Systick_Signal_Handler(int sig){
    if (!interrupt_flag)
        return;
    int ret = pthread_mutex_trylock(&systick_mutex);
    if (ret == 0) {
        #ifdef DEBUG
            printf("\n[Enter Handler in %lu]\n", pthread_self());
        #endif
        PMCU_BE_Disable_Irq();
        g_SysTickVal += 250000;
        pmcu_systick_handler(); // 调用目标 CPU 的时钟中断处理程序
        pthread_mutex_unlock(&systick_mutex);
        PMCU_BE_Enable_Irq();
    }
}

void Suspend_Thread_Self() {
    #ifdef DEBUG
        printf("[PMCU] Thread %lu try to suspend thread %lu\n", pthread_self(), pthread_self());
    #endif
    int sig;
    sigset_t signal;
    sigemptyset(&signal);
    sigaddset(&signal, SIG_RESUME);
    PMCU_BE_Disable_Irq();
    PMCU_BE_Thread_Disable_Tick();
    pthread_mutex_unlock(&single_core_mutex);
    #ifdef DEBUG
        printf("[PMCU] Thread %lu suspend\n", pthread_self());
    #endif
    sigwait(&signal, &sig);
    pthread_mutex_lock(&single_core_mutex);
    PMCU_BE_Thread_Enable_Tick();
    PMCU_BE_Enable_Irq();
    //if (nesting_counter == 0)
    //    PMCU_BE_Enable_Irq();
    //else
    //    PMCU_BE_Disable_Irq();
    #ifdef DEBUG
        printf("[PMCU] Thread %lu resume\n", pthread_self());
    #endif    
}

// Suspend_Thread
// 挂起线程: 将 thread 挂起
void Suspend_Thread(pthread_t thread){
    #ifdef DEBUG
        printf("[PMCU] Thread %lu try to suspend thread %lu\n", pthread_self(), thread);
    #endif
    // 设置调度器标志
    pthread_mutex_lock(&scheduler_mutex);
    creat_not_done = 1;
    pthread_mutex_unlock(&scheduler_mutex);
    // 挂起目标线程
    pthread_kill(thread, SIG_SUSPEND);
    // 等待挂起完成
    while (creat_not_done == 1)
        sleep(0); 
    #ifdef DEBUG
        printf("[PMCU] Thread %lu resume\n", pthread_self());
    #endif
}
// 响应挂起信号响应函数
void Suspend_Signal_Handler(int sig){
    sigset_t signal;
    sigemptyset(&signal);
    sigaddset(&signal, SIG_RESUME);
    creat_not_done = 0;
    pthread_mutex_unlock(&single_core_mutex);
    #ifdef DEBUG
        printf("[PMCU] Thread %lu suspend\n", pthread_self());
    #endif
    sigwait(&signal, &sig);
    pthread_mutex_lock(&single_core_mutex);
    PMCU_BE_Enable_Irq();
    //if (nesting_counter == 0)
    //    PMCU_BE_Enable_Irq();
    //else
    //   PMCU_BE_Disable_Irq();
}
// 重启线程
void ResumeThread(pthread_t thread) {
    #ifdef DEBUG
        printf("[PMCU] Thread %lu try to resume thread %lu\n", pthread_self(), thread);
    #endif
    if (!pthread_equal(pthread_self(), thread))
        pthread_kill(thread, SIG_RESUME);
}
// 重启信号响应函数
void Resume_Signal_Handler(int sig) {
    // 等待上一个执行线程交权
    // pthread_mutex_lock(&single_core_mutex);
}

// Setup_Signal_Handler
// 连接信号处理函数
void Setup_Signal_Handler(){
    #ifdef DEBUG
        printf("[PMCU] Thread %lu: Setup_Signal_Handler()\n", pthread_self());
    #endif
    // 设置初始化线程锁
    pthread_mutex_lock(&single_core_mutex);
    // 初始化线程结构体
    memset(pmcu_threads, 0, sizeof(pmcu_threads));
    // 设置信号处理函数
    struct sigaction suspend, resume, sigtick;
    suspend.sa_flags = 0;
    suspend.sa_handler = Suspend_Signal_Handler;
    sigfillset(&suspend.sa_mask);
    resume.sa_flags = 0;
    resume.sa_handler = Resume_Signal_Handler;
    sigfillset(&resume.sa_mask);
    sigtick.sa_flags = 0;
    sigtick.sa_handler = Systick_Signal_Handler;
    sigfillset(&sigtick.sa_mask);
    assert(sigaction(SIG_SUSPEND, &suspend, NULL ) == 0);
    assert(sigaction(SIG_RESUME, &resume, NULL ) == 0);
    assert(sigaction(SIG_TICK, &sigtick, NULL ) == 0);
    // 标记初始线程
    init_thread = pthread_self();
}

extern unsigned int LOS_TaskDelete(unsigned int);

// PMCU_ThreadExit
// 包装线程退出时需要进行的相关收尾工作
static void PMCU_ThreadExit(void *param){
    PMCU_BE_Disable_Irq();
    PMCU_Thread *thread = (PMCU_Thread *)param;
    for (int i = 0; i < THREAD_NUMBER; i++) {
        if (pthread_equal(pmcu_threads[i].Thread, thread->Thread)) {
            #ifdef DEBUG
                printf("[PMCU] Clean Task %d\n", pmcu_threads[i].taskid);
            #endif
            pmcu_threads[i].valid = 0;
            if (pmcu_threads[i].critical_nesting > 0) {
                pmcu_threads[i].critical_nesting = 0;
                nesting_counter = 0;
            }
            int result = LOS_TaskDelete(pmcu_threads[i].taskid);
            break;
        }
    }   
}

// Thread_Wrapper
// 线程包装函数: params 是参数 PMCU_Thread 结构体
void *Thread_Wrapper( void * params ){
    PMCU_Thread* thread = (PMCU_Thread*)params;
    #ifdef DEBUG
        printf("[PMCU] Thread_Wrapper() %d\n", thread->taskid);
    #endif
    // 设置单核锁
    pthread_mutex_lock(&single_core_mutex);
    // 设置终止方式
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    // 设置结束回调函数
    pthread_cleanup_push(PMCU_ThreadExit, thread);
    // 挂起当前执行的线程
    Suspend_Thread(pthread_self());
    // 执行目标函数
    thread->real_func(thread->real_params[0], thread->real_params[1]);
    // 回调结束回调函数
    pthread_cleanup_pop(1);
    return NULL;
}

// PMCU_BE_Task_Create
// 创建任务
void PMCU_BE_Task_Create(void* func, unsigned int params[], int param_count, unsigned int taskid, void (*tick_handler)(void)) {    
    printf("[Task Create in %lu]\n", pthread_self());
    
    // 关闭 IRQ
    PMCU_BE_Disable_Irq();
    pthread_mutex_lock(&systick_mutex);

    // 第一次进行任务创建的初始化工作：必定由开发板主函数所在线程执行
    if (first_time) {
        pmcu_systick_handler = tick_handler;
        Setup_Signal_Handler();
        first_time = 0;
    }

    // 寻找一个可用的任务控制块，分配第 i 块给目标任务
    int i = 0;
    for (i = 0; i < THREAD_NUMBER; i++ ) {
        if (pmcu_threads[i].valid == 0) {
            pmcu_threads[i].critical_nesting = 0;
            pmcu_threads[i].taskid = taskid;
            break;
        }
    }
    assert(i < THREAD_NUMBER);
    
    // 加载线程目标函数和线程参数
    pmcu_threads[i].real_func = func;
    if (params == NULL)
        pmcu_threads[i].param_number = 0;
    else {
        pmcu_threads[i].param_number = param_count;
        for (int j = 0; j < param_count; j++)
            pmcu_threads[i].real_params[j] = params[j];
    }

    // 创建目标线程
    pthread_attr_t thread_attr;
    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
    pmcu_threads[i].valid = 1;
    pthread_create(&pmcu_threads[i].Thread, &thread_attr, Thread_Wrapper, &pmcu_threads[i]);
    #ifdef DEBUG
        printf("[PMCU] Create pthread for task %d. Waiting for pthread suspend.\n", taskid);
    #endif
    // 放行创建的线程
    creat_not_done = 1;
    pthread_mutex_unlock(&single_core_mutex);
    // 等待目标线程交权
    while (creat_not_done) { 
        sleep(0);
    }
    pthread_mutex_lock(&single_core_mutex);

    // 开启 IRQ
    PMCU_BE_Enable_Irq();
    pthread_mutex_unlock(&systick_mutex);

    #ifdef DEBUG
        printf("[PMCU] Create pthread for task %d done.\n", taskid);
    #endif
}

// Start_Scheduler
// 以周期 interval 对任务 taskid 进行调度
void Start_Scheduler(unsigned int taskid, unsigned int interval){
    #ifdef DEBUG
        printf("[PMCU] Thread %lu: Start Scheduler\n", pthread_self());
    #endif
    // 启动任务绑定的计时器，在 interval 时间间隔后进行下一次调度
    Setup_Ticker(interval);
    // 启动调度器
    for(int i = 0; i < THREAD_NUMBER; i++) {
        if (pmcu_threads[i].taskid == taskid && pmcu_threads[i].valid == 1) {
            // 启动目标线程
            ResumeThread(pmcu_threads[i].Thread);
            // 挂起当前线程
            Suspend_Thread(pthread_self());
            break;
        }
    }
    // 调度工作已经完成，撤销并结束线程
    pthread_cancel(ender_thread);
    usleep(100);
    // 回收开启的互斥锁
    pthread_mutex_destroy( &systick_mutex );
    pthread_mutex_destroy( &single_core_mutex );
    pthread_mutex_destroy( &scheduler_mutex );
    usleep(100);
}

// Stop_Scheduler
// 用于停止调度
void Stop_Scheduler() {
    // 停止所有信号的信号处理函数
    struct sigaction sig_action;
    sig_action.sa_flags = 0;
    sig_action.sa_handler = SIG_IGN;
    sigfillset(&sig_action.sa_mask);
    sigaction(SIG_TICK, &sig_action, NULL);
    sigaction(SIG_RESUME, &sig_action, NULL);
    sigaction(SIG_SUSPEND, &sig_action, NULL);
    // 检查当前存在的所有有效线程
    for (int i = 0; i < THREAD_NUMBER; i++) {
        if (pmcu_threads[i].taskid != -1) {
            if (pthread_equal(pmcu_threads[i].Thread, pthread_self())) {
                // 是当前执行的线程？交给 init_thread 去撤销
                ender_thread = pmcu_threads[i].Thread;
            } else {
                // 不是当前执行的线程？直接撤销
                pthread_cancel(pmcu_threads[i].Thread);
                usleep(1000);
            }
        }
    }
    // 重启 init_thread 线程，完成最后的清理工作
    pthread_kill(init_thread, SIG_RESUME);
    // 结束当前的线程
    pthread_mutex_unlock(&single_core_mutex);
    pthread_exit(0);
}

// Pthread_Schedule
// 在 pthread 层面进行执行线程的切换，保证仅有一个线程处于运行状态
void Pthread_Schedule(unsigned int new_task_id, unsigned int run_task_id){
    // 拒绝无效调度
    if (new_task_id == run_task_id)
        return;  
    #ifdef DEBUG
        printf("\n\033[22;32m[PMCU] Task Switch %d to %d\033[0m\n", run_task_id, new_task_id);
    #endif DEBUG
    // 将 new_task_id 的对应线程 resume
    int i = 0, j = 0;
    for (; i < THREAD_NUMBER; i++) {
        if (new_task_id == pmcu_threads[i].taskid && pmcu_threads[i].valid == 1) {
            break;
        }
    }
    assert (i < THREAD_NUMBER);
    if (pthread_equal(pthread_self(), pmcu_threads[i].Thread))
        return;
    //PMCU_SetNesting(i, nesting_counter);
    //nesting_counter = PMCU_GetNesting(j);
    ResumeThread(pmcu_threads[i].Thread);
    // 线程调度完成
    pthread_mutex_unlock(&systick_mutex);
    PMCU_BE_Enable_Irq();
    // 将 run_task_id 的对应线程 suspend
    for (; j < THREAD_NUMBER; j++) {
        // 目标是运行线程，将其 suspend
        if (run_task_id == pmcu_threads[j].taskid && pmcu_threads[j].valid == 1) {
            Suspend_Thread_Self(pmcu_threads[j].Thread);
            break;
        }
        // 目标是结束的线程，将其杀掉
        if (run_task_id == pmcu_threads[j].taskid && pmcu_threads[j].valid == 0) {
            #ifdef DEBUG
                printf("[PMCU] Thread %lu: Exit Thread %lu\n", pthread_self(), pmcu_threads[j].Thread);
            #endif
            if (pthread_equal(pmcu_threads[j].Thread, pthread_self())) {
                pmcu_threads[j].taskid = -1;
                pmcu_threads[j].Thread = 0;
                pthread_mutex_unlock(&single_core_mutex);
                pthread_exit(0);
            } else {
                pthread_cancel(pmcu_threads[j].Thread);
                usleep(100);
                pmcu_threads[j].taskid = -1;
                pmcu_threads[j].Thread = 0;
            }
            break;
        }
    }
}
