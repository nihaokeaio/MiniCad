//
// Created by ZQD on 2026/5/31.
//

#ifndef MINICAD_MESSAGEINFO_H
#define MINICAD_MESSAGEINFO_H
#include <string>


namespace MessageInfo {
    enum ElementChangeFlag:uint8_t {
        Add,
        Remove,
        Update
    };

    enum class ElementUpdateHint:uint8_t {
        All,
        Geometry,
        Transform,
        Property
    };

    inline std::string ElementChange("ElementChange");

    struct ElementChangePayLoad {
        ElementChangeFlag changeType = Add;
        ElementUpdateHint updateHint = ElementUpdateHint::All;
        ElementId elementId{ElementId::InvalidId};
    };
};


#endif //MINICAD_MESSAGEINFO_H
