/* Memory Dynamic Partition */
#include <cstdio>
#include <cstdlib>
#define MAX_MEM_SIZE 65535

using namespace std;

// MState 内存块的状态
enum MState {UNUSED, USED};
// Memory 内存块结构体
struct Memory {
    int startAddr;          // 起始地址
    int endAddr;            // 结束地址
    int size;               // 内存块大小
    int pid;                // 进程ID
    MState state;           // 内存块状态
    struct Memory *next;    // 下一个内存块
};
// ReqList 请求列表
struct Request {
    int sn;         // serial number
    int pid;        // process id
    int op;         // operation
    int opVol;      // volume of operation
} rList[1024];
// pFunc 函数指针：用于不同的内存分配算法
typedef void (*pFunc)(Request request, Memory *mem);
// FF 分配函数
void FFalloc(Request request, Memory *mem);
// BF 分配函数
void BFalloc(Request request, Memory *mem);
// WF 分配函数
void WFalloc(Request request, Memory *mem);
// 内存释放函数
void memFree(Request request, Memory *mem);
// 结果输出函数
void output(Request request, Memory *mem);

int main()
{
    int algNum = 0;             // number of algorithms
    int memSize = 0;            // size of memory
    int num = 0;                // number of current request
    pFunc pAlloc;               // function to allocate
    Memory *memory;             // memory
    // 1. 读取算法和内存大小
    scanf("%d %d", &algNum, &memSize);
    // 2. 读取请求序列
    while (~scanf("%d/%d/%d/%d", &rList[num].sn, &rList[num].pid, &rList[num].op, &rList[num].opVol)) { num++; }
    // Test Start: Input
//    int tmp = 17;
//    for (int i = 0; i < tmp; i++) {
//        scanf("%d/%d/%d/%d", &rList[i].sn, &rList[i].pid, &rList[i].op, &rList[i].opVol);
//    }
//    num = tmp;
    // Test End
    // 3. 初始化内存
    memory = (Memory*)malloc(sizeof(Memory));
    memory->startAddr = 0;
    memory->endAddr = memSize - 1;
    memory->size = memSize;
    memory->pid = -1;
    memory->state = UNUSED;
    memory->next = nullptr;
    // 4. 算法选择
    switch (algNum) {
        case 1: pAlloc = FFalloc; break;
        case 2: pAlloc = BFalloc; break;
        case 3: pAlloc = WFalloc; break;
        default: {
            printf("Unknown algorithm");
            exit(EXIT_FAILURE);
        }
    }
    // 5. 执行算法
    for (int i = 0; i < num; i++) {
        if (rList[i].op == 1) {
            pAlloc(rList[i], memory);
        } else if (rList[i].op == 2) {
            memFree(rList[i], memory);
        } else {
            printf("Error: Invalid operation number %d", i);
            exit(EXIT_FAILURE);
        }
        output(rList[i], memory);
    }
    return 0;
}
// FF 分配函数
void FFalloc(Request request, Memory *mem)
{
    while (mem != nullptr) {
        if (request.opVol < mem->size && mem->state == UNUSED) {
            // 1. 将进程装入内存
            int oriEndAddr = mem->endAddr;
            mem->endAddr = mem->startAddr + request.opVol - 1;
            mem->size = request.opVol;
            mem->pid = request.pid;
            mem->state = USED;
            // 2. 剩余空闲内存重新接入链表
            auto *restMem = (Memory*)malloc(sizeof(Memory));
            restMem->startAddr = mem->endAddr + 1;
            restMem->endAddr = oriEndAddr;
            restMem->size = restMem->endAddr - restMem->startAddr + 1;
            restMem->pid = -1;
            restMem->state = UNUSED;
            restMem->next = mem->next;
            mem->next = restMem;
            break;
        } else if (request.opVol == mem->size && mem->state == UNUSED) {
            mem->pid = request.pid;
            mem->state = USED;
            break;
        }
        mem = mem->next;
    }
}
// BF 分配函数
void BFalloc(Request request, Memory *mem)
{
    int tmpRestMSize;
    int minRestMSize = MAX_MEM_SIZE;
    Memory *tmpMem = nullptr;
    while (mem != nullptr) {
        // 寻找剩余空间最小的空闲空间
        if (request.opVol <= mem->size && mem->state == UNUSED) {
            tmpRestMSize = mem->size - request.opVol;
            if (tmpRestMSize < minRestMSize) {
                minRestMSize = tmpRestMSize;
                tmpMem = mem;
            }
        }
        mem = mem->next;
    }
    if (tmpMem != nullptr) {
        // 与 FF 类似
        int oriEndAddr = tmpMem->endAddr;
        tmpMem->endAddr = tmpMem->startAddr + request.opVol - 1;
        tmpMem->size = request.opVol;
        tmpMem->pid = request.pid;
        tmpMem->state = USED;
        if (minRestMSize > 0) {
            auto *restMem = (Memory*)malloc(sizeof(Memory));
            restMem->startAddr = tmpMem->endAddr + 1;
            restMem->endAddr = oriEndAddr;
            restMem->size = minRestMSize;
            restMem->pid = -1;
            restMem->state = UNUSED;
            restMem->next = tmpMem->next;
            tmpMem->next = restMem;
        } else if (minRestMSize == 0) {
            // Do nothing!
        }
    }
}
// WF 分配函数
void WFalloc(Request request, Memory *mem)
{
    int maxRestMSize = -1;
    Memory *tmpMem = nullptr;
    while (mem != nullptr) {
        // 寻找最大空闲空间
        if (request.opVol <= mem->size && mem->state == UNUSED && maxRestMSize < mem->size) {
            maxRestMSize = mem->size;
            tmpMem = mem;
        }
        mem = mem->next;
    }
    if (tmpMem != nullptr) {
        // 与 FF 类似
        int oriEndAddr = tmpMem->endAddr;
        tmpMem->endAddr = tmpMem->startAddr + request.opVol - 1;
        tmpMem->size = request.opVol;
        tmpMem->pid = request.pid;
        tmpMem->state = USED;
        if (maxRestMSize > 0) {
            auto *restMem = (Memory*)malloc(sizeof(Memory));
            restMem->startAddr = tmpMem->endAddr + 1;
            restMem->endAddr = oriEndAddr;
            restMem->size = restMem->endAddr - restMem->startAddr + 1;
            restMem->pid = -1;
            restMem->state = UNUSED;
            restMem->next = tmpMem->next;
            tmpMem->next = restMem;
        } else if (maxRestMSize == 0) {
            // Do nothing!
        }
    }
}
// 内存释放函数
void memFree(Request request, Memory *mem)
{
    /*
     * 四种情况:
     * 1. 当前内存块前后均有空闲空间——>释放并合并其前后空闲空间
     * 2. 当前内存块前有空闲空间——>释放并合并其前面空闲空间
     * 3. 当前内存块后有空闲空间——>释放并合并其后面空闲空间
     * 4. 当前内存块前后均无空闲空间——>仅释放当前内存块
     * 其中，当前块还有三种特殊情况:
     * 1. 当前块是唯一内存块——>相当于情况4
     * 2. 当前块是第一个内存块——>相当于情况3和情况4
     * 3. 当前块是最后一个内存块——>相当于情况2和情况4
     */
    Memory *prevMem, *currMem, *nextMem;
    prevMem = mem;
    currMem = mem;
    nextMem = nullptr;
    // 1. 寻找当前请求的进程所在的内存块
    while (currMem != nullptr) {
        // 更新 Next Memory
        nextMem = currMem->next;
        // 判断当前内存块内的进程是否为发出请求的进程
        if (currMem->pid == request.pid) {
            // 更改内存状态并退出循环
            currMem->state = UNUSED;
            break;
        }
        // 更新 Previous Memory 和 Current Memory
        prevMem = currMem;
        currMem = nextMem;
    }
    // 2. 合并空闲内存空间并释放内存
    if (prevMem != currMem && nextMem != nullptr && prevMem->state == UNUSED && nextMem->state == UNUSED) {
        // 合并前后空闲空间
        prevMem->endAddr = nextMem->endAddr;
        prevMem->size = prevMem->size + currMem->size + nextMem->size;
        prevMem->pid = -1;
        prevMem->next = nextMem->next;
        free(currMem);
        free(nextMem);
    } else if (prevMem != currMem && prevMem->state == UNUSED && (nextMem == nullptr || nextMem->state == USED)) {
        // 合并前空闲空间
        prevMem->endAddr = currMem->endAddr;
        prevMem->size = prevMem->size + currMem->size;
        prevMem->pid = -1;
        prevMem->next = currMem->next;
        free(currMem);
    } else if (nextMem != nullptr && nextMem->state == UNUSED && (prevMem->state == USED || prevMem == currMem)) {
        // 合并后空闲空间
        currMem->endAddr = nextMem->endAddr;
        currMem->size = currMem->size + nextMem->size;
        currMem->pid = -1;
        currMem->next = nextMem->next;
        free(nextMem);
    } else {
        // Do Nothing
    }
}
// 结果输出函数
void output(Request request, Memory *mem)
{
    printf("%d", request.sn);
    while (mem != nullptr) {
        if (mem->state == USED) {
            printf("/%d-%d.1.%d", mem->startAddr, mem->endAddr, mem->pid);
        } else if (mem->state == UNUSED) {
            printf("/%d-%d.0", mem->startAddr, mem->endAddr);
        }
        mem = mem->next;
    }
    printf("\n");
}