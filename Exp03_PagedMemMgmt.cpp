/* Paged Memory Management */
#include <cstdio>
#include <cstdlib>

#define MAX_SIZE 1024

using namespace std;

enum memMgmtAlg {OPT = 1, FIFO, LRU};           // 页面置换算法标签
enum pageFlag {MISS = 0, HIT};                  // 页面命中标签
enum fullFlag {NOT_FULL = 0, FULL};             // 驻留集空闲页标签

struct residentSet {                            // 驻留集
    int pid;                                    // 进程号
    int priority;                               // 优先级
    int distance;                               // 距离
};
typedef void (*pUpdateFunc)(residentSet* rSet, int pageNum, int hitPage);
typedef void (*pReplaceFunc)(residentSet* rSet, int pageNum, const int* procSeq, int procNum, int currProc);
void pageAdd(residentSet* rSet, int pageNum, int freePage, const int* procSeq, int currProc);
void updateOPTandFIFO(residentSet* rSet, int pageNum, int hitPage);
void updateLRU(residentSet* rSet, int pageNum, int hitPage);
void replaceOPT(residentSet* rSet, int pageNum, const int* procSeq, int procNum, int currProc);
void replaceFIFO(residentSet* rSet, int pageNum, const int* procSeq, int procNum, int currProc);
void replaceLRU(residentSet* rSet, int pageNum, const int* procSeq, int procNum, int currProc);

int main()
{
    // 页面置换算法
    int mmAlgNum;                               // 页面置换算法序号
    pUpdateFunc pageUpdate;                     // 更新驻留集页面
    pReplaceFunc pageReplace;                   // 替换驻留集页面
    // 驻留集
    int pagesNum;                               // 驻留集页面数
    // 进程序列
    int procSeq[MAX_SIZE] = {0};                // 进程序列
    int procNum = 0;                            // 进程序列总数
    // 缺页中断
    pageFlag hitFlag;                           // 命中标志
    int hitPage;                                // 命中页号
    fullFlag isFull;                            // 空闲标志
    int freePage;                               // 空闲页号
    int missTimes = 0;                          // 缺页次数
    // 1. 读入页面置换算法序号和驻留集页面数
    scanf("%d %d", &mmAlgNum, &pagesNum);
    // 2. 初始化驻留集和页面置换算法
    residentSet rSet[pagesNum];
    for (int i = 0; i < pagesNum; i++) {
        rSet[i].pid = -1;
        rSet[i].priority = -1;
        rSet[i].distance = MAX_SIZE;
    }
    switch (mmAlgNum) {
        case OPT: {
            pageUpdate = updateOPTandFIFO;
            pageReplace = replaceOPT;
            break;
        }
        case FIFO: {
            pageUpdate = updateOPTandFIFO;
            pageReplace = replaceFIFO;
            break;
        }
        case LRU: {
            pageUpdate = updateLRU;
            pageReplace = replaceLRU;
            break;
        }
        default: {
            printf("Unrecognized Algorithm.");
            exit(EXIT_FAILURE);
        }
    }
    // 3. 读入进程序列
    char tmpChar;
    while (scanf("%d", &procSeq[procNum])) {
        procNum++;
        tmpChar = getchar();
        if (tmpChar == ',') continue;
        else if (tmpChar == '\n') break;
    }
    // 4. 模拟执行
    for (int i = 0; i < procNum; i++) {
        // 1. 检查是否有空闲页面
        isFull = FULL;
        freePage = -1;
        for (int j = 0; j < pagesNum; j++) {
            if (rSet[j].pid == -1) {
                isFull = NOT_FULL;              // 说明驻留集中还有空闲页面
                freePage = j;                   // 记录驻留集中空闲页下标
                break;
            }
        }
        // 2. 检查是否命中
        hitFlag = MISS;
        hitPage = -1;
        for (int j = 0; j < pagesNum; j++) {
            if (rSet[j].pid == procSeq[i]) {
                hitFlag = HIT;                  // 说明驻留集中存在命中的页面
                hitPage = j;                    // 记录命中页驻留集下标
                break;
            }
        }
        // 3. 将页面加载到驻留集中
        //    已命中——>重置该进程参数并进入下一个进程(continue)
        //    未命中——>缺页中断
        //            驻留集未满：加载到驻留集中空闲页中
        //            驻留集已满：先采用页面置换策略得到空闲页，再加载该进程
        if (hitFlag) {                          // 命中驻留集中的页面：更新参数
            pageUpdate(rSet, pagesNum, hitPage);
        } else {
            if (isFull) {                       // 驻留集已满：页面置换
                pageReplace(rSet, pagesNum, procSeq, procNum, i);
            } else {               // 驻留集未满：将进程页面添加到驻留集中
                pageAdd(rSet, pagesNum, freePage, procSeq, i);
            }
            missTimes++;
        }
        // 4. 输出
        // 4.1 输出当前驻留集中进程序列
        for (int k = 0; k < pagesNum; k++) {
            if (rSet[k].pid != -1) printf("%d,", rSet[k].pid);
            else printf("-,");
        }
        // 4.2 输出是否命中
        printf("%d", hitFlag);
        // 4.3 输出结束符
        if (i < procNum - 1) printf("/");
        else printf("\n");
    }
    // 4.4 输出缺页次数
    printf("%d\n", missTimes);
    return 0;
}

