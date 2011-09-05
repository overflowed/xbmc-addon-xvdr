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

#include <stdint.h>
#include <string>

extern "C" {
#include "libTcpSocket/os-dependent_socket.h"
}

class cResponsePacket;
class cRequestPacket;

class cXVDRSession
{
public:
  cXVDRSession();
  virtual ~cXVDRSession();

  virtual bool      Open(const std::string& hostname, int port, const char *name = NULL);
  virtual bool      Login();
  virtual void      Close();
  virtual void      Abort();

  void              SetTimeout(int timeout_ms);
  void              SetCompression(int level);

  cResponsePacket*  ReadMessage();
  bool              SendMessage(cRequestPacket* vrp);

  cResponsePacket*  ReadResult(cRequestPacket* vrp);
  bool              ReadSuccess(cRequestPacket* m);

  int                GetProtocol()   { return m_protocol; }
  const std::string& GetServerName() { return m_server; }
  const std::string& GetVersion()    { return m_version; }

protected:

  typedef enum {
    XVDR_NOTICE,
    XVDR_ERROR,
    XVDR_DEBUG
  } XVDR_LEVEL;

  virtual void Log(XVDR_LEVEL, ...);
  virtual void Notification(XVDR_LEVEL, ...);

  void SleepMs(int ms);

  bool TryReconnect();
  bool IsOpen() { return m_fd != INVALID_SOCKET; }

  virtual void* AllocatePacket(uint8_t** pBuffer, int length);
  virtual void FreePacket(void* packet);

  virtual void OnDisconnect();
  virtual void OnReconnect();

  virtual void SignalConnectionLost();
  bool ConnectionLost() { return m_connectionLost; }

  std::string     m_hostname;
  int             m_port;
  std::string     m_name;
  int             m_timeout; // timeout in ms (2000ms default)
  int             m_compression; // compression level (6 default)

private:

  bool readData(uint8_t* buffer, int totalBytes);

  socket_t    m_fd;
  int         m_protocol;
  std::string m_server;
  std::string m_version;
  bool        m_connectionLost;

  struct {
        uint32_t opCodeID;
        uint32_t streamID;
        int64_t pts;
        int64_t dts;
        uint32_t duration;
        uint32_t userDataLength;
  } m_streamPacketHeader;

  struct {
        uint32_t requestID;
        uint32_t userDataLength;
  } m_responsePacketHeader;

};
