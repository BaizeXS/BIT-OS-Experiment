#include<cstdio>
#include<cstdlib>
#include<algorithm>

#define MAX_SIZE 65535

using namespace std;

enum diskScheduleAlg {_FCFS = 1, _SSTF, _SCAN, _CSCAN};     // 磁盘调度算法标签
enum taskState {UNFINISHED, FINISHED};                      // 磁道访问任务状态

struct dSeekTask {                                          // 寻道任务结构体
    int track;                                              // 目标磁道
    int distance;                                           // 当前磁道到目标磁道的距离
    taskState state;                                        // 任务状态
} tasks[MAX_SIZE];                                          // 任务队列
int sTable[MAX_SIZE];                                       // 任务计划表
int taskNum;                                                // 任务总数
int totalTracks;                                            // 总寻道数

void FCFS();                                                // 先来先服务
void SSTF();                                                // 最短寻道时间优先
void SCAN(int mvDirection);                                 // 扫描法
void CSCAN(int mvDirection);                                // 循环扫描法
int getDistance(int x, int y);                              // 距离计算函数
void output();                                              // 输出结果函数

int main() {
    // 1. 读取算法、当前轨道号以及磁臂移动方向并进行初始化
    int algNum;
    int position;
    int direction;
    taskNum = 0;
    totalTracks = 0;
    scanf("%d %d %d", &algNum, &position, &direction);
    sTable[0] = position;
    // 2. 读取磁道请求序列
    char tmpChar;
    while (scanf("%d", &tasks[taskNum].track)) {
        tasks[taskNum].distance = MAX_SIZE;
        tasks[taskNum].state = UNFINISHED;
        taskNum++;
        tmpChar = getchar();
        if (tmpChar == ',') continue;
        else if (tmpChar == '\n') break;
    }
    // 3. 执行算法
    switch (algNum) {
        case _FCFS: FCFS(); break;
        case _SSTF: SSTF(); break;
        case _SCAN: SCAN(direction); break;
        case _CSCAN: CSCAN(direction); break;
    }
    // 4. 输出结果
    output();
    return 0;
}
// 距离计算函数
int getDistance(int x, int y) {
    return abs(x-y);
}
// 先来先服务
void FCFS() {
    for (int i = 0; i < taskNum; i++) {
        sTable[i+1] = tasks[i].track;
        tasks[i].state = FINISHED;
        totalTracks += getDistance(sTable[i], tasks[i].track);
    }
}
// 最短寻道时间优先
void SSTF() {
    // 外层循环用来完成sTable的构建，内层循环用于选择距离最近的磁道
    for (int i = 1; i <= taskNum; i++) {
        // 寻找当前距离最小的磁道
        int minDis = MAX_SIZE;
        int tmpDis = MAX_SIZE;
        int tag = 0;
        for (int j = 0; j < taskNum; j++) {
            if (tasks[j].state == UNFINISHED) {
                tmpDis = getDistance(sTable[i-1], tasks[j].track);
                if (tmpDis < minDis) {
                    minDis = tmpDis;
                    tag = j;
                }
            }
        }
        sTable[i] = tasks[tag].track;
        tasks[tag].state = FINISHED;
        totalTracks += minDis;
    }
}
// 扫描法
void SCAN(int mvDirection) {
    // 1. 排序
    if (mvDirection == 0) {
        sort(tasks, tasks + taskNum, [] (dSeekTask a, dSeekTask b) { return a.track < b.track; });
    } else if (mvDirection == 1) {
        sort(tasks, tasks + taskNum, [] (dSeekTask a, dSeekTask b) { return a.track > b.track; });
    }
    // 2. 获取分隔下标
    int tag = 0;
    for (int i = 0; i < taskNum; i++) {
        if (mvDirection==0 && tasks[i].track > sTable[0]) {
            tag = i;
            break;
        } else if (mvDirection==1 && tasks[i].track < sTable[0]) {
            tag = i;
            break;
        }
    }
    // 3. 填表
    int sTag = 0;
    for (int i = tag - 1; i >= 0; i--) {
        sTable[sTag+1] = tasks[i].track;
        tasks[i].state = FINISHED;
        totalTracks += getDistance(sTable[sTag], tasks[i].track);
        sTag++;
    }
    for (int i = tag; i < taskNum; i++) {
        sTable[sTag+1] = tasks[i].track;
        tasks[i].state = FINISHED;
        totalTracks += getDistance(sTable[sTag], tasks[i].track);
        sTag++;
    }
}
// 循环扫描法
void CSCAN(int mvDirection) {
    // 1. 排序
    if (mvDirection == 0) {
        sort(tasks, tasks + taskNum, [] (dSeekTask a, dSeekTask b) { return a.track < b.track; });
    } else if (mvDirection == 1) {
        sort(tasks, tasks + taskNum, [] (dSeekTask a, dSeekTask b) { return a.track > b.track; });
    }
    // 2. 获取分隔下标
    int tag = 0;
    for (int i = 0; i < taskNum; i++) {
        if (mvDirection==0 && tasks[i].track > sTable[0]) {
            tag = i;
            break;
        } else if (mvDirection==1 && tasks[i].track < sTable[0]) {
            tag = i;
            break;
        }
    }
    // 3. 填表
    int sTag = 0;
    // 填写分隔符左边的任务
    for (int i = tag - 1; i >= 0; i--) {
        sTable[sTag+1] = tasks[i].track;
        tasks[i].state = FINISHED;
        totalTracks += getDistance(sTable[sTag], tasks[i].track);
        sTag++;
    }
    // 填写分隔符右边的任务
    for (int i = taskNum - 1; i >= tag; i--) {
        sTable[sTag+1] = tasks[i].track;
        tasks[i].state = FINISHED;
        totalTracks += getDistance(sTable[sTag], tasks[i].track);
        sTag++;
    }
}
// 输出函数
void output() {
    for (int i = 0; i < taskNum; i++) {
        printf("%d,", sTable[i]);
    }
    printf("%d\n", sTable[taskNum]);
    printf("%d\n", totalTracks);
}