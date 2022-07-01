#include "simple_vector.h"
#include "old_tests.h"

#include <cassert>
#include <iostream>
#include <numeric>

using namespace std;

class X {
public:
    X()
        : X(5) {
    }

    X(size_t num)
        : x_(num) {
    }

    X(const X& other) = delete;
    X& operator=(const X& other) = delete;

    X(X&& other) noexcept {
        x_ = exchange(other.x_, 0);
    }

    X& operator=(X&& other) noexcept {
        x_ = exchange(other.x_, 0);
        return *this;
    }

    size_t GetX() const {
        return x_;
    }

private:
    size_t x_;
};

SimpleVector<int> GenerateVector(size_t size) {
    SimpleVector<int> v(size);
    iota(v.begin(), v.end(), 1);
    return v;
}

void TestTemporaryObjConstructor() {
    const size_t size = 1000000;
    cout << "Test with temporary object, copy elision" << endl;
    SimpleVector<int> moved_vector(GenerateVector(size));
    assert(moved_vector.GetSize() == size);
    cout << "Done!" << endl << endl;
}

void TestTemporaryObjOperator() {
    const size_t size = 1000000;
    cout << "Test with temporary object, operator=" << endl;
    SimpleVector<int> moved_vector;
    assert(moved_vector.GetSize() == 0);
    moved_vector = GenerateVector(size);
    assert(moved_vector.GetSize() == size);
    cout << "Done!" << endl << endl;
}

void TestNamedMoveConstructor() {
    const size_t size = 1000000;
    cout << "Test with named object, move constructor" << endl;
    SimpleVector<int> vector_to_move(GenerateVector(size));
    assert(vector_to_move.GetSize() == size);

    SimpleVector<int> moved_vector(move(vector_to_move));
    assert(moved_vector.GetSize() == size);
    assert(vector_to_move.GetSize() == 0);
    cout << "Done!" << endl << endl;
}

void TestNamedMoveOperator() {
    const size_t size = 1000000;
    cout << "Test with named object, operator=" << endl;
    SimpleVector<int> vector_to_move(GenerateVector(size));
    assert(vector_to_move.GetSize() == size);

    SimpleVector<int> moved_vector = move(vector_to_move);
    assert(moved_vector.GetSize() == size);
    assert(vector_to_move.GetSize() == 0);
    cout << "Done!" << endl << endl;
}

void TestNoncopiableMoveConstructor() {
    const size_t size = 5;
    cout << "Test noncopiable object, move constructor" << endl;
    SimpleVector<X> vector_to_move;
    for (size_t i = 0; i < size; ++i) {
        vector_to_move.PushBack(X(i));
    }

    SimpleVector<X> moved_vector = move(vector_to_move);
    assert(moved_vector.GetSize() == size);
    assert(vector_to_move.GetSize() == 0);

    for (size_t i = 0; i < size; ++i) {
        assert(moved_vector[i].GetX() == i);
    }
    cout << "Done!" << endl << endl;
}

void TestNoncopiablePushBack() {
    const size_t size = 5;
    cout << "Test noncopiable push back" << endl;
    SimpleVector<X> v;
    for (size_t i = 0; i < size; ++i) {
        v.PushBack(X(i));
    }

    assert(v.GetSize() == size);

    for (size_t i = 0; i < size; ++i) {
        assert(v[i].GetX() == i);
    }
    cout << "Done!" << endl << endl;
}

void TestСopiableInsert() {
    const size_t size = 5;
    cout << "Test copiable insert" << endl;
    SimpleVector<int> v;
    for (int i = 0; i < size; ++i) {
        v.PushBack(i);
    }

    // в начало
    v.Insert(v.begin(), size + 1);
    assert(v.GetSize() == size + 1);
    assert(*(v.begin()) == size + 1);

    // в конец
    v.Insert(v.end(), size + 2);
    assert(v.GetSize() == size + 2);
    assert(*(v.end() - 1) == size + 2);

    // в середину
    v.Insert(v.begin() + 3, size + 3);
    assert(v.GetSize() == size + 3);
    assert(*(v.begin() + 3) == size + 3);

    // в конец и проверка возвращаемого знаечния
    assert(v.Insert(v.end(), -10) == v.end() - 1);
    cout << "Done!" << endl << endl;
}

void TestNoncopiableInsert() {
    const size_t size = 5;
    cout << "Test noncopiable insert" << endl;
    SimpleVector<X> v;
    for (size_t i = 0; i < size; ++i) {
        v.PushBack(X(i));
    }

    // в начало
    v.Insert(v.begin(), X(size + 1));
    assert(v.GetSize() == size + 1);
    assert(v.begin()->GetX() == size + 1);

    // в конец
    v.Insert(v.end(), X(size + 2));
    assert(v.GetSize() == size + 2);
    assert((v.end() - 1)->GetX() == size + 2);

    // в середину
    v.Insert(v.begin() + 3, X(size + 3));
    assert(v.GetSize() == size + 3);
    assert((v.begin() + 3)->GetX() == size + 3);
    cout << "Done!" << endl << endl;
}

void TestNoncopiableErase() {
    const size_t size = 3;
    cout << "Test noncopiable erase" << endl;
    SimpleVector<X> v;
    for (size_t i = 0; i < size; ++i) {
        v.PushBack(X(i));
    }

    /*for (auto it = v.begin(); it != v.end(); it++) {
        std::cout << it->GetX() << " ";
    }
    std::cout << std::endl;*/

    auto it = v.Erase(v.begin());

    /*for (auto it = v.begin(); it != v.end(); it++) {
        std::cout << it->GetX() << " ";
    }
    std::cout << std::endl;*/

    assert(it->GetX() == 1);
    cout << "Done!" << endl << endl;
}

int main() {
    TestTemporaryObjConstructor();
    TestTemporaryObjOperator();
    TestNamedMoveConstructor();
    TestNamedMoveOperator();
    TestNoncopiableMoveConstructor();
    TestNoncopiablePushBack();

    TestСopiableInsert();
    TestNoncopiableInsert();
    
    TestNoncopiableErase();

    // ТЕСТЫ ИЗ 9 ТЕМЫ
    Test1();
    Test2();
    TestReserveConstructor();
    TestReserveMethod();

    return 0;
}