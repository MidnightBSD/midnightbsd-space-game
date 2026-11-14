#include "Iterator.h"

#include <cassert>
#include <type_traits>

struct OwnedValue {
    static int destroyed;

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

        list.insertObject(new OwnedValue);
        list.insertObject(new OwnedValue);
        assert(!list.isEmpty());

        Iterator<OwnedValue> iterator(&list);
        assert(iterator.nextObject() != nullptr);
        iterator.removeCurrentPosition();
        assert(!list.isEmpty());

        list.clearList();
        assert(list.isEmpty());
        assert(OwnedValue::destroyed == 2);

        list.insertObject(new OwnedValue);
    }

    assert(OwnedValue::destroyed == 3);
    return 0;
}
