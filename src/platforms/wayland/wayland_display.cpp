/*
 * Copyright © 2019 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 2 or 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "wayland_display.h"

#include <mir/options/option.h>
#include <boost/throw_exception.hpp>
#include <optional>

namespace mpw = mir::platform::wayland;

namespace
{
struct WaylandDisplay
{
    struct wl_display* const wl_display;

    WaylandDisplay(char const* wayland_host) :
        wl_display{wl_display_connect(wayland_host)} {}

    ~WaylandDisplay() { if (wl_display) wl_display_disconnect(wl_display); }
};

char const* wayland_host_option_name{"wayland-host"};
char const* wayland_host_option_description{"Socket name for host compositor"};
}

void mpw::add_connection_options(boost::program_options::options_description& config)
{
    config.add_options()
        (wayland_host_option_name,
         boost::program_options::value<std::string>(),
         wayland_host_option_description);
}

auto mpw::connection(options::Option const& options) -> struct wl_display*
{
    auto const wayland_host = options.is_set(wayland_host_option_name) ?
        std::make_optional(options.get<std::string>(wayland_host_option_name)) :
        std::nullopt;

    static auto const wayland_display = std::make_unique<WaylandDisplay>(
        wayland_host ? wayland_host->c_str() : nullptr);

    if (wayland_display->wl_display)
    {
        return wayland_display->wl_display;
    }
    else if (wayland_host)
    {
        BOOST_THROW_EXCEPTION(std::runtime_error(
            "Failed to connect to Wayland display '" + wayland_host.value() + "'"));
    }
    else
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("Failed to connect to default Wayland display"));
    }
}

auto mir::platform::wayland::connection_options_supplied(mir::options::Option const& options) -> bool
{
    return options.is_set(wayland_host_option_name);
}
