/*
 * Copyright (c) 2015-2019 Dubalu LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include "logger_fwd.h"
#include "colors.h"

#define LOG_COL NO_COLOR
#define DEBUG_COL rgb(105, 105, 105)
#define INFO_COL rgb(63,119,179)
#define NOTICE_COL rgb(116,185,255)
#define WARNING_COL rgb(255,177,66)
#define ERR_COL rgb(179,57,57)
#define CRIT_COL rgb(238,82,83)
#define ALERT_COL rgb(238,82,83)
#define EMERG_COL rgb(238,82,83)

#define L_MARK _LOG(false, LOG_DEBUG, "🔥  " DEBUG_COL, args)

///

#define L_OBJ_BEGIN L_NOTHING
#define L_OBJ_END L_NOTHING
#define L_DATABASE_BEGIN L_NOTHING
#define L_DATABASE_END L_NOTHING
#define L_DATABASE_WRAP_BEGIN L_NOTHING
#define L_DATABASE_WRAP_END L_NOTHING
#define L_EV_BEGIN L_NOTHING
#define L_EV_END L_NOTHING

///

#define L_ERRNO L_NOTHING
#define L_CALL L_NOTHING
#define L_TIME L_NOTHING
#define L_CONN L_NOTHING
#define L_RAFT L_NOTHING
#define L_RAFT_PROTO L_NOTHING
#define L_DISCOVERY L_NOTHING
#define L_DISCOVERY_PROTO L_NOTHING
#define L_REPLICATION L_NOTHING
#define L_OBJ L_NOTHING
#define L_THREADPOOL L_NOTHING
#define L_DATABASE L_NOTHING
#define L_DATABASE_WAL L_NOTHING
#define L_HTTP L_NOTHING
#define L_BINARY L_NOTHING
#define L_REPLICA L_NOTHING
#define L_HTTP_PROTO_PARSER L_NOTHING
#define L_EV L_NOTHING
#define L_HTTP_WIRE L_NOTHING
#define L_BINARY_WIRE L_NOTHING
#define L_REPLICA_WIRE L_NOTHING
#define L_TCP_WIRE L_NOTHING
#define L_TCP_ENQUEUE L_NOTHING
#define L_UDP_WIRE L_NOTHING
#define L_HTTP_PROTO L_NOTHING
#define L_BINARY_PROTO L_NOTHING
#define L_REPLICA_PROTO L_NOTHING
#define L_INDEX L_NOTHING
#define L_SEARCH L_NOTHING

////////////////////////////////////////////////////////////////////////////////
// Enable the following when needed.

// #undef L_OBJ_BEGIN
// #define L_OBJ_BEGIN L_DELAYED_1000
// #undef L_OBJ_END
// #define L_OBJ_END L_DELAYED_N_UNLOG
// #undef L_DATABASE_BEGIN
// #define L_DATABASE_BEGIN L_DELAYED_600
// #undef L_DATABASE_END
// #define L_DATABASE_END L_DELAYED_N_UNLOG
// #undef L_DATABASE_WRAP_BEGIN
// #define L_DATABASE_WRAP_BEGIN L_DELAYED_100
// #undef L_DATABASE_WRAP_END
// #define L_DATABASE_WRAP_END L_DELAYED_N_UNLOG
// #undef L_EV_BEGIN
// #define L_EV_BEGIN L_DELAYED_200
// #undef L_EV_END
// #define L_EV_END L_DELAYED_N_UNLOG

// #undef L_CALL
// #define L_CALL L_STACKED_DIM_GREY
