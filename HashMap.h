#include<vector>
#include<functional>
#include<utility>
#include<memory>
#include<stdexcept>
#include<iterator>
using std::vector;
using std::shared_ptr;

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
private:
    size_t alpha = 0;
    size_t number_of_elems = 0;
    Hash hasher;
    vector<shared_ptr<std::pair<const KeyType, ValueType>>> elems;
    vector<KeyType> keys;
    vector<char> filled;

    void simple_insert(std::pair<KeyType, ValueType> cur, vector<KeyType>& tmp_keys, vector<shared_ptr<std::pair<const KeyType, ValueType>>>& tmp_elems, vector<char>& now_fill) noexcept {
        size_t nh = hasher(cur.first) % tmp_elems.size();
        while (now_fill[nh] == -1 || (now_fill[nh] == 1 && !(tmp_keys[nh] == cur.first))) {
            nh = (nh + 1) % tmp_elems.size();
        }
        if (now_fill[nh] == 1)
            return;
        tmp_keys[nh] = cur.first;
        now_fill[nh] = 1;
        tmp_elems[nh] = shared_ptr<std::pair<const KeyType, ValueType>>(new std::pair<const KeyType, ValueType> (cur.first, cur.second));
        ++number_of_elems;
        ++alpha;
    }

    void reload(size_t new_size) noexcept {
        number_of_elems = 0;
        alpha = 0;
        vector<shared_ptr<std::pair<const KeyType, ValueType>>> new_elems(new_size);
        vector<KeyType> new_keys(new_size);
        vector<char> new_fill(new_size);
        for (size_t i = 0; i < elems.size(); ++i) {
            if (filled[i] == 1) {
                auto new_elem = elems[i]->second;
                simple_insert({keys[i], new_elem}, new_keys, new_elems, new_fill);
            }
        }
        swap(elems, new_elems);
        swap(keys, new_keys);
        swap(filled, new_fill);
    }

