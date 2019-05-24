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

int CrdtAlgorithm::findPositionInsert(Symbol s, std::vector<Symbol>& symbols) {
    int left = 0;
    int right = symbols.size()- 1;
    int mid, compareNum;

    if (symbols.empty() || s.compareTo(symbols[left]) < 0)
        return left;
    else if (s.compareTo(symbols[right]) > 0)
        return symbols.size();

    while (left + 1 < right) {
        mid = std::floor(left + (right - left) / 2);
        compareNum = s.compareTo(symbols[mid]);

        if (compareNum == 0)
            return mid;
        else if (compareNum > 0)
            left = mid;
        else
            right = mid;
    }

    if (s.compareTo(symbols[left]) == 0)
        return left;
    else
        return right;
}

std::vector<int>* CrdtAlgorithm::generatePosBetween(Symbol* s1, Symbol* s2,
                                                   std::vector<int> *newPos, int index1, int level, int index2) {
    int baseValue = (int)(std::pow(2, level) * CrdtAlgorithm::base);
    auto pos1 = s1 != nullptr? s1->getPos() : std::vector<int>();
    auto pos2 = s2 != nullptr? s2->getPos() : std::vector<int>();
    int id1 = index1 < pos1.size() ? pos1[index1] : 0;
    int id2 = index2 < pos2.size() ? pos2[index2] : baseValue;

    if (id2 - id1 > 1) {
        int newDigit = CrdtAlgorithm::generateIdBetween(id1, id2);
        newPos->push_back(newDigit);
        return newPos;
    } else if (id2 - id1 == 1) {
        newPos->push_back(id1);
        return CrdtAlgorithm::generatePosBetween(s1, s2, newPos, index1+1, level+1);
    } else if (id1 == id2) {
        int comp = s1->compareTo(*s2);
        if(comp > 0) {
            newPos->push_back(id1);
            return CrdtAlgorithm::generatePosBetween(s1, s2, newPos, index1+1, level+1, -1);
        } else if(comp == 0) {
            newPos->push_back(id1);
            return CrdtAlgorithm::generatePosBetween(s1, s2, newPos, index1+1, level+1, index2+1);
        }
    }
    throw std::runtime_error("Fix Position Sorting");
}
