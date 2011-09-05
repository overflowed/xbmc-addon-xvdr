#pragma once
/*
 *      Copyright (C) 2010 Alwin Esch (Team XBMC)
 *      Copyright (C) 2011 Alexander Pipelka
 *      http://www.xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "XVDRSession.h"
#include "thread.h"

#include <string>
#include <map>

class cResponsePacket;
class cRequestPacket;

class cXVDRData : public cXVDRSession, public cThread
{
public:

  struct XVDRChannel {
    uint32_t    channeluid;
    bool        radio;
    int         channelnumber;
    std::string name;
    int         caid;
  };

  struct XVDREvent {
    uint32_t    eventuid;
    uint32_t    channeluid;
    int         channelnumber;
    time_t      starttime;
    time_t      endtime;
    int         genretype;
    int         genresubtype;
    int         parentalrating;
    std::string title;
    std::string plotoutline;
    std::string plot;
  };

  struct XVDRChannelGroup {
    std::string name;
    bool        radio;
  };

  struct XVDRChannelGroupMember {
    std::string name;
    uint32_t    channelgroupuid;
    int         channelgroupnumber;
  };

  struct XVDRTimer {
  };

  struct XVDRRecording {
  };

  cXVDRData();
  virtual ~cXVDRData();

  bool        Open(const std::string& hostname, int port, const char* name = NULL);
  bool        Login();
  void        Abort();

  bool        SupportChannelScan();
  bool        EnableStatusInterface(bool onOff);
  bool        GetDriveSpace(long long *total, long long *used);

  int         GetChannelsCount();
  bool        RequestChannels(bool radio = false);

  bool        RequestEPGForChannel(const XVDRChannel& channel, time_t start, time_t end);

  int         GetChannelGroupCount(bool automatic);
  bool        RequestChannelGroups(bool bRadio);

  bool        RequestChannelGroupMembers(const XVDRChannelGroup& group);

  int         GetTimersCount();
  bool        RequestTimers();

  bool        AddTimer(const XVDRTimer& timer);
  bool        GetTimerInfo(unsigned int timernumber, XVDRTimer& timer);
  bool        DeleteTimer(const XVDRTimer& timer, bool force = false);
  bool        RenameTimer(const XVDRTimer& timer, const std::string& newname);
  bool        UpdateTimer(const XVDRTimer& timer);

  int         GetRecordingsCount();
  bool        RequestRecordings();
  bool        RenameRecording(const XVDRRecording&, const std::string& newname);
  bool        DeleteRecording(const XVDRRecording& recinfo);

  cResponsePacket*  ReadResult(cRequestPacket* vrp);

protected:

  virtual void TransferChannel(const XVDRChannel& channel);
  virtual void TransferEpgEntry(const XVDREvent& event);
  virtual void TransferChannelGroup(const XVDRChannelGroup& group);
  virtual void TransferChannelGroupMember(const XVDRChannelGroupMember& member);
  virtual void TransferTimer(const XVDRTimer& timer);
  virtual void TransferRecording(const XVDRRecording& timer);

  virtual void Action(void);
  virtual bool OnResponsePacket(cResponsePacket *pkt);

  void SignalConnectionLost();
  void OnDisconnect();
  void OnReconnect();

private:

  bool SendPing();

  struct SMessage
  {
    cCondWait       *event;
    cResponsePacket *pkt;
  };
  typedef std::map<int, struct SMessage> SMessages;

  cMutex          m_Mutex;
  SMessages       m_queue;
  bool            m_aborting;
  bool            m_handleMessages;
};
