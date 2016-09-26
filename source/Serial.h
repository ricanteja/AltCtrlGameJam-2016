/*	
	This code was modified from the Arduino Serial C++ tutorial found on the official site.
	I hope in the future to make something a bit more robust than this but for now it will do.
*/

#ifndef SERIAL_H
#define SERIAL_H

#include <cstdio>
#include <cstdlib>
#include <windows.h>

#define ARDUINO_WAIT_TIME 2000

namespace moony
{
	class Serial
	{
	public:
		Serial(char* port_name)
		{
			m_connected = false;

			m_com_port = CreateFile(port_name, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

			if(m_com_port == INVALID_HANDLE_VALUE)
			{
				if(GetLastError() == ERROR_FILE_NOT_FOUND)
					logError() << "Handle was not attached. Reason: %s not available." << port_name;
				else
					logError() << "Unkown Error";
			}
			else
			{
				DCB serial_args = {0};

				if(!GetCommState(m_com_port, &serial_args))
					logError() << "Failed to get current serial parameters!";
				else
				{
					serial_args.BaudRate=CBR_9600;	// BAUD RATE
					serial_args.ByteSize=8;
					serial_args.StopBits=ONESTOPBIT;
					serial_args.Parity=NOPARITY;
					serial_args.fDtrControl = DTR_CONTROL_ENABLE;

					if(!SetCommState(m_com_port, &serial_args))
						logDebug() << "Could not set Serial Port parameters";
					else
					{
						m_connected = true;
						PurgeComm(m_com_port, PURGE_RXCLEAR | PURGE_TXCLEAR);

						Sleep(ARDUINO_WAIT_TIME);
					}
				}
			}
		}

		~Serial()
		{
			if(m_connected)
			{
				m_connected = false;
				CloseHandle(m_com_port);
			}
		}

		int read(char* buffer, unsigned int size)
		{
			DWORD bytes_read;
			unsigned int bytes_left;

			ClearCommError(m_com_port, &m_errors, &m_status);

			if(m_status.cbInQue > 0)
			{
				if(m_status.cbInQue > size)
					bytes_left = size;
				else
					bytes_left = m_status.cbInQue;

				if(ReadFile(m_com_port, buffer, bytes_left, &bytes_read, NULL))
					return bytes_read;
			}

			return 0;
		}

		char getChar()
		{
			char c = 0;
			DWORD bytes_read;

			ClearCommError(m_com_port, &m_errors, &m_status);

			if(m_status.cbInQue > 0 && ReadFile(m_com_port, &c, 1, &bytes_read, NULL))
				return c;
		}



		bool write(char* buffer, unsigned int size)
		{
			DWORD bytes_sent;

			if(!WriteFile(m_com_port, (void *)buffer, size, &bytes_sent, 0))
			{
				ClearCommError(m_com_port, &m_errors, &m_status);
				return false;
			}

			return true;
		}

		bool isConnected()
		{
			return m_connected;
		}

	private:
		HANDLE m_com_port;
		bool m_connected;
		COMSTAT m_status;
		DWORD m_errors;
	};
}

#endif