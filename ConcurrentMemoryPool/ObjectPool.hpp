#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <unistd.h>
using std::cout;
using std::endl;

template <class T>
class ObjectPool
{
public:
    // 内存池申请空间
    T *New()
    {
        T *obj = nullptr;
        // 申请空间之前需要先看内存池内是否有剩余内存, 如果有则优先使用
        if (_freeList != nullptr)
        {
            // 如果有则头删freelist的内存结点
            void *next = *((void **)_freeList);
            obj = (T *)_freeList;
            _freeList = next;
        }
        else
        {
            if (_left < sizeof(T))
            {
                // 如果剩余空间小于要申请的最小单位, 则向系统申请一块大内存
                _memory = (char *)malloc(128 * 1024);
                // 申请失败则抛出异常
                if (_memory == nullptr)
                {
                    throw std::bad_alloc();
                }
                _left = 128 * 1024; // 剩余空间直接丢弃, 使用新空间
            }
            // 保证申请的内存大于一个指针的空间大小, 便于回收内存
            obj = (T *)_memory;
            size_t objSize = sizeof(T) < sizeof(void *) ? sizeof(void *) : sizeof(T);
            _memory += objSize;
            _left -= objSize;
        }
        // 申请内存顺便进行初始化
        new (obj) T;
        return obj;
    }
    void Delete(T *obj)
    {
        // 回收内存, 顺便调用析构
        obj->~T();
        // 回收到freeList, 进行头插即可
        *(void **)obj = _freeList; // 获取到obj的头部4/8个字节, 保存原链表第一个内存块(地址)
        _freeList = (void *)obj;
    }

private:
    // 内存池
    char *_memory = nullptr;
    // 回收链表, 由于不知道会存什么类型, 所以用void*
    void *_freeList = nullptr;
    // 需要知道当前内存池的剩余内存有多少字节
    size_t _left = 0;
};
struct TreeNode
{
    int _val;
    TreeNode *_left;
    TreeNode *_right;

    TreeNode()
        : _val(0), _left(nullptr), _right(nullptr)
    {
    }
};

void TestObjectPool()
{
    // 申请释放的轮次
    const size_t Rounds = 20;

    // 每轮申请释放多少次
    const size_t N = 1000000;

    std::vector<TreeNode *> v1;
    v1.reserve(N);

    size_t begin1 = clock();
    for (size_t j = 0; j < Rounds; ++j)
    {
        for (int i = 0; i < N; ++i)
        {
            v1.push_back(new TreeNode);
        }
        for (int i = 0; i < N; ++i)
        {
            delete v1[i];
        }
        v1.clear();
    }

    size_t end1 = clock();

    std::vector<TreeNode *> v2;
    v2.reserve(N);

    ObjectPool<TreeNode> TNPool;
    size_t begin2 = clock();
    for (size_t j = 0; j < Rounds; ++j)
    {
        for (int i = 0; i < N; ++i)
        {
            v2.push_back(TNPool.New());
        }
        for (int i = 0; i < N; ++i)
        {
            TNPool.Delete(v2[i]);
        }
        v2.clear();
    }
    size_t end2 = clock();

    cout << "new cost time:" << end1 - begin1 << endl;
    cout << "object pool cost time:" << end2 - begin2 << endl;
}
