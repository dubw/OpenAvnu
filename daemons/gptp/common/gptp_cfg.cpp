/*************************************************************************************************************
Copyright (c) 2015, Coveloz Consulting Ltda
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS LISTED "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS LISTED BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Attributions: The inih library portion of the source code is licensed from
Brush Technology and Ben Hoyt - Copyright (c) 2009, Brush Technology and Copyright (c) 2009, Ben Hoyt.
Complete license and copyright information can be found at
https://github.com/benhoyt/inih/commit/74d2ca064fb293bc60a77b0bd068075b293cf175.
*************************************************************************************************************/

/**
 * @file
 * MODULE SUMMARY : Reads the .ini file and parses it into information
 * to be used on daemon_cl
 */

#include <algorithm>
#include <iostream>

/* need Microsoft version for strcasecmp() from GCC strings.h */
#ifdef _MSC_VER
#define strcasecmp _stricmp
#else
#include <strings.h>
#endif

#include <errno.h>
#include <stdlib.h>
#include <sstream>
#include <map>

#include "gptp_cfg.hpp"

bool GptpIniParser::sHasIniValues = false;

GptpIniParser::GptpIniParser()
{
    sHasIniValues = false;
}

GptpIniParser::GptpIniParser(const std::string& filename)
{
    sHasIniValues = false;
    _error = ini_parse(filename.c_str(), iniCallBack, this);
}

GptpIniParser::~GptpIniParser()
{
}

/****************************************************************************/

int GptpIniParser::parserError(void)
{
    return _error;
}

/****************************************************************************/

