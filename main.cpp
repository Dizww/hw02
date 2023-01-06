/* 基于智能指针实现双向链表 */
#include <cstdio>
#include <memory>
#include <iostream>

template <typename T>
class Node {
public:
    // 这两个指针会造成什么问题？请修复
    std::unique_ptr<Node> next;
    Node*                 prev;
    // 如果能改成 unique_ptr 就更好了!

    T value;

    // 这个构造函数有什么可以改进的？
    Node(T val):
        next(nullptr),
        prev(nullptr),
        value(val)
    {
    }

    void insert(T val) {
        auto node = std::make_unique<Node>(val);
                                        
        if(this->next != nullptr){
            node->next = std::move(this->next);
            node->prev = this;
        }
        this->next = std::move(node);
        this->next->prev = this->next.get();
    }

    void erase() {
        if(next != nullptr)
        {
            this->next->prev = this->prev;
        }            
        if(prev != nullptr)
        {
            this->prev->next = std::move(this->next);
        }
    }

    ~Node() {
        printf("~Node()\n");   // 应输出多少次？为什么少了？
    }
};

template <typename T>
class List {

    using Node = Node<T>;
public:
    std::unique_ptr<Node> head;

    List() = default;

    List(List const &other)
    {
        printf("List 被拷贝！\n");
        //head = other.head;  // 这是浅拷贝！
        Node*  other_node = other.head.get();

        this->head = std::make_unique<Node>(other.head->value);
        Node* cursor_node = this->head.get();                   

        while(other_node->next !=  nullptr){
            std::unique_ptr<Node> temp_next = std::make_unique<Node>(other_node->next->value);
            temp_next->prev = cursor_node;
            cursor_node->next = std::move(temp_next);
            cursor_node = cursor_node->next.get();
            other_node = other_node->next.get();
        }

        // 请实现拷贝构造函数为 **深拷贝**
    }

    List &operator=(List const &) = delete;  // 为什么删除拷贝赋值函数也不出错？

    List(List &&) = default;
    List &operator=(List &&) = default;

    Node* front() const {
        return head.get();
    }

    T pop_front() {
        int ret = head->value;
        head = std::move(head->next);
        return ret;
    }

    void push_front(T value) {
        auto node = std::make_unique<Node>(value);
        node->next = std::move(head);
        if (head)
            head->prev = node.get();
        head = std::move(node);
    }

    Node* at(size_t index) const {
        auto curr = front();
        for (size_t i = 0; i < index; i++) {
            curr = curr->next.get();
        }
        return curr;
    }
};

template <typename T>
void print(const List<T>& lst) {  // 有什么值得改进的？
    printf("[");
    for (auto curr = lst.front(); curr; curr = curr->next.get()) {
        std::cout << " " << curr->value;
    }
    printf(" ]\n");
}

int main() {
    List<float> a;

    a.push_front(7);
    a.push_front(5);
    a.push_front(8);
    a.push_front(2);
    a.push_front(9);
    a.push_front(4);
    a.push_front(1);

    print(a);   // [ 1 4 9 2 8 5 7 ]

    a.at(2)->erase();

    print(a);   // [ 1 4 2 8 5 7 ]

    List b = a;

    a.at(3)->erase();

    print(a);   // [ 1 4 2 5 7 ]
    print(b);   // [ 1 4 2 8 5 7 ]

    b = {};
    a = {};

    return 0;
}
