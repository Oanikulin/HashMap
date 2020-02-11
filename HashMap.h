#include<vector>
#include<functional>
#include<utility>
#include<memory>
#include<stdexcept>
#include<iterator>

namespace HashConstants {
    constexpr int TO_RELOAD = 4;
    constexpr int INIT_SIZE = 4;
}
template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
private:
    size_t alpha = 0; // number of inserted and deleted elements
    size_t element_count = 0;
    size_t begin_index = -1;
    Hash hasher;
    std::vector<std::shared_ptr<std::pair<const KeyType, ValueType>>> elements;
    std::vector<char> filled;

    void simple_insert(const std::pair<KeyType, ValueType>& cur,
                       std::vector<std::shared_ptr<std::pair<const KeyType, ValueType>>>& tmp_elements,
                       std::vector<char>& now_fill) noexcept {
        size_t current_hash = hasher(cur.first) % tmp_elements.size();
        while (now_fill[current_hash] == -1 ||
               (now_fill[current_hash] == 1 && !(tmp_elements[current_hash]->first == cur.first))) {
            current_hash = (current_hash + 1) % tmp_elements.size();
        }
        if (now_fill[current_hash] == 1)
            return;
        now_fill[current_hash] = 1;
        if (begin_index > current_hash)
            begin_index = current_hash;
        tmp_elements[current_hash] = std::make_shared<std::pair<const KeyType, ValueType>>(cur.first, cur.second);
        ++element_count;
        ++alpha;
    }

    void reload(size_t new_size) noexcept {
        element_count = 0;
        alpha = 0;
        begin_index = -1;
        std::vector<std::shared_ptr<std::pair<const KeyType, ValueType>>> new_elements(new_size);
        std::vector<char> new_fill(new_size);
        for (size_t i = 0; i < elements.size(); ++i) {
            if (filled[i] == 1) {
                std::pair<KeyType, ValueType> to_insert = (*elements[i]);
                simple_insert(to_insert, new_elements, new_fill);
            }
        }
        swap(elements, new_elements);
        swap(filled, new_fill);
    }

