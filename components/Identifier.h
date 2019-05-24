//
// Created by s41m0n on 20/05/19.
//

#ifndef COOPERATIVEEDITOR_IDENTIFIER_H
#define COOPERATIVEEDITOR_IDENTIFIER_H


// Forward declaration of class boost::serialization::access
#pragma once
namespace boost {
    namespace serialization {
        class access;
    }
}

/**
 * Identifier class, used to identify a symbol
 *
 * @author Simone Magnani - s41m0n
 *
 */
class Identifier {

private:
    ///The editor specific ID
    unsigned int editorId;
    ///Unique digit inside the editor
    unsigned int digit;
    ///Class to access Boost to serialize this class
    friend class boost::serialization::access;

    ///Method called by Boost to serialize
    template<typename Archive>
    void serialize(Archive& ar, const unsigned version) {
        ar & editorId & digit;
    }

public:
    ///Classic constructor, given both the two parameter
    Identifier(unsigned int editorId, unsigned int digit);
    ///Constructor used to create a non-valid Identifier to be filled
    Identifier();
    ///Method to compare two identifier by their private variables
    int compareTo(Identifier other);

};


#endif //COOPERATIVEEDITOR_IDENTIFIER_H
