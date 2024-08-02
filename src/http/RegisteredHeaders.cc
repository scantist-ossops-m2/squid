/*
 * Copyright (C) 1996-2016 The Squid Software Foundation and contributors
 *
 * Squid software is distributed under GPLv2+ license and includes
 * contributions from numerous individuals and organizations.
 * Please see the COPYING and CONTRIBUTORS files for details.
 */

#include "squid.h"
#include "RegisteredHeaders.h"

#include <ostream>
#include <vector>

namespace Http
{
/* glue to code generated by gperf */
#include "http/RegisteredHeadersHash.cci"

HeaderTableRecord::HeaderTableRecord() :
    name(""), id(HdrType::BAD_HDR), type(HdrFieldType::ftInvalid),
    list(false), request(false), reply(false), hopbyhop(false), denied304(false)
{}

HeaderTableRecord::HeaderTableRecord(const char *n) :
    name(n), id(HdrType::BAD_HDR), type(HdrFieldType::ftInvalid),
    list(false), request(false), reply(false), hopbyhop(false), denied304(false)
{}

HeaderTableRecord::HeaderTableRecord(const char *n, HdrType theId, HdrFieldType theType, int theKind) :
    name(n), id(theId), type(theType),
    list(theKind & HdrKind::ListHeader), request(theKind & HdrKind::RequestHeader),
    reply(theKind & HdrKind::ReplyHeader), hopbyhop(theKind & HdrKind::HopByHopHeader),
    denied304(theKind & HdrKind::Denied304Header)
{}

const HeaderTableRecord&
HeaderLookupTable_t::lookup (const char *buf, const std::size_t len) const {
    const HeaderTableRecord *r = HttpHeaderHashTable::lookup(buf, len);
    if (!r || r->id == Http::HdrType::OTHER)
        return BadHdr;
    return *r;
}
const HeaderTableRecord HeaderLookupTable_t::BadHdr {"*INVALID*:", Http::HdrType::BAD_HDR, Http::HdrFieldType::ftInvalid, HdrKind::None};

HeaderLookupTable_t::HeaderLookupTable_t()
{
    initCache();
}

void
HeaderLookupTable_t::initCache()
{
    idCache.resize(TOTAL_KEYWORDS);
    for (int j = MIN_HASH_VALUE; j <= MAX_HASH_VALUE; ++j) { //MAX_HASH_VALUE is exported by gperf
        if (HttpHeaderDefinitionsTable[j].name[0] != '\0') { //some slots are empty
            idCache[static_cast<int>(HttpHeaderDefinitionsTable[j].id)] =
                & HttpHeaderDefinitionsTable[j];
        }
    }
    //check after the fact. The cache array must be full
    for (auto e : idCache) {
        assert(e->name);
    }
}
const HeaderLookupTable_t HeaderLookupTable;

}; /* namespace Http */

std::ostream&
operator<< (std::ostream &s, Http::HdrType id)
{
    if (Http::any_HdrType_enum_value(id))
        s << Http::HeaderLookupTable.lookup(id).name << '[' << static_cast<int>(id) << ']';
    else
        s << "Invalid-Header[" << static_cast<int>(id) << ']';
    return s;
}

