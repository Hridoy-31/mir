/*
 * Copyright © 2013 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Alexandros Frantzis <alexandros.frantzis@canonical.com>
 */

#include "rpc_report.h"

#include "mir/report/logging/logger.h"

#include "mir_protobuf_wire.pb.h"

#include <boost/exception/diagnostic_information.hpp>
#include <sstream>

namespace mrl = mir::report::logging;
namespace mcll = mir::client::logging;

namespace
{
std::string const component{"rpc"};
}

mcll::RpcReport::RpcReport(std::shared_ptr<mrl::Logger> const& logger)
    : logger{logger}
{
}

void mcll::RpcReport::invocation_requested(
    mir::protobuf::wire::Invocation const& invocation)
{
    std::stringstream ss;
    ss << "Invocation request: id: " << invocation.id()
       << " method_name: " << invocation.method_name();

    logger->log(mrl::Logger::debug, ss.str(), component);
}

void mcll::RpcReport::invocation_succeeded(
    mir::protobuf::wire::Invocation const& invocation)
{
    std::stringstream ss;
    ss << "Invocation succeeded: id: " << invocation.id()
       << " method_name: " << invocation.method_name();

    logger->log(mrl::Logger::debug, ss.str(), component);
}

void mcll::RpcReport::invocation_failed(
    mir::protobuf::wire::Invocation const& invocation,
    boost::system::error_code const& error)
{
    std::stringstream ss;
    ss << "Invocation failed: id: " << invocation.id()
       << " method_name: " << invocation.method_name()
       << " error: " << error.message();

    logger->log(mrl::Logger::error, ss.str(), component);
}

void mcll::RpcReport::header_receipt_failed(
    boost::system::error_code const& error)
{
    std::stringstream ss;
    ss << "Header receipt failed: " << " error: " << error.message();

    logger->log(mrl::Logger::error, ss.str(), component);
}

void mcll::RpcReport::result_receipt_succeeded(
    mir::protobuf::wire::Result const& result)
{
    std::stringstream ss;
    ss << "Result received: id: " << result.id();

    logger->log(mrl::Logger::debug, ss.str(), component);
}

void mcll::RpcReport::result_receipt_failed(
    std::exception const& ex)
{
    std::stringstream ss;
    ss << "Result receipt failed: reason: " << ex.what();

    logger->log(mrl::Logger::error, ss.str(), component);
}

void mcll::RpcReport::event_parsing_succeeded(
    MirEvent const& /*event*/)
{
    std::stringstream ss;
    /* TODO: Log more information about event */
    ss << "Event parsed";

    logger->log(mrl::Logger::debug, ss.str(), component);
}

void mcll::RpcReport::event_parsing_failed(
    mir::protobuf::Event const& /*event*/)
{
    std::stringstream ss;
    /* TODO: Log more information about event */
    ss << "Event parsing failed";

    logger->log(mrl::Logger::warning, ss.str(), component);
}

void mcll::RpcReport::orphaned_result(
    mir::protobuf::wire::Result const& result)
{
    std::stringstream ss;
    ss << "Orphaned result: " << result.ShortDebugString();

    logger->log(mrl::Logger::error, ss.str(), component);
}

void mcll::RpcReport::complete_response(
    mir::protobuf::wire::Result const& result)
{
    std::stringstream ss;
    ss << "Complete response: id: " << result.id();

    logger->log(mrl::Logger::debug, ss.str(), component);
}

void mcll::RpcReport::result_processing_failed(
    mir::protobuf::wire::Result const& /*result*/,
    std::exception const& ex)
{
    std::stringstream ss;
    ss << "Result processing failed: reason: " << ex.what();

    logger->log(mrl::Logger::error, ss.str(), component);
}

void mcll::RpcReport::file_descriptors_received(
    google::protobuf::Message const& /*response*/,
    std::vector<int32_t> const& fds)
{
    std::stringstream ss;
    ss << "File descriptors received: ";
    for (auto f : fds)
        ss << f << " ";

    logger->log(mrl::Logger::debug, ss.str(), component);
}

void mcll::RpcReport::connection_failure(std::exception const& x)
{
    std::stringstream ss;
    ss << "Connection failure: " << boost::diagnostic_information(x) << std::endl;

    logger->log(mrl::Logger::warning, ss.str(), component);
}