public:

    std::pair<const KeyType, ValueType>& get_val(int index) noexcept {
        return *elements[index];
    }

    const std::pair<const KeyType, ValueType>& get_val(int index) const noexcept {
        return *elements[index];
    }

    char get_filled(int index) const noexcept {
        return filled[index];
    }

    class iterator : public std::iterator<std::forward_iterator_tag, HashMap<KeyType, ValueType>*> {
    private:
        HashMap* par;
        size_t index;
    public:
        iterator(): par(0), index(0) {}
        iterator(HashMap* par_v, size_t in): par(par_v), index(in) {}
        bool operator == (iterator other) {
            return index == other.index;
        }
        bool operator != (iterator other) {
            return index != other.index;
        }
        std::pair<const KeyType, ValueType>& operator *() {
            return par->get_val(index);
        }

        std::pair<const KeyType, ValueType>* operator ->() {
            return &(par->get_val(index));
        }

        iterator& operator++() {
            if ((*this) == par->end())
                return (*this);
            ++index;
            while ((*this) != par->end() && par->get_filled(index) != 1)
                ++index;
            return (*this);
        }

        iterator operator++(int) {
            iterator tmp = iterator(par, index);
            ++(*this);
            return tmp;
        }

        size_t get_ind() const {
            return index;
        }

    };

    class const_iterator : public std::iterator<std::forward_iterator_tag, const HashMap<KeyType, ValueType>*> {
    private:
        const HashMap * par;
        size_t index;
    public:
        const_iterator(): par(0), index(0) {}

        const_iterator(const HashMap* par_v, size_t in) {
            par = par_v;
            index = in;
        }
        bool operator == (const_iterator other) {
            return index == other.index;
        }
        bool operator != (const_iterator other) {
            return index != other.index;
        }
        const std::pair<const KeyType, ValueType>& operator *() const {
            return par->get_val(index);
        }
        const std::pair<const KeyType, ValueType>* operator ->() const {
            return &(par->get_val(index));
        }

        size_t get_ind() const {
            return index;
        }

        const_iterator& operator++() {
            if ((*this) == par->end())
                return (*this);
            ++index;
            while ((*this) != par->end() && par->get_filled(index) != 1)
                ++index;
            return (*this);
        }

        const_iterator operator++(int) {
            const_iterator tmp = const_iterator(par, index);
            ++(*this);
            return tmp;
        }
    };

    iterator begin() noexcept {
        if (element_count == 0)
            return iterator(this, elements.size());
        return iterator(this, begin_index);
    }

    iterator end() noexcept {
        return iterator(this, elements.size());
    }

    const_iterator begin() const noexcept {
        if (element_count == 0)
            return const_iterator(this, elements.size());
        return const_iterator(this, begin_index);
    }

    const_iterator end() const {
        return const_iterator(this, elements.size());
    }

    iterator find(const KeyType& cur) noexcept {
        if (elements.size() == 0)
            return iterator(this, elements.size());
        size_t current_hash = hasher(cur) % elements.size();
        while (filled[current_hash] == -1 ||
               (filled[current_hash] == 1 && !(elements[current_hash]->first == cur))) {
            current_hash = (current_hash + 1) % elements.size();
        }
        if (filled[current_hash] == 1 && elements[current_hash]->first == cur)
            return iterator(this, current_hash);
        return iterator(this, elements.size());
    }

    const_iterator find(const KeyType& cur) const noexcept {
        if (elements.size() == 0)
            return const_iterator(this, elements.size());
        size_t current_hash = hasher(cur) % elements.size();
        while (filled[current_hash] == -1 ||
               (filled[current_hash] == 1 && !(elements[current_hash]->first == cur)))
            current_hash = (current_hash + 1) % elements.size();
        if (filled[current_hash] == 1 && elements[current_hash]->first == cur)
            return const_iterator(this, current_hash);
        return const_iterator(this, elements.size());
    }


    void clear() noexcept {
        alpha = 0;
        element_count = 0;
        elements.resize(0);
        filled.resize(0);
    }

    HashMap() = default;

    HashMap(Hash tmp_hasher): alpha(0), element_count(0), hasher(tmp_hasher), elements(0), filled(0) {}

    void insert(const std::pair<KeyType, ValueType>& cur) noexcept {
        if (element_count == 0)
            reload(HashConstants::INIT_SIZE);
        if ((alpha + 1) * 2 > elements.size())
            reload(HashConstants::TO_RELOAD * element_count);
        if (find(cur.first) == end()) {
            simple_insert(cur, elements, filled);
        }
    }

    void erase(KeyType del) noexcept {
        auto pos = find(del);
        if (pos == end())
            return;
        filled[pos.get_ind()] = -1;
        elements[pos.get_ind()].reset();
        --element_count;
        if (element_count == 0)
            begin_index = 0;
        else if (pos.get_ind() == begin_index) {
            begin_index = 0;
            while (filled[begin_index] != 1)
                ++begin_index;
        }
        if (element_count * HashConstants::TO_RELOAD < elements.size())
            reload(HashConstants::TO_RELOAD * element_count >> 1);
    }

    template<class InputIt>
    HashMap(InputIt first, InputIt last) noexcept {
        for (auto cur_it = first; cur_it != last; ++cur_it)
            insert(*cur_it);
    }

    template<class InputIt>
    HashMap(InputIt first, InputIt last, Hash tmp_hasher) : alpha(0), element_count(0), hasher(tmp_hasher), elements(0), filled(0) {
        for (auto cur_it = first; cur_it != last; ++cur_it)
            insert(*cur_it);
    }

    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> init) noexcept {
        for (const auto& cur : init)
            insert(cur);
    }

    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> init, Hash tmp_hasher) : alpha(0), element_count(0), hasher(tmp_hasher), elements(0), filled(0) {
        for (const auto& cur : init)
            insert(cur);
    }

    HashMap(HashMap<KeyType, ValueType, Hash>& other): alpha(0), element_count(0), hasher(other.hasher), elements(0), filled(0) {
        for (const auto& current : other.elements)
            if (current != nullptr)
                insert(*current);
    }

    size_t size() const noexcept {
        return element_count;
    }

    bool empty() const noexcept {
        return (element_count == 0);
    }

    Hash hash_function() const noexcept {
        return hasher;
    }

    ValueType& operator [] (const KeyType& key) noexcept {
        auto pos = find(key);
        if (pos == end()) {
            insert({key, ValueType()});
        }
        pos = find(key);
        return pos->second;
    }

    const ValueType& at(const KeyType& key) const {
        auto pos = find(key);
        if (pos == end())
            throw std::out_of_range("error");
        return pos->second;
    }

    HashMap& operator = (const HashMap& other) noexcept {
        if (&other == this)
            return (*this);
        clear();
        for (size_t i = 0; i < other.elements.size(); ++i) {
            if (other.filled[i] != 1)
                continue;
            insert(*(other.elements[i]));
        }
        return (*this);
    }
};
