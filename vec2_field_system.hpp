#include "evilution_model.hpp"
#include <memory>

namespace evilution {

class Vec2FieldSystem {

  public:
    static std::unique_ptr<EvilutionModel> createCircleModel(EvilutionDevice& device, unsigned int numSides);
};
} // namespace evilution