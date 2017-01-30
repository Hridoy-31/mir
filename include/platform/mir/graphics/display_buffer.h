/*
 * Copyright © 2013 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Alexandros Frantzis <alexandros.frantzis@canonical.com>
 */

#ifndef MIR_GRAPHICS_DISPLAY_BUFFER_H_
#define MIR_GRAPHICS_DISPLAY_BUFFER_H_

#include <mir/geometry/rectangle.h>
#include <mir/graphics/renderable.h>
#include <mir_toolkit/common.h>
#include <glm/glm.hpp>

#include <memory>

namespace mir
{
namespace graphics
{

class Buffer;

class NativeDisplayBuffer
{
protected:
    NativeDisplayBuffer() = default;
    virtual ~NativeDisplayBuffer() = default;
    NativeDisplayBuffer(NativeDisplayBuffer const&) = delete;
    NativeDisplayBuffer operator=(NativeDisplayBuffer const&) = delete;
};

/**
 * Interface to an output framebuffer.
 */
class DisplayBuffer
{
public:
    virtual ~DisplayBuffer() = default;

    /** The area the DisplayBuffer occupies in the virtual screen space. */
    virtual geometry::Rectangle view_area() const = 0;

    /** This will render renderlist to the screen and post the result to the 
     *  screen if there is a hardware optimization that can be done.
     *  \param [in] renderlist 
     *      The renderables that should appear on the screen if the hardware
     *      is capable of optmizing that list somehow. If what you want
     *      displayed on the screen cannot be represented by a RenderableList,
     *      then you should render using a graphics library like OpenGL.
     *  \returns
     *      True if the hardware can (and has) fully composite/overlay the list;
     *      False if the hardware platform cannot composite the list, and the
     *      caller should then render the list another way using a graphics
     *      library such as OpenGL.
    **/
    virtual bool overlay(RenderableList const& renderlist) = 0;

    /**
     * Returns a transformation that the renderer must apply to all rendering.
     * There is usually no transformation required (just the identity matrix)
     * but in other cases this will represent transformations that the display
     * hardware is unable to do itself, such as screen rotation, flipping,
     * reflection, scaling or keystone correction.
     */
    virtual glm::mat2 transformation() const = 0;

    /** Returns a pointer to the native display buffer object backing this
     *  display buffer.
     *
     *  The pointer to the native display buffer remains valid as long as the
     *  display buffer object is valid.
     */
    virtual NativeDisplayBuffer* native_display_buffer() = 0;

protected:
    DisplayBuffer() = default;
    DisplayBuffer(DisplayBuffer const& c) = delete;
    DisplayBuffer& operator=(DisplayBuffer const& c) = delete;

    class Transformation : public glm::mat2
    {
    public:
        void reset()         { mat() = glm::mat2(); }
        void orient(MirOrientation ori)
        {
            int cos, sin;
            switch (ori)
            {
            case mir_orientation_normal:   sin =  0; cos =  1;  break;
            case mir_orientation_left:     sin =  1; cos =  0;  break;
            case mir_orientation_inverted: sin =  0; cos = -1;  break;
            case mir_orientation_right:    sin = -1; cos =  0;  break;
            }
            mat() = glm::mat2(cos, sin, -sin, cos) * mat();
        }
        void mirror(MirMirrorMode mode)
        {
            int x = 1, y = 1;
            if (mode == mir_mirror_mode_horizontal)
                x = -1;
            else if (mode == mir_mirror_mode_vertical)
                y = -1;
            mat() = glm::mat2(x, 0, 0, y) * mat();
        }
    private:
        glm::mat2 const& const_mat() const { return *this; }
        glm::mat2& mat() { return *this; }
    };
};

}
}

#endif /* MIR_GRAPHICS_DISPLAY_BUFFER_H_ */
