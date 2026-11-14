#include "Iterator.h"

#include <cassert>
#include <type_traits>

struct OwnedValue {
    static int destroyed;
    int id = 0;

    ~OwnedValue() {
        ++destroyed;
    }
};

int OwnedValue::destroyed = 0;

int main() {
    static_assert(__cplusplus >= 202002L);
    static_assert(!std::is_copy_constructible_v<LinkedList<OwnedValue>>);
    static_assert(!std::is_copy_assignable_v<LinkedList<OwnedValue>>);

    Node<OwnedValue> emptyNode;
    assert(emptyNode.previousNode == nullptr);
    assert(emptyNode.object == nullptr);
    assert(emptyNode.nextNode == nullptr);

    {
        LinkedList<OwnedValue> list;
        assert(list.isEmpty());

        auto *first = new OwnedValue;
        first->id = 1;
        auto *second = new OwnedValue;
        second->id = 2;
        list.insertObject(first);
        list.insertObject(second);
        assert(!list.isEmpty());

        Iterator<OwnedValue> iterator(&list);
        assert(iterator.nextObject()->id == 2);
        iterator.removeCurrentPosition();
        assert(!list.isEmpty());

        iterator.rewind();
        assert(iterator.nextObject()->id == 1);

        list.clearList();
        assert(list.isEmpty());
        assert(OwnedValue::destroyed == 2);

        list.insertObject(new OwnedValue);
    }

    assert(OwnedValue::destroyed == 3);
    return 0;
}