void pageAdd(residentSet* rSet, int pageNum, int freePage, const int* procSeq, int currProc)
{
    // 驻留集中已有页优先级均下调一个单位
    for (int i = 0; i < pageNum; i++)
        if (rSet[i].pid != -1)
            rSet[i].priority += 1;
    rSet[freePage].pid = procSeq[currProc];
    rSet[freePage].priority = 0;
}
void updateOPTandFIFO(residentSet* rSet, int pageNum, int hitPage)
{
    // 驻留集中已有页优先级调整
    for (int i = 0; i < pageNum; i++)
        if (rSet[i].pid != -1)
            rSet[i].priority += 1;
}
void updateLRU(residentSet* rSet, int pageNum, int hitPage)
{
    int freePages = 0;
    for (int i = 0; i < pageNum; i++) {
        if (rSet[i].pid == -1)
            freePages++;
    }
    residentSet tmp = rSet[hitPage];
    for (int i = hitPage; i < pageNum - freePages; i++) {
        rSet[i] = rSet[i+1];
    }
    rSet[pageNum-freePages-1] = tmp;
}
void replaceOPT(residentSet* rSet, int pageNum, const int* procSeq, int procNum, int currProc)
{
    int tmpIndex = 0;                           // 临时下标
    int maxDistance = 0;                        // 最大距离
    // 驻留集中已有页优先级调整
    for (int i = 0; i < pageNum; i++) {
        if (rSet[i].pid != -1)
            rSet[i].priority += 1;
        rSet[i].distance = MAX_SIZE;
    }
    // 计算距离
    for (int i = 0; i < pageNum; i++) {
        for (int j = currProc; j < procNum; j++) {
            if (rSet[i].pid == procSeq[j]) {
                rSet[i].distance = j - currProc;
                break;
            }
        }
    }
    // 选择最大距离
    maxDistance = rSet[0].distance;
    for (int i = 0; i < pageNum; i++) {
        if (rSet[i].distance > maxDistance ||
            (rSet[i].distance == maxDistance && rSet[i].priority > rSet[tmpIndex].priority)) {
            maxDistance = rSet[i].distance;
            tmpIndex = i;
        }
    }
    rSet[tmpIndex].pid = procSeq[currProc];
    rSet[tmpIndex].priority = 0;
}
void replaceFIFO(residentSet* rSet, int pageNum, const int* procSeq, int procNum, int currProc)
{
    int toReplace = 0;
    // 驻留集中已有页优先级调整
    for (int i = 0; i < pageNum; i++)
        if (rSet[i].pid != -1)
            rSet[i].priority += 1;
    // 寻找优先级最低的页面
    for (int i = 0; i < pageNum; i++)
        if (rSet[i].priority > rSet[toReplace].priority)
            toReplace = i;
    // 替换
    rSet[toReplace].pid = procSeq[currProc];
    rSet[toReplace].priority = 0;
}
void replaceLRU(residentSet* rSet, int pageNum, const int* procSeq, int procNum, int currProc)
{
    for (int i = 0; i < pageNum; i++) {
        rSet[i] = rSet[i+1];
    }
    rSet[pageNum - 1].pid = procSeq[currProc];
}
