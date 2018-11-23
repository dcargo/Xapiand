/** @file remotesubmatch.cc
 *  @brief SubMatch class for a remote database.
 */
/* Copyright (C) 2006,2007,2009,2010,2011,2014,2015,2018 Olly Betts
 * Copyright (C) 2007,2008 Lemur Consulting Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include "config.h"

#include "xapian/matcher/remotesubmatch.h"

#include "xapian/common/debuglog.h"
#include "xapian/backends/remote/remote-database.h"
#include "xapian/weight/weightinternal.h"

using namespace std;

void
RemoteSubMatch::prepare_match(Xapian::Weight::Internal& total_stats)
{
    LOGCALL_VOID(MATCH, "RemoteSubMatch::prepare_match", total_stats);
    Xapian::Weight::Internal remote_stats;
    db->get_remote_stats(remote_stats);
    total_stats += remote_stats;
}

void
RemoteSubMatch::start_match(const std::string& query_id,
				Xapian::doccount first,
			    Xapian::doccount maxitems,
			    Xapian::doccount check_at_least,
			    Xapian::Weight::Internal & total_stats)
{
    LOGCALL_VOID(MATCH, "RemoteSubMatch::start_match", query_id | first | maxitems | check_at_least | total_stats);
    db->send_global_stats(query_id, first, maxitems, check_at_least, total_stats);
}
