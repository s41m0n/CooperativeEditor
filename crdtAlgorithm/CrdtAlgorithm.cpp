//
// Created by s41m0n on 24/05/19.
//

#include "CrdtAlgorithm.h"

int CrdtAlgorithm::generateIdBetween(int id1, int id2){
    return (id2 + id1) / 2;
}

int CrdtAlgorithm::findPositionErase(Symbol s, std::vector<Symbol>& symbols) {
    int left = 0, mid, right = symbols.size() - 1;

    // check if struct is empty or char is less than first char
    if (symbols.empty() || s.compareTo(symbols[left]) < 0)
        return 0;
    else if (s.compareTo(symbols[right]) > 0)
        return symbols.size();


    // binary search
    while (left + 1 < right) {
        mid = std::floor(left + (right - left) / 2.0);

        int comp = s.compareTo(symbols[mid]);
        if (comp == 0)
            return mid;
        else if (comp < 0)
            right = mid;
        else
            left = mid;
    }
    return -1;
}