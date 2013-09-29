#include <string>
#include <glm/glm.hpp>
using namespace glm;
using namespace std;
class CameraNVMParser {
    public:
    static void getCameraMatrix(string, mat3x3&, mat3x4&, vec3&, double&);
};
