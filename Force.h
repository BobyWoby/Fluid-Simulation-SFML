//
// Created by bestd on 3/20/2024.
//

#ifndef FLUIDSIMSFML_FORCE_H
#define FLUIDSIMSFML_FORCE_H
namespace F{

    class Force{
    private:
        float direction; // in radians
        float magnitude;
    public:
        Force(float dx, float my, bool polar = true);
        Force add(Force f1);
        float getMagnitude();
        float getDirection();
        void setMagnitude(float m);
        void setDirection(float d);

    };
}


#endif //FLUIDSIMSFML_FORCE_H
