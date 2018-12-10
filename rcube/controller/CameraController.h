#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include "../components/Camera.h"
#include "../components/Transform.h"

namespace rcube {

/**
 * The CameraController class is the base class from which different kinds of
 * controller classes can be built.
 * By default, the base class does not provide any functionality except holding the camera
 * entity which is to be controlled, and movement and rotation speeds.
 *
 * The class can be extended by overriding the update() method.
 */
class CameraController {
public:

    struct InputState {
        int x, y;
        bool mouse_left;
        bool mouse_middle;
        bool mouse_right;
        bool alt;
        bool ctrl;
        bool shift;
    };

    /**
     * Constructor
     */
    CameraController(int width, int height, float move_speed=1.f, float rotate_speed=1.f);
    /**
     * Virtual destructor
     */
    virtual ~CameraController() = default;

    /**
     * Get the camera movement speed
     * @return movement speed
     */
    virtual float moveSpeed() const;

    /**
     * Set the camera movement speed
     * @param movement speed
     */
    virtual void setMoveSpeed(float val);

    /**
     * Get the camera rotation speed
     * @param rotation speed
     */
    virtual float rotateSpeed() const;

    /**
     * Set the camera rotation speed
     * @param rotation speed
     */
    virtual void setRotateSpeed(float val);

    /**
     * Set the size of the viewport when it is resized.
     */
    virtual void resize(float viewport_width, float viewport_height);

    /**
     * Set the camera to be controlled
     * @param EntityHandle with a Camera and Transform component
     */
    virtual void setEntity(EntityHandle entity);

    Camera * camera() const;

    virtual void update(const InputState &state) = 0;

protected:
    Camera *camera_;
    Transform *transform_;
    float width_, height_;
    float move_speed_, rotate_speed_;
};

} // namespace rcube

#endif // CAMERACONTROLLER_H