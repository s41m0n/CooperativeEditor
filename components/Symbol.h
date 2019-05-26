//
// Created by s41m0n on 20/05/19.
//

#ifndef COOPERATIVEEDITOR_SYMBOL_H
#define COOPERATIVEEDITOR_SYMBOL_H

#include <vector>
#include <boost/serialization/vector.hpp>
#include "Identifier.h"

// Forward declaration of class boost::serialization::access
#pragma once
namespace boost {
    namespace serialization {
        class access;
    }
}


/**
 * Symbol class, to identify each inserted character
 *
 * @author Simone Magnani - s41m0n
 */
class Symbol {

private:
    ///The character represented
    char character;
    ///The symbol identifier
    Identifier id;
    ///The position of this symbol
    std::vector<int> position;
    ///Class to access Boost to serialize this class
    friend class boost::serialization::access;

    ///Method called by Boost to serialize
    template<typename Archive>
    void serialize(Archive& ar, const unsigned version) {
        ar & character & id & position;
    }

public:
    ///Classic constructor with all parameters
    Symbol(char character, int siteId, int digit, std::vector<int> &position);
    ///Constructor to create a non-valid symbol to be filled
    Symbol() = default;
    ///Method to compare two symbols
    const int compareTo(const Symbol &s);
    ///Method to get the character
    const char getChar();
    ///Method to get the position
    const std::vector<int> getPos();

};


#endif //COOPERATIVEEDITOR_SYMBOL_H
