#include<vector>
#include<algorithm>
#include<iterator>

template<typename T>
class set {
private:
    struct node {
        T* value;
        node* next;
        node* left;
        node* right;
        node* prev;
        node* top;
        uint16_t h;

        bool operator < (const node &r) {
            return *value < *(r.value);
        }

        bool operator == (const node &r) {
            return *value == *(r.value);
        }

        node(T* value = nullptr, node* top = nullptr) :
            left(nullptr), right(nullptr),
            prev(nullptr), next(nullptr),
            top(top), value(value)
        {
            if (top) {
                h = top->h + 1;
            }
            else {
                h = 1;
            }
        }

        void clear() {
            delete value;
            if (left) left->clear();
            if (right) right->clear();
            delete this;
        }
    };

    node* top;
    node* _begin;
    node* _end;

    uint64_t size;

    node* found_near(const T& value) const {
        node* cur_node = top;
        node* prev = nullptr;
        while (cur_node) {
            prev = cur_node;
            if (*cur_node->value == value)
                return cur_node;
            cur_node = (value < *cur_node->value) ? cur_node->left : cur_node->right;
        }
        return prev;
    }

public:
    void swap(set& second) {
        std::swap(top, second.top);
        std::swap(_begin, second._begin);
        std::swap(_end, second._end);
        std::swap(size, second.size);
    }

    set operator=(const set& other)
    {
        set tmp(other);
        swap(tmp);
        return *this;
    }

    class iterator {
    public:
        node* cur;

        iterator(node* cur) : cur(cur) {};

        iterator& operator++() {
            if(cur) cur = cur->next;
            return *this;
        }
        iterator& operator++(int) {
            if(cur){
                auto res = *this;
                cur = cur->next;
                return res;
            }
            return *this;
        }
        iterator& operator--() {
            if (cur) cur = cur->prev;
            return *this;
        }

        iterator& operator--(int) {
            if(cur){
                auto res = *this;
                cur = cur->prev;
                return res;
            }
            return *this;
        }

        bool operator != (const iterator &it) { return cur != it.cur; }
        bool operator == (const iterator &it) { return cur == it.cur; }

        T& operator*() {
            return *(cur->value);
        }

        bool isValid() const {
            return cur != nullptr;
        }
    };

    typedef iterator const_iterator;




    iterator begin() {
        return iterator(_begin->next);
    }

    iterator end() {
        return iterator(_end);
    }
    iterator rbegin() {
        return iterator(_end);
    }

    iterator rend() {
        return iterator(_begin->next);
    }


    set() {
        top = nullptr;
        _begin = nullptr;
        _end = nullptr;
        size = 0;
    }

    set(const set& other) {
        top = nullptr;
        _begin = nullptr;
        _end = nullptr;
        size = 0;
        for(auto& el : other) {
            this->insert(el);
        }
    }

    std::pair<iterator, bool> insert(const T& value) {

        node* new_node = new node();
        new_node->value = new T(value);

        if (!empty()) {
            node* cur_node = top;
            while (true) {
                if (*cur_node == *new_node)
                    return std::make_pair<iterator, bool>(iterator(cur_node), false);
                bool isLeft = *new_node < *cur_node;
                node** way = isLeft ? &cur_node->left : &cur_node->right;
                if (*way) {
                    cur_node = *way;
                } else {
                    *way = new_node;
                    new_node->top = cur_node;
                    if (isLeft) {
                        new_node->prev = cur_node->prev;
                        cur_node->prev = new_node;
                        new_node->prev->next = new_node;
                        new_node->next = cur_node;
                    }
                    else {
                        new_node->next = cur_node->next;
                        cur_node->next = new_node;
                        new_node->next->prev = new_node;
                        new_node->prev = cur_node;
                    }

                    size++;
                    return std::make_pair<iterator, bool>(iterator(new_node), true);
                }
            }
        }
        else {
            size++;
            _begin = new node();
            _begin->next = new_node;
            new_node->prev = _begin;

            _end = new node();
            _end->prev = new_node;
            new_node->next = _end;

            top = new_node;

            return std::make_pair<iterator, bool>(iterator(new_node), true);
        }
    }

    iterator find(const T &value) const {
        node* cur_node = top;
        while (cur_node) {
            if (*cur_node->value == value)
                return iterator(cur_node);
            cur_node = (value < *cur_node->value) ? cur_node->left : cur_node->right;
        }
        return iterator(nullptr);
    }

    iterator erase(const T& value) {
        return erase(find(value));
    }
    iterator erase(const iterator it) {
        if (!(it.isValid()))
            return it;
        if (find(*(it.cur->value)) != it)
            return it;

        node* cur = it.cur;
        delete cur->value;

        while (cur->left || cur->right) {
            node* next;

            if (cur->left && cur->right) {
                next = (cur->left->h < cur->right->h) ? cur->right : cur->left;
            }
            else if(cur->left){
                next = cur->left;
            }
            else {
                next = cur->right;
            }
            cur->value = next->value;

            if (cur->left || cur->right) {
                cur = next;
            }
            else {
                break;
            }
        }

        if (!empty()) {
            bool isLeft = cur->top->left == cur;
            if (isLeft) {
                cur->top->left = nullptr;
                cur->top->prev = cur->prev;
                cur->prev->next = cur->top;
            }
            else {
                cur->top->right = nullptr;
                cur->top->next = cur->next;
                cur->next->prev = cur->top;
            }
        }
        else {
            delete _begin;
            delete _end;
            _begin = nullptr;
            _end = nullptr;
            top = nullptr;
        }

        node* next = cur->next;
        delete cur;
        size--;

        return iterator(next);
    }

    iterator upper_bound(const T& value) const {
        node* nearest = found_near(value);
        if (!nearest)
            return iterator(nullptr);
        if (*nearest->value < value)
            nearest = nearest->next;
        if (!nearest->value)
            return iterator(nullptr);
        return iterator(nearest);
    }
    iterator lower_bound(const T& value) const {
        node* nearest = found_near(value);
        if (!nearest)
            return iterator(nullptr);
        if (*nearest->value > value) // ! (< && ==)
            nearest = nearest->prev;
        if (!nearest->value)
            return iterator(nullptr);
        return iterator(nearest);
    }

    bool empty() {
        return size == 0; // or top == nullptr
    }

    void clear() {
        if (!empty()) {
            top->clear();
            top = nullptr;
            delete _begin;
            delete _end;
            _begin = nullptr;
            _end = nullptr;
            size = 0;
        }
    }
};

template<typename T>
void swap(set<T>& first, set<T>& second) {
    first.swap(second);
}