int GptpIniParser::iniCallBack(void *user, const char *section,
 const char *name, const char *value)
{
    GptpIniParser *parser = (GptpIniParser*)user;
    bool valOK = false;

    if( parseMatch(section, "ptp") )
    {
        if( parseMatch(name, "priority1") )
        {
            errno = 0;
            char *pEnd;
            unsigned char p1 = (unsigned char) strtoul(value, &pEnd, 10);
            if( *pEnd == '\0' && errno == 0) {
                valOK = true;
                parser->_config.priority1 = p1;
            }
        }
    }
    else if( parseMatch(section, "port") )
    {

        if (parseMatch(name, "initialLogAnnounceInterval"))
        {
            errno = 0;
            char *pEnd;
            int8_t ilai = strtoul(value, &pEnd, 10);
            if( *pEnd == '\0' && errno == 0) {
                valOK = true;
                parser->_config.initialLogAnnounceInterval = ilai;
            }
        }
        else if (parseMatch(name, "initialLogSyncInterval"))
        {
            errno = 0;
            char *pEnd;
            int8_t ilsi = strtol(value, &pEnd, 10);
            if( *pEnd == '\0' && errno == 0) {
                valOK = true;
                parser->_config.initialLogSyncInterval = ilsi;
            }
        }
        else if( parseMatch(name, "announceReceiptTimeout") )
        {
            errno = 0;
            char *pEnd;
            unsigned int art = strtoul(value, &pEnd, 10);
            if( *pEnd == '\0' && errno == 0) {
                valOK = true;
                parser->_config.announceReceiptTimeout = art;
            }
        }
        else if( parseMatch(name, "syncReceiptTimeout") )
        {
            errno = 0;
            char *pEnd;
            unsigned int srt = strtoul(value, &pEnd, 10);
            if( *pEnd == '\0' && errno == 0) {
                valOK = true;
                parser->_config.syncReceiptTimeout = srt;
            }
        }
        else if( parseMatch(name, "neighborPropDelayThresh") )
        {
            errno = 0;
            char *pEnd;
            int64_t nt = strtoul(value, &pEnd, 10);
            if( *pEnd == '\0' && errno == 0) {
                valOK = true;
                parser->_config.neighborPropDelayThresh = nt;
            }
        }
        else if( parseMatch(name, "syncReceiptThresh") )
        {
            errno = 0;
            char *pEnd;
            unsigned int st = strtoul(value, &pEnd, 10);
            if( *pEnd == '\0' && errno == 0) {
                valOK = true;
                parser->_config.syncReceiptThresh = st;
            }
        }
        else if( parseMatch(name, "seqIdAsCapableThresh") )
        {
            errno = 0;
            char *pEnd;
            unsigned int sidt = strtoul(value, &pEnd, 10);
            if( *pEnd == '\0' && errno == 0) {
                valOK = true;
                parser->_config.seqIdAsCapableThresh = sidt;
            }
        }
        else if( parseMatch( name, "lostPdelayRespThresh") )
        {
            errno = 0;
            char *pEnd;
            uint16_t lostpdelayth = (uint16_t) strtoul(value, &pEnd, 10);
            if( *pEnd == '\0' && errno == 0 ) {
                valOK = true;
                parser->_config.lostPdelayRespThresh = lostpdelayth;
            }
        }
        else if (parseMatch(name, "delayMechanism"))
        {
            std::string toCompare = value;
            std::transform(toCompare.begin(), toCompare.end(),
                toCompare.begin(), ::tolower);
            std::map<std::string, PortType> nameMap = {
             {"e2e", V2_E2E}, 
             {"p2p", V2_P2P}};
            auto it = nameMap.find(toCompare);
            parser->_config.delayMechanism = it == nameMap.end() 
                ? V2_P2P : it->second;
            valOK = true;
        }
    }
    else if( parseMatch(section, "eth") )
    {
        if( parseMatch(name, "phy_delay_gb_tx") )
        {
            errno = 0;
            char *pEnd;
            int phdly = strtoul(value, &pEnd, 10);
            if( *pEnd == '\0' && errno == 0) {
                valOK = true;
                parser->_config.phyDelay.gb_tx_phy_delay = phdly;
            }
        }

        else if( parseMatch(name, "phy_delay_gb_rx") )
        {
            errno = 0;
            char *pEnd;
            int phdly = strtoul(value, &pEnd, 10);
            if( *pEnd == '\0' && errno == 0) {
                valOK = true;
                parser->_config.phyDelay.gb_rx_phy_delay = phdly;
            }
        }

        else if( parseMatch(name, "phy_delay_mb_tx") )
        {
            errno = 0;
            char *pEnd;
            int phdly = strtoul(value, &pEnd, 10);
            if( *pEnd == '\0' && errno == 0) {
                valOK = true;
                parser->_config.phyDelay.mb_tx_phy_delay = phdly;
            }
        }

        else if( parseMatch(name, "phy_delay_mb_rx") )
        {
            errno = 0;
            char *pEnd;
            int phdly = strtoul(value, &pEnd, 10);
            if( *pEnd == '\0' && errno == 0) {
                valOK = true;
                parser->_config.phyDelay.mb_rx_phy_delay = phdly;
            }
        }
        else if (parseMatch(name, "unicast_nodes"))
        {
            parser->_config.unicastNodes = parser->Split(value);
            valOK = true;
        }

    }

    if(valOK)
    {
        sHasIniValues = true;
    }
    else
    {
        std::cerr << "Unrecognized configuration item: section=" << section << ", name=" << name << std::endl;
        return 0;
    }

    return 1;
}


/****************************************************************************/

const std::list<std::string> GptpIniParser::Split(const std::string& values) const
{
    GPTP_LOG_VERBOSE("Split  START");
    std::list<std::string> nodeList;
    const char kDelim = ',';
    std::stringstream ss(values);
    std::string node;
    while (std::getline(ss, node, kDelim))
    {
        GPTP_LOG_VERBOSE("Split   node:%s", node.c_str());
       if (!node.empty())
       {
            nodeList.push_back(node);
       }
    }
    GPTP_LOG_VERBOSE("Split  END   nodeList.size():%d", nodeList.size());
    return nodeList;
}

bool GptpIniParser::parseMatch(const char *s1, const char *s2)
{
    return strcasecmp(s1, s2) == 0;
}

