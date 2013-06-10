/*
 * Copyright © 2013 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Robert Carr <robert.carr@canonical.com>
 */

#include "window_manager.h"

#include "mir/shell/focus_controller.h"
#include "mir/shell/session_manager.h"
#include "mir/shell/session.h"
#include "mir/shell/surface.h"

#include <linux/input.h>

#include <cassert>
#include <cstdlib>

namespace me = mir::examples;
namespace msh = mir::shell;

me::WindowManager::WindowManager()
    : max_fingers(0)
{
}

void me::WindowManager::set_focus_controller(std::shared_ptr<msh::FocusController> const& controller)
{
    focus_controller = controller;
}

void me::WindowManager::set_session_manager(
    std::shared_ptr<msh::SessionManager> const& sm)
{
    session_manager = sm;
}

mir::geometry::Point average_pointer(MirMotionEvent const& motion)
{
    using namespace mir;
    using namespace geometry;

    Point avg;
    int x = 0, y = 0, count = (int)motion.pointer_count;

    for (int i = 0; i < count; i++)
    {
        x += motion.pointer_coordinates[i].x;
        y += motion.pointer_coordinates[i].y;
    }

    x /= count;
    y /= count;

    return Point{X{x}, Y{y}};
}

bool me::WindowManager::handle(MirEvent const& event)
{
    assert(focus_controller);

    if (event.key.type == mir_event_type_key &&
        event.key.action == mir_key_action_down &&
        event.key.modifiers & mir_key_modifier_alt &&
        event.key.scan_code == KEY_TAB)  // TODO: Use keycode once we support keymapping on the server side
    {
        focus_controller->focus_next();
        return true;
    }
    else if (event.type == mir_event_type_motion &&
             session_manager)
    {
        geometry::Point cursor = average_pointer(event.motion);

        // FIXME: https://bugs.launchpad.net/mir/+bug/1189379
        MirMotionAction action =
            (MirMotionAction)(event.motion.action & ~0xff00);

        std::shared_ptr<msh::Session> app =
            session_manager->focussed_application().lock();

        int fingers = (int)event.motion.pointer_count;

        if (action == mir_motion_action_down)
            max_fingers = 1;

        if (app)
        {
            // FIXME: We need to be able to select individual surfaces in
            //        future and not just the "default" one.
            std::shared_ptr<msh::Surface> surf = app->default_surface();

            if (surf &&
                (event.motion.modifiers & mir_key_modifier_alt ||
                 fingers >= 3))
            {
                if (//event.motion.button_state == 0 ||
                    action == mir_motion_action_pointer_down)
                {
                    relative_click = cursor - surf->top_left();
                    if (fingers > max_fingers)
                        max_fingers = fingers;
                }
                else if (event.motion.action == mir_motion_action_move ||
                         event.motion.button_state & mir_motion_button_primary)
                { // Drag gesture...
                    if (max_fingers <= 3)  // one mouse or three fingers
                    {
                        geometry::Point dir = cursor - relative_click;
                        surf->move_to(dir);
                        return true;
                    }
                }
            }
        }

        if (max_fingers == 4 && action == mir_motion_action_up)
        { // Four fingers released
            geometry::Point dir = cursor - relative_click;
            if (abs(dir.x.as_int()) > 100)  // Fudge sensitivity
            {
                focus_controller->focus_next();
                return true;
            }
        }
    }
    return false;
}
