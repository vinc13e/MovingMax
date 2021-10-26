//
// Created by vicente on 10/25/21.
//

#ifndef MOVINGMAX_MOVINGMAX_H
#define MOVINGMAX_MOVINGMAX_H


#include <cstdint>
#include <iostream>
#include <memory>
#include <queue>
#include <set>
#include <functional>

using std::cout;
using std::endl;

template<class T>
class MovingMax {

public:
    MovingMax() = delete;
    MovingMax(int size) : max_size(size),  dataS([this](sp_int64_t a, sp_int64_t b) {return *a < *b;}) {}

    int64_t process(T n){
        auto elem = std::make_shared<int64_t> (n);
        if(dataQ.size()==max_size){
            //remove the oldest element from both queue and ordered set;
            auto &oldest = dataQ.front();
            std::cout << "removing " << *oldest << endl;
            dataS.erase(oldest);
            dataQ.pop();
        }
        dataQ.push(elem);
        dataS.insert(std::move(elem));
        return **dataS.crbegin();
    }

private:
    using sp_int64_t = std::shared_ptr<T>;
    static bool cmp(const sp_int64_t& a, const sp_int64_t& b) {return *a < *b;};

    int max_size;

    std::queue<sp_int64_t> dataQ;
    std::multiset<sp_int64_t, std::function<bool(sp_int64_t, sp_int64_t)>> dataS;


};


#endif //MOVINGMAX_MOVINGMAX_H