public:

    std::pair<const KeyType, ValueType>& get_val(int ind) noexcept {
        return *elems[ind];
    }

    const std::pair<const KeyType, ValueType>& get_val(int ind) const noexcept{
        return *elems[ind];
    }

    char get_fil(int ind) const noexcept {
        return filled[ind];
    }

    class iterator : public std::iterator<std::forward_iterator_tag,HashMap<KeyType, ValueType>*> {
    private:
        HashMap* par;
        size_t ind;
    public:
        iterator(): par(0), ind(0) {}
        iterator(HashMap* par_v, size_t in): par(par_v), ind(in) {}
        bool operator == (iterator oth) {
            return ind == oth.ind;
        }
        bool operator != (iterator oth) {
            return ind != oth.ind;
        }
        std::pair<const KeyType, ValueType>& operator *() {
            return par->get_val(ind);
        }

        std::pair<const KeyType, ValueType>* operator ->() {
            return &(par->get_val(ind));
        }

        iterator& operator = (iterator oth) {
            par = oth.par;
            ind = oth.ind;
            return (*this);
        }

        iterator& operator++() {
            if ((*this) == par->end())
                return (*this);
            ++ind;
            while ((*this) != par->end() && par->get_fil(ind) != 1)
                ++ind;
            return (*this);
        }

        iterator operator++(int) {
            iterator tmp = iterator(par, ind);
            ++(*this);
            return tmp;
        }

        int get_ind() const {
            return ind;
        }

    };

    class const_iterator : public std::iterator<std::forward_iterator_tag, const HashMap<KeyType, ValueType>*> {
    private:
        const HashMap * par;
        size_t ind;
    public:
        const_iterator(): par(0), ind(0) {}

        const_iterator(const HashMap* par_v, size_t in) {
            par = par_v;
            ind = in;
        }
        bool operator == (const_iterator oth) {
            return ind == oth.ind;
        }
        bool operator != (const_iterator oth) {
            return ind != oth.ind;
        }
        const std::pair<const KeyType, ValueType>& operator *() const {
            return par->get_val(ind);
        }
        const std::pair<const KeyType, ValueType>* operator ->() const {
            return &(par->get_val(ind));
        }

        int get_ind() const {
            return ind;
        }

        const_iterator& operator = (const_iterator oth) {
            par = oth.par;
            ind = oth.ind;
            return (*this);
        }

        const_iterator& operator++() {
            if ((*this) == par->end())
                return (*this);
            ++ind;
            while ((*this) != par->end() && par->get_fil(ind) != 1)
                ++ind;
            return (*this);
        }

        const_iterator operator++(int) {
            const_iterator tmp = const_iterator(par, ind);
            ++(*this);
            return tmp;
        }
    };

    iterator begin() noexcept {
        if (number_of_elems == 0)
            return iterator(this, elems.size());
        int nh = 0;
        while (filled[nh] != 1)
            ++nh;
        return iterator(this, nh);
    }

    iterator end() noexcept {
        return iterator(this, elems.size());
    }

    const_iterator begin() const noexcept {
        if (number_of_elems == 0)
            return const_iterator(this, elems.size());
        int nh = 0;
        while (filled[nh] != 1)
            ++nh;
        return const_iterator(this, nh);
    }

    const_iterator end() const {
        return const_iterator(this, elems.size());
    }

    iterator find(const KeyType& cur) noexcept {
        if (elems.size() == 0)
            return iterator(this, elems.size());
        size_t nh = hasher(cur) % elems.size();
        while (filled[nh] == -1 || (filled[nh] == 1 && !(keys[nh] == cur))) {
            nh = (nh + 1) % elems.size();
        }
        if (filled[nh] == 1 && keys[nh] == cur)
            return iterator(this, nh);
        return iterator(this, elems.size());
    }

    const_iterator find(const KeyType& cur) const noexcept {
        if (elems.size() == 0)
            return const_iterator(this, elems.size());
        size_t nh = hasher(cur) % elems.size();
        while (filled[nh] == -1 || (filled[nh] == 1 && !(keys[nh] == cur)))
            nh = (nh + 1) % elems.size();
        if (filled[nh] == 1 && keys[nh] == cur)
            return const_iterator(this, nh);
        return const_iterator(this, elems.size());
    }


    void clear() noexcept {
        alpha = 0;
        number_of_elems = 0;
        elems.resize(0);
        keys.resize(0);
        filled.resize(0);
    }

    HashMap() noexcept {
    }

    HashMap(Hash tmp_hasher): alpha(0), number_of_elems(0), hasher(tmp_hasher), elems(0), keys(0), filled(0) {}

    void insert(std::pair<KeyType, ValueType> cur) noexcept {
        if (number_of_elems == 0)
            reload(4);
        if ((alpha + 1) * 2 > elems.size())
            reload(4 * number_of_elems);
        if (find(cur.first) == end())
            simple_insert(cur, keys, elems, filled);
    }

    void erase(KeyType del) noexcept {
        auto pos = find(del);
        if (pos == end())
            return;
        filled[pos.get_ind()] = -1;
        elems[pos.get_ind()].reset();
        --number_of_elems;
        if (number_of_elems * 4 < elems.size())
            reload(2 * number_of_elems);
    }

    template<class InputIt>
    HashMap(InputIt first, InputIt last) noexcept {
        for (auto cur_it = first; cur_it != last; ++cur_it)
            insert(*cur_it);
    }

    template<class InputIt>
    HashMap(InputIt first, InputIt last, Hash tmp_hasher) : alpha(0), number_of_elems(0), hasher(tmp_hasher), elems(0), keys(0), filled(0) {
        for (auto cur_it = first; cur_it != last; ++cur_it)
            insert(*cur_it);
    }

    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> init) noexcept {
        for (auto& cur : init)
            insert(cur);
    }

    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> init, Hash tmp_hasher) : alpha(0), number_of_elems(0), hasher(tmp_hasher), elems(0), keys(0), filled(0) {
        for (auto& cur : init)
            insert(cur);
    }

    HashMap(HashMap<KeyType, ValueType, Hash>& oth): alpha(0), number_of_elems(0), hasher(oth.hasher), elems(0), keys(0), filled(0) {
        for (size_t i = 0; i < oth.elems.size(); ++i) {
            if (oth.filled[i] != 1)
                continue;
            insert(*(oth.elems[i]));
        }
    }

    size_t size() const noexcept {
        return number_of_elems;
    }

    bool empty() const noexcept {
        return (number_of_elems == 0);
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

    HashMap& operator = (const HashMap& oth) noexcept {
        if (&oth == this)
            return (*this);
        clear();
        for (size_t i = 0; i < oth.elems.size(); ++i) {
            if (oth.filled[i] != 1)
                continue;
            insert(*(oth.elems[i]));
        }
        return (*this);
    }
};
