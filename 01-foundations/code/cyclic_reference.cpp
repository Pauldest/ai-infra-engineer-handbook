// =============================================================================
//  循环引用演示：为什么需要 weak_ptr —— 亲眼看到内存泄漏 vs 正确释放
// =============================================================================
//
//  编译运行：
//    clang++ -std=c++17 -O2 -Wall cyclic_reference.cpp -o cyclic && ./cyclic
//
//  用标准库 std::shared_ptr / std::weak_ptr 演示（这里重点是概念，不是造轮子）。
//  观察：BAD 版本两个 Node 都不析构（泄漏）；GOOD 版本正确析构。
// =============================================================================

#include <iostream>
#include <memory>

// ---- BAD：父子互相用 shared_ptr 持有 → 循环引用 → 引用计数永不归零 ----
namespace bad {
struct Node {
    std::shared_ptr<Node> other;         // 双向都用 shared_ptr
    int id;
    Node(int i) : id(i) { std::cout << "  [构造] bad::Node#" << id << "\n"; }
    ~Node()             { std::cout << "  [析构] bad::Node#" << id << "\n"; }
};
void run() {
    std::cout << "--- BAD（循环引用，会泄漏）---\n";
    auto a = std::make_shared<Node>(1);
    auto b = std::make_shared<Node>(2);
    a->other = b;   // a 引用 b（b 的 strong = 2）
    b->other = a;   // b 引用 a（a 的 strong = 2）
    std::cout << "  离开作用域前 a.use_count = " << a.use_count() << "\n"; // 2
    // 离开作用域：a、b 局部变量各 -1 → 都还剩 1（彼此还引用着）→ 永不析构！
}
} // namespace bad

// ---- GOOD：其中一个方向改用 weak_ptr → 打破环 → 正确释放 ----
namespace good {
struct Node {
    std::shared_ptr<Node> next;          // 一个方向 shared
    std::weak_ptr<Node>   prev;          // 另一个方向 weak（不增加 strong 计数）
    int id;
    Node(int i) : id(i) { std::cout << "  [构造] good::Node#" << id << "\n"; }
    ~Node()             { std::cout << "  [析构] good::Node#" << id << "\n"; }
};
void run() {
    std::cout << "--- GOOD（weak_ptr 打破环，正确释放）---\n";
    auto a = std::make_shared<Node>(1);
    auto b = std::make_shared<Node>(2);
    a->next = b;    // strong 引用
    b->prev = a;    // weak 引用：不增加 a 的 strong 计数
    std::cout << "  a.use_count = " << a.use_count() << "\n"; // 1（weak 不计入 strong）
    // 离开作用域：strong 正常归零 → 两个 Node 都被析构
}
} // namespace good

int main() {
    bad::run();
    std::cout << "  ^ 注意：上面没有打印任何 [析构] —— 内存泄漏了！\n\n";
    good::run();
    std::cout << "  ^ 两个 Node 都正确析构。\n";
    return 0;
}